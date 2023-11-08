#include <algorithm>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/LangOptions.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <consumer.h>
#include <cstdint>
#include <debug.h>
#include <functional>
#include <llvm/Support/Casting.h>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <util.h>
#include <vector>

using namespace clang;
using namespace clang::ast_matchers;
using namespace std;
using namespace llvm;

namespace {

const string desc = "This bug is a semantic error where a variable is "
                    "mistakenly altered, leading to boundary issues by "
                    "deviating from its intended value in the program";
const string id0 = "numberVariable";
const string id1 = "assignment";

inline void modifyBound(const DeclRefExpr *bo, const SourceManager *sm,
                        Rewriter &r) {
  // 注意 getEndLoc 可能返回代表范围结束的令牌的开始位置
  SourceLocation loc = bo->getEndLoc();
  // 返回紧随给定令牌之后的第一个字符的位置
  loc = Lexer::getLocForEndOfToken(loc, 0, *sm, r.getLangOpts());

  // 修改边界, 范围过大反而不真实
  int num = randomNumber(1, 6);
  int op = randomNumber(0, 2);

  string num_str = to_string(num);
  string op_str = ((op == 0) ? " +" : " -");

  r.InsertTextAfter(loc, op_str + " " + num_str);

  outs() << sm->getPresumedLineNumber(loc) << " "
         << sm->getPresumedColumnNumber(loc) << "\n"
         << desc << "\n";
}

class BoundCallbackComm : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit BoundCallbackComm(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {
    const auto *bo = result.Nodes.getNodeAs<DeclRefExpr>(id0);
    const SourceManager *sm = result.SourceManager;

    if (bo && sm) {
      function<void(Rewriter &)> f = [bo, sm](Rewriter &r) {
        modifyBound(bo, sm, r);
      };

      all.push_back(f);
    }
  }
};

#ifdef AMMON_DEBUG
void printExpr(const Expr *expression, const SourceManager *sm) {
  if (!expression || !sm)
    return;

  SourceRange srcRange = expression->getSourceRange();
  SourceLocation start = srcRange.getBegin();
  SourceLocation end =
      Lexer::getLocForEndOfToken(srcRange.getEnd(), 0, *sm, LangOptions());

  bool invalid;
  StringRef text = Lexer::getSourceText(
      CharSourceRange::getCharRange(start, end), *sm, LangOptions(), &invalid);

  if (!invalid) {
    INFO(text);
  } else {
    ERROR("Unable to extract source text");
  }
}
#endif

bool isLastEquals(const BinaryOperator *bin, const SourceManager *sm) {
  if (!bin || !sm) {
    return false;
  }

  // 等号的确切位置
  SourceLocation loc = bin->getOperatorLoc();
  // 将 loc 移动到等号的下一个 token 位置
  loc = Lexer::getLocForEndOfToken(loc, 0, *sm, LangOptions());

  Lexer lexer(sm->getSpellingLoc(loc), LangOptions(),
              sm->getBufferData(sm->getFileID(loc)).data(),
              sm->getCharacterData(loc),
              sm->getBufferData(sm->getFileID(loc)).end());

  Token token;

  while (!lexer.LexFromRawLexer(token)) {
    // 如果 token 是分号 return true
    if (token.is(tok::semi)) {
      return true;
    }

    // 找到另一个等号，不是最后一个赋值
    if (token.is(tok::equal)) {
      return false;
    }
  }

  // 没有找到其他的等号，是最后一个赋值
  return true;
}

// 选取右值表达式
void rightDeclRefs(vector<const DeclRefExpr *> &result, const Expr *e,
                   const SourceManager *sm) {
  if (!e)
    return;
#ifdef AMMON_DEBUG
  printExpr(e, sm);
#endif
  // 是否是变量使用
  const DeclRefExpr *dre = dyn_cast<DeclRefExpr>(e);

  // dre 不是 DeclRefExpr
  if (dre == nullptr) {
    // 在子节点中递归寻找
    for (const Stmt *child : e->children()) {
      const Expr *cur = dyn_cast<Expr>(child);
      if (cur == nullptr)
        continue;
      rightDeclRefs(result, cur, sm);
    }
    return;
  }

  // dre 是 DeclRefExpr
  const VarDecl *vd = dyn_cast<VarDecl>(dre->getDecl());

  // dre 引用的可能不是变量声明 (而是函数声明等), 所以这里是确认 vd 是
  // VarDecl?
  if (vd == nullptr)
    return;
  // vd 是 arithmetic?
  if (!vd->getType()->isArithmeticType())
    return;

  result.push_back(dre);
}

class BoundCallbackAssi : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit BoundCallbackAssi(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {
    // 获取赋值表达式 =
    const BinaryOperator *assi = result.Nodes.getNodeAs<BinaryOperator>(id1);
    const SourceManager *sm = result.SourceManager;

    if (assi && sm) {
      // 保证是直到下一个分号为止最后一个 =
      if (!isLastEquals(assi, sm))
        return;

      // 获取赋值右侧的表达式并忽略隐式的类型转换
      const Expr *rhs = assi->getRHS()->IgnoreImpCasts();

      // 收集所有的符合要求的 DeclRefExpr
      vector<const DeclRefExpr *> valid;
      rightDeclRefs(valid, rhs, sm);

      for (const DeclRefExpr *bo : valid) {
        if (bo == nullptr)
          continue;

        function<void(Rewriter &)> f = [bo, sm](Rewriter &r) {
          modifyBound(bo, sm, r);
        };

        all.push_back(f);
      }
    }
  }
};

} // namespace

BoundSemantic::BoundSemantic(vector<function<void(Rewriter &)>> &all) {
  this->callbacks.push_back(
      unique_ptr<MatchFinder::MatchCallback>(new BoundCallbackComm(all)));
  this->callbacks.push_back(
      unique_ptr<MatchFinder::MatchCallback>(new BoundCallbackAssi(all)));

  // 寻找所有的数值类型变量使用, 并且如果上下文中有某些操作符时跳过
  // 如果有等号就忽略
  StatementMatcher m0 =
      declRefExpr(isExpansionInMainFile(),
                  to(anyOf(varDecl(hasType(realFloatingPointType())),
                           varDecl(hasType(isInteger())))),
                  unless(hasAncestor(unaryOperator(
                      hasAnyOperatorName("++", "--", "->", ".")))),
                  unless(hasAncestor(binaryOperator(hasOperatorName("=")))))
          .bind(id0);

  // 集中处理等号情况
  StatementMatcher m1 =
      binaryOperator(isExpansionInMainFile(), hasOperatorName("="),
                     unless(hasAncestor(unaryOperator(
                         hasAnyOperatorName("++", "--", "->", ".")))),
                     forEachDescendant(declRefExpr(
                         to(anyOf(varDecl(hasType(realFloatingPointType())),
                                  varDecl(hasType(isInteger())))))))
          .bind(id1);

  finder.addMatcher(m0, this->callbacks[0].get());
  finder.addMatcher(m1, this->callbacks[1].get());
}

void BoundSemantic::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string BoundSemantic::name() const { return "BoundSemantic"; }

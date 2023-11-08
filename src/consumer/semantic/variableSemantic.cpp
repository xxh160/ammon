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
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <util.h>
#include <vector>

using namespace clang;
using namespace clang::ast_matchers;
using namespace std;
using namespace llvm;

namespace {

const string id0 = "numberVariable";
const string id10 = "assignment";
const string id11 = "rightVariable";

inline void modifyBound(const DeclRefExpr *bo, const SourceManager *sm,
                        Rewriter &r) {
  // 注意 getEndLoc 可能返回代表范围结束的令牌的开始位置
  SourceLocation loc = bo->getEndLoc();
  // 返回紧随给定令牌之后的第一个字符的位置
  loc = Lexer::getLocForEndOfToken(loc, 0, *sm, r.getLangOpts());

  // 修改边界
  int num = randomNumber(INT32_MIN, INT32_MAX);
  string num_str = to_string(num);
  r.InsertTextAfter(loc, " + " + num_str);

  outs() << sm->getPresumedLineNumber(loc) << " "
         << sm->getPresumedColumnNumber(loc) << " VariableError\n";
}

class VariableCallbackComm : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit VariableCallbackComm(vector<function<void(Rewriter &)>> &all)
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
void printExpr(const Expr *expression, const SourceManager *sm,
               const LangOptions &langOpts) {
  if (!expression || !sm)
    return;

  SourceRange srcRange = expression->getSourceRange();
  SourceLocation start = srcRange.getBegin();
  SourceLocation end =
      Lexer::getLocForEndOfToken(srcRange.getEnd(), 0, *sm, langOpts);

  bool invalid;
  StringRef text = Lexer::getSourceText(
      CharSourceRange::getCharRange(start, end), *sm, langOpts, &invalid);

  if (!invalid) {
    INFO(text);
  } else {
    INFO("Unable to extract source text");
  }
}
#endif

void collectVarUses(vector<const DeclRefExpr *> &intVarUses, const Expr *e,
                    const LangOptions &opts, const SourceManager *sm) {
  if (!e)
    return;
#ifdef AMMON_DEBUG
  printExpr(e, sm, opts);
#endif
  const DeclRefExpr *dre = dyn_cast<DeclRefExpr>(e);

  if (dre == nullptr) {
    // 在子节点中递归寻找
    for (const Stmt *child : e->children()) {
      collectVarUses(intVarUses, dyn_cast<Expr>(child), opts, sm);
    }
    return;
  }

  // 如果当前是 DeclRefExpr 就加入 vector
  if (const auto *vd = dyn_cast<VarDecl>(dre->getDecl())) {
    if (vd->getType()->isIntegerType()) {
      intVarUses.push_back(dre);
    }
  }
}

class VariableCallbackAssi : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit VariableCallbackAssi(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {
    // 获取赋值表达式 =
    const BinaryOperator *assi = result.Nodes.getNodeAs<BinaryOperator>(id10);
    const SourceManager *sm = result.SourceManager;

    if (assi && sm) {
      // 获取赋值右侧的表达式并忽略隐式的类型转换
      const Expr *rhs = assi->getRHS()->IgnoreImpCasts();

      // 通过递归访问来收集所有的整型变量的 DeclRefExpr
      vector<const DeclRefExpr *> intVarUses;
      collectVarUses(intVarUses, rhs, result.Context->getLangOpts(), sm);

      for (const auto *bo : intVarUses) {
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

VariableSemantic::VariableSemantic(vector<function<void(Rewriter &)>> &all) {
  this->callbacks.push_back(
      unique_ptr<MatchFinder::MatchCallback>(new VariableCallbackComm(all)));
  this->callbacks.push_back(
      unique_ptr<MatchFinder::MatchCallback>(new VariableCallbackAssi(all)));

  // 寻找所有的 int 变量使用, 并且如果上下文中有某些操作符时跳过
  // 如果有等号就忽略
  StatementMatcher m0 =
      declRefExpr(isExpansionInMainFile(), to(varDecl(hasType(isInteger()))),
                  unless(hasAncestor(unaryOperator(
                      hasAnyOperatorName("++", "--", "->", ".")))),
                  unless(hasAncestor(binaryOperator(hasOperatorName("=")))))
          .bind(id0);

  // 集中处理等号情况
  StatementMatcher m1 =
      binaryOperator(
          isExpansionInMainFile(), hasOperatorName("="),
          forEachDescendant(
              declRefExpr(to(varDecl(hasType(isInteger())))).bind(id11)))
          .bind(id10);

  finder.addMatcher(m0, this->callbacks[0].get());
  finder.addMatcher(m1, this->callbacks[1].get());
}

void VariableSemantic::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string VariableSemantic::name() const { return "VariableSemantic"; }

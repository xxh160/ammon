#include <algorithm>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <consumer.h>
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

const string id = "parentheses";

class ParenCallback : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit ParenCallback(vector<function<void(Rewriter &)>> &all) : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {
    const Stmt *bo = result.Nodes.getNodeAs<Stmt>(id);
    const SourceManager *sm = result.SourceManager;

    if (bo && sm) {
      function<void(Rewriter &)> f = [bo, sm](Rewriter &r) {
        // 左括号位置 右括号位置
        SourceLocation lParenLoc, rParenLoc;

        if (const CallExpr *callExpr = dyn_cast<CallExpr>(bo)) {
          rParenLoc = callExpr->getRParenLoc();
          // 获取左括号的位置
          if (callExpr->getNumArgs() == 0) {
            // 如果没有参数
            lParenLoc = rParenLoc.getLocWithOffset(-1);
          } else {
            // 有参数，使用第一个参数位置
            lParenLoc = callExpr->getArg(0)->getBeginLoc().getLocWithOffset(-1);
            // 向左搜寻，以确保找到左括号的位置（考虑有空格和其他字符的情况）
            while (sm->getCharacterData(lParenLoc)[-1] != '(') {
              lParenLoc = lParenLoc.getLocWithOffset(-1);
            }
          }
        } else if (const ParenExpr *parenExpr = dyn_cast<ParenExpr>(bo)) {
          lParenLoc = parenExpr->getLParen();
          rParenLoc = parenExpr->getRParen();
        } else {
          return;
        }

        char c = '(';
        SourceLocation loc = lParenLoc;

        int i = randomNumber(0, 2);
        if (i == 0) {
          c = ')';
          loc = rParenLoc;
        }

        r.RemoveText(loc, 1);

        outs() << sm->getPresumedLineNumber(loc) << " "
               << sm->getPresumedColumnNumber(loc)
               << " InvalidParenthesesError " << c << "\n";
      };

      all.push_back(f);
    }
  }
};

} // namespace

ParenSyntax::ParenSyntax(vector<function<void(Rewriter &)>> &all) {
  this->callbacks.push_back(unique_ptr<MatchFinder::MatchCallback>(new ParenCallback(all)));

  // 所有括号表达式, 函数声明的括号
  StatementMatcher m = anyOf(callExpr(isExpansionInMainFile()).bind(id),
                             parenExpr(isExpansionInMainFile()).bind(id));

  finder.addMatcher(m, this->callbacks[0].get());
}

void ParenSyntax::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string ParenSyntax::name() const { return "ParenSyntax"; }

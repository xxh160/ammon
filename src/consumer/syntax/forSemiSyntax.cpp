#include <algorithm>
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/Stmt.h>
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

const string desc =
    "This bug is a syntax error where a comma is mistakenly used instead "
    "of a semicolon within a for loop";
const string id = "for";

class ForSemiCallback : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit ForSemiCallback(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {
    const ForStmt *bo = result.Nodes.getNodeAs<ForStmt>(id);
    SourceManager *sm = result.SourceManager;
    if (bo == nullptr) return;

    // 获取初始子句的结束位置，这是第一个分号的位置
    const Stmt *init = bo->getInit();
    // 获取条件语句的结束位置, 这是第二个分号的位置
    const Stmt *cond = bo->getCond();

    if ((init || cond)&&sm) {
      function<void(Rewriter &)> f = [init, cond, sm](Rewriter &r) {
        SourceLocation loc;

        int i = randomNumber(0, 2);

        if (init != nullptr)
          loc = init->getEndLoc();
        if (i == 0 && cond != nullptr)
          loc = cond->getEndLoc();

        // 使用Rewriter替换分号为逗号
        r.ReplaceText(loc, 1, ",");

        outs() << sm->getPresumedLineNumber(loc) << " "
               << sm->getPresumedColumnNumber(loc) << "\n"
               << desc << "\n";
      };

      all.push_back(f);
    }
  }
};

} // namespace

ForSemiSyntax::ForSemiSyntax(vector<function<void(Rewriter &)>> &all) {
  this->callbacks.push_back(
      unique_ptr<MatchFinder::MatchCallback>(new ForSemiCallback(all)));

  // 所有 for 语句
  StatementMatcher m = forStmt(isExpansionInMainFile()).bind(id);

  finder.addMatcher(m, this->callbacks[0].get());
}

void ForSemiSyntax::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string ForSemiSyntax::name() const { return "ForSemiSyntax"; }

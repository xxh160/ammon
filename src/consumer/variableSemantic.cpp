#include <algorithm>
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Lexer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <consumer.h>
#include <debug.h>
#include <functional>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <util.h>
#include <vector>

using namespace clang;
using namespace clang::ast_matchers;
using namespace std;

namespace {

const static string id = "numberVariable";

class VariableCallback : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit VariableCallback(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {
    const auto *bo = result.Nodes.getNodeAs<DeclRefExpr>(id);
    const SourceManager *sm = result.SourceManager;
    if (bo) {
      function<void(Rewriter &)> f = [bo, sm](Rewriter &r) {
        SourceLocation loc = bo->getEndLoc();
        loc = Lexer::getLocForEndOfToken(loc, 0, *sm, r.getLangOpts());
        r.InsertTextAfter(loc, " + 10086");

        llvm::outs() << sm->getPresumedLineNumber(loc) << " "
                     << sm->getPresumedColumnNumber(loc) << " VariableError\n";
      };

      all.push_back(f);
    }
  }
};

} // namespace

VariableSemantic::VariableSemantic(vector<function<void(Rewriter &)>> &all) {
  callback = unique_ptr<MatchFinder::MatchCallback>(new VariableCallback(all));

  // 寻找所有的 int 变量使用, 并且如果上下文中有某些操作符时跳过
  StatementMatcher m =
      declRefExpr(isExpansionInMainFile(), to(varDecl(hasType(isInteger()))),
                  unless(hasParent(unaryOperator(
                      hasAnyOperatorName("++", "--", "->", ".")))))
          .bind(id);

  finder.addMatcher(m, callback.get());
}

void VariableSemantic::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string VariableSemantic::name() const {
  return "VariableSemantic";
}

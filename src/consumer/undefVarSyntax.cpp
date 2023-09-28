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

const string id = "varUse";

class UndefVarCallback : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit UndefVarCallback(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {
    const DeclRefExpr *bo = result.Nodes.getNodeAs<DeclRefExpr>(id);
    const SourceManager *sm = result.SourceManager;

    if (bo && sm) {
      function<void(Rewriter &)> f = [bo, sm](Rewriter &r) {
        const NamedDecl *nd = dyn_cast<NamedDecl>(bo->getDecl());

        const string oldn = nd->getName().str();
        const string newn = oldn + randomStr(oldn.size());

        r.ReplaceText(bo->getSourceRange(), newn);
        SourceLocation loc = bo->getBeginLoc();

        outs() << sm->getPresumedLineNumber(loc) << " "
               << sm->getPresumedColumnNumber(loc) << " UndefinedVariableError " << oldn
               << " " << newn << "\n";
      };

      all.push_back(f);
    }
  }
};

} // namespace

UndefVarSyntax::UndefVarSyntax(vector<function<void(Rewriter &)>> &all) {
  callback = unique_ptr<MatchFinder::MatchCallback>(new UndefVarCallback(all));

  // 所有有声明的变量使用
  StatementMatcher m =
      declRefExpr(isExpansionInMainFile(), hasDeclaration(varDecl())).bind(id);
  finder.addMatcher(m, callback.get());
}

void UndefVarSyntax::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string UndefVarSyntax::name() const { return "UndefVarSyntax"; }

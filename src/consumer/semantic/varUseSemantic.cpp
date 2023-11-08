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

const string desc =
    "This bug is a semantic error where the variable <b> is mistakenly used in "
    "place of the intended variable <a> of the same type";
const string id = "numberOperand";

class VarUseCallback : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit VarUseCallback(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {}
};

} // namespace

VarUseSemantic::VarUseSemantic(vector<function<void(Rewriter &)>> &all) {
  this->callbacks.push_back(
      unique_ptr<MatchFinder::MatchCallback>(new VarUseCallback(all)));

  // 所有变量使用
  StatementMatcher m =
      declRefExpr(isExpansionInMainFile(), to(varDecl(hasType(isInteger()))))
          .bind(id);

  finder.addMatcher(m, this->callbacks[0].get());
}

void VarUseSemantic::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string VarUseSemantic::name() const { return "OperandSemantic"; }

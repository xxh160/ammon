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

const static string id = "numberOperand";

class OperandCallback : public MatchFinder::MatchCallback {
public:
  vector<function<void(Rewriter &)>> &all;

  explicit OperandCallback(vector<function<void(Rewriter &)>> &all)
      : all(all) {}

  void run(const MatchFinder::MatchResult &result) override {}
};

} // namespace

OperandSemantic::OperandSemantic(vector<function<void(Rewriter &)>> &all) {
  callback = unique_ptr<MatchFinder::MatchCallback>(new OperandCallback(all));

  // 所有变量使用
  StatementMatcher m =
      declRefExpr(isExpansionInMainFile(), to(varDecl(hasType(isInteger()))))
          .bind(id);
  finder.addMatcher(m, callback.get());
}

void OperandSemantic::HandleTranslationUnit(clang::ASTContext &context) {
  finder.matchAST(context);
}

string OperandSemantic::name() const {
  return "OperandSemantic";
}

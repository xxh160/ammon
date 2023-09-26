#include <clang/AST/ASTConsumer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <consumer.h>
#include <debug.h>
#include <llvm/Support/raw_ostream.h>
#include <util.h>

using namespace clang;
using namespace std;

namespace {

enum {
  OPERATOR_SEMANTIC = 0,
  VARIABLE_SEMANTIC,
  OPERAND_SEMANTIC,
  UNDEFVAR_SYNTAX,
  CONSUMER_COUNT
};

}

unique_ptr<ASTConsumer>
ASTConsumerFactory::randomASTConsumer(vector<function<void(Rewriter &)>> &all) {
#ifdef AMMON_DEBUG
  int index = UNDEFVAR_SYNTAX;
#else
  int index = randomNumber(0, CONSUMER_COUNT);
#endif
  CustomASTConsumer *res = nullptr;

  switch (index) {
  case OPERATOR_SEMANTIC:
    res = new OperatorSemantic(all);
    break;
  case VARIABLE_SEMANTIC:
    res = new VariableSemantic(all);
    break;
  case OPERAND_SEMANTIC:
    res = new OperandSemantic(all);
    break;
  case UNDEFVAR_SYNTAX:
    res = new UndefVarSyntax(all);
    break;
  default:
    ERROR("Random Error");
    break;
  }

#ifdef AMMON_DEBUG
  INFO(res->name());
#endif

  return unique_ptr<ASTConsumer>(res);
}

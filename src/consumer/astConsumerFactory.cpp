#include "exp.h"
#include <clang/AST/ASTConsumer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <consumer.h>
#include <debug.h>
#include <llvm/Support/raw_ostream.h>
#include <unordered_set>
#include <util.h>

using namespace clang;
using namespace std;

namespace {

enum {
  OPERATOR_SEMANTIC = 0,
  VARIABLE_SEMANTIC,
  OPERAND_SEMANTIC,
  UNDEFVAR_SYNTAX,
  FORSEMI_SYNTAX,
  PAREN_SYNTAX,
  CONSUMER_COUNT
};

unordered_set<int> blacklist;

} // namespace

unique_ptr<ASTConsumer>
ASTConsumerFactory::randomASTConsumer(vector<function<void(Rewriter &)>> &all) {
  // 没有可用的了
  if (blacklist.size() == CONSUMER_COUNT) {
    throw NoMoreConsumerExp();
  }

#ifdef AMMON_DEBUG
  int index = PAREN_SYNTAX;
#else
  int index = randomNumber(3, CONSUMER_COUNT);
#endif

  while (blacklist.find(index) != blacklist.end()) {
    index = randomNumber(3, CONSUMER_COUNT);
  }
  blacklist.insert(index);

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
  case FORSEMI_SYNTAX:
    res = new ForSemiSyntax(all);
    break;
  case PAREN_SYNTAX:
    res = new ParenSyntax(all);
    break;

  default:
    // 到不了这里
    break;
  }

#ifdef AMMON_DEBUG
  INFO(res->name());
#endif

  return unique_ptr<ASTConsumer>(res);
}

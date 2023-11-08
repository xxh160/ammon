#include "exp.h"
#include <clang/AST/ASTConsumer.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include <consumer.h>
#include <debug.h>
// #include <llvm/Support/raw_ostream.h>
#include <unordered_set>
#include <util.h>

using namespace clang;
using namespace std;

namespace {

enum {
  // 语义错误
  OPERATOR_SEMANTIC = 0,
  VARIABLE_SEMANTIC,
  OPERAND_SEMANTIC,
  // 语法错误
  UNDEFVAR_SYNTAX,
  FORSEMI_SYNTAX,
  PAREN_SYNTAX,
  CONSUMER_COUNT
};

unordered_set<int> blacklist;

} // namespace

unique_ptr<ASTConsumer>
ASTConsumerFactory::randomASTConsumer(vector<function<void(Rewriter &)>> &all) {
  int begin = 0;
  int end = 2;
  if (begin > end) ERROR("Random Consumer Error");

  // 没有可用的了
  if (blacklist.size() >= (size_t)(end - begin)) {
    throw NoMoreConsumerExp();
  }

#ifdef AMMON_DEBUG
  int index = VARIABLE_SEMANTIC;
#else
  int index = randomNumber(begin, end);
#endif

  while (blacklist.find(index) != blacklist.end()) {
    index = randomNumber(begin, end);
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

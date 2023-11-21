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

unique_ptr<ASTConsumer>
constructConsumer(vector<function<void(Rewriter &)>> &all, int type) {
  CustomASTConsumer *res = nullptr;

  switch (type) {
  case OPERATOR_SEMANTIC:
    res = new OperatorSemantic(all);
    break;
  case BOUND_SEMANTIC:
    res = new BoundSemantic(all);
    break;
  case VARUSE_SEMANTIC:
    res = new VarUseSemantic(all);
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
    ERROR("Consumer Construction Error");
    break;
  }

#ifdef AMMON_DEBUG
  INFO(res->name());
#endif

  return unique_ptr<ASTConsumer>(res);
}

unordered_set<int> blacklist;

} // namespace

// 如果不指定就 random 一个 ASTConsumer
unique_ptr<ASTConsumer>
ASTConsumerFactory::getASTConsumer(vector<function<void(Rewriter &)>> &all,
                                   int type) {
  // 有效的 type 输入
  if (isValidType(type)) {
    return constructConsumer(all, type);
  }

  // 自己 random 一个 ASTConsumer, 并且之后不 random 重复的
  int begin = OPERATOR_SEMANTIC;
  int end = BOUND_SEMANTIC + 1;

  if (begin > end)
    ERROR("Consumer Construction Error");

  // 没有可用的了
  if (blacklist.size() >= (size_t)(end - begin)) {
    throw NoMoreConsumerExp();
  }

  while (true) {
    type = randomNumber(begin, end);
    // 在 blacklist 中
    if (blacklist.find(type) != blacklist.end())
      continue;

    blacklist.insert(type);
    break;
  }

  return constructConsumer(all, type);
}


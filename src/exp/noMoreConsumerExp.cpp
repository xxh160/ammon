#include <exp.h>

const char *NoMoreConsumerExp::what() const noexcept {
  return "No more ASTConsumer available";
}

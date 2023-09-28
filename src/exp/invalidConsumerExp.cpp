#include <exp.h>

const char *InvalidConsumerExp::what() const noexcept {
  return "This ASTConsumer is Invalid";
}

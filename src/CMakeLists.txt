# 可执行文件路径
target_sources(ammon PRIVATE
  ./consumer/astConsumerFactory.cpp
  ./consumer/operatorSemantic.cpp
  ./consumer/variableSemantic.cpp
  ./consumer/operandSemantic.cpp
  ./consumer/undefVarSyntax.cpp
  ./util/rand.cpp
  ./main.cpp
)

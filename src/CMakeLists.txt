# 可执行文件路径
# 定义源文件的根目录变量
set(CONSUMER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/consumer/")
set(SEMANTIC_DIR "${CONSUMER_DIR}semantic/")
set(SYNTAX_DIR "${CONSUMER_DIR}syntax/")
set(EXP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/exp/")
set(UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/util/")

# 根据目录分组源文件
set(CONSUMER_COMMON_SOURCES
  ${CONSUMER_DIR}astConsumerFactory.cpp
)

set(CONSUMER_SEMANTIC_SOURCES
  ${SEMANTIC_DIR}operatorSemantic.cpp
  ${SEMANTIC_DIR}variableSemantic.cpp
  ${SEMANTIC_DIR}operandSemantic.cpp
)

set(CONSUMER_SYNTAX_SOURCES
  ${SYNTAX_DIR}undefVarSyntax.cpp
  ${SYNTAX_DIR}forSemiSyntax.cpp
  ${SYNTAX_DIR}parenSyntax.cpp
)

set(EXP_SOURCES
  ${EXP_DIR}invalidConsumerExp.cpp
  ${EXP_DIR}noMoreConsumerExp.cpp
)

set(UTIL_SOURCES
  ${UTIL_DIR}rand.cpp
)

# 主文件单独列出
set(MAIN_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

# 添加到目标
target_sources(ammon PRIVATE
  ${CONSUMER_COMMON_SOURCES}
  ${CONSUMER_SEMANTIC_SOURCES}
  ${CONSUMER_SYNTAX_SOURCES}
  ${EXP_SOURCES}
  ${UTIL_SOURCES}
  ${MAIN_SOURCE}
)

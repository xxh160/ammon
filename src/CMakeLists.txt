# 可执行文件路径
# 定义源文件的根目录变量
set(CONSUMER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/consumer/")

# 根据目录分组源文件
set(CONSUMER_COMMON_SOURCES
  ${CONSUMER_DIR}astConsumerFactory.cpp
)

set(CONSUMER_SEMANTIC_DIR "${CONSUMER_DIR}semantic/")
set(CONSUMER_SEMANTIC_SOURCES
  ${CONSUMER_SEMANTIC_DIR}operatorSemantic.cpp
  ${CONSUMER_SEMANTIC_DIR}boundSemantic.cpp
  ${CONSUMER_SEMANTIC_DIR}varUseSemantic.cpp
)

set(CONSUMER_SYNTAX_DIR "${CONSUMER_DIR}syntax/")
set(CONSUMER_SYNTAX_SOURCES
  ${CONSUMER_SYNTAX_DIR}undefVarSyntax.cpp
  ${CONSUMER_SYNTAX_DIR}forSemiSyntax.cpp
  ${CONSUMER_SYNTAX_DIR}parenSyntax.cpp
)

set(EXP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/exp/")
set(EXP_SOURCES
  ${EXP_DIR}invalidConsumerExp.cpp
  ${EXP_DIR}noMoreConsumerExp.cpp
)

set(UTIL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/util/")
set(UTIL_SOURCES
  ${UTIL_DIR}rand.cpp
)

# 主文件
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

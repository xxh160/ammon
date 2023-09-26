# cmake 版本
cmake_minimum_required(VERSION 3.25)

# 工程名
project(ammon VERSION "1.0")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 添加编译选项
add_compile_options(-fno-rtti
  -Wall
  -fno-omit-frame-pointer
  -g
)

# 添加宏定义
option(AMMON_DEBUG "Enable ammon debug outputs" off)

if (AMMON_DEBUG)
  add_compile_definitions(AMMON_DEBUG=1)
endif()

# 添加链接选项
# add_link_options()

# 引入 llvm clang 依赖
find_package(LLVM REQUIRED CONFIG
  HINTS "../llvm-project/build-release-static/lib/cmake/llvm"
)
message(STATUS "Found LLVM: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVM: ${LLVM_CMAKE_DIR}")
find_package(Clang REQUIRED CONFIG
  HINTS "../llvm-project/build-release-static/lib/cmake/clang"
)

# 设置包含路径
include_directories(SYSTEM "${LLVM_INCLUDE_DIRS};${CLANG_INCLUDE_DIRS}")
include_directories(./include)

# 可执行文件
add_executable(ammon)

# 源代码路径
add_subdirectory(src)

# 链接库
target_link_libraries(ammon PRIVATE
  clang-cpp
  # clangAST
)

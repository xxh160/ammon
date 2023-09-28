# Ammon 

## 语法错误

对于语法和语义正确的代码, 插入以下类型的缺陷:

- 未定义变量错误: 使用了未定义的变量, 现实情况可能是打错了变量名. 随机选择一个变量使用, 将它改成一个没有出现过的变量名.
- For 循环逗号错误: for 循环中分号写成逗号. 找到 for 循环将任意一个分号改成逗号.
- 无效括号错误: 括号不完整. 将随机一个左括号或者右括号删除.

## 语义错误

对于语法和语义正确的代码, 插入以下类型的缺陷:

- 变量使用错误: 随机挑选类型 `T` 变量 `a`, 并替换为已声明的类型 `T` 变量 `b`.
- 操作数错误: 挑选随机类型为数值类型的变量 `a`, 在它前后插入 +/- 1 的操作.
- 操作符错误: 挑选随机操作符, 将它修改成任意其它操作符.

## 输出格式

``` txt
==== Start file_name ====
error_row error_column error_type [...] # 可能有额外的信息
modified code # 可能有很多行
==== End ====
```

## LLVM

编译命令, 以 Debug 模式为例:

``` shell
cd llvm-project
mkdir build-debug
cd build-debug

cmake -DLLVM_ENABLE_PROJECTS=clang
    -DCMAKE_BUILD_TYPE=Debug
    -DLLVM_APPEND_VC_REV=Off
    -DLLVM_LINK_LLVM_DYLIB=On
    -DLLVM_OPTIMIZED_TABLEGEN=On
    -DLLVM_CCACHE_BUILD=TRUE
    -DLLVM_USE_SPLIT_DWARF=On
    -DLLVM_TARGETS_TO_BUILD="host;"
    -G "Unix Makefiles"
    ../llvm
```

目前来看, 只要不将 llvm 编译成动态库, 就不会出现依赖缺失的问题.


# Ammon 

请忽略糟糕的架构设计...

## 目前问题

* [ ] 语义错误 `boundSemantic.cpp` **偶尔**会莫名其妙修改 `string` 等, 而且手动完全无法复现, 可能要在连续多次执行才可能触发
* [ ] 语义错误 `varUseSemantic.cpp` 尚未完成
* [ ] Leetcode 有自己预定义的数据结构, 这些都没定义 
* [ ] 语义错误 `operatorSemantic.cpp` 替换的操作符可能会出错, 主要问题出在操作符重载那里

## 语法错误

对于语法和语义正确的代码, 插入以下类型的缺陷:

- 未定义变量错误: 使用了未定义的变量, 现实情况可能是打错了变量名. 随机选择一个变量使用, 将它改成一个没有出现过的变量名.
- For 循环逗号错误: for 循环中分号写成逗号. 找到 for 循环将任意一个分号改成逗号.
- 无效括号错误: 括号不完整. 将随机一个左括号或者右括号删除.

## 语义错误

对于语法和语义正确的代码, 插入以下类型的缺陷:

- 变量使用错误: 某个该用变量 `a` 的地方用成了同一类型的变量 `b`. 随机挑选类型 `T` 变量 `a`, 并替换为已声明的类型 `T` 变量 `b`.
- 边界错误: 对数值进行了不正确的增加或减少，从而导致变量值与预期不符. 挑选随机类型为数值类型的变量 `a`, 在它后边插入 +/- number 的操作.
- 操作符错误: 使用了错误的操作符. 挑选随机操作符, 将它修改成任意其它操作符.

## 输出格式

``` txt
==== Start file_name ====
error_row error_column [...] # 可能有额外的信息
error_description # 一行
modified code # 可能有很多行
==== End ====
```

## LLVM

编译命令, 以 `Release` 模式为例:

``` shell
cd llvm-project
mkdir build-debug
cd build-debug

cmake -DLLVM_ENABLE_PROJECTS=clang \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_APPEND_VC_REV=Off \
    -DLLVM_LINK_LLVM_DYLIB=On \
    -DLLVM_OPTIMIZED_TABLEGEN=On \
    -DLLVM_CCACHE_BUILD=TRUE \
    -DLLVM_USE_SPLIT_DWARF=On \
    -DLLVM_TARGETS_TO_BUILD="host;" \
    -G "Unix Makefiles" \
    ../llvm
```

目前来看, 只要不将 llvm 编译成动态库, 就不会出现依赖缺失的问题.

注意编译类型如果是 `Debug` 的话运行会非常慢.


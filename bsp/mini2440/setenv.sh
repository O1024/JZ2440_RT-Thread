#!/bin/bash

# BSP板级代码目录
export BSP_ROOT="$PWD"

# RT代码路径
export RTT_ROOT="$PWD/../../"

# 交叉编译工具链路径
export RTT_EXEC_PATH="/opt/gcc-arm-none-eabi-10.3-2021.10/bin"

# 交叉编译工具链前缀
export RTT_CC_PREFIX="arm-none-eabi-"

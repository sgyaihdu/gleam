#!/bin/sh

# I2C 读取寄存器脚本，用于验证 BOE701 1080p60 初始化序列

# 配置参数
i2c_bus=5  # I2C 总线号，根据实际情况修改
i2c_addr=0x98 # I2C 设备地址，根据 BOE701 的地址修改
reg_width=2  # 寄存器地址宽度 (字节)
data_width=2 # 数据宽度 (字节), BOE701 通常是2字节写

# 预期的 I2C 命令数组，从你的 C 代码中提取
set -- \
"0x53,0x00,0,0x28" \
"0x51,0x00,0,0xFF" \
"0x51,0x01,0,0x01" \
"0x80,0x00,0,0x01" \
"0x80,0x01,0,0xF0" \
"0x80,0x02,0,0x87" \
"0x81,0x00,0,0x03" \
"0x81,0x01,0,0x28" \
"0x81,0x02,0,0x00" \
"0x81,0x03,0,0x0C" \
"0x81,0x04,0,0x00" \
"0x81,0x05,0,0x08" \
"0x81,0x06,0,0x00" \
"0x82,0x00,0,0x03" \
"0x82,0x01,0,0x28" \
"0x82,0x02,0,0x00" \
"0x82,0x03,0,0x0C" \
"0x82,0x04,0,0x00" \
"0x82,0x05,0,0x08" \
"0x82,0x06,0,0x00" \
"0x35,0x00,0,0x00" \
"0xFF,0x00,0,0x5A" \
"0xFF,0x01,0,0x80" \
"0x65,0x00,0,0x12" \
"0xF9,0x12,0,0x1E" \
"0xFF,0x00,0,0x5A" \
"0xFF,0x01,0,0x81" \
"0xF4,0x00,0,0x0C" \
"0x25,0x00,1,0x01" \
"0x26,0x00,1,0x01" \
"0x11,0x00,1,0x00" \
"0x29,0x00,1,0x00" \
"0x0A,0x00,0,0x00"


# 循环执行每个 I2C 命令
for cmd in "$@"; do
    # echo "$cmd" | IFS=',' read -r reg_addr_high reg_addr_low ignore expected_value

    # 使用 while 循环和 case 语句解析命令字符串，避免空变量问题
    reg_addr_high=""
    reg_addr_low=""
    ignore=""
    expected_value=""

    while [ -n "$cmd" ]; do
        case "$cmd" in
            *,*)  val=$(echo "$cmd" | cut -d ',' -f 1)
                  cmd=$(echo "$cmd" | cut -d ',' -f 2-)
                  ;;
            *)   val="$cmd"
                 cmd=""
                 ;;
        esac

        # 根据字段顺序赋值
        if [ -z "$reg_addr_high" ]; then
            reg_addr_high="$val"
        elif [ -z "$reg_addr_low" ]; then
            reg_addr_low="$val"
        elif [ -z "$ignore" ]; then
            ignore="$val"
        elif [ -z "$expected_value" ]; then
            expected_value="$val"
        fi
    done
    # 组合寄存器地址
    reg_addr=$(printf "0x%02X%02X" "$reg_addr_high" "$reg_addr_low")

    # 检查是否需要跳过读取
    if [ "$ignore" = "1" ] && [ "$expected_value" = "0x00" ]; then  # 注意：sh 中使用单个等号进行比较
        echo "$reg_addr: write_null"
        continue
    fi

    # 执行读取命令
    output=$(i2c_read "$i2c_bus" "$i2c_addr" "$reg_addr" "$reg_addr" "$reg_width" "$data_width" 2>/dev/null)

    # 检查命令执行结果
    if [ $? -ne 0 ]; then
        echo "读取寄存器 $reg_addr 失败！请检查 I2C 连接和设备地址。"
        continue
    fi

    # 提取读取值
    read_value=$(echo "$output" | grep "$reg_addr:" | awk '{print $2}')

    # 检查读取值是否为空
    if [ -z "$read_value" ]; then
        echo "读取寄存器 $reg_addr 失败！未能获取返回值."
        continue
    fi

    # 将读取值和预期值转换为整数
    read_value_int=$(printf "%d" "$read_value")
    expected_value_int=$(printf "%d" "$expected_value")


    # 使用数值比较
    if [ "$read_value_int" -eq "$expected_value_int" ]; then
        echo "$reg_addr: 读取值 $read_value 与预期值 $expected_value 匹配."
    else
        echo "$reg_addr: 读取值 $read_value 与预期值 $expected_value 不匹配! 请检查."
    fi

    # # 处理延迟，如果ignore字段不为0, 则表示需要延迟
    # if [[ "$ignore" != "0" ]]; then
    #   if [[ "$expected_value" == "0x00" ]]; then
    #       usleep 100000 # 100ms 延迟
    #   fi
    # fi

done

echo "[END]"

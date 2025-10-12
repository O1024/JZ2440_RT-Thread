#include <s3c24x0.h>

#define GPx4_UP_DISABLE (1 << 4)
#define GPx5_UP_DISABLE (1 << 5)
#define GPx6_UP_DISABLE (1 << 6)
#define GPx7_UP_DISABLE (1 << 7)
#define GPx4_OUT        (0x01 << (4 * 2))
#define GPx5_OUT        (0x01 << (5 * 2))
#define GPx6_OUT        (0x01 << (6 * 2))

int  led_main() __attribute__((section(".boot_early")));
void mem_setup() __attribute__((section(".boot_early")));

int led_main()
{
    // GPF4/5/6引脚连接LED1/2/4，当输出低电平时LED点亮，同时作为输出引脚时关闭内部上拉
    GPFUP  |= (GPx4_UP_DISABLE | GPx5_UP_DISABLE | GPx6_UP_DISABLE);
    GPFCON |= (GPx4_OUT | GPx5_OUT | GPx6_OUT);

    while (1)
    {
        // 定义LED引脚掩码
        const unsigned int LED_MASK = (1 << 4) | (1 << 5) | (1 << 6);
        const unsigned int LED1     = 1 << 4;
        const unsigned int LED2     = 1 << 5;
        const unsigned int LED3     = 1 << 6;

        // LED1点亮，其他熄灭
        GPFDAT = (GPFDAT | LED_MASK) & ~LED1;
        for (volatile int i = 0; i < 10000; i++); // 延时

        // LED2点亮，其他熄灭
        GPFDAT = (GPFDAT | LED_MASK) & ~LED2;
        for (volatile int i = 0; i < 10000; i++); // 延时

        // LED3点亮，其他熄灭
        GPFDAT = (GPFDAT | LED_MASK) & ~LED3;
        for (volatile int i = 0; i < 10000; i++); // 延时
    }

    return 0;
}

/* 设置控制SDRAM的13个寄存器 */
void mem_setup()
{
    BWSCON   = 0x22011110;
    BANKCON0 = 0x00000700;
    BANKCON1 = 0x00000700;
    BANKCON2 = 0x00000700;
    BANKCON3 = 0x00000700;
    BANKCON4 = 0x00000700;
    BANKCON5 = 0x00000700;
    BANKCON6 = 0x00018005;
    BANKCON7 = 0x00018005;
    REFRESH  = 0x008C07A3;
    BANKSIZE = 0x000000B1;
    MRSRB6   = 0x00000030;
    MRSRB7   = 0x00000030;

    return;
}

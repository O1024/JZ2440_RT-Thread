#include <s3c24x0.h>

#define GPx4_UP_DISABLE (1 << 4)
#define GPx5_UP_DISABLE (1 << 5)
#define GPx6_UP_DISABLE (1 << 6)
#define GPx7_UP_DISABLE (1 << 7)
#define GPx4_OUT        (0x01 << (4 * 2))
#define GPx5_OUT        (0x01 << (5 * 2))
#define GPx6_OUT        (0x01 << (6 * 2))

#define GSTATUS1 (*(volatile unsigned int *)0x560000B0)
#define BUSY     1

#define NAND_SECTOR_SIZE 512
#define NAND_BLOCK_MASK  (NAND_SECTOR_SIZE - 1)

#define NAND_SECTOR_SIZE_LP 2048
#define NAND_BLOCK_MASK_LP  (NAND_SECTOR_SIZE_LP - 1)

void mem_setup() __attribute__((section(".boot_early")));

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

int           nand_read() __attribute__((section(".boot_early")));
int           nand_init() __attribute__((section(".boot_early")));
int           nand_reset() __attribute__((section(".boot_early")));
void          nand_select(void) __attribute__((section(".boot_early")));
void          nand_deselect(void) __attribute__((section(".boot_early")));
void          nand_cmd(unsigned char cmd) __attribute__((section(".boot_early")));
void          nand_addr_byte(unsigned char addr) __attribute__((section(".boot_early")));
unsigned char nand_data(void) __attribute__((section(".boot_early")));

void nand_select(void)
{
    NFCONT &= ~(1 << 1);

    return;
}

/*禁止片选*/
void nand_deselect(void)
{
    NFCONT |= (1 << 1);

    return;
}

//发送命令
void nand_cmd(unsigned char cmd)
{
    volatile int i;
    NFCMD = cmd;
    for (i = 0; i < 10; i++); //延时是为了保证数据的稳定

    return;
}

//发送地址
void nand_addr_byte(unsigned char addr)
{
    volatile int i;
    NFADDR = addr;
    while (!(NFSTAT & (1 << 0)));
}

//读取1字节的数据
unsigned char nand_data(void)
{
    return NFDATA;
}

int nand_reset()
{
    int i;

    nand_select();

    nand_cmd(0xff);

    while (!(NFSTAT & BUSY))
        for (i = 0; i < 10; i++);

    /* 取消片选信号 */
    nand_deselect();

    return 0;
}

int nand_init()
{
#define TACLS  0
#define TWRPH0 3
#define TWRPH1 0

    /* 设置时序 */
    NFCONF = (TACLS << 12) | (TWRPH0 << 8) | (TWRPH1 << 4);
    /* 使能NAND Flash控制器, 初始化ECC, 禁止片选 */
    NFCONT = (1 << 4) | (1 << 1) | (1 << 0);

    nand_reset();

    return 0;
}

int nand_read_id()
{
    nand_select();
    nand_cmd(0x90);
    nand_addr_byte(0x00);

    *(unsigned int *)0x30000000 = 0;
    *(unsigned int *)0x30000004 = 0;

    *(unsigned int *)0x30000000 = nand_data();
    *(unsigned int *)0x30000004 = nand_data();

    /* 取消片选信号 */
    nand_deselect();

    return 0;
}


int nand_read()
{
    int            i, j, x;
    int            col, page;
    unsigned long  start_addr = 0;
    unsigned int   size       = 0x40000;
    unsigned char *buf        = 0x30000000;

    if ((start_addr & NAND_BLOCK_MASK_LP) || (size & NAND_BLOCK_MASK_LP))
    {
        return 1; /* 地址或长度不对齐 */
    }

    /* 选中芯片 */
    nand_select();

    for (i = start_addr; i < (start_addr + size);)
    {
        /* 发出READ0命令 */
        nand_cmd(0);

        /* Write Address */
        // write_addr(i);
        col  = i & NAND_BLOCK_MASK_LP;
        page = i / NAND_SECTOR_SIZE_LP;

        NFADDR = col & 0xff;          /* Column Address A0~A7 */
        for (x = 0; x < 10; x++);
        NFADDR = (col >> 8) & 0x0f;   /* Column Address A8~A11 */
        for (x = 0; x < 10; x++);
        NFADDR = page & 0xff;         /* Row Address A12~A19 */
        for (x = 0; x < 10; x++);
        NFADDR = (page >> 8) & 0xff;  /* Row Address A20~A27 */
        for (x = 0; x < 10; x++);
        NFADDR = (page >> 16) & 0x03; /* Row Address A28~A29 */
        for (x = 0; x < 10; x++);

        nand_cmd(0x30);

        while (!(NFSTAT & BUSY))
            for (x = 0; x < 10; x++);

        for (j = 0; j < NAND_SECTOR_SIZE_LP; j++, i++)
        {
            *buf = nand_data();
            buf++;
        }
    }

    /* 取消片选信号 */
    nand_deselect();

    return 0;
}

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

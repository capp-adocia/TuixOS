// init/main.c 实现C内核

void _start(void);
void kernel_main(void);

/**
 * 在指定位置输出一个字符
 * @param c 要输出的字符
 * @param row 行位置 (0-24)
 * @param col 列位置 (0-79)
 */
static void put_char(char c, int row, int col)
{
    char* video = (char*)0xB8000 + (row * 80 + col) * 2;
    video[0] = c;
    video[1] = 0x0E;
}
/**
 * 计算字符串长度
 * @param str 要计算长度的字符串
 */
static int strlen(const char* str)
{
    int len = 0;
    while(str[len] != '\0') len++;
    return len;
}

/**
 * 整型转为字符串
 * @param num 待转换的整数
 * @param buffer 输出字符串
 */
static inline void int_to_str(int num, char* buffer)
{
    if (num < 10) {
        buffer[0] = '0' + num;
        buffer[1] = '\0';
    } else if (num < 100) {
        buffer[0] = '0' + (num / 10);
        buffer[1] = '0' + (num % 10);
        buffer[2] = '\0';
    } else {
        // 处理三位数
        buffer[0] = '0' + (num / 100);
        buffer[1] = '0' + ((num / 10) % 10);
        buffer[2] = '0' + (num % 10);
        buffer[3] = '\0';
    }
}

/**
 * 在指定位置输出一个字符串
 * @param str 要输出的字符串
 * @param row 行位置 (0-24)
 * @param col 列位置 (0-79)
 */
static inline void kprint(char* str, int row, int col)
{
    for(int i = 0; str[i] != '\0'; i++)
    {
        put_char(str[i], row, col + i);
    }
}

/**
 * 清屏函数
 */
void static inline clear_screen(void)
{
    char* video = (char*)0xB8000;
    for(int i = 0;i < 80 * 25 * 2;i += 2)
    {
        video[i] = ' ';
        video[i + 1] = 0x07;
    }
}

void _start(void)
{
    kernel_main();
}


void kernel_main(void) {
    clear_screen();
    
    const char* logo[] = {
        "H  H  Y   Y  DDD   RRRR    A   N   N  GGGG  EEEEE   A  ",
        "H  H   Y Y   D  D  R   R  A A  NN  N G      E      A A ",
        "HHHH    Y    D  D  RRRR  AAAAA N N N G  GGG EEEE  AAAAA",
        "H  H    Y    D  D  R R   A   A N  NN G   GG E     A   A",
        "H  H    Y    DDD   R  RR A   A N   N  GGGG  EEEEE A   A"
    };
    
    int logo_height = 5;
    int logo_width = 60;
    
    int pos_x = 10, pos_y = 7;
    int old_x = 10, old_y = 7;
    int speed_x = 1, speed_y = 1;
    
    while(1) {
        // 清除旧位置（用空格覆盖）
        for (int i = 0; i < logo_height; i++) {
            for (int j = 0; j < logo_width; j++) {
                put_char(' ', old_y + i, old_x + j);
            }
        }
        
        // 保存旧位置
        old_x = pos_x;
        old_y = pos_y;
        
        // 显示Logo在新位置
        for (int i = 0; i < logo_height; i++) {
            kprint(logo[i], pos_y + i, pos_x);
        }
        
        // 显示固定版本信息
        kprint("HydrangeaOS v0.01", 23, 30);
        
        // 更新位置
        pos_x += speed_x;
        pos_y += speed_y;
        
        // 边界检测和反弹
        if (pos_x <= 0 || pos_x + logo_width >= 80) {
            speed_x = -speed_x;
        }
        if (pos_y <= 0 || pos_y + logo_height >= 25) {
            speed_y = -speed_y;
        }
        
        // 边界限制
        if (pos_x < 0) pos_x = 0;
        if (pos_x + logo_width >= 80) pos_x = 80 - logo_width - 1;
        if (pos_y < 0) pos_y = 0;
        if (pos_y + logo_height >= 25) pos_y = 25 - logo_height - 1;
        
        // 延时控制速度
        for (volatile int i = 0; i < 80000000; i++);
    }
}
/* driver/screen.c */

#include <Silan/screen.h>

void put_char(const char c, int row, int col)
{
    if (row < 0 || row >= 25 || col < 0 || col >= 80)
    {
        return; // 忽略越界访问
    }
    char *video = (char *)0xB8000 + (row * 80 + col) * 2;
    video[0] = c;
    video[1] = 0x0E;
}

void kprintf(int row, int col, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char buffer[128]; // 更小的缓冲区测试
    int pos = 0;

    for (int i = 0; fmt[i] != '\0' && pos < 127; i++)
    {
        if (fmt[i] == '%' && fmt[i + 1] == 's')
        {
            char *str = va_arg(args, char *);
            i++; // 跳过 's'
            if (str)
            {
                for (int j = 0; str[j] != '\0' && pos < 127; j++)
                {
                    buffer[pos++] = str[j];
                }
            }
        }
        else if (fmt[i] == '%' && fmt[i + 1] == 'd')
        {
            int num = va_arg(args, int);
            i++; // 跳过 'd'
            // 简单数字转换
            if (num == 0)
            {
                if (pos < 127)
                    buffer[pos++] = '0';
            }
            else
            {
                char num_buf[16];
                int j = 0;
                while (num > 0 && j < 15)
                {
                    num_buf[j++] = '0' + (num % 10);
                    num /= 10;
                }
                while (j > 0 && pos < 127)
                {
                    buffer[pos++] = num_buf[--j];
                }
            }
        }
        else if (fmt[i] == '%' && fmt[i + 1] == 'x')
        {
            uint32_t num = va_arg(args, uint32_t);
            i++; // 跳过 'x'
            // 十六进制转换
            char hex_buf[9];
            for (int j = 7; j >= 0; j--)
            {
                int digit = (num >> (j * 4)) & 0xF;
                hex_buf[7 - j] = digit < 10 ? '0' + digit : 'A' + digit - 10;
            }
            hex_buf[8] = '\0';
            for (int j = 0; hex_buf[j] != '\0' && pos < 127; j++)
            {
                buffer[pos++] = hex_buf[j];
            }
        }
        else
        {
            buffer[pos++] = fmt[i];
        }
    }

    buffer[pos] = '\0';
    va_end(args);

    kprint(row, col, buffer);
}

void kprint(int row, int col, const char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        put_char(str[i], row, col + i);
    }
}

void clear_screen(void)
{
    char *video = (char *)0xB8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2)
    {
        video[i] = ' ';
        video[i + 1] = 0x07;
    }
}
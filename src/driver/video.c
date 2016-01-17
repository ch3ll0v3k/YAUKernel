#include <driver/video.h>
#include <sys/asm.h>
#include <libc/string.h>
#include <stdarg.h>

unsigned int line = 0;
unsigned int column = 0;

void video_move_cursor()
{
    unsigned short c_pos = line * LINE_SIZE + column;
    outb(0x3D4, 0x0F);
    outb(0x3D5, c_pos);
    outb(0x3D4, 0x0E);
    outb(0x3D5, c_pos >> 8);
}

void video_line_clean(unsigned int line)
{
    char *video = SCREEN(line, 0);
    int i;

    for(i=0; i<LINE_SIZE*2; i += 2)
    {
        video[i] = ' ';
        video[i+1] = CURSOR_COLOR;
    }
}

void video_copy_line(int d, int s)
{
    char *dest = SCREEN(d, 0);
    char *source = SCREEN(s, 0);
    int i;

    for(i=0; i<LINE_SIZE*2; i++)
    {
        dest[i] = source[i];
    }

}

void video_screen_clean()
{
    for(line=0; line<LINE_COUNT; line++)
    {
        video_line_clean(line);
    }

    line = 0;
    column = 0;
}

void scroll(unsigned int n)
{
    int source = n;
    int dest = 0;

    while(source < LINE_COUNT)
    {
        video_line_clean(dest);
        video_copy_line(dest, source);
        video_line_clean(source);
        source++;
        dest++;
    }

    line = line - n;
    if(line < 0)
    {
        line = 0;
    }
}

void video_putchar_color(char c, char color)
{
    char *video = CURRENT_SCREEN();

    if(c == '\n')
    {
        line++;
        column = 0;
    }
    else if(c == '\t')
    {
        column = column + 8 - (column%8);
    }
    else if(c == '\r')
    {
        column = 0;
    }
    else if (c == '\b') // backspace
    {
        video -= 2;
        video[0] = ' ';
        video[1] = CURSOR_COLOR;
        column--;
    } else {
        video[0] = c;
        video[1] = color;
        column++;
    }

    if(column == LINE_SIZE)
    {
        line++;
        column = 0;
    }

    if(line == LINE_COUNT)
    {
        scroll(1);
    }

    video_move_cursor();
}

void video_print_number_color(unsigned int number, unsigned int base, char color)
{
    char chars[] = "0123456789abcdef";

    if(base > 16)
    {
        base = 16;
    }

    if(number < base)
    {
        video_putchar_color(chars[number], color);
    } else {
        video_print_number_color(number/base, base, color);
        video_putchar_color(chars[number%base], color);
    }

}

void video_print_number(unsigned int number, unsigned int base)
{
    video_print_number_color(number, base, COLOR(WHITE, BLACK));
}

void video_putchar(char c)
{
    video_putchar_color(c, COLOR(WHITE, BLACK));
}

void video_print_color(char *str, char color)
{
    char *current = str;

    while(*current != '\0')
    {
        video_putchar_color(*current, color);
        current++;
    }
}

void video_print(char *str)
{
    video_print_color(str, COLOR(WHITE, BLACK));
}

void video_printf(char *s, ...)
{
    va_list ap;
    char buffer[32];
    char c;
    int int_val;
    unsigned int unsigned_int_val;
    int negative;

    va_start(ap, s);

    while((c = *s++) != 0)
    {
        negative = 0;

        if(c == 0)
        {
            break;
        }
        else if(c == '%')
        {
            c = *s++;

            if(c == 'd')
            {
                int_val = va_arg(ap, int);
                if(int_val < 0)
                {
                    unsigned_int_val = -int_val;
                    negative = 1;
                } else {
                    unsigned_int_val = int_val;
                }

                itoa(buffer, unsigned_int_val, 10);
                if(negative)
                {
                    video_print("-");
                }
                video_print(buffer);
            }
            else if(c == 'u')
            {
                unsigned_int_val = va_arg(ap, int);
                itoa(buffer, unsigned_int_val, 10);
                video_print(buffer);
            }
            else if(c == 'x')
            {
                unsigned_int_val = va_arg(ap, int);
                itoa(buffer, unsigned_int_val, 16);
                video_printf("%s", buffer);
            }
            else if(c == 'p')
            {
                video_printf("0x%x", va_arg(ap, int));
            }
            else if(c == 's')
            {
                video_print((char *) va_arg(ap, int));
            }

        } else {
            video_putchar(c);
        }
    }

}

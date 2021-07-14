#ifndef CURTAIN_H_MACROS
#define CURTAIN_H_MACROS

#ifndef IN
    #define IN
#endif
#ifndef OUT
    #define OUT
#endif
#ifndef DEBUG
    #define DEBUG puts("\e[31;1mDEBUG\e[0m")
#endif
#ifndef ABS
    #define ABS(x) (((x) < 0) ? -x : x)
#endif
#ifndef MIN
    #define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef true
    #define true 1
#endif
#ifndef false
    #define false 0
#endif

#define C_TINY_BUFFER_SIZE 16
#define C_SMALL_BUFFER_SIZE 64
#define STRING_BLOCK_SIZE 32
#define C_BUFFER_SIZE 1024

#define NDEBUG(s) printf("\e[31mDEBUG\e[0m %s\n", s)

#define clearscr write(STDOUT_FILENO, "\e[2J\e[H", sizeof("\e[2J\e[H") - 1)
#define refreshscr write(STDOUT_FILENO, "\ec", sizeof("\ec") - 1)
#define hide_cursor write(STDOUT_FILENO, "\e[?25l", sizeof("\e[?25l") - 1)
#define show_cursor write(STDOUT_FILENO, "\e[?25h", sizeof("\e[?25h") - 1)

#define setcoord(coord, r, c) coord.row = r; coord.column = c
#define setcolor(c, red, green, blue) c.r = red; c.g = green; c.b = blue

#define is_bound_by(b1, b2, p) ((b1).column <= (p).column && (p).column <= (b2).column && (b1).row <= (p).row && (p).row <= (b2).row)

#define mssleep(s) usleep((s) * 1000)

#endif
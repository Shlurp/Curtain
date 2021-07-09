#include "curtain.h"

struct termios orig_attr = {0};

void disable_rawmode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_attr);
}

int enable_rawmode(){
    int error_check = 0;
    struct termios new_attr = {0};

    error_check = tcgetattr(STDIN_FILENO, &orig_attr);
    if(-1 == error_check){
        goto cleanup;
    }

    error_check = atexit(disable_rawmode);
    if(error_check != 0){
        error_check = -1;
        goto cleanup;
    }

    new_attr = orig_attr;

    new_attr.c_iflag &= ~(IXON);
    new_attr.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    new_attr.c_cc[VMIN] = 0;
    new_attr.c_cc[VTIME] = 1;

    error_check = tcsetattr(STDIN_FILENO, TCSANOW, &new_attr);

cleanup:
    return error_check;
}

void view_keypresses(){
    char c = 0;
    int n = 0;

    enable_rawmode();

    while(c != 'q'){
        n = read(STDIN_FILENO, &c, sizeof(c));
        if(n == 0){
            //write(STDOUT_FILENO, "Read\n", 5);
            continue;
        }
        if(c < ' '){
            printf("(%d)\n", c);
        }
        else{
            printf("(%d) %c\n", c, c);
        }
    }
}

int getwinsize(int * rows, int * cols){
    struct winsize size = {0};
    int error_check = 0;

    error_check = ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    if(-1 == error_check){
        goto cleanup;
    }

    *rows = size.ws_row;
    *cols = size.ws_col;

cleanup:
    return error_check;
}

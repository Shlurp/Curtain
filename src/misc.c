#include "curtain.h"

struct termios orig_attr = {0};

void disable_rawmode(){
    show_cursor;
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

// The majority of this code is courtesy of Kilo: https://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html
int scan_keypress(){
    char c = 0;
    int n = 0;
    int keypress = -1;
    char ctrl_seq[3] = {0};

    while(n != sizeof(char)){
        n = read(STDIN_FILENO, &c, sizeof(c));
        if(-1 == n && errno != EAGAIN){
            c = -1;
            goto cleanup;
        }
    }

    if('\e' == c){
        n = read(STDIN_FILENO, ctrl_seq, sizeof(char));
        if(n != sizeof(char)){
            keypress = c;
            goto cleanup;
        }

        n = read(STDIN_FILENO, ctrl_seq + 1, sizeof(char));
        if(n != sizeof(char)){
            keypress = c;
            goto cleanup;
        }

        if('[' == ctrl_seq[0]){
            if('0' < ctrl_seq[1] && ctrl_seq[1] < '9'){
                n = read(STDIN_FILENO, ctrl_seq + 2, sizeof(char));
                if(n != sizeof(char)){
                    keypress = c;
                    goto cleanup;
                }

                if('~' == ctrl_seq[2]){
                    switch(ctrl_seq[1]){
                        case '1': keypress = HOME; goto cleanup;
                        case '3': keypress = DELETE; goto cleanup;
                        case '4': keypress = END; goto cleanup;
                        case '5': keypress = PAGE_UP; goto cleanup;
                        case '6': keypress = PAGE_DOWN; goto cleanup;
                        case '7': keypress = HOME; goto cleanup;
                        case '8': keypress = END; goto cleanup;
                    }
                }
            }
            else{
                switch(ctrl_seq[1]){
                    case 'A': keypress = ARROW_UP; goto cleanup;
                    case 'B': keypress = ARROW_DOWN; goto cleanup;
                    case 'C': keypress = ARROW_RIGHT; goto cleanup;
                    case 'D': keypress = ARROW_LEFT; goto cleanup;
                    case 'H': keypress = HOME; goto cleanup;
                    case 'F': keypress = END; goto cleanup;
                }
            }
        }
        else if('O' == ctrl_seq[0]){
            switch (ctrl_seq[1]) {
                case 'H': keypress = HOME; goto cleanup;
                case 'F': keypress = END; goto cleanup;
            }
        }

        keypress = '\e';
    }
    else{
        keypress = c;
    }

cleanup:
    return keypress;
}

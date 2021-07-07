#include "curtain.h"

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

int getinput(textbox_t * textbox){
    int error_check = 0;
    int input = 0;
    char * text = NULL;
    FILE * fp = NULL;
    int len = 0;
    lines_t * lines = textbox->lines;

    fp = fopen("test.txt", "w+");

    while(error_check != -1){
        print_textbox(textbox);

        input = scan_keypress();
        if(input == textbox->enter_char){
            error_check = textbox->on_enter(lines);
            if(-1 == error_check || 0 == error_check){
                goto cleanup;
            }
        }
        
        if(KEYTYPES_BEGIN < input && input < KEYTYPES_END){
            switch(input){
                case ARROW_UP: error_check = relative_change_currline(lines, 0, -1, false); break;
                case ARROW_DOWN: error_check = relative_change_currline(lines, 0, 1, false); break;
                case ARROW_RIGHT: error_check = relative_change_currline(lines, 1, 0, true); break;
                case ARROW_LEFT: error_check = relative_change_currline(lines, -1, 0, true); break;
                case PAGE_UP: error_check = change_curr_line(lines, 0); break;
                case PAGE_DOWN: error_check = change_curr_line(lines, lines->num_lines-1); break;
                case HOME: lines->curr_index = 0; break;
                case END: lines->curr_index = lines->curr_line->text.length; break;
                case DELETE: error_check = remove_char_from_line(lines); break;
            }

            if(-1 == error_check){
                perror("Error");
                goto cleanup;
            }
        }
        else if(isprint(input)){
            error_check = insert_char_to_line(lines, input);
            if(-1 == error_check){
                perror("INSERT CHAR error");
            }
        }
        else if(iscntrl(input)){
            switch(input){
                case '\n':
                    error_check = insert_newline(lines); 
                    if(-1 == error_check){
                        perror("INSERT NEWLINE error");
                    }
                    break;
                case '\b': remove_char_from_line(lines); break;
                case 127: remove_char_from_line(lines); break;
                case 19: 
                    text = convert_lines_to_string(lines, &len);
                    fwrite(text, sizeof(char), len, fp);
                    free(text);
                    break;
                case 3: goto cleanup;
                default: break;
            }
        }
    }


cleanup:
    fclose(fp);
    free_lines(lines);
    return error_check;
}

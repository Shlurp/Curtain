#include "curtain.h"

int getinput(textbox_t * textbox){
    int error_check = 0;
    int input = 0;
    lines_t * lines = textbox->lines;

    show_cursor;

    while(error_check != -1){
        print_textbox(textbox);

        input = scan_keypress();
        if(input == textbox->enter_char){
            if(NULL == textbox->on_enter){
                error_check = 0;
                goto cleanup;
            }
            
            error_check = textbox->on_enter(lines, NULL);
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
                case 27: case 3: goto cleanup;
                default: break;
            }
        }
    }


cleanup:
    hide_cursor;
    return error_check;
}

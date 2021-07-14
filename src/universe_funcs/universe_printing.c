#include "curtain.h"

object_t * make_error_label(coordinate_t size){
    coordinate_t start = {size.row / 3, size.column / 3};
    coordinate_t end = {2 * size.row / 3, 2 * size.column / 3};
    color_t bg_color = {255, 0, 0};
    color_t fg_color = {255, 200, 200};

    return init_label("Error!", start, end, bg_color, fg_color);
}

void next_obj(universe_t * universe){
    if(universe->objects != NULL && (universe->num_interactables != 0 || (universe->num_interactables != 1 && !is_interactable(universe->curr_obj)))){
        if(BUTTON == universe->curr_obj->obj->object_type){
            universe->curr_obj->obj->obj.button.hover = false;
            universe->curr_obj->obj->reprint = true;
        }

        do{
            if(NULL == universe->curr_obj->prev){
                universe->curr_obj = universe->last_obj;
            }
            else{
                universe->curr_obj = universe->curr_obj->prev;
            }
        }while(!is_interactable(universe->curr_obj));

        if(BUTTON == universe->curr_obj->obj->object_type){
            universe->curr_obj->obj->obj.button.hover = true;
            universe->curr_obj->obj->reprint = true;
        }
    }
}

int run_universe(universe_t * universe){
    int error_check = 0;
    char input = 0;
    char buf[C_SMALL_BUFFER_SIZE] = {0};
    object_t * error_label = NULL;

    enable_rawmode();
    refreshscr;
    hide_cursor;

    while(true){
        error_check = print_universe(universe);
        
        if(-1 == error_check){
            error_label = make_error_label(universe->size);
            if(error_check != -1){
                print_obj(error_label);
            }

            goto cleanup;
        }

        if(universe->curr_obj != NULL && TEXTBOX == universe->curr_obj->obj->object_type){
            error_check = sprintf(buf, "\e[%i;%iH", universe->curr_obj->obj->obj.textbox.start.row, universe->curr_obj->obj->obj.textbox.start.column);
            write(STDOUT_FILENO, buf, error_check);
            show_cursor;
        }
        else{
            hide_cursor;
        }

        input = scan_keypress();
        switch(input){
            case '\t': 
                next_obj(universe); break;
            case '\n': 
                if(TEXTBOX == universe->curr_obj->obj->object_type){
                    error_check = getinput(&universe->curr_obj->obj->obj.textbox);
                    if(-1 == error_check){
                        error_label = make_error_label(universe->size);
                        if(error_check != -1){
                            print_obj(error_label);
                        }

                        goto cleanup;
                    }
                }
                else if(BUTTON == universe->curr_obj->obj->object_type && universe->curr_obj->obj->obj.button.on_click != NULL){
                    error_check = universe->curr_obj->obj->obj.button.on_click(NULL);
                    if(-1 == error_check){
                         error_label = make_error_label(universe->size);
                        if(error_check != -1){
                            print_obj(error_label);
                        }

                        goto cleanup;
                    }
                }
                break;
            case 'q': goto cleanup;
            default: break;
        }
    }

cleanup:
    if(error_label != NULL){
        free_obj(error_label);
    }

    return error_check;
}
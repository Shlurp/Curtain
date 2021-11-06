#include "curtain.h"

void free_obj(object_t * obj){
    switch(obj->object_type){
        case LABEL: free(obj->obj.label.text); break;
        case TEXTBOX: 
            free_lines(obj->obj.textbox.lines); 
            free(obj->obj.textbox.lines); 
            if(obj->obj.textbox.objs != NULL){
                free(obj->obj.textbox.objs); 
            }
            break;
        case BUTTON: 
            free(obj->obj.button.text); 
            if(obj->obj.button.objs != NULL){
                free(obj->obj.button.objs);
            }
            break;
        default: break;
    }

    free(obj);
}

int move_obj(universe_t * universe, object_t * object, coordinate_t move){
    int error_check = 0;

    if(!object_exists(universe, object)){
        error_check = -1;
        errno = EEXIST;
        goto cleanup;
    }

    switch(object->object_type){
        case RECT: object->obj.rectangle.start.column += move.column; object->obj.rectangle.start.row += move.row; break;
        case LABEL: object->obj.label.start.column += move.column; object->obj.label.start.row += move.row; break;
        case TEXTBOX: object->obj.textbox.start.column += move.column; object->obj.textbox.start.row += move.row; break;
        case BUTTON: object->obj.button.start.column += move.column; object->obj.button.start.row += move.row; break;
        default: error_check = -1; errno = EINVAL; goto cleanup;
    }

    universe->dirty = true;

cleanup:
    return error_check;
}

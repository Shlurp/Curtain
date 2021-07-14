#include "curtain.h"

void free_obj(object_t * obj){
    switch(obj->object_type){
        case LABEL: free(obj->obj.label.text); break;
        case TEXTBOX: free_lines(obj->obj.textbox.lines); free(obj->obj.textbox.lines); break;
        case BUTTON: free(obj->obj.button.text); break;
        default: break;
    }

    free(obj);
}
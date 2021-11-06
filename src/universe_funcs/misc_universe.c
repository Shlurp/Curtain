#include "curtain.h"

void free_universe(universe_t * universe){
    obj_node_t * curr_node = universe->objects;
    obj_node_t * next_node = NULL;

    while(curr_node != NULL){
        next_node = curr_node->next;
        free_obj(curr_node->obj);
        free(curr_node);
        curr_node = next_node;
    }
    
    free(universe);
}

void print_universe_data(universe_t * universe){
    obj_node_t * curr_node = universe->objects;

    while(curr_node != NULL){
        switch(curr_node->obj->object_type){
            case RECT: printf("Rect >>"); break;
            case LABEL: printf("Label >> "); break;
            case TEXTBOX: printf("Textbox >> "); break;
            case BUTTON: printf("Button (%s)>> ", (curr_node->obj->obj.button.hover) ? "true" : "false"); break;
            default: break;
        }

        curr_node = curr_node->next;
    }
}
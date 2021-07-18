#include "curtain.h"

universe_t * init_universe(){
    universe_t * universe = NULL;
    int error_check = 0;
    int i = 0;

    universe = calloc(sizeof(universe_t), 1);
    if(NULL == universe){
        goto cleanup;
    }

    error_check = getwinsize(&universe->size.row, &universe->size.column);
    if(-1 == error_check){
        free(universe);
        universe = NULL;
        goto cleanup;
    }

    /*universe->obj_map = calloc(universe->size.row, sizeof(obj_node_t **));
    if(NULL == universe->obj_map){
        error_check = -1;
        goto cleanup;
    }

    for(i=0; i<universe->size.row; i++){
        universe->obj_map[i] = calloc(universe->size.column, sizeof(obj_node_t *));
        if(NULL == universe->obj_map[i]){
            while(i >= 0){
                free(universe->obj_map[i]);
                i--;
            }
            free(universe->obj_map);
            error_check = -1;
            goto cleanup;
        }
    }

    universe->objects = NULL;
    universe->last_obj = NULL;
    universe->curr_obj = NULL;
    universe->num_interactables = 0;
    universe->dirty = 0;*/

cleanup:
    return universe;
}

bool object_exists(universe_t * universe, object_t * obj){
    obj_node_t * curr_node = universe->objects;
    bool exists = false;
    
    while(curr_node != NULL && !exists){
        if(curr_node->obj == obj){
            exists = true;
        }

        curr_node = curr_node->next;
    }

    return exists;
}

obj_node_t * init_node(object_t * obj, obj_node_t * next, obj_node_t * prev){
    obj_node_t * node = NULL;

    node = malloc(sizeof(obj_node_t));
    if(NULL == node){
        goto cleanup;
    }

    node->obj = obj;
    node->next = next;
    node->prev = prev;

cleanup:
    return node;
}

int add_object(universe_t * universe, object_t * obj){
    int error_check = 0;
    int i = 0;
    int j = 0;
    coordinate_t start = {0};
    coordinate_t end = {0};
    coordinate_t origin = {1, 1};
    obj_node_t * object_node = NULL;

    if(object_exists(universe, obj)){
        error_check = -1;
        errno = EEXIST;
        goto cleanup;
    }

    getcoords(obj, start, end);

    if(!is_bound_by(origin, universe->size, start) || !is_bound_by(origin, universe->size, end)){
        error_check = -1;
        errno = EADDRNOTAVAIL;
        goto cleanup;
    }

    object_node = init_node(obj, universe->objects, NULL);
    if(NULL == object_node){
        goto cleanup;
    }

    if(universe->objects != NULL){
        universe->objects->prev = object_node;
    }
    else{
        universe->curr_obj = object_node;
        if(BUTTON == universe->curr_obj->obj->object_type){
            universe->curr_obj->obj->obj.button.hover = true;
        }
    }

    universe->objects = object_node;
    if(NULL == universe->last_obj){
        universe->last_obj = object_node;
    }

    if(BUTTON == obj->object_type || TEXTBOX == obj->object_type){
        universe->num_interactables ++;
    }
#if 0
    /* Whelp, we're gonna need a lotta nodes, bye bye memory ): 
    I think a 2d array of linked lists is still more efficient than a 3d array though. Maybe it doesn't need to be doubly linked though. */
    switch(obj->object_type){
        case LABEL: start = obj->obj.label.start; end = obj->obj.label.end; break;
        case RECT: start = obj->obj.rectangle.start; end = obj->obj.rectangle.end; break;
        case TEXTBOX: start = obj->obj.textbox.start; end = obj->obj.textbox.end; break;
    }

    for(i=start.row-1; i<end.row; i++){
        for(j=start.column-1; j<end.column; j++){
            object_node = init_node(obj, universe->obj_map[i][j], NULL);
            if(NULL == object_node){
                goto cleanup;
            }

            if(universe->obj_map[i][j] != NULL){
                universe->obj_map[i][j]->prev = object_node;
            }
            universe->obj_map[i][j] = object_node;
        }
    }
#endif

cleanup:
    return error_check;
}

bool remove_object(universe_t * universe, object_t * object){
    obj_node_t * curr_node = universe->objects;
    bool found = false;

    while(curr_node != NULL){
        if(curr_node->obj == object){
            if(curr_node->prev != NULL){
                curr_node->prev->next = curr_node->next;
            }
            else{
                universe->objects = curr_node->next;
            }

            if(curr_node->next != NULL){
                curr_node->next->prev = curr_node->prev;
            }
            else{
                universe->last_obj = curr_node->prev;
            }

            if(curr_node == universe->curr_obj){
                if(curr_node->prev != NULL){
                    universe->curr_obj = curr_node->prev;
                }
                else{
                    universe->curr_obj = curr_node->next;
                }
            }

            if(is_interactable(curr_node)){
                universe->num_interactables --;
            }

            free_obj(curr_node->obj);
            free(curr_node);
            curr_node = NULL;

            found = true;
        }
        else{
            curr_node = curr_node->next;
        }
    }

    return found;
}
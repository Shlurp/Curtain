#include "curtain.h"

int init_string(OUT string_t * string){
    int error_check = 0;

    string->str = calloc(STRING_BLOCK_SIZE, sizeof(char));
    if(NULL == string->str){
        error_check = -1;
        goto cleanup;
    }

    string->length = 0;
    string->num_allocated_blocks = 1;

cleanup:
    return error_check;
}


/**
 * 
 */
int shift_string(int index, int amount, string_t * string){
    int error_check = 0;
    int i = 0;

    if(index > string->length){
        printf("INDEX: %i LENGTH: %i\n", index, string->length);
        error_check = -1;
        errno = EINVAL;
        goto cleanup;
    }

    if(amount > 0){
        if(string->length + amount >= string->num_allocated_blocks * STRING_BLOCK_SIZE){    // Allocate more space
            i = (string->length + amount - string->num_allocated_blocks * STRING_BLOCK_SIZE) / STRING_BLOCK_SIZE + 1 + string->num_allocated_blocks;
            string->str = realloc(string->str, i * STRING_BLOCK_SIZE);
            if(NULL == string->str){
                error_check = -1;
                goto cleanup;
            }
            memset(string->str + string->length, 0, i * STRING_BLOCK_SIZE - string->length);
            string->num_allocated_blocks = i;
        }
        for(i=string->length+amount-1; i>index+amount; i--){
            string->str[i] = string->str[i-amount];
        }
        memset(string->str + index, 0, amount);
    }
    else if(amount < 0){
        if(index + amount < -1){
            error_check = -1;
            errno = EINVAL;
            goto cleanup;
        }

        for(i=index; i<string->length + amount; i++){
            string->str[i] = string->str[i - amount];
        }
        memset(string->str + string->length + amount, 0, -amount);
    }

cleanup:
    return error_check;
}

/**
 * 
 */
int insert_char(char c, int index, string_t * string){
    int error_check = 0;

    error_check = shift_string(index, 1, string);
    if(-1 == error_check){
        printf("CHAR: %i %c\n", c, c);
        goto cleanup;
    }

    string->str[index] = c;
    string->length ++;

cleanup:
    return error_check;
}

int append_char(char c, string_t * string){
    int error_check = 0;

    if(string->length + 1 >= string->num_allocated_blocks * STRING_BLOCK_SIZE){
        string->num_allocated_blocks ++;
        string->str = realloc(string->str, string->num_allocated_blocks * STRING_BLOCK_SIZE);
        if(NULL == string->str){
            error_check = -1;
            goto cleanup;
        }
        memset(string->str+string->length, 0, string->num_allocated_blocks * STRING_BLOCK_SIZE - string->length);
    }

    string->str[string->length] = c;
    string->length ++;

cleanup:
    return error_check;
}

int insert_string(int index, string_t * dest, string_t * src){
    int error_check = 0;

    error_check = shift_string(index, src->length, dest);
    if(-1 == error_check){
        goto cleanup;
    }

    memcpy(dest->str + index, src->str, src->length);

    dest->length += src->length;

cleanup:
    return error_check;
}

int set_string(string_t * string, char * s){
    int error_check = 0;
    int slen = 0;

    slen = strnlen(s, C_BUFFER_SIZE);

    if(string->str != NULL){
        free(string->str);
    }

    string->str = calloc((slen / STRING_BLOCK_SIZE + 1) * STRING_BLOCK_SIZE, sizeof(char));
    if(NULL == string->str){
        error_check = -1;
        goto cleanup;
    }

    memcpy(string->str, s, slen);
    string->length = slen;
    string->num_allocated_blocks = slen / STRING_BLOCK_SIZE + 1;

cleanup:
    return error_check;
}

char * cslice_string(string_t * string, int index, int length){
    int len = length;
    char * slice = NULL;

    if(index >= string->length){
        goto cleanup;
    }
    if(index < 0){
        index = 0;
    }
    if(0 == string->length){
        goto cleanup;
    }
    if(index + len > string->length){
        len = string->length - index;
    }

    if(len > 0){
        slice = malloc(len + 1);
        if(NULL == slice){
            goto cleanup;
        }

        slice[len] = 0;

        memcpy(slice, string->str + index, len);
    }

cleanup:
    return slice;
}

string_t * slice_string(string_t * string, int index, int length){
    int error_check = 0;
    string_t * string_slice = NULL;
    char * slice = NULL;

    slice = cslice_string(string, index, length);

    string_slice = malloc(sizeof(string_t));
    if(NULL == string_slice){
        goto cleanup;
    }
    memset(string_slice, 0, sizeof(string_t));

    if(slice != NULL){
        error_check = set_string(string_slice, slice);
        if(-1 == error_check){
            string_slice = NULL;
        }
        free(slice);
    }
    else{
        error_check = init_string(string_slice);
        if(-1 == error_check){
            string_slice = NULL;
        }
    }

cleanup:
    return string_slice;
}

int move_slice(string_t * dest, string_t * src, int dest_index, int src_index, int length){
    int error_check = 0;
    string_t * slice = NULL;

    if(dest_index > dest->length || src_index + length > src->length){
        errno = EINVAL;
        error_check = -1;
        goto cleanup;
    }

    slice = slice_string(src, src_index, length);
    if(NULL == slice){
        error_check = -1;
        goto cleanup;
    }

    error_check = shift_string(src_index+length, -length, src);
    src->length -= length;
    
    error_check = insert_string(dest_index, dest, slice);
    if(-1 == error_check){
        goto cleanup;
    }

    free_string(slice);
    free(slice);

cleanup:
    return error_check;
}

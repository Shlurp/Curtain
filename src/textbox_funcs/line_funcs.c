#include "curtain.h"

int init_line(line_t ** line, line_t * next, line_t * prev){
    int error_check = 0;

    *line = malloc(sizeof(line_t));
    if(NULL == *line){
        error_check = -1;
        goto cleanup;
    }
    memset(*line, 0, sizeof(line_t));

    error_check = init_string(&(*line)->text);
    if(-1 == error_check){
        goto cleanup;
    }

    (*line)->next_line = next;
    (*line)->prev_line = prev;

cleanup:
    return error_check;
}

int init_lines(lines_t * lines, int num_above, int total_rows, int total_cols){
    int error_check = 0;

    if(num_above >= total_rows){
        errno = EINVAL;
        error_check = -1;
        goto cleanup;
    }

    error_check = init_line(&lines->first_line, NULL, NULL);
    if(-1 == error_check){
        goto cleanup;
    }

    lines->curr_line = lines->first_line;
    lines->num_lines = 1;
    lines->curr_line_no = 0;
    lines->curr_index = 0;
    lines->num_lines_above = num_above;
    lines->total_lines = total_rows;
    lines->total_cols = total_cols;

cleanup:
    return error_check;
}

line_t * get_line_no(lines_t * lines, int line_no){
    int i = 0;
    line_t * line = NULL;

    if(line_no >= lines->num_lines){
        errno = EACCES;
        goto cleanup;
    }

    if(ABS(line_no - lines->curr_line_no) >= line_no){       // Requested line number is further from current line then beginning
        line = lines->first_line;
        for(i=0; i<line_no; i++){
            line = line->next_line;
        }
    }
    else if(lines->curr_line_no <= line_no){
        line = lines->curr_line;
        for(i=lines->curr_line_no; i<line_no; i++){
            line = line->next_line;
        }
    }
    else{
        line = lines->curr_line;
        for(i=lines->curr_line_no; i>line_no; i--){
            line = line->prev_line;
        }
    }

cleanup:
    return line;
}

int add_newline(lines_t * lines){
    line_t * newline = NULL;
    int error_check = 0;

    error_check = init_line(&newline, lines->curr_line->next_line, lines->curr_line);
    if(-1 == error_check){
        goto cleanup;
    }

    if(newline->next_line != NULL){
        newline->next_line->prev_line = newline;
    }
    lines->curr_line->next_line = newline;
    lines->curr_line = newline;
    lines->curr_line_no ++;
    lines->num_lines ++;
    lines->curr_index = 0;

cleanup:
    return error_check;
}

int insert_newline(lines_t * lines){
    int error_check = 0;
    bool move = true;
    int curr_index = lines->curr_index;

    if(lines->curr_index >= lines->curr_line->text.length){
        move = false;
    }

    error_check = add_newline(lines);
    if(-1 == error_check){
        goto cleanup;
    }

    if(move){
        error_check = move_slice(&lines->curr_line->text, &lines->curr_line->prev_line->text, 0, curr_index, lines->curr_line->prev_line->text.length - curr_index);
        if(-1 == error_check){
            goto cleanup;
        }
        
        lines->curr_index = lines->curr_line->text.length;
    }

cleanup:
    return error_check;
}

int remove_line(lines_t * lines, int lineno){
    int error_check = 0;
    line_t * line = NULL;

    line = get_line_no(lines, lineno);
    if(NULL == line){
        error_check = -1;
        goto cleanup;
    }
	
	if(lines->first_line == line){
	    if(NULL == lines->first_line->next_line){
	        goto cleanup;
	    }
        lines->first_line = line->next_line;
    }
    
    if(lines->curr_line == line){
        lines->curr_line = line->prev_line;
        lines->curr_line_no --;
    }
    
    if(line->next_line != NULL){
        line->next_line->prev_line = line->prev_line;
    }
    if(line->prev_line != NULL){
        line->prev_line->next_line = line->next_line;
    }

    free_line(line);

    lines->num_lines --;

cleanup:
    return error_check;
}

int elevate_line(lines_t * lines, int lineno){
    int error_check = 0;
    line_t * line = NULL;

    line = get_line_no(lines, lineno);
    if(NULL == line){
        error_check = -1;
        goto cleanup;
    }

    error_check = append_string(&line->prev_line->text, &line->text);
    if(-1 == error_check){
        goto cleanup;
    }

    error_check = remove_line(lines, lineno);

cleanup:
    return error_check;
}

void println(line_t * line){
    prints(&line->text);
}

int relative_change_currline(lines_t * lines, int dx, int dy, bool wrap){
    int error_check = 0;
    int x = lines->curr_index + dx;
    int y = lines->curr_line_no + dy;
    line_t * curr_line = NULL;
    line_t * newline = NULL;

    if(y == lines->num_lines){
        curr_line = get_line_no(lines, lines->num_lines-1);
        if(-1 == error_check){
            goto cleanup;
        }

        error_check = init_line(&newline, NULL, curr_line);
        if(-1 == error_check){
            goto cleanup;
        }

        curr_line->next_line = newline;
        lines->curr_line = newline;
        lines->curr_line_no ++;
        lines->num_lines ++;
        lines->curr_index = 0;
        goto cleanup;
    }
    else if(y >= 0){
        y %= lines->num_lines;
    }
    else{
        if(-y == lines->num_lines){
            y = 0;
        }
        else{
            y = lines->num_lines - (-y) % lines->num_lines;
        }
    }
    
    curr_line = get_line_no(lines, y);
    if(NULL == curr_line){
        error_check = -1;
        goto cleanup;
    }

    if(wrap){
        if(x >= 0){
            x %= curr_line->text.length + 1;
        }
        else{
            if(-x == curr_line->text.length + 1){
                x = 0;
            }
            else{
                x = curr_line->text.length + 1 - (-x) % (curr_line->text.length + 1);
            }
        }
    }
    else{
        if(x > curr_line->text.length){
            x = curr_line->text.length;
        }
        else if(x < 0){
            x = 0;
        }
    }

    lines->curr_line = curr_line;
    lines->curr_line_no = y;
    lines->curr_index = x;

cleanup:
    return error_check;
}

int get_lines(lines_t * lines, string_t ** visible, int * total_lines, int * curr_index){
    int i = 0;
    int error_check = 0;
    line_t * curr_line = lines->curr_line;

    *total_lines = 0;
    curr_line = lines->curr_line;
    for(i=0; curr_line->prev_line != NULL && i < lines->num_lines_above; i++){
        curr_line = curr_line->prev_line;
        (*curr_index) ++;
    }

    for(i=0; curr_line != NULL && i < lines->total_lines; i++){
        visible[i] = slice_string(&curr_line->text, lines->curr_index - lines->total_cols, lines->total_cols);
        if(NULL == visible[i] && errno != 0){
            error_check = -1;
            goto cleanup;
        }
        (*total_lines) ++;
        curr_line = curr_line->next_line;
    }

cleanup:
    return error_check;
}

void free_lines(lines_t * lines){
    line_t * next_line = NULL;
    lines->curr_line = lines->first_line;

    while(lines->curr_line != NULL){
        next_line = lines->curr_line->next_line;
        free_line(lines->curr_line);
        lines->curr_line = next_line;
    }
}

char * convert_lines_to_string(lines_t * lines, int * len){
    int length = 0;
    char * text = 0;
    line_t * curr_line = lines->first_line;

    while(curr_line != NULL){
        length += curr_line->text.length + 1;
        curr_line = curr_line->next_line;
    }

    text = malloc(length);
    if(NULL == text){
        goto cleanup;
    }

    curr_line = lines->first_line;
    length = 0;
    while(curr_line != NULL){
        memcpy(text + length, curr_line->text.str, curr_line->text.length);
        length += curr_line->text.length + 1;
        text[length-1] = '\n';
        curr_line = curr_line->next_line;
    }

    text[length-1] = 0;

    *len = length-1;

cleanup:
    return text;
}

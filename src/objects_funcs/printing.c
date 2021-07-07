#include "curtain.h"

int print_rect(rect_t * rect){
    int error_check = 0;
    char * bg = NULL;
    char newline[C_TINY_BUFFER_SIZE] = {0};
    int newline_length = 0;
    int allocated_length = 0;
    int bg_length = 0;
    int i = 0;

    newline_length = sprintf(newline, "\e[1B\e[%iD", rect->end.column - rect->start.column + 1);
    allocated_length = 34 + (rect->end.row - rect->start.row + 1) * (rect->end.column - rect->start.column + 1 + newline_length) - newline_length;
    bg = malloc(allocated_length);
    if(NULL == bg){
        error_check = -1;
        goto cleanup;
    }

    memset(bg, ' ', allocated_length);
    bg_length = sprintf(bg, "\e[%i;%iH\e[48;2;%i;%i;%im", rect->start.row, rect->start.column, rect->color.r, rect->color.g, rect->color.b);
    bg_length += rect->end.column - rect->start.column + 2;

    for(i=0; i<rect->end.row - rect->start.row; i++){
        memcpy(bg+bg_length, newline, newline_length);
        bg_length += newline_length + rect->end.column - rect->start.column + 1;
    }

    memcpy(bg + bg_length, "\e[0m", sizeof("\e[0m")-1);
    bg_length += sizeof("\e[0m") - 1;
    error_check = write(STDOUT_FILENO, bg, bg_length);

cleanup:
    if(bg != NULL){
        free(bg);
    }

    return error_check;
}

int print_label(label_t * label){
    int error_check = 0;
    char * bg = NULL;
    char newline[C_TINY_BUFFER_SIZE] = {0};
    int newline_length = 0;
    int allocated_length = 0;
    int bg_length = 0;
    int i = 0;
    int tlen = strnlen(label->text, C_BUFFER_SIZE);

    newline_length = sprintf(newline, "\e[1B\e[%iD", label->end.column - label->start.column + 1);
    allocated_length = 50 + (label->end.row - label->start.row + 1) * (label->end.column - label->start.column + 1 + newline_length) - newline_length;
    bg = malloc(allocated_length);
    if(NULL == bg){
        error_check = -1;
        goto cleanup;
    }

    memset(bg, ' ', allocated_length);
    bg_length = sprintf(bg, "\e[%i;%iH\e[48;2;%i;%i;%im\e[38;2;%i;%i;%im", label->start.row, label->start.column, label->bg_color.r, label->bg_color.g, label->bg_color.b, label->fg_color.r, label->fg_color.g, label->fg_color.b);

    for(i=0; i<label->end.row - label->start.row + 1; i++){
        if(i * (label->end.column - label->start.column + 1) < tlen){
            memcpy(bg + bg_length, label->text + i * (label->end.column - label->start.column + 1), MIN(label->end.column - label->start.column + 1, tlen - i * (label->end.column - label->start.column + 1)));
        }
        bg_length += label->end.column - label->start.column + 1;
        memcpy(bg+bg_length, newline, newline_length);
        bg_length += newline_length;
    }

    memcpy(bg + bg_length, "\e[0m", sizeof("\e[0m")-1);
    bg_length += sizeof("\e[0m") - 1;
    error_check = write(STDOUT_FILENO, bg, bg_length);

cleanup:
    if(bg != NULL){
        free(bg);
    }

    return error_check;
}

int print_textbox(textbox_t * textbox){
    int error_check = 0;
    string_t ** visible = NULL;
    char output[C_BUFFER_SIZE] = {0};
    char newline[C_TINY_BUFFER_SIZE] = {0};
    int length = 0;
    int newline_length = 0;
    int i = 0;
    int total_lines = 0;
    int curr_line_index = 0;
    int curr_col_index = textbox->lines->curr_index;
    int eraser_length = 0;
    int allocated_length = 0;
    lines_t * lines = textbox->lines;
    char * eraser = NULL;

    newline_length = sprintf(newline, "\e[1B\e[%iD", lines->total_cols);

    allocated_length = 31 + (newline_length + textbox->end.column - textbox->start.column + 1) * (textbox->end.row - textbox->start.row) + textbox->end.column - textbox->start.column + 1;
    eraser = malloc(allocated_length);
    if(NULL == eraser){
        error_check = -1;
        goto cleanup;
    }

    memset(eraser, ' ', allocated_length);

    eraser_length = sprintf(eraser, "\e[48;2;%i;%i;%im\e[%i;%iH", textbox->bg_color.r, textbox->bg_color.g, textbox->bg_color.b, textbox->start.row, textbox->start.column);
    eraser[eraser_length] = ' ';
    eraser_length += textbox->end.column - textbox->start.column + 1;
    for(i=0; i<textbox->end.row - textbox->start.row; i++){
        memcpy(eraser + eraser_length, newline, newline_length);
        eraser_length += newline_length + textbox->end.column - textbox->start.column + 1;
    }

    /*for(i=0; i<eraser_length; i++){
        if(isprint(eraser[i])){
            printf("%i >> (%d) '%c'\n", i, eraser[i], eraser[i]);
        }
        else{
            printf("%i >> (%d)\n", i, eraser[i]);
        }
    }
    printf("%i %i %i %i %i\n", lines->total_lines, allocated_length, eraser_length, textbox->end.row - textbox->start.row + 1, lines->total_lines);

    exit(0);*/

    if(lines->curr_index > lines->total_cols){
        curr_col_index = lines->total_cols;
    }

    visible = calloc(lines->total_lines, sizeof(string_t *));
    if(NULL == visible){
        error_check = -1;
        goto cleanup;
    }

    error_check = get_lines(lines, visible, &total_lines, &curr_line_index);
    if(-1 == error_check){
        goto cleanup;
    }

    write(STDOUT_FILENO, eraser, eraser_length);
    sprintf(output, "\e[%i;%iH", textbox->start.row, textbox->start.column);
    write(STDOUT_FILENO, output, C_TINY_BUFFER_SIZE);
    for(i=0; i<total_lines && visible[i] != NULL; i++){
        if(visible[i]->length > 0){
            sprintf(output, "\e[38;2;%i;%i;%im%s", textbox->fg_color.r, textbox->fg_color.g, textbox->fg_color.b, visible[i]->str);
            length = visible[i]->length;
            length = sprintf(output, "%s\e[1B\e[%iD", output, length);
            write(STDOUT_FILENO, output, length);
        }
        else{
            write(STDOUT_FILENO, "\e[1B", sizeof("\e[1B"));
        }

        free_string(visible[i]);
        free(visible[i]);
    }

    length = sprintf(output, "\e[%i;%iH", textbox->start.row + curr_line_index, textbox->start.column + curr_col_index);
    write(STDOUT_FILENO, output, length);
    free(visible);

cleanup:
    if(eraser != NULL){
        free(eraser);
    }

    return error_check;
}

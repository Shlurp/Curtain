#ifndef CURTAIN_DATATYPES_H
#define CURTAIN_DATATYPES_H

#include "macros.h"

typedef unsigned char bool;

typedef enum key_type_e {KEYTYPES_BEGIN=1000, 
                         ARROW_UP, ARROW_DOWN, ARROW_RIGHT, ARROW_LEFT,
                         PAGE_UP, PAGE_DOWN,
                         HOME, END,
                         DELETE,
                         KEYTYPES_END}key_type_t;

typedef enum obj_type_e {LABEL, RECT, TEXTBOX}obj_type_t;

typedef struct string_s{
    int num_allocated_blocks;
    int length;
    char * str;
}string_t;

typedef struct line_s{
    string_t text;
    struct line_s * next_line;
    struct line_s * prev_line;
}line_t;

typedef struct lines_s{
    int num_lines;
    line_t * first_line;
    line_t * curr_line;
    int curr_line_no;
    int curr_index;
    int num_lines_above;
    int total_lines;
    int total_cols;
}lines_t;

typedef struct color_s{
    unsigned char r;
    unsigned char g;
    unsigned char b;
}color_t;

typedef struct coordinate_s{
    int row;
    int column;
}coordinate_t;

typedef struct label_s{
    color_t bg_color;
    color_t fg_color;
    char * text;

    coordinate_t start;
    coordinate_t end;
}label_t;

typedef struct rect_s{
    color_t color;

    coordinate_t start;
    coordinate_t end;
}rect_t;

typedef struct textbox_s{
    color_t bg_color;
    color_t fg_color;

    coordinate_t start;
    coordinate_t end;

    lines_t * lines;
    int enter_char;
    int (*on_enter)(lines_t *);

    unsigned char position_change : 1;
}textbox_t;

typedef struct object_u{
    obj_type_t object_type;

    union obj_u{
        textbox_t textbox;
        rect_t rectangle;
        label_t label;
    } obj;

    union obj_u * prev;

    unsigned char reprint : 1;
}object_t;

typedef struct obj_node_s{
    object_t * obj;
    struct obj_node_s * next;
    struct obj_node_s * prev;
}obj_node_t;

typedef struct universe_s{
    coordinate_t size;
    obj_node_t * objects;
    obj_node_t *** obj_map;
}universe_t;

#endif
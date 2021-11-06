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

typedef enum obj_type_e {LABEL, RECT, TEXTBOX, BUTTON}obj_type_t;

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

typedef struct object_s object_t;
typedef struct universe_s universe_t;

typedef struct textbox_s{
    color_t bg_color;
    color_t fg_color;

    coordinate_t start;
    coordinate_t end;

    int num_objs;
    object_t ** objs;
    void * args;
    lines_t * lines;
    int enter_char;
    int (*on_enter)(lines_t *, int, object_t **, void *);
}textbox_t;

typedef struct button_s{
    color_t bg_color;
    color_t hover_bg_color;
    color_t fg_color;

    coordinate_t start;
    coordinate_t end;

    bool hover;
    char * text;

    int num_objs;
    object_t ** objs;
    void * args;
    int (*on_click)(universe_t *, int, object_t **, void *);
}button_t;

struct object_s{
    obj_type_t object_type;

    union obj_u{
        textbox_t textbox;
        rect_t rectangle;
        label_t label;
        button_t button;
    } obj;

    unsigned char reprint : 1;
};

typedef struct obj_node_s{
    object_t * obj;
    struct obj_node_s * next;
    struct obj_node_s * prev;
}obj_node_t;

struct universe_s{
    coordinate_t size;
    int num_interactables;
    obj_node_t * objects;
    obj_node_t * last_obj;
    obj_node_t * curr_obj;

    unsigned char dirty : 1;
};

#endif
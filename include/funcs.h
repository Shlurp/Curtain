#ifndef CURTAIN_H_FUNCS
#define CURTAIN_H_FUNCS

#include "datatypes.h"
#include <stdlib.h>
#include <errno.h>

#ifndef IN
    #define IN
#endif
#ifndef OUT
    #define OUT
#endif

/**** String functions ****/

/**
 * @brief: Initializes a string
 * @param{OUT} string: String to initialize
 * 
 * @returns: 0 on success, else -1 and errno is set (by calloc)
 */
int init_string(string_t * string);
/**
 * @brief: Shifts a string
 * @param{IN} index: Index to start shifting at
 * @param{IN} amount: Amount to shift (negative for left shifting)
 * @param{OUT} string: String to shift
 * 
 * @returns: 0 on success, else -1 (and errno set)
 */
int shift_string(int index, int amount, string_t * string);
int insert_char(char c, int index, string_t * string);
int append_char(char c, string_t * string);
int insert_string(int index, string_t * dest, string_t * src);
int set_string(string_t * string, char * s);
string_t * slice_string(string_t * string, int index, int length);
char * cslice_string(string_t * string, int index, int length);
int move_slice(string_t * dest, string_t * src, int dest_index, int src_index, int length);

#define prints(string) write(1, (string)->str, (string)->length)

static inline int append_string(string_t * dest, string_t * src){
    return insert_string(dest->length, dest, src);
}

static inline void free_string(string_t * string){
    free(string->str);
    string->length = 0;
    string->num_allocated_blocks = 0;
}

static inline int delete_char(int index, string_t * string){
    int error_check = shift_string(index, -1, string); 
    if(error_check != -1){
        string->length --;
    }

    return error_check;
}

static inline void delete_char_from_end(string_t * string){
    if(string->length > 0){
        string->str[string->length-1] = 0;
        string->length --;
    }
}

/**** Line functions ****/

int init_line(line_t ** line, line_t * next, line_t * prev);
int init_lines(lines_t * lines, int num_above, int total_rows, int total_cols);
line_t * get_line_no(lines_t * lines, int line_no);
int add_newline(lines_t * lines);
int insert_newline(lines_t * lines);
int elevate_line(lines_t * lines, int lineno);
int get_lines(lines_t * lines, string_t ** visible, int * total_lines, int * curr_index);
char * convert_lines_to_string(lines_t * lines, int * len);
void println(line_t * line);
void free_lines(lines_t * lines);

static inline int insert_char_to_line(lines_t * lines, char c){
    int error_check = insert_char(c, lines->curr_index, &lines->curr_line->text);
    if(error_check != -1){
        lines->curr_index ++;
    }
    return error_check;
}

static inline int remove_char_from_line(lines_t * lines){
    int error_check = 0;

    if(lines->curr_index > 0){
        error_check = delete_char(lines->curr_index-1, &lines->curr_line->text);
        if(error_check != -1){
            lines->curr_index --;
        }
    }
    else if(lines->curr_line != lines->first_line){
        lines->curr_index = lines->curr_line->prev_line->text.length;
        error_check = elevate_line(lines, lines->curr_line_no);
        if(error_check != -1){
        }
    }

    return error_check;
}

static inline int change_curr_line(lines_t * lines, int lineno){
    int error_check = 0;
    line_t * line = NULL;

    line = get_line_no(lines, lineno);
    if(NULL == line){
        error_check = -1;
    }

    lines->curr_line = line;
    lines->curr_line_no = lineno;
    if(lines->curr_index > line->text.length){
        lines->curr_index = line->text.length;
    }

    return error_check;
}

int relative_change_currline(lines_t * lines, int dx, int dy, bool wrap);

static inline void free_line(line_t * line){
    free_string(&line->text);
    if(line != NULL){
        free(line);
    }
}

/**** Input functions ****/

int getinput(textbox_t * textbox);

/**** Object Constructors ****/

object_t * init_label(char * text, coordinate_t start, coordinate_t end, color_t bgcolor, color_t fgcolor);
object_t * init_rect(coordinate_t start, coordinate_t end, color_t color);
object_t * init_textbox(coordinate_t start, coordinate_t end, color_t fg_color, color_t bg_color, int enter_char, int (*on_enter)(lines_t *, void *));
object_t * init_button(char * text, coordinate_t start, coordinate_t end, color_t bgcolor, color_t fgcolor, color_t hover_bgcolor, int (*on_click) (universe_t *, void *));

/**** Object Destructor ****/

void free_obj(object_t * obj);

/**** Printing Functions ****/

int print_rect(rect_t * rect);
int print_label(label_t * label);
int print_textbox(textbox_t * textbox);
int print_button(button_t * button);
int quick_remove(universe_t * universe, object_t * obj, color_t color);

static inline int print_obj(object_t * obj){
    int return_value = -1;
    switch(obj->object_type){
        case RECT: return_value = print_rect(&obj->obj.rectangle); break;
        case LABEL: return_value = print_label(&obj->obj.label); break;
        case TEXTBOX: return_value = print_textbox(&obj->obj.textbox); break;
        case BUTTON: return_value = print_button(&obj->obj.button); break;
        default: errno = EINVAL; break;
    }

    return return_value;
}

static inline int erase_area(coordinate_t start, coordinate_t end, color_t color){
    object_t * rect = NULL;
    int error_check = 0;

    rect = init_rect(start, end, color);
    if(-1 == error_check){
        goto cleanup;
    }

    error_check = print_obj(rect);

    free_obj(rect);

cleanup:
    return error_check;
}

/**** Miscellaneous functions ****/

int enable_rawmode();
void view_keypresses();
int getwinsize(int * rows, int * cols);
int scan_keypress();

/**** Object Functions ****/

static inline void change_color(object_t * obj, color_t * bg_color, color_t * fg_color, color_t * hover_color){
    switch(obj->object_type){
        case RECT: obj->obj.rectangle.color = *bg_color; break;
        case LABEL:
            if(bg_color != NULL){
                obj->obj.label.bg_color = *bg_color;
            }
            if(fg_color != NULL){
                obj->obj.label.fg_color = *fg_color;
            }
            break;
        case TEXTBOX:
            if(bg_color != NULL){
                obj->obj.textbox.bg_color = *bg_color;
            }
            if(fg_color != NULL){
                obj->obj.textbox.fg_color = *fg_color;
            }
            break;
        case BUTTON:
            if(bg_color != NULL){
                obj->obj.button.bg_color = *bg_color;
            }
            if(fg_color != NULL){
                obj->obj.button.fg_color = *fg_color;
            }
            if(hover_color != NULL){
                obj->obj.button.hover_bg_color = *hover_color;
            }
            break;
    }   

    obj->reprint = true;
}

int move_obj(universe_t * universe, object_t * object, coordinate_t move);

#define is_interactable(object_node) ((object_node)->obj->object_type == BUTTON || (object_node)->obj->object_type == TEXTBOX)

#define getcoords(obj, start, end) switch(obj->object_type){ \
        case LABEL: start = obj->obj.label.start; end = obj->obj.label.end; break; \
        case RECT: start = obj->obj.rectangle.start; end = obj->obj.rectangle.end; break; \
        case TEXTBOX: start = obj->obj.textbox.start; end = obj->obj.textbox.end; break; \
        case BUTTON: start = obj->obj.button.start; end = obj->obj.button.end; break; \
    }

/**** Universe Functions ****/

bool object_exists(universe_t * universe, object_t * obj);
universe_t * init_universe();
int add_object(universe_t * universe, object_t * obj);
int print_universe(universe_t * universe, bool print_all);
int run_universe(universe_t * universe);
void free_universe(universe_t * universe);
void print_universe_data(universe_t * universe);
bool remove_object(universe_t * universe, object_t * object);

#endif

#include "curtain.h"

object_t * init_label(char * text, coordinate_t start, coordinate_t end, color_t bgcolor, color_t fgcolor){
    object_t * label = NULL;
    int tlen = 0;

    label = malloc(sizeof(object_t));
    if(NULL == label){
        goto cleanup;
    }

    tlen = strnlen(text, C_BUFFER_SIZE);

    label->obj.label.bg_color = bgcolor;
    label->obj.label.fg_color = fgcolor;
    label->obj.label.start = start;
    label->obj.label.end = end;
    label->obj.label.text = malloc(tlen + 1);
    if(NULL == label->obj.label.text){
        free(label);
        label = NULL;
        goto cleanup;
    }
    memcpy(label->obj.label.text, text, tlen+1);

    label->object_type = LABEL;
    label->reprint = true;

cleanup:
    return label;
}

object_t * init_rect(coordinate_t start, coordinate_t end, color_t color){
    object_t * rectangle = NULL;

    rectangle = malloc(sizeof(object_t));
    if(NULL == rectangle){
        goto cleanup;
    }

    rectangle->obj.rectangle.color = color;
    rectangle->obj.rectangle.start = start;
    rectangle->obj.rectangle.end = end;

    rectangle->object_type = RECT;
    rectangle->reprint = true;

cleanup:
    return rectangle;
}

object_t * init_textbox(coordinate_t start, coordinate_t end, color_t fg_color, color_t bg_color, int enter_char, int (*on_enter)(lines_t *)){
    object_t * textbox = NULL;
    int error_check = 0;

    textbox = malloc(sizeof(object_t));
    if(NULL == textbox){
        goto cleanup;
    }

    textbox->obj.textbox.bg_color = bg_color;
    textbox->obj.textbox.fg_color = fg_color;
    textbox->obj.textbox.start = start;
    textbox->obj.textbox.end = end;
    textbox->obj.textbox.enter_char = enter_char;
    textbox->obj.textbox.on_enter = on_enter;

    textbox->obj.textbox.lines = malloc(sizeof(lines_t));
    if(NULL == textbox->obj.textbox.lines){
        goto cleanup;
    }

    error_check = init_lines(textbox->obj.textbox.lines, (end.row - start.row - 1) / 2, end.row - start.row, end.column - start.column + 1);
    if(-1 == error_check){
        free(textbox);
        textbox = NULL;
        goto cleanup;
    }

    textbox->object_type = TEXTBOX;
    textbox->reprint = true;

cleanup:
    return textbox;
}

object_t * init_button(char * text, coordinate_t start, coordinate_t end, color_t bgcolor, color_t fgcolor, color_t hover_bgcolor, int (*on_click) (universe_t *, void *)){
    object_t * button = NULL;
    int tlen = 0;

    button = malloc(sizeof(object_t));
    if(NULL == button){
        goto cleanup;
    }

    tlen = strnlen(text, C_BUFFER_SIZE);

    button->obj.button.bg_color = bgcolor;
    button->obj.button.fg_color = fgcolor;
    button->obj.button.hover_bg_color = hover_bgcolor;
    button->obj.button.hover = false;
    button->obj.button.start = start;
    button->obj.button.end = end;
    button->obj.button.on_click = on_click;
    button->obj.button.text = malloc(tlen + 1);
    if(NULL == button->obj.button.text){
        free(button);
        button = NULL;
        goto cleanup;
    }
    memcpy(button->obj.button.text, text, tlen+1);

    button->object_type = BUTTON;
    button->reprint = true;

cleanup:
    return button;
}
#include "curtain.h"

int main(){
    coordinate_t start = {0};
    coordinate_t end = {0};
    color_t fg_color = {0};
    color_t bg_color = {0};
    object_t * title = NULL;
    object_t * bg = NULL;
    object_t * textbox = NULL;

    setcoord(start, 2, 15);
    setcoord(end, 2, 45);
    setcolor(fg_color, 255, 255, 255);
    title = init_label(" Curtain - terminal graphics", start, end, bg_color, fg_color);
    
    setcoord(start, 1, 5);
    setcoord(end, 50, 55);
    setcolor(bg_color, 200, 100, 255);
    bg = init_rect(start, end, bg_color);

    setcoord(start, 20, 7);
    setcoord(end, 49, 54);
    setcolor(bg_color, 0, 0, 0);
    textbox = init_textbox(start, end, fg_color, bg_color, -1, NULL);

    refreshscr;
    enable_rawmode();
    print_obj(bg);
    print_obj(title);
    getinput(&textbox->obj.textbox);
}

#include "curtain.h"

int main(){
    /** 
     * Making a simple (not yet working) GUI for a communication program 
     * This obviously needs error-checking, but it gets the point across (probably)
     */

    /* A bunch of variables, the coordinate and color variables are used to set the coordinates and colors of objects   */
    coordinate_t start = {0};
    coordinate_t end = {0};
    color_t fg_color = {0};
    color_t bg_color = {0};
    color_t hover_color = {0};      // For the button

    /* All of the objects that we're going to add to the GUI */
    object_t * title = NULL;
    object_t * bg = NULL;
    object_t * textbox = NULL;
    object_t * button = NULL;

    /* The universe, the struct with all of the data for the curtain enviornment */
    universe_t * universe = NULL;
    
    /* Set the coordinates using the setcoord macro (it's pretty self-explanatory) */
    setcoord(start, 1, 1);
    setcoord(end, 30, 55);
    /* Set the color using the setcolor macro (uses RGB) */
    setcolor(bg_color, 0, 0, 0);
    /* Create the background for the GUI, a simple colored rectangle */
    bg = init_rect(start, end, bg_color);

    /* Create the textbox - the box for user input */
    setcoord(start, 20, 2);
    setcoord(end, 29, 48);
    setcolor(bg_color, 0, 200, 0);
    textbox = init_textbox(start, end, fg_color, bg_color, -1, NULL);

    /* Create the "Send" button. The hover_color is the color of the button when the user hovers over it */
    setcoord(start, 20, 50);
    setcoord(end, 21, 54);
    setcolor(bg_color, 100, 100, 100);
    setcolor(fg_color, 255, 255, 255);
    setcolor(hover_color, 150, 150, 150);
    button = init_button("Send", start, end, bg_color, fg_color, hover_color, NULL);

    /* Create the title - basically a rectangle with text */
    setcoord(start, 2, 16);
    setcoord(end, 2, 39);
    setcolor(bg_color, 0, 255, 0);
    setcolor(fg_color, 0, 0, 0);
    title = init_label(" Curtain - GUI designer", start, end, bg_color, fg_color);

    /* Initialize the universe, nothing interesting happens here other than getting the window size really */
    universe = init_universe();

    /* Adding all of the objects to the universe */
    add_object(universe, bg);
    add_object(universe, title);
    add_object(universe, textbox);
    add_object(universe, button);
    
    quick_remove(universe, title, fg_color);
    /* Running the universe - once this is run, everything is out of the programmer's hands. The user can move between "interactables" - 
       buttons and textboxes and put in input */
    run_universe(universe);

    /* Free the universe after finishing */
    free_universe(universe);

    exit(0);
}

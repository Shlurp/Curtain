# **Curtain - Quickstart**

## **This is outdated and needs a rewrite**

Curtain, as a library for quickly creating GUIs provides many methods of creating such. This tutorial will provide you with the basic knowledge of how to use Curtain in a high-level view. While Curtain itself isn't especially complicated, this tutorial will not delve far into its internals.

***

## **Getting Familiar With Curtain Objects**

The most useful objects that Curtain provides are the objects that represent graphical objects on your screen. They are the following:
+ `rect_t` - Simple colored rectangles good for creating backgrounds and simple art.
+ `label_t` - Colored rectangles that also have text. Good for labeling things.
+ `textbox_t` - Colored rectangles that allow the user (not programmer) to input text.
+ `button_t` - Colored rectangles that allow the user to "click" on them.

**Labels vs Rectangles**  
Labels and rectangles are quite similar, so why are there two different datatypes for them? The answer is quite simple. Because labels require Curtain to print and align text, it takes longer to print a label than a rectangle, as well as labels use slightly more memory (which doesn't actually matter as objects are stored in a union).  
While this shouldn't be an issue for the most part, in the event that someone wants to use a lot of rectangles, it doesn't make much sense to have labels and rectangles as one datatype.

**User Input**  
Textboxes and buttons, being only datastructs, do not by themselves get user input. They provide the fundamentals for getting user input but still require a function (which Curtain provides) to run them. We'll get to that later.

<br>

Curtain also provides one more important object that you should familiarize yourself with: `universe_t`. A Curtain universe stores all of the data of the objects currently in the Curtain enviornment. It allows for user interaction with and connection between Curtain objects.

### **Object Structures and Initialization**
Curtain has four types of Graphical Objects, which are all stored in the generalized `object_t` object:
```c
typedef struct object_u{
    obj_type_t object_type;

    union obj_u{
        textbox_t textbox;
        rect_t rectangle;
        label_t label;
        button_t button;
    } obj;

    unsigned char reprint : 1;
}object_t;
```

This is essentially a tagged union, along with the field **reprint** which is set to **true** if the object needs to be reprinted and **false** otherwise. The datatypes in the union are defined like so:

+ `rect_t` is defined like so (in [datatypes.h](../include/datatypes.h)):
    ```c
    typedef struct rect_s{
        color_t color;

        coordinate_t start;
        coordinate_t end;
    }rect_t;
    ```

    `color_t` is a datastruct for RGB colors, and `coordinate_t` is a a 2D point in the terminal. So all a rectangle stores is its color and start and end points.

    You can initialize a `rect_t` object with the [init_rect](../src/objects_funcs/initialize.c/init) function:
    ```c
    object_t * init_rect(coordinate_t start, coordinate_t end, color_t color)
    ```

+ `label_t` is defined like so (in [datatypes.h](../include/datatypes.h)):
    ```c
    typedef struct label_s{
        color_t bg_color;
        color_t fg_color;
        char * text;

        coordinate_t start;
        coordinate_t end;
    }label_t;
    ```

    **bg_color** is the color of the label itself, **fg_color** is the color of the text, **text** is the text itself, and **start** and **end** are the start and endpoints of the label respectively.

    You can initialize a `label_t` object with the [init_label](../src/objects_funcs/initialize.c/init) function:
    ```c
    object_t * init_label(char * text, coordinate_t start, coordinate_t end, color_t bgcolor, color_t fgcolor)
    ```

+ `textbox_t` is defined like so (in [datatypes.h](../include/datatypes.h)):
    ```c
    typedef struct textbox_s{
        color_t bg_color;
        color_t fg_color;

        coordinate_t start;
        coordinate_t end;

        lines_t * lines;
        int enter_char;
        int (*on_enter)(lines_t *, void *);
    }textbox_t;
    ```

    As you can see, a textbox is slightly more complicated than a label or rectangle. **bg_color**, **fg_color**, **start**, and **end** should be self-explanatory.  
    `lines_t` **\* lines** is a datastruct that represents the actual text editor enviornment. You probably shouldn't mess with it. (And it is set automatically during initialization) 
    **enter_char** is the character that the user needs to press to pass the input onto the rest of the program. If you don't want it to do such a thing, set it to -1.  
    **on_enter** is the function that should be called once the user inputs **enter_char**.

    You can initialize a `textbox_t` object with the [init_textbox](../src/objects_funcs/initialize.c/init) function:
    ```c
    object_t * init_textbox(coordinate_t start, coordinate_t end, color_t fg_color, color_t bg_color, int enter_char, int (*on_enter)(lines_t *, void *))
    ```

+ `button_t` is defined like so (in [datatypes.h](../include/datatypes.h)):
    ```c
    typedef struct button_s{
        color_t bg_color;
        color_t hover_bg_color;
        color_t fg_color;

        coordinate_t start;
        coordinate_t end;

        bool hover;
        char * text;

        int (*on_click)(universe_t *, void *);
    }button_t;
    ```

    Unlike the previous three objects, `button_t` also has the variable **hover_bg_color** along with the usual colors. **hover_bg_color** is the color of the button when the user "hovers" over it.  
    **hover** is a boolean value of whether or not the user is hovering over the button. Unlike other fields, it cannot be set during initialization.  
    **text** is the text of the button.  
    **on_click** is the function to be clicked when the button is clicked.

    You can initialize a `button_t` object with the [init_button](../src/objects_funcs/initialize.c/init) function:
    ```c
    object_t * init_button(char * text, coordinate_t start, coordinate_t end, color_t bgcolor, color_t fgcolor, color_t hover_bgcolor, int (*on_click) (universe_t *, void *))
    ```

Curtain also stores enviornment data in the `universe_t` datastruct:
```c
typedef struct universe_s{
    coordinate_t size;
    int num_interactables;
    obj_node_t * objects;
    obj_node_t * last_obj;
    obj_node_t * curr_obj;

    unsigned char dirty : 1;
}universe_t;
```

**size** is the size of the terminal (at the time of initialization).  
**num_interactables** is the number of objects the user can interact with (buttons and textboxes).  
**objects** is a pointer to the first object in the linked list of objects (the object at the bottom).  
**last_object** was the last object added to the enviornment.  
**curr_obj** is the current selected (interactable) object.  
**dirty** is if the entire universe needs to be reprinted (if an object was moved or removed).

***

## **Necessary Functions and Macros**

In order to create a GUI, there are three steps:
1. Creating the objects and universe
2. Adding the objects to the universe
3. Running the universe

### **Creating Objects**
We have already covered the object constructors (init_rect, etc). There are also a couple of useful macros that you should be aware of:
+ setcoord:  
    This allows you to change the value of a coordinate:

    ```c
    #define setcoord(coord, r, c) (coord).row = (r); (coord).column = (c)
    ```

    **coord** is the `coordinate_t` struct you want to change, **r** is the row you want to change it to, and **c** is the column you want to change it to.

+ setcolor:
    This allows you to change the value of a color:

    ```c
    #define setcolor(c, red, green, blue) (c).r = (red); (c).g = (green); (c).b = (blue)
    ```

    **c** is the `color_t` struct that you want to change  
    **red** is the value of red you want to change to (in RGB), same for green and blue.

For example, if we want to create a new rectangle:
```c
coordinate_t start = {0};
coordinate_t end = {0};
color_t bg_color = {0};
object_t * bg = NULL;

setcoord(start, 1, 1);
setcoord(end, 10, 10);
setcolor(bg_color, 0, 0, 255);
bg = init_rect(start, end, bg_color);
```

Will create a new rectangle **bg** of size 10x10 that starts at the first row and first column.

**Creating the Universe**  
All Graphical objects that you wish to appear onscreen should be stored in a `universe_t` struct. In order to initialize one, you should use the `init_universe` constructor:
```c
universe_t * init_universe()
```

This will initialize an empty universe, allowing you to add as many objects as you want. Universes handle printing for you, and they allow for interaction between objects.

<br>

### **Adding Objects to a Universe**
Adding an object to a universe basically groups it with all of the objects in the GUI. By giving the object to the universe, you can interact with your GUI on a higher-level independent from independent objects. Adding an object is quite easy, just use the `add_object` function:
```c
int add_object(universe_t * universe, object_t * obj)
```

<br>

### **Running the Universe**
Once you are finished adding objects to your universe/GUI, you can run it with the `run_universe` function:
```c
int run_universe(universe_t * universe)
```
which is essentially a premade function that prints the GUI and accepts user input. If the function doesn't suit your needs, there are a lot of other "lower-level" functions that Curtain provides for handling objects that you can use to create your own `run_universe` function.  

The `run_universe` function allows the user to hit the tab button to switch between interactables and the enter button to interact with them. While hovering over a button, hitting enter will click it, and while hovering over a textbox, hitting enter will allow you to type into it. To quit a textbox, either input the `enter_char` for the textbox (to input the data), or ctrl-c or esc to quit (without inputting).
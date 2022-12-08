# Project 3
## CS 453 Scientific Visualization
### Matthew Morgan


### How to run the program
1. Open the solution in Visual Studio
    - In Visual Studio 2022, select File > Open > Project/Solution
    - then open `learnply/learnply.sln`
2. Start without debugging
    - Press the green arrow at top of the screen, or key-chord Ctrl + F5, or run `Debug/learnply.exe`
3. Choose a visualization
    - Press the key `1`
        - display mode 1: solid rendering.
    - Press the key `2`
        - display mode 2: show wireframe.
    - Press the key `3`
        - display mode 3: render each quad with colors of verticies.
    - Press the key `4`
        - display mode 4: show streamlines.
    - Press the key `5`
        - display mode 5: show IBVF.

4. Choose a visualization
    - Press the key `r`
        - resets to the default view
    - Press the key `t`
        - (1. a.) displays a grey scale map of scalar values at the mesh grid points.
    - Press the key `y`
        - (1. b.) displays a bi-color map of scalar values from red to blue.
    - Press the key `u`
        - (1. c.) displays a rainbow map heat map.
    - Press the key `i`
        - (2.) displays the scalar values as height.
    - Press the key `o`
        - (3.) displays the scalar values as height and grey scale map.
    - Press the key `p`
        - (3.) displays the scalar values as height and bi-color map.
    - Press the key `[`
        - (3.) displays the scalar values as height and rainbow map.

    - Press the key `z`
        - (2. a.) displays same color contours.
    - Press the key `x`
        - (2. b.) displays bi-color contours.
    - Press the key `c`
        - (2. c.) combine contours with height fields.
    - Press the key `v`
        - (2. d.) combine colors/height/contours. 
    - Press the key `b`
        - (3. a.) combine colors/height/contours and show critical points.

    - Press the key `a`
        - Displays IBVF for the vector field.
    - Press the key `s`
        - Displays noise pattern.
    - Press the key `d`
        - Displays IBVF flow stream.
    - Press the key `h`
        - Displays the image.
    - Press the key `j`
        - Displays the sobel edges from the image.
    - Press the key `l`
        - Displays the IBVF of the edges of the image.

    - Press 'alt' + left click to draw a stremaline.
    

5. Exit
    - Press the escape key to exit and close the window.

### Options
1. Select a .ply file to view
    - modify line 76 of `learnply.cpp` to the filepath of the .ply file you would like to view.


int NUM_CONTOURS = 60;
double HEIGHT_MULTIPLIER = 0.7;
double CAP_HEIGHT = 6;
bool APPLY_NORM = true;
bool APPLY_LOG = false;

bool is_pca = false;
bool draw_descent_dots = false;

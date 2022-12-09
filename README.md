# Term Project
## CS 453/553 Scientific Visualization
### Charles Ison and Matthew Morgan


### How to run the program
1. Open the solution in Visual Studio
    - In Visual Studio 2022, select File > Open > Project/Solution
    - then open `learnply/learnply.sln`

2. Start without debugging
    - Press the green arrow at top of the screen
    - *or* key-chord: `Ctrl + F5`

3. Choose a rendering mode
    - Press the key `1`
        - display mode 1: solid rendering.
    - Press the key `2`
        - display mode 2: show wireframe mesh.
    - Press the key `3`
        - display mode 3: color gradient.

4. Choose a visualization
    - Press the key `r`
        - resets everything to the default view.

    - Height:
        - Press the key `a`
            - displays the the loss landscape as height.
        - Press the `s` key:
            - sets the mesh of vertices to a height of 0, which creates the visualization of a floor.
        - Press the `d` key:
            - removes the mesh of vertices from the visualization (leaving just the contours).

    - Contours:
        - Press the key `z`
            - displays same color contours.
        - Press the key `x`
            - displays a bi-color gradient of contours from red to blue.
        - Press the key `c`
            - displays contours and shows critical points.
        
    - Color:
        - Press the key `t`
            - displays a greyscale map of the loss landscape.
        - Press the key `y`
            - displays a bi-color map of the loss landscape from red to blue.
        - Press the key `u`
            - displays a rainbow-colored map of the loss landscape.

    - Gradient Descent (for PCA only):
        -  Press the key `g`
            - displays the gradient descent curve. The yellow dot is the start and the cyan dot is the end of the gradient descent.


5. Exit
    - Press the escape key to exit and close the window.

### Options
1. Select a .ply file to view
    - Is this a PCA loss landscape?
        - Yes:
            1. modify line 26 of `learnply.cpp` to the filepath of the .ply file you would like to view.
            2.  modify line 27 of `learnply.cpp` to the filepath of the .csv file of the gradient descent.
            3. set `bool is_pca = true;` on line 28 of `learnply.cpp`

        - No:
            1. modify line 26 of `learnply.cpp` to the filepath of the .ply file you would like to view.
            2. set `bool is_pca = false;` on line 28 of `learnply.cpp`



### List of the parameters we used to create our visualizations:
1. ResNet PCA:
```
char* ply_filepath = "../python_scripts/runs/PCA/ResNet_PCA.ply";
char* csv_filepath = "../python_scripts/runs/PCA/ResNet_PCA_gradient_descent_results.csv";
bool is_pca = true;

double CAP_HEIGHT = 1;
double HEIGHT_MULTIPLIER = 10;
bool APPLY_NORM = false;
bool APPLY_LOG = false;
int NUM_CONTOURS = 60;
```

2. VGG PCA:
```
char* ply_filepath = "../python_scripts/runs/PCA/VGG_PCA.ply";
char* csv_filepath = "../python_scripts/runs/PCA/VGG_PCA_gradient_descent_results.csv";
bool is_pca = true;

double CAP_HEIGHT = 1;
double HEIGHT_MULTIPLIER = 10;
bool APPLY_NORM = false;
bool APPLY_LOG = false;
int NUM_CONTOURS = 40;
```

3. ResNet Random:
```
char* ply_filepath = "../python_scripts/runs/Random/ResNet_Random.ply";
char* csv_filepath = "../python_scripts/runs/PCA/ResNet_PCA_gradient_descent_results.csv";
bool is_pca = false;

double CAP_HEIGHT = 6;
double HEIGHT_MULTIPLIER = 0.7;
bool APPLY_NORM = true;
bool APPLY_LOG = false;
int NUM_CONTOURS = 40;
```

4. VGG Random:
```
char* ply_filepath = "../python_scripts/runs/Random/VGG_Random.ply";
char* csv_filepath = "../python_scripts/runs/PCA/VGG_PCA_gradient_descent_results.csv";
bool is_pca = false;

double CAP_HEIGHT = 6;
double HEIGHT_MULTIPLIER = 0.7;
bool APPLY_NORM = true;
bool APPLY_LOG = false;
int NUM_CONTOURS = 40;
```

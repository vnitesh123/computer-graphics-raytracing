# Computer Graphics (CAP-5705) : Ray Tracing

## Introduction 

This project implements a ray tracing algorithm to render a scene composed of simple objects like spheres and tetrahedra with different colors and sizes.

It also provides an option to chose the view type for rendering, which is Orghgraphic view or Perspective view

## Requirements

C++ version - 11 & above


We will be needing few OpenGL libraries for visualization of the image rendered from the scene

### Library Installation Commands

GLEW 
```
brew install glew
```
GLFW 
```
brew install glfw
```

## Execution Steps

Checkout to the project folder and follow the below steps

Command to complile the program

```
clang++ -std=c++14  -I<GLFW INCLUDE PATH> -I<GLEW INCLUDE PATH> -L<GLFW LIBRARY PATH> -L<GLEW LIBRARY PATH> -lglfw -lglew  -framework OpenGL main.cpp
```

Replace the variables : `<GLFW INCLUDE PATH>  <GLEW INCLUDE PATH>  <GLFW LIBRARY PATH>  <GLEW LIBRARY PATH>` with their respective paths

Values for the above variables can be found by executing

```
brew info glew
brew info glfw
```

An example for the final command based on the values in our local machine

```
clang++ -std=c++14  -I/opt/homebrew/Cellar/glfw/3.3.8/include/ -I/opt/homebrew/Cellar/glew/2.2.0_1/include/ -L/opt/homebrew/Cellar/glfw/3.3.8/lib/ -L/opt/homebrew/Cellar/glew/2.2.0_1/lib/ -lglfw -lglew  -framework OpenGL main.cpp
```

After compiling the program execute the program using the command

```
./a.out
```
 
Note: When prompted for perspective view, type `y` and press enter to proceed with perspective view. For any other input string, it proceeds with default orthographic view.

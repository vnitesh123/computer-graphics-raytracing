#include "image.h"
#include "camera.h"
#include "surface.h"
#include<iostream>

#include<vector>
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace std;

double DOUBLE_MAX = numeric_limits<double>::max();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
	"gl_Position = vec4(aPos, 1.0);\n"
	"ourColor = aColor;\n"
	"TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture1, TexCoord);\n"
    "}\n\0";
    
bool hit_surface(vector<Surface*> surfaces, vector<double>e, vector<double> d, IntersectionInfo &info){
    bool surface_hit=false;
    for (auto& s:surfaces){
        if(s->intersect(e,d,info)){
            surface_hit = true;
        }
    }
    return surface_hit;
}

vector<double> getMirrorReflection(vector<double> d, vector<double> n){
    return unit_vector(subtract_vectors(d, scalar_product(n,2*dot(d,n))));
}

void findImagePixels(vector<Surface*> surfaces, image &img, camera cam, vector<vector<vector<double>>> points, vector<double> background, vector<vector<double>> ls, vector<double> light_intensities, double phong_exponent){
    for(int i=0;i<img.x_res;i++){
        for(int j=0;j<img.y_res;j++){
            img.set_pixel(i,j,background);
            IntersectionInfo info = IntersectionInfo(DOUBLE_MAX);
            vector<double> ray_direction = cam.d;
            vector<double> ray_origin = points[i][j];
            if (cam.is_perspective_camera){
                ray_origin = cam.e;
                ray_direction = unit_vector(subtract_vectors(points[i][j],cam.e));
            }
            bool surface_hit = hit_surface(surfaces, ray_origin, ray_direction, info);
            if (surface_hit){
                // add reflection
                IntersectionInfo new_info = IntersectionInfo(DOUBLE_MAX);
                vector<double> mirror_reflection = {0,0,0};
                if(info.km>0.01){
                    vector<double> r = getMirrorReflection(ray_direction,info.normal);
                    bool surface_hit2 = hit_surface(surfaces, info.hit_loc, r, new_info);
                    vector<double> mirror_reflection = {0,0,0};
                    if (surface_hit2){
                         //Lm
                         mirror_reflection = scalar_product(img.calculate_pixel_value_at_point(ls,new_info,light_intensities, phong_exponent, r), info.km);
                    } 
                    // La+Ld+Lm
                    img.set_pixel(i,j,add_vectors(img.calculate_pixel_value_at_point(ls,info,light_intensities, phong_exponent, ray_direction), mirror_reflection));
                    
                } else{
                    img.set_pixel(i,j,img.calculate_pixel_value_at_point(ls,info,light_intensities, phong_exponent, ray_direction));
                } 
            }

        }
    }
}

int main(){
    // Scene 1
    vector<double> e = {-8.5,3.25,0.5}; 
    vector<double> d={5,-1,0};
    vector<double> up={1,5,0};
    

    // Scene 2 
    //vector<double> e = {0,0,0.5};
    //vector<double> d={1,0,0};
    //vector<double> up={0,1,0};

    up = unit_vector(up);
    d = unit_vector(d);

    bool is_perspective_camera = false;
    cout<<"Do you want perspective view? [y/n]"<<endl;
    string key;
    cin >> key ;
    if(key == "y"){
        is_perspective_camera = true;
        // in case of perspective view we are moving the camera a little nearer to get a relistic scene, 
        // if we want to render from a different location, the camera position can be changed accordingly
        e = {0.5,1.25,0.5};
    }

    camera cam = camera(e,up,d, 1, is_perspective_camera);
    image img = image(500,500,6,6);

    vector<vector<vector<double>>>points = cam.get_view_plane(img.w, img.h, img.x_res, img.y_res);
    vector<double> c1={2.5,0.5,2.5}; // 0.5 radius
    vector<double> c2={4.5, 0.75, 0.75}; // 0.75 radius

    // radius of spheres
    double r1 = 0.5;
    double r2 = 0.75;
    
    // Tetrahedron vertices
    vector<double> a3 = {3, 0, -2};
    vector<double> b3 = {2, 0, -1};
    vector<double> c3 = {3, 0, -0.5};
    vector<double> d3 = {2.5, 1, -1};

    //red
    vector<double>color_red = {220,0,0};

    // yellow
    vector<double>color_yellow = {255,255,0};

    // blue
    vector<double>color_blue = {0,255,255};


    vector<double> background = {20,20,20};
    vector<double> plane_color = {160,160,160};

    // ambient shading coefficients
    double ka1 = 0.4; // for sphere 1
    double ka2 = 0.4; // for sphere 2
    double ka3 = 0.4; // for tetrahedron
    double ka4 = 0.4; // for plane (ground)

    // diffuse shading coefficients
    double kd1 = 0.4; // for sphere 1
    double kd2 = 0.4; // for sphere 2
    double kd3 = 0.4; // for tetrahedron
    double kd4 = 0.4; // for plane (ground)

     // specular shading coefficients
    double ks1 = 0.2; // for sphere 1
    double ks2 = 0.2; // for sphere 2
    double ks3 = 0.2; // for tetrahedron
    double ks4 = 0.0; // for plane (ground)

    double km1 = 0.0; // for sphere 1
    double km2 = 0.0; // for sphere 2
    double km3 = 0.0; // for tetrahedron
    double km4 = 0.2; // for plane (ground)


    // normal of the plane surface
    vector<double> ground_normal = {0,1,0};
    ground_normal = unit_vector(ground_normal);

    // arbitrary point on the ground plane
    vector<double> ground_point ={3,0,2};

    double phong_exponent = 40;

    // light direction
    // light source 1
    vector<double> ls1= {-1,2,0};
    ls1 = unit_vector(ls1);

    // light source 2 
    vector<double> ls2 ={0,1,0};

    // incase of single light source use these variable
    vector<vector<double>>single_light_source = {ls1};
    vector<double>single_light_intensity = {1}; 

    // in case of multi light source use the below list as light source
    vector<vector<double>>multi_light_source = {ls1, ls2};
    vector<double>multi_light_intensities = {0.25,0.75};


    // Objects
    Sphere s1 = Sphere(c1, r1, color_yellow, ka1, kd1, ks1, km1);
    Sphere s2 = Sphere(c2, r2, color_blue, ka2, kd2, ks2, km2);
    Triangle s3 = Triangle(a3, c3, b3, color_red, ka3, kd3, ks3, km3);
    Triangle s4 = Triangle(d3, b3, c3, color_red, ka3, kd3, ks3, km3);
    Triangle s5 = Triangle(d3, a3, b3, color_red, ka3, kd3, ks3, km3);
    Triangle s6 = Triangle(d3, c3, a3, color_red, ka3, kd3, ks3, km3);
    Plane s7 = Plane(ground_normal, ground_point, plane_color, ka4, kd4, ks4, km4);

    vector<Surface*> surfaces;
    surfaces.push_back(&s1);
    surfaces.push_back(&s2);
    surfaces.push_back(&s3);
    surfaces.push_back(&s4);
    surfaces.push_back(&s5);
    surfaces.push_back(&s6);
    surfaces.push_back(&s7);

    findImagePixels(surfaces, img, cam, points, background, single_light_source, single_light_intensity, phong_exponent);
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Display RGB Array", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // GLEW: load all OpenGL function pointers
    glewInit();

    // build and compile the shaders
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture 
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create the image (RGB Array) to be displayed
    const int width  = img.x_res; // keep it in powers of 2!
    const int height = img.y_res; // keep it in powers of 2!
    unsigned char image[width*height*3];
    for(int j = width-1; j >= 0; j--)
    {
        for (int i = height-1; i >= 0; i--)
        {
            int idx = (j * width + i) * 3;
            image[idx] = (unsigned char) img.pixels[i][j][0]  ; //((i+j) % 2) * 255;
            image[idx+1] = (unsigned char) img.pixels[i][j][1];
            image[idx+2] = (unsigned char) img.pixels[i][j][2];
        }
    }

    unsigned char *data = &image[0];
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
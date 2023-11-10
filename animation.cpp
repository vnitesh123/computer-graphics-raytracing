#include <cmath>
#include<iostream>
#include<vector>
#include <thread>
#include "camera.h"
#include "ray_vector.h"
#include "image.h"
#include "surface.h"
using namespace std;

double pi = 3.14159265359;
double DOUBLE_MAX = numeric_limits<double>::max();

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

void generate_animation(vector<double> pivot, double pd, vector<double> right, int t){
    vector<double> initial_e = {-5,0,0.5};
    double r=5;
    double degrees = t * pi / 180 ;
    vector<double> e_rotated = { (-1*pd*cos(degrees) + pivot[0]), (pd*sin(degrees) + pivot[1]), initial_e[2]};
    vector<double> d = unit_vector(subtract_vectors(pivot, e_rotated));
    vector<double> up_rotated = unit_vector(cross(right, d));
    vector<double> e = e_rotated;
    vector<double> up = up_rotated;
    camera cam = camera(e,up,d, 1, false);
    image img = image(500,500,6,6);

    vector<vector<vector<double>>>points = cam.get_view_plane(img.w, img.h, img.x_res, img.y_res);
    vector<double> c1={2.5,0.5,2.5}; // 0.5 radius
    vector<double> c2={4.5, 0.75, 0.75}; // 1 radius
    
    // Triangle 1
    vector<double> a3 = {3, 0, -2};
    vector<double> b3 = {2, 0, -1};
    vector<double> c3 = {3, 0, -0.5};
    vector<double> d3 = {2.5, 1, -1};


    vector<double>color_green = {0,220,0};
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

    vector<double> ground_normal = {0,1,0};
    ground_normal = unit_vector(ground_normal);

    // light direction
    vector<double> ls= {-1,2,0};
    ls = unit_vector(ls);

    vector<vector<double>> light_sources = {ls};

    // arbitrary point on the ground plane
    vector<double> ground_point ={3,0,2};

    vector<double> light_intensities = {1};
    double phong_exponent = 40;

    

    Sphere s1 = Sphere(c1, 0.5, color_yellow, ka1, kd1, ks1, km1);
    Sphere s2 = Sphere(c2, 0.75, color_blue, ka2, kd2, ks2, km2);
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

    for(int i=0;i<img.x_res;i++){
        for(int j=0;j<img.y_res;j++){
            img.set_pixel(i,j,background);
            IntersectionInfo info = IntersectionInfo(DOUBLE_MAX);
            bool surface_hit = hit_surface(surfaces, points[i][j], d, info);
            if (surface_hit){
                IntersectionInfo new_info = IntersectionInfo(DOUBLE_MAX);
                vector<double> mirror_reflection = {0,0,0};
                if(info.km>0.01){
                    vector<double> r = getMirrorReflection(d,info.normal);
                    bool surface_hit2 = hit_surface(surfaces, info.hit_loc, r, new_info);
                    vector<double> mirror_reflection = {0,0,0};
                    if (surface_hit2){
                        //Lm
                        mirror_reflection = scalar_product(img.calculate_pixel_value_at_point(light_sources, new_info, light_intensities, phong_exponent, r), info.km);
                    } 
                    // La+Ld+Lm
                    img.set_pixel(i,j,add_vectors(img.calculate_pixel_value_at_point(light_sources,info, light_intensities, phong_exponent, cam.d), mirror_reflection));
                    
                } else{
                    img.set_pixel(i,j,img.calculate_pixel_value_at_point(light_sources, info, light_intensities, phong_exponent, cam.d));
                } 
            }

        }
    }

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
        string image_path = "animation_images/";
        image_path = image_path + to_string(t) + ".png";
        const char* file_path = image_path.c_str();
        img.save_image(image, file_path);
    }
}

int main(){
    vector<double> pivot = {3.5,1,0.5};
    vector<double> right={0,0,1};
    double pivot_radius = 5.0;

    // set t_min and t_max for the rotation angle
    int tmin = 30;
    int tmax = 150;
    for(int t = tmin; t< tmax+1; t++){
        generate_animation(pivot, pivot_radius, right, t);
    }
    return 0;
}

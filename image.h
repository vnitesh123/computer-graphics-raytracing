#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include<vector>
#include<math.h>
#include<string>
#include "ray_vector.h"
#include "IntersectionInfo.h"
#include "stb/stb_image_write.h"

class image{
    public:
        // image dimensions
        int x_res; // resolution of the image along x
        int y_res; // resolution of the image along y
        double w; // width of the image
        double h; // height of the image

        vector<vector<vector<double> > > pixels;

        image (int x_res, int y_res, double w, double h){
            this->x_res = x_res;
            this->y_res = y_res;
            this->h = h;
            this->w = w;
            this->pixels = vector<vector<vector<double> > >(x_res, vector<vector<double> >(y_res, vector<double>(3,0)));
        }

        void set_pixel(int i, int j, vector<double> rgb){
            this->pixels[i][j] = rgb;
        }

        vector<double> calculate_pixel_value_at_point(vector<vector<double>> ls, IntersectionInfo info, vector<double> intensities, double phong_exp, vector<double> d){
            vector<double> diffuse_shading = {0,0,0};
            vector<double> ambient_shading = {0,0,0};
            vector<double> specular_shading = {0,0,0};

            for(int i=0;i<ls.size();i++){
                diffuse_shading = add_vectors(diffuse_shading, scalar_product(info.color,max(0.0,dot(ls[i],info.normal)) * intensities[i] * info.kd));
                ambient_shading = add_vectors(ambient_shading, scalar_product(info.color, intensities[i] * info.ka));
                vector<double> bisector = unit_vector(subtract_vectors(ls[i],d));
                specular_shading = add_vectors(specular_shading, scalar_product(info.color, pow(max(0.0,dot(bisector,info.normal)),phong_exp) * intensities[i] * info.ks));
            }

            return add_vectors(add_vectors(diffuse_shading, ambient_shading), specular_shading);
        }


        bool save_image(unsigned char image[], const char* file_path){
            int saved;
            stbi_flip_vertically_on_write(1);
            saved = stbi_write_png(file_path, x_res, y_res, 3, image, x_res*3);
            return saved;
        }

};

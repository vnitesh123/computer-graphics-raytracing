#include<iostream>
#include<vector>
#include<math.h>
#include "ray_vector.h"

class camera{

    public:
        // camera basis

        // camera origin
        vector<double> e;

        // up vector
        vector<double> up;

        // view direction
        vector<double> d;

        // right vector = (d x up) 
        vector<double> right;

        // view plane distance
        double dist;

        // Is perspective camera
        bool is_perspective_camera;

        camera(vector<double> e, vector<double> up, vector<double>d, double dist, bool is_perspective_camera){
            this->d = d;
            this->dist = dist;
            this->e = e;
            this->up = up;
            this->right = cross(d,up);
            this->is_perspective_camera = is_perspective_camera;
        }

        vector<vector<vector<double> > > get_view_plane(double w, double h, double res_x, double res_y){
            vector<double> view_plane_centre = add_vectors(e, scalar_product(d, dist));

            vector<double> view_plane_bottom_left = add_vectors(add_vectors(view_plane_centre, scalar_product(up, -1*h/2)), scalar_product(right, -1*w/2));

            vector<vector<vector<double> > >pixel_points(res_x,vector<vector<double> >(res_y, vector<double>(3)));

            for(int i=0;i<res_x;i++){
                for(int j=0;j<res_y;j++){
                    pixel_points[i][j] = add_vectors(view_plane_bottom_left, scalar_product(right, (i+0.5)*w/res_x));
                    pixel_points[i][j] = add_vectors(pixel_points[i][j], scalar_product(up, (j+0.5)*h/res_y));
                }
            }

            return pixel_points;
        } 

};


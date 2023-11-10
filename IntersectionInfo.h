#ifndef IntersectionInfo_H
#define IntersectionInfo_H

#include "ray_vector.h"
#include<iostream>
#include<vector>
#include<math.h>

class IntersectionInfo{
    public :
        // scalar in e + td at intersection
        double t;

        // point at which ray intersects with object
        vector<double> hit_loc;

        // normal to the surface at intersection point
        vector<double> normal;

        IntersectionInfo(double t){
            this->t = t;
        }

        vector<double> color;
        double ka,kd,ks,km;

        void setInfo(double t, vector<double> hit_loc, vector<double> normal, vector<double> color, double ka, double kd, double ks, double km){
            this->t = t;
            this->normal=normal;
            this->hit_loc = hit_loc;
            this->ka=ka;
            this->kd=kd;
            this->color=color;
            this->ks=ks;
            this->km = km;
        }

};

#endif // IntersectionInfo_H
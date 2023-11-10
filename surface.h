#include "ray_vector.h"
#include<iostream>
#include<vector>
#include<math.h>
#include<map>
#include<string>
#include "IntersectionInfo.h"

using namespace std;

class Surface{
    public:
        virtual bool intersect(vector<double> e, vector<double> d, IntersectionInfo& result)= 0;
        virtual vector<double> getColor()=0;
        virtual ~Surface(){}

};

class Sphere : public Surface{
    public:
        vector<double> c; 
        double r;
        vector<double> color;

        // surface’s ambient coefficient
        double ka;

        // surface’s diffuse coefficient
        double kd; 

        // surface’s specular coefficient
        double ks;

        // surface’s mirror coefficient
        double km;

        Sphere(vector<double> c, double r, vector<double>color, double ka, double kd, double ks, double km){
            this->c = c;
            this->color=color;
            this->r=r;
            this->ka = ka;
            this->kd =kd;
            this->ks = ks;
            this->km = km;
        }
        
        /**
         * To find normal to surface at intersection point
         *
         * @param intersection_point is point on surface of sphere where ray hits.
         * @return unit vector normal to sphere surface at intersection point
         */
        vector<double> normal(vector<double> intersection_point){
            vector<double> n;
            n = unit_vector(subtract_vectors(intersection_point, c));
            return n;
        }
        /**
         * To find if ray intersects sphere
         *
         * @param e represent ray_origin.
         * @param d represent ray_direction.
         * @param result contain closest intersection point info.
         * @return true if ray intersect sphere and intersection point is closer than intersection point in result, else false
         */
        bool intersect(vector<double> e, vector<double> d, IntersectionInfo& result)
        {
                double a = dot(d, d);
                double b = 2 * ( dot(d,e) - dot(d, c) );
                double k = dot(e, e) + dot(c, c) - 2 * dot(e, c) - r * r;
                double disc = ( b * b ) -  ( 4 * a * k );
                if (disc >= 0.0) {
                    double t;
                    t = ( 0.5 * (-1*b - sqrt(disc)) / a);
                    if (t < result.t && t>0.0){
                        vector<double> intersection_point;
                        vector<double> v_normal;
                        intersection_point = add_vectors(e, scalar_product(d, t));
                        v_normal = normal(intersection_point);
                        result.setInfo(t, intersection_point, v_normal, this->color, this->ka, this->kd, this->ks, this->km);
                        return true;
                    }
                };
                return false;
        }

        vector<double> getColor(){
            return this->color;
        }
         

};

class Triangle : public Surface{
    public:
        // vertices of triangle 
        vector<double> a;
        vector<double> b;
        vector<double> c;

        // color of surface
        vector<double> color;
        
        // outward normal to surface
        vector<double> normal;

        // surface’s ambient coefficient
        double ka;

        // surface’s diffuse coefficient
        double kd; 
        
        // surface’s specular coefficient
        double ks;

        // surface’s mirror coefficient
        double km;

        Triangle(vector<double> a, vector<double> b, vector<double> c, vector<double>color, double ka, double kd, double ks, double km){
            this->a = a;
            this->b = b;
            this->c = c;
            this->color=color;
            this->ka = ka;
            this->kd =kd;
            this->ks = ks;
            this->km = km;
            this->normal = unit_vector(cross(subtract_vectors(this->c, this->b), subtract_vectors(this->a, this->b)));
        }

        /**
         * To find if ray intersects sphere
         *
         * @param e represent ray_origin.
         * @param d represent ray_direction.
         * @param result contain closest intersection point info.
         * @return true if ray intersect triangle and intersection point is closer than intersection point in result, else false
         */
        bool intersect(vector<double> e, vector<double> d, IntersectionInfo& result){
            map<string, double> mat;
            mat["a"] = a[0] - b[0];
            mat["b"] = a[1] - b[1];
            mat["c"] = a[2] - b[2];
            mat["d"] = a[0] - c[0];
            mat["e"] = a[1] - c[1];
            mat["f"] = a[2] - c[2];
            mat["g"] = d[0];
            mat["h"] = d[1];
            mat["i"] = d[2];
            mat["j"] = a[0] - e[0];
            mat["k"] = a[1] - e[1];
            mat["l"] = a[2] - e[2];

            // This is value of M calculated in Ray-Triangle Intersection in the Book.
            double eihf = (mat["e"] * mat["i"]) - (mat["h"] * mat["f"]);
            double gfdi = (mat["g"] * mat["f"]) - (mat["d"] * mat["i"]);
            double dheg = (mat["d"] * mat["h"]) - (mat["e"] * mat["g"]);
            double akjb = (mat["a"] * mat["k"]) - (mat["j"] * mat["b"]);
            double jcal = (mat["j"] * mat["c"]) - (mat["a"] * mat["l"]);
            double blkc = (mat["b"] * mat["l"]) - (mat["k"] * mat["c"]);

            double m = mat["a"] * (eihf) + mat["b"] * (gfdi) + mat["c"] * (dheg);

            double t = -1 * (1/m) * (mat["f"] * (akjb) + mat["e"] * (jcal) + mat["d"] * (blkc));

            if (t < 0.0 || t > result.t){
                return false;
            }

            double gamma = (1/m) * (mat["i"] * (akjb) + mat["h"] * (jcal) + mat["g"] * (blkc));

            if ( gamma < 0.0 || gamma > 1.0){
                return false;
            }

            double beta = (1/m) * (mat["j"] * (eihf) + mat["k"] * (gfdi) + mat["l"] * (dheg));

            if (beta < 0.0 || beta > (1.0 - gamma)){
                return false;
            }

            if (t < result.t){
                vector<double> intersection_point;
                intersection_point = add_vectors(e, scalar_product(d, t));
    
                result.setInfo(t, intersection_point, normal, color, ka, kd, ks, km);
                return true;
            }

            return false;
        
        }

        vector<double> getColor(){
        return this->color;
    }

};

class Plane : public Surface {
    public:
        vector<double> color;

        // surface’s ambient coefficient
        double ka;

        // surface’s diffuse coefficient
        double kd; 
        
        // surface’s specular coefficient
        double ks;

        // mirror refection coefficient
        double km;

        // plane's normal
        vector<double> normal;

        // point on a plane
        vector<double> a;

        Plane(vector<double> normal, vector<double> a, vector<double>color, double ka, double kd, double ks, double km){
            this->ka = ka;
            this->kd = kd;
            this->ks = ks;
            this->km = km;
            this->normal = normal;
            this->color = color;
            this->a = a;
        }

        bool intersect(vector<double> e, vector<double> d, IntersectionInfo& result)
        {       
                double denominator = dot(d,this->normal);
                if (denominator == 0.0){
                    return false;
                }
                double numerator = dot(subtract_vectors(this->a,e), this->normal);
                double t = numerator/denominator;
                if (t < result.t && t>0.0001){

                    vector<double> intersection_point = add_vectors(e, scalar_product(d, t));

                    result.setInfo(t, intersection_point, this->normal, this->color, this->ka, this->kd, this->ks, this->km);
                    return true;
                }
                return false;
        }

        vector<double> getColor(){
        return this->color;
        }
};

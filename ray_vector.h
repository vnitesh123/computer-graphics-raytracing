#ifndef ray_vector_H
#define ray_vector_H

#include<iostream>
#include<vector>
#include<math.h>
using namespace std;

vector<double> add_vectors(vector<double> a, vector<double> b){
    vector<double> res = {a[0]+b[0], a[1]+b[1], a[2]+b[2]};
    return res;
}

vector<double> subtract_vectors(vector<double> a, vector<double> b){
    vector<double> res = {a[0]-b[0], a[1]-b[1], a[2]-b[2]};
    return res;
}

double dot(vector<double> a, vector<double> b){
    return ((a[0]*b[0]) + (a[1] *b[1]) + (a[2]*b[2]));
}

double magnitude(vector<double> a){
    return sqrt(dot(a,a));
}

vector<double> unit_vector(vector<double> a){
    double mag = magnitude(a);
    for(int i=0;i<3;i++){
        a[i]/=mag;
    }

    return a;
}

vector<double> scalar_product(vector<double> a, double k){
    for(int i=0;i<3;i++){
        a[i]*=k;
    }
    return a;
}

vector<double> cross(vector<double> a, vector<double> b){
    vector<double> x = {(a[1]*b[2])-(a[2]*b[1]), (a[2]*b[0])-(a[0]*b[2]), (a[0]*b[1])-(a[1]*b[0])};

    return x;
}

// destination point  = p+td
// p - start, t - scalar , d - direction of vector
vector<double> destination(vector<double> p, double t, vector<double> d){
    return add_vectors(p, scalar_product(d,t));
}

#endif // ray_vector_H

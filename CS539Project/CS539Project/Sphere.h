//
//  Sphere.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/29/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#ifndef __CS539Project__Sphere__
#define __CS539Project__Sphere__

#include <stdio.h>
#include "Angel.h"

#define N 4
#define M 16*64*3

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

class Sphere{
private:
    float radius;
    vec3 center;
    vec3 direction;
    point4 spherePoints[M];
    vec3 sphereNormalData[M];
    vec3 sphereNormals[M];
    int sphereIndex = 0;
    
    GLuint modelMatrixLoc, projectionMatrixLoc, vPositionLoc, vNormalLoc, translationLoc, scaleLoc;
    GLuint vao, vbo;
    
    // four equally spaced points on the unit circle
    point4 v[4]= {vec4(0.0, 0.0, 1.0, 1.0), vec4(0.0, 0.942809, -0.333333, 1.0),
        vec4(-0.816497, -0.471405, -0.333333, 1.0),
        vec4(0.816497, -0.471405, -0.333333, 1.0)};
    
    void initSphere(GLuint);
    point4 unit(const point4&);
    void triangle(point4, point4, point4);
    void divide_triangle(point4, point4, point4, int);
    void assignParametricNormals();
    void tetrahedron(int n);
    
public:
    Sphere(vec3, float, vec4, GLuint);
    
    void update();
    void draw(mat4, mat4, GLuint);
    
    vec3 getCenter()    {   return center;  }
    float getRadius()   {   return radius;  }
    
    void setCenter(vec3 c){
        center = c;
    }
};

#endif /* defined(__CS539Project__Sphere__) */

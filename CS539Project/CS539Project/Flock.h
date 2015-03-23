//
//  Flock.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/18/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#ifndef __CS539Project__Flock__
#define __CS539Project__Flock__

#include <stdio.h>
#include "Angel.h"
#include <vector>

struct Boid{
    vec4 velocity;
    vec4 position;
    int key;
    Boid(float x, float y, float z){
        position = vec4(x, y, z, 0);
    }
};

class Flock{
public:
    Flock();
    Flock(int);
    Flock(int, vec3, vec3);
    void updateFlock(float);
    void setDirection(vec4);
    void renderFlock(mat4, mat4);
    void initFlock();
    void checkBounds();
    GLuint vbo,vao;
    GLuint modelMatrixLoc, projectionMatrixLoc;
    GLuint vPositionLoc, vColorLoc, translateLoc, rotateLoc;
    GLuint program;
private:
    int numBoids;
    std::vector<Boid> boids;
    vec4 direction;
    vec3 upperBound, lowerBound;
    vec3 avgBoidPosition;
    vec3 boidModelVertices[4] = {vec3(0,0,0),
                                vec3(-0.5,0,-0.5),
                                vec3(1,0,0),
                                vec3(-0.5,0,0.5)};
    vec4 boidModelColors[4];
    vec4 boidModelColor = vec4(1,0,0,1);
    vec3 updateAvgBoidPos();
    vec4 pickRandomDirection();
    vec4 pickNewDirection(vec4);
    vec4 ruleSeparation(int);// returns a vector to add to velocity
    vec4 ruleAlignment(int);// returns a vector to add to velocity
    vec4 ruleCohesion(int);// returns a vector to add to velocity
};

#endif /* defined(__CS539Project__Flock__) */

//
//  Flock.cpp
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/18/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#include "Flock.h"
#include <vector>
#include <stdlib.h>
#include <time.h>

Flock::Flock(){
    boids.push_back(Boid(2, 2, 2));
    direction = pickRandomDirection();
    upperBound = vec3( 1, 1, 1);
    lowerBound = vec3(-1,-1,-1);
}

Flock::Flock(int num){
    numBoids = num;
    for(int i=0; i<numBoids; i++){
        boids.push_back(Boid(2, 2, 2));
        boids[i].key=i;
    }
    direction = pickRandomDirection();
    upperBound = vec3( 1, 1, 1);
    lowerBound = vec3(-1,-1,-1);
}

Flock::Flock(int num, vec3 UB, vec3 LB){
    numBoids = num;
    for(int i=0; i<numBoids; i++){
        boids.push_back(Boid(8, 3, 8));
        boids[i].key=i;
    }
    direction = pickRandomDirection();
    upperBound = UB;
    lowerBound = LB;
}

vec3 Flock::updateAvgBoidPos(){
    vec4 tmp;
    for(int i=0; i<numBoids; i++){
        tmp += boids[i].position;
    }
    if(numBoids!=0){
        tmp /= (float)numBoids;
    }
    else{
        std::cout << "NumBoids == 0"<<std::endl;
    }
    return vec3(tmp.x, tmp.y, tmp.z);
}

vec4 Flock::pickRandomDirection(){
    //return vec4((rand()%2)-1,(rand()%2)-1,(rand()%2)-1,1);
    return vec4(1,0,0,1);
}

vec4 Flock::pickNewDirection(vec4 oldDirection){
    //srand(time(NULL));
    //return (-oldDirection)+vec4(((rand()%1)-0.5), ((rand()%1)-0.5), ((rand()%1)-0.5), 0);
    vec4 output =normalize(vec4(8,3,8,1)-avgBoidPosition);
    output.w =1;
    return output;
}

void Flock::setDirection(vec4 newDir){
    direction = newDir;
}

void Flock::checkBounds(){
    /*for(int i =0; i<numBoids; i++){
        if(boids[i].position.x>upperBound.x || boids[i].position.x<lowerBound.x ||
           boids[i].position.y>upperBound.y || boids[i].position.y<lowerBound.y ||
           boids[i].position.z>upperBound.z || boids[i].position.z<lowerBound.z){
            //direction = normalize(vec4(8,3,8,1)-boids[i].position);
            //direction.w = 1;
            setDirection(-direction);
            std::cout<<"New Direction: "<<direction<<std::endl;
        }
    }*/
    
    std::cout<<" *** AVERAGE BOID POSITION: " <<avgBoidPosition<< " *** "<<std::endl;
    if(avgBoidPosition.x>upperBound.x || avgBoidPosition.x<lowerBound.x ||
       avgBoidPosition.y>upperBound.y || avgBoidPosition.y<lowerBound.y ||
       avgBoidPosition.z>upperBound.z || avgBoidPosition.z<lowerBound.z){
        //direction = normalize(vec4(8,3,8,1)-boids[i].position);
        //direction.w = 1;
        direction = pickNewDirection(direction);
        std::cout<<"New Direction: "<<direction<<std::endl;
    }

    
}

void Flock::updateFlock(float deltaTime){
    avgBoidPosition = updateAvgBoidPos();
    if(count%10 == 0){
        checkBounds();
    }
    count++;
    for(int i=0; i < numBoids; i++){
        vec4 separation = ruleSeparation(i);
        //std::cout<<"boid #" << boids[i].key << " Separation: " << '\t' << separation << std::endl;
        vec4 alignment = ruleAlignment(i);
        //std::cout<<"boid #" << boids[i].key << " Alignment: " << '\t' << alignment << std::endl;
        vec4 cohesion = ruleCohesion(i);
        //std::cout<<"boid #" << boids[i].key << " Cohesion: " << '\t' << cohesion << std::endl;
        
        boids[i].velocity += separation;
        boids[i].velocity += alignment;
        boids[i].velocity += cohesion;
        boids[i].velocity.w = 1;
        
        //std::cout<<"boid #" << boids[i].key << " Velocity: " << '\t' << boids[i].velocity << std::endl;
        boids[i].position += normalize(boids[i].velocity*deltaTime);
        boids[i].position.w = 0;
        std::cout<<"boid #" << boids[i].key << " POSITION: " << '\t' << boids[i].position <<std::endl;
        //std::cout<<std::endl;
    }
    std::cout<<std::endl;
}


vec4 Flock::ruleSeparation(int index){
    int closestBoid = index;
    float closestDistance = INFINITY;
    float curDist;
    for(int j=0; j< numBoids; j++){
        if(j!=index){
            curDist = sqrtf(powf(boids[j].position.x-boids[index].position.x, 2)+
                            powf(boids[j].position.y-boids[index].position.y, 2)+
                            powf(boids[j].position.z-boids[index].position.z, 2));
            if(curDist <= closestDistance){
                closestBoid = j;
                closestDistance = curDist;
            }
        }
    }
    //std::cout<< "boid #" << boids[index].key<< " separation:"<<'\t'<<normalize(-boids[closestBoid].position)*closestDistance<<std::endl;
    vec4 output = normalize(-boids[closestBoid].position)*closestDistance;
    output.w = 1;
    return output;
}

vec4 Flock::ruleAlignment(int index){
    vec4 center = vec4(8,3,8,0);
    /*float distToCenter =sqrtf(powf(center.x-boids[index].position.x, 2)+
                              powf(center.y-boids[index].position.y, 2)+
                              powf(center.z-boids[index].position.z, 2));*/
    
    float distToCenter = length(center-boids[index].position);
    //std::cout<<" ** DISTANCE TO CENTER: " << distToCenter <<" ** "<<std::endl;
    if(distToCenter == 0){
        return direction;
    }
    else{
        return vec4(direction.x*distToCenter, direction.y*distToCenter, direction.z*distToCenter, 1);
    }
}

vec4 Flock::ruleCohesion(int index){
    float distToAvgPos = sqrtf(powf(avgBoidPosition.x-boids[index].position.x, 2)+
                               powf(avgBoidPosition.y-boids[index].position.y, 2)+
                               powf(avgBoidPosition.z-boids[index].position.z, 2));
    //std::cout<< "boid #" << boids[index].key<< " Cohesion:"<<'\t'<<distToAvgPos*0.5<< " to "<<avgBoidPosition<<std::endl;
    return avgBoidPosition*distToAvgPos*0.1; //??
}


void Flock::initFlock(){
    program = InitShader("flock_vShader.glsl", "flock_fShader.glsl");
    modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
    projectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");
    vPositionLoc = glGetAttribLocation(program, "vPosition");
    vColorLoc = glGetAttribLocation(program, "vColor");
    translateLoc = glGetUniformLocation(program, "translation");
    rotateLoc = glGetUniformLocation(program, "rotation");
    
    for(int i=0; i<4; i++){
        boidModelColors[i] = boidModelColor;
    }
    
    glGenBuffers(1, &vbo);
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*4, boidModelVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vPositionLoc);
    glVertexAttribPointer(vPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vColorLoc);
    glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec3)*4));
    
    
    //notdone

    glBindVertexArrayAPPLE(0);
}

void Flock::renderFlock(mat4 modelMatrix, mat4 projectionMatrix){
    glUseProgram(program);
    
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix);
    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_TRUE, projectionMatrix);
    // Loop through boids
    glBindVertexArrayAPPLE(vao);
    for(int i=0; i<numBoids; i++){
        //bind translate
        //bind rotatel
        //draw!
        glUniformMatrix4fv(translateLoc, 1, GL_TRUE, Translate(boids[i].position));
        //rotate???
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }
    glBindVertexArrayAPPLE(0);
}



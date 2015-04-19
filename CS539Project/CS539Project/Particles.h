//
//  Particles.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 4/17/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#ifndef CS539Project_Particles_h
#define CS539Project_Particles_h

#include <iostream>
#include "Angel.h"
#include <vector>
#include <stdlib.h>
#include <time.h>


struct Particle{
    vec3 initVel;
    vec3 initPos;
    vec3 accel;
    float life;
    float ttl;
    
    Particle(){
        initVel = vec3(0,0,0);
        initPos = vec3(0,0,0);
        accel = vec3(0,0,0);
        life = 0;
        ttl = 0;
    }
    
    Particle(vec3 v, vec3 p, vec3 a, float l, float t){
        initVel = v;
        initPos = p;
        accel = a;
        life = l;
        ttl = t;
    }
    
    void Update(float dT){
        life += dT;
    }
    // current pos = initPos + initVel*life + (0.5)*accel*life*life;
};


class ParticleEffect{
protected:
    std::vector<Particle> parts;
    int maxParticles;
    int rate;
    int numCreated;
    
    GLuint modelMatrixLoc, projectionMatrixLoc, initVelLoc, initPosLoc, accelLoc, lifeLoc;
    GLuint vao, vbo;

    int count;
    Particle createParticle(){
        srand(time(NULL)+rand()%100);
        int thetai = rand()%360;
        float thetaf = thetai * (3.1415926/180.0);
        float radius = 10.0;
        vec3 pos = vec3(8,1,8);
        vec3 vel = vec3(radius*cos(thetaf), 10, radius*sin(thetaf));
        vec3 acc = vec3(2, -50, 2);
        float ttl = 0.5 + (rand()%100)/1000;
        return Particle(vel, pos, acc, 0, ttl);
    }
    
public:
    ParticleEffect(int max, int r){
        maxParticles = max;
        rate = r;
        for(int i=0; i<maxParticles; i++){
            parts.push_back(createParticle());
        }
    }
    
    void initParticleEffet(GLuint program){
        modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
        projectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");
        initVelLoc = glGetUniformLocation(program, "initVel");
        initPosLoc = glGetUniformLocation(program, "initPos");
        accelLoc = glGetUniformLocation(program, "accel");
        lifeLoc = glGetAttribLocation(program, "life");

        
        glGenBuffers(1, &vbo);
        glGenVertexArraysAPPLE(1, &vao);
        glBindVertexArrayAPPLE(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float), NULL, GL_STATIC_DRAW);

        
        glEnableVertexAttribArray(lifeLoc);
        glVertexAttribPointer(lifeLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
        

        
        glBindVertexArrayAPPLE(0);

    }

    
    void Update(float dT){
        for(int i=0; i<parts.size(); i++){
            parts[i].Update(dT);
            if(parts[i].life > parts[i].ttl){
                //std::cout<< "NEW PARTICLE~ "<<count <<std::endl;
                //count++;
                parts.erase(parts.begin() + i);
                //parts.push_back(createParticle());
                i--;
            }
        }
        for(int i=0; i <= rate; i++){
            if(parts.size() < maxParticles){
                parts.push_back(createParticle());
            }
        }
    }
    
    void Draw(mat4 modelMatrix, mat4 projectionMatrix, GLuint program){
        glUseProgram(program);
        //        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //        glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vec3)*parts.size(), &parts[0]);

        //std::cout<< "Life: " <<parts[0].life<<std::endl;
        for(int i = 0; i<parts.size(); i++){
            glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix);
            glUniformMatrix4fv(projectionMatrixLoc, 1, GL_TRUE, projectionMatrix);
            glUniform3fv(initVelLoc, 1, parts[i].initVel);
            glUniform3fv(initPosLoc, 1, parts[i].initPos);
            glUniform3fv(accelLoc, 1, parts[i].accel);
            
            glPointSize(10);

            glBindVertexArrayAPPLE(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            float data[] = {parts[i].life};
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float), data);
            glDrawArrays(GL_POINTS, 0, 10);
            glBindVertexArrayAPPLE(0);

        }
        
//        glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix);
//        glUniformMatrix4fv(projectionMatrixLoc, 1, GL_TRUE, projectionMatrix);
//        
//        
//        glBindVertexArrayAPPLE(vao);
//        glDrawArrays(GL_POINTS, 0, 1);
//        glBindVertexArrayAPPLE(0);

    }
};



#endif

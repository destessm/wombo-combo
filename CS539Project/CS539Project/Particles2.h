//
//  Particles2.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 4/24/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#ifndef CS539Project_Particles2_h
#define CS539Project_Particles2_h

#include <iostream>
#include "Angel.h"
#include <vector>
#include <stdlib.h>
#include <time.h>

struct Particle{
    vec3 curPosit, curAccel, curColor;
    float curOpaci, curLife, TTL;
    vec3 initPos, initVel;
    
    Particle(){
        curPosit = vec3(0,0,0);
        //curVeloc = vec3(0,0,0);
        curAccel = vec3(0,0,0);
        curColor = vec3(0,0,0);
        curOpaci = 0;
        curLife = 0;
        TTL = 0;
    }
    
    Particle(vec3 initPos, vec3 initVel, vec3 initAcc, vec3 initCol, float initOpa, float ttl){
        curPosit = initPos;
        //curVeloc = initVel;
        curAccel = initAcc;
        curColor = initCol;
        curOpaci = initOpa;
        curLife = 0;
        TTL = ttl;
    }
};

class ParticleData{
public:
    ParticleData(){
    }
    std::vector<Particle> data;
    std::vector<vec3> positions;
    void updataPositions(){
        positions.clear();
        for(int i=0; i<data.size(); i++){
            positions.push_back(data[i].curPosit);
        }
    }
};


class ParticleController{
protected:
    vec3 initVel, initPos, initAcc, initCol;
    float initOpa;
    vec3 finalAcc, finalCol;
    float finalOpa, ttl;
    int rate;
    
    Particle createParticle(){
        srand(time(NULL)+rand()%1000);
        int thetai = rand()%360;
        float thetaf = thetai * (3.1415926/180.0);
        float radius = 10.0;
        vec3 pos = vec3(8,1,8);
        vec3 vel = vec3(radius*cos(thetaf), 15, radius*sin(thetaf));
        vec3 acc = vec3(2, -50, 2);
        float ttl = 1 + (rand()%100)/100;
        vec3 col = vec3(0,0,1);
        return Particle(pos, vel, acc, col, 1.0, ttl);
    }
public:
    int maxParticles;
    ParticleController(vec3 iV, vec3 iP, vec3 iA, vec3 fA, vec3 iC, vec3 fC, float iO, float fO, float t, int max, int r){
        initVel  = iV;
        //finalVel = fV;
        initPos  = iP;
        //finalPos = fP;
        initAcc  = iA;
        finalAcc = fA;
        initCol  = iC;
        finalCol = fC;
        initOpa  = iO;
        finalOpa = fO;
        ttl = t;
        maxParticles = max;
        rate = r;
    }
    void Update(float dT, ParticleData pd){
        for(int i=0; i<pd.data.size(); i++){
            pd.data[i].curLife += dT;
            if(pd.data[i].curLife >= pd.data[i].TTL){
                //if the particle needs to die
                pd.data.erase(pd.data.begin()+i);
                i--;
            }
        }
        
        for(int i=0; i<pd.data.size(); i++){
            //now update all fields
            pd.data[i].curPosit = (1.0/6.0)*(initAcc-finalAcc)*(0-pd.data[i].TTL)*pd.data[i].curLife*pd.data[i].curLife*pd.data[i].curLife + (0.5)*initAcc*pd.data[i].curLife*pd.data[i].curLife + initVel*pd.data[i].curLife + pd.data[i].initPos;
        }
        
        for(int i=0; i <= rate; i++){
            if(pd.data.size() < maxParticles){
                pd.data.push_back(createParticle());
            }
        }
        
        pd.updataPositions();
    }
};

class ParticleRenderer{
protected:
    GLuint modelMatrixLoc, projectionMatrixLoc, initVelLoc, initPosLoc, accelLoc, lifeLoc;
    GLuint vao, vbo;
public:
    ParticleRenderer(){
        
    }
    void initRenderer(ParticleController pc, GLuint program){
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*pc.maxParticles, NULL, GL_STATIC_DRAW);
        
        
        glEnableVertexAttribArray(lifeLoc);
        glVertexAttribPointer(lifeLoc, 1, GL_FLOAT, GL_FALSE, 0, 0);
        
        glBindVertexArrayAPPLE(0);

    }
    void render(ParticleData pd, GLuint program, mat4 modelMatrix, mat4 projectionMatrix){
        glUseProgram(program);
        //        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //        glBufferSubDataARB(GL_ARRAY_BUFFER, 0, sizeof(vec3)*parts.size(), &parts[0]);
        
        //std::cout<< "Life: " <<parts[0].life<<std::endl;
        for(int i = 0; i<pd.data.size(); i++){
            glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix);
            glUniformMatrix4fv(projectionMatrixLoc, 1, GL_TRUE, projectionMatrix);
            
            //glPointSize(2);
            
            glBindVertexArrayAPPLE(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3)*pd.positions.size(), &pd.positions[0]);
            glDrawArrays(GL_POINTS, 0, pd.positions.size());
            glBindVertexArrayAPPLE(0);
        }
    }
};




#endif

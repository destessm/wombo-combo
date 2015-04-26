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
#include "stb_image.h"

struct Particle{
    vec3 curPosit, curAccel;
    vec4 curColor;
    float curLife, TTL;
    vec3 initPos, initAcc, initVel, finalAcc;
    vec4 initCol;
    Particle(){
        curPosit = vec3(0,0,0);
        initPos = curPosit;
        curAccel = vec3(0,0,0);
        initAcc = curAccel;
        curColor = vec4(0,0,0,0);
        initCol = curColor;
        curLife = 0;
        TTL = 0;
    }
    Particle(vec3 iP, vec3 iV, vec3 iA, vec3 fA, vec4 iC, float ttl){
        curPosit = iP;
        initPos = iP;
        initVel = iV;
        curAccel = iA;
        initAcc = iA;
        finalAcc = fA;
        curColor = iC;
        initCol = iC;
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
    std::vector<vec3> quadPositions;
    std::vector<vec4> colors;
    std::vector<vec4> quadColors;
    std::vector<vec2> textureCoords;
    float texSize = 0.1;
    
    void updateData(vec4 eye, vec4 at, vec4 up){
//        positions.clear();
//        colors.clear();
//        for(int i=0; i<data.size(); i++){
//            positions.push_back(data[i].curPosit);
//            colors.push_back(data[i].curColor);
//        }
        vec4 right4 = cross(at-eye, up);
        vec3 right = vec3(right4.x, right4.y, right4.z);
        vec3 up3 = vec3(up.x, up.y, up.z);
        quadPositions.clear();
        quadColors.clear();
        textureCoords.clear();
        for(int i=0; i<data.size(); i++){
            //quadPositions.push_back(vec3(data[i].curPosit.x, data[i].curPosit.y-texSize, data[i].curPosit.z));
            quadPositions.push_back(data[i].curPosit - normalize(up3)*texSize);
            textureCoords.push_back(vec2(0,0));
            //quadPositions.push_back(vec3(data[i].curPosit.x+texSize, data[i].curPosit.y, data[i].curPosit.z));
            quadPositions.push_back(data[i].curPosit + normalize(right)*texSize);
            textureCoords.push_back(vec2(1,1));
            quadPositions.push_back(data[i].curPosit);
            textureCoords.push_back(vec2(1,0));
            
            //quadPositions.push_back(vec3(data[i].curPosit.x, data[i].curPosit.y-texSize, data[i].curPosit.z));
            quadPositions.push_back(data[i].curPosit - normalize(up3)*texSize);
            textureCoords.push_back(vec2(0,0));
            //quadPositions.push_back(vec3(data[i].curPosit.x+texSize, data[i].curPosit.y-texSize, data[i].curPosit.z));
            quadPositions.push_back((data[i].curPosit + normalize(right)*texSize) - normalize(up3)*texSize);
            textureCoords.push_back(vec2(0,1));
            //quadPositions.push_back(vec3(data[i].curPosit.x+texSize, data[i].curPosit.y, data[i].curPosit.z));
            quadPositions.push_back(data[i].curPosit + normalize(right)*texSize);
            textureCoords.push_back(vec2(1,1));
            
            colors.push_back(data[i].curColor);
            colors.push_back(data[i].curColor);
            colors.push_back(data[i].curColor);
            
            colors.push_back(data[i].curColor);
            colors.push_back(data[i].curColor);
            colors.push_back(data[i].curColor);
        }
    }
    
    void updateLife(int index, float newLife){
        if(index > data.size() || index <0){
            std::cout<<"[life]  bad index: "<< index <<std::endl;
        }
        else{
            data[index].curLife = newLife;
        }
    }
    void updatePosition(int index, vec3 newPosition){
        if(index > data.size() || index <0){
            std::cout<<"[pos]   bad index: "<< index <<std::endl;
        }
        else{
            data[index].curPosit = newPosition;
        }
    }
    void updateColor(int index, vec4 newColor){
        if(index > data.size() || index <0){
            std::cout<<"[color] bad index: "<< index <<std::endl;
        }
        else{
            data[index].curColor = newColor;
        }
    }
};


class ParticleController{
protected:
    vec3 initVel, initPos, initAcc;
    float initOpa;
    vec3 finalAcc;
    vec4 initCol, finalCol;
    float ttl;
    
public:
    int maxParticles;
    int rate;
    ParticleController(vec3 iV, vec3 iP, vec3 iA, vec3 fA, vec4 iC, vec4 fC, float t, int max, int r){
        initVel  = iV;
        initPos  = iP;
        initAcc  = iA;
        finalAcc = fA;
        initCol  = iC;
        finalCol = fC;
        ttl = t;
        maxParticles = max;
        rate = r;
    }
    ParticleController(){}
    void initController(vec4 iC, vec4 fC, int max, int r){
        initCol = iC;
        finalCol = fC;
        maxParticles = max;
        rate = r;

    }
    void Update(float dT, ParticleData pd, vec4 eye, vec4 at, vec4 up){
        //std::cout<<"Delta Time: " << dT <<std::endl;
        if(pd.data.size() == 0){
            // bang in some particles if empty
            for(int i=0; i<maxParticles/2; i++){
                pd.data.push_back(createParticle());
            }
        }
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
            //pd.data[i].curPosit = (1.0/6.0)*(initAcc-finalAcc)*(0-pd.data[i].TTL)*pd.data[i].curLife*pd.data[i].curLife*pd.data[i].curLife + (0.5)*initAcc*pd.data[i].curLife*pd.data[i].curLife + initVel*pd.data[i].curLife + pd.data[i].initPos;
            
            pd.data[i].curPosit = pd.data[i].initPos + pd.data[i].initVel*pd.data[i].curLife + (0.5)*pd.data[i].initAcc*pd.data[i].curLife*pd.data[i].curLife;
            pd.data[i].curColor = initCol + (finalCol-initCol)*(pd.data[i].curLife/pd.data[i].TTL);
        }
        for(int i=0; i <= rate; i++){
            if(pd.data.size() < maxParticles){
                pd.data.push_back(createParticle());
            }
        }
        pd.updateData(eye, at, up);
    }
    
    float updateLife(float dT, Particle p){
        return p.curLife + dT;
    }
    
    vec3 updatePosition(Particle p){
        //return p.initPos + p.initVel*p.curLife + (0.5)*p.initAcc*p.curLife*p.curLife;
        
        //pd.data[i].curPosit = (1.0/6.0)*(initAcc-finalAcc)*(0-pd.data[i].TTL)*pd.data[i].curLife*pd.data[i].curLife*pd.data[i].curLife + (0.5)*initAcc*pd.data[i].curLife*pd.data[i].curLife + initVel*pd.data[i].curLife + pd.data[i].initPos;

        return (1.0/6.0)*(p.initAcc-p.finalAcc)*(0-p.TTL)*p.curLife*p.curLife*p.curLife + (0.5)*p.initAcc*p.curLife*p.curLife + p.initVel*p.curLife + p.initPos;
    }
    
    vec4 updateColor(Particle p){
        float r = p.initCol.x + (finalCol.x-initCol.x)*(p.curLife/p.TTL);
        if(r > 1){ r = 1; }
        float g = p.initCol.y + (finalCol.y-initCol.y)*(p.curLife/p.TTL);
        if(g > 1){ g = 1; }
        float b = p.initCol.z + (finalCol.z-initCol.z)*(p.curLife/p.TTL);
        if(b > 1){ b = 1; }
        float a = p.initCol.w + (finalCol.w-initCol.w)*(p.curLife/p.TTL);
        if(a > 1){ a = 1; }
        
        //std::cout<< "R G B A : " << r << " " << g << " " << b << " " << a <<std::endl;
        
        return vec4(r, g, b, a);
    }
    
    Particle createParticle(){
        srand(time(NULL)+rand()%1000);
        int thetai = rand()%360;
        float thetaf = thetai * (3.1415926/180.0);
        float radius = 3.0;
        vec3 pos = vec3(8-0.1*sin(thetaf), 2, 8+0.1*cos(thetaf));
        vec3 vel = vec3(radius*cos(thetaf), 2, radius*sin(thetaf));
        vec3 iAcc = vec3(radius*cos(thetaf),2,radius*sin(thetaf));
        vec3 fAcc = vec3(3*radius*cos(thetaf-3.1415926), -5, 3*radius*sin(thetaf-3.1415926)); // flat spiral

        
        

        float ttl = 2 + (rand()%100)/100;
        //vec4 col = vec4(0,0,1, 1);
        return Particle(pos, vel, iAcc, fAcc, initCol, ttl);
    }

};

class ParticleRenderer{
protected:
    GLuint modelMatrixLoc, projectionMatrixLoc, vColorLoc, vPositionLoc, texture, vTextureCoordsLoc;
    GLuint vao, vbo;
    GLubyte* particleTexture;
    int imgHeight, imgWidth, imgChannels;

public:
    ParticleRenderer(){
        //doesn't need anything
    }
    void initRenderer(ParticleController pc, GLuint program){
        
        modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
        projectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");
        vPositionLoc = glGetAttribLocation(program, "vPosition");
        vColorLoc = glGetAttribLocation(program, "vColor");
        vTextureCoordsLoc = glGetAttribLocation(program, "vTexCoords");
        
        particleTexture = glmReadPPM("particleTexture.ppm", &imgWidth, &imgHeight);
        //particleTexture = stbi_load("particleImage.png", &imgWidth, &imgHeight, &imgChannels, 0);
        glGenTextures(1, &texture);
        
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, particleTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
        glActiveTexture(GL_TEXTURE4); //?
        
        glGenBuffers(1, &vbo);
        glGenVertexArraysAPPLE(1, &vao);
        glBindVertexArrayAPPLE(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*pc.maxParticles*4 + sizeof(vec4)*pc.maxParticles*4+sizeof(vec2)*pc.maxParticles*4, NULL, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(vPositionLoc);
        glVertexAttribPointer(vPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vColorLoc);
        glVertexAttribPointer(vColorLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(vTextureCoordsLoc);
        glVertexAttribPointer(vTextureCoordsLoc, 2, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArrayAPPLE(0);

    }
    void render(ParticleData pd, GLuint program, mat4 modelMatrix, mat4 projectionMatrix){

        std::cout<< "current size: " <<pd.data.size() <<std::endl;
        
        glUseProgram(program);
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix);
        glUniformMatrix4fv(projectionMatrixLoc, 1, GL_TRUE, projectionMatrix);
        
        glUniform1i(glGetUniformLocation(program, "texture"), 4);
        
        glBindTexture(GL_TEXTURE_2D, texture);

        
        glPointSize(1);
            
        glBindVertexArrayAPPLE(vao);
        

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3)*pd.quadPositions.size(), &pd.quadPositions[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3)*pd.quadPositions.size(), sizeof(vec4)*pd.quadColors.size(), &pd.quadColors[0]);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3)*pd.quadPositions.size()+sizeof(vec4)*pd.quadColors.size(), sizeof(vec2)*pd.textureCoords.size(), &pd.textureCoords[0]);

        glDrawArrays(GL_TRIANGLES, 0, pd.quadPositions.size());
        glBindVertexArrayAPPLE(0);
        
        
    }
};


class ParticleEffect{
protected:
    ParticleData particledata;
    ParticleController controller;
    ParticleRenderer renderer;
    float timeToCreatePartciles = 5.0;
    float currentTime = 0.0;
public:
    ParticleEffect(){
        //vec3 none = vec3(0,0,0);
        particledata = ParticleData();
        controller = ParticleController();
        controller.initController(vec4(1,0,0,1), vec4(1,1,0,1), 25000, 200);
        renderer = ParticleRenderer();
    }
    void init(GLuint program){
        renderer.initRenderer(controller, program);
    }
    void update(float dT, vec4 eye, vec4 at, vec4 up){
        //std::cout<<"dT: " << dT <<std::endl;
        currentTime += dT;
        std::vector<int> toBeDeleted;
        for(int i=0; i< particledata.data.size(); i++){
            particledata.updateLife(i, controller.updateLife(dT, particledata.data[i]));
            if(particledata.data[i].curLife > particledata.data[i].TTL){
                toBeDeleted.push_back(i);
            }
            particledata.updatePosition(i, controller.updatePosition(particledata.data[i]));
            particledata.updateColor(i, controller.updateColor(particledata.data[i]));
        }
        for(int i=0; i<toBeDeleted.size(); i++){
            particledata.data.erase(particledata.data.begin()+toBeDeleted[i]);
        }
        if(currentTime < timeToCreatePartciles){
            for(int i=0; i<controller.rate; i++){
                if(particledata.data.size() < controller.maxParticles){
                    particledata.data.push_back(controller.createParticle());
                }
            }
        }
        else{
            currentTime -= timeToCreatePartciles;
        }
        particledata.updateData(eye, at, up);
    }
    
    void render(GLuint program, mat4 modelMatrix, mat4 projectionMatrix){
        renderer.render(particledata, program, modelMatrix, projectionMatrix);
    }
};


#endif

//
//  Sphere.cpp
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/29/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#include "Sphere.h"

Sphere::Sphere(vec3 position, float rad, vec4 dir, GLuint program){
    divide_triangle(v[0], v[1], v[2], N);
    divide_triangle(v[3], v[2], v[1], N );
    divide_triangle(v[0], v[3], v[1], N );
    divide_triangle(v[0], v[3], v[2], N );
    assignParametricNormals();
    center = position;
    radius = rad;
    direction = vec3(dir.x, dir.y, dir.z);
    initSphere(program);
}

// move a point to unit circle
point4 Sphere::unit(const point4 &p)
{
    point4 c;
    double d=0.0;
    for(int i=0; i<3; i++) d+=p[i]*p[i];
    d=sqrt(d);
    if(d > 0.0) for(int i=0; i<3; i++) c[i] = p[i]/d;
    c[3] = 1.0;
    return c;
}

void Sphere::triangle( point4  a, point4 b, point4 c)
{
    // define which color will be used for this triangle
    vec3 curNormal = normalize(cross((b-a),(c-a)));
    
    // store vertice, store normal, increment indices
    spherePoints[sphereIndex]= a;
    sphereNormalData[sphereIndex] = curNormal;
    sphereIndex++;
    
    
    spherePoints[sphereIndex] = b;
    sphereNormalData[sphereIndex] = curNormal;
    sphereIndex++;
    
    spherePoints[sphereIndex] = c;
    sphereNormalData[sphereIndex] = curNormal;
    sphereIndex++;
}


void Sphere::divide_triangle(point4 a, point4 b, point4 c, int n)
{
    point4 v1, v2, v3;
    if(n>0)
    {
        v1 = unit(a + b);
        v2 = unit(a + c);
        v3 = unit(b + c);
        divide_triangle(a ,v2, v1, n-1);
        divide_triangle(c ,v3, v2, n-1);
        divide_triangle(b ,v1, v3, n-1);
        divide_triangle(v1 ,v2, v3, n-1);
    }
    else triangle(a, b, c);
}

void Sphere::assignParametricNormals(){
    for(int i = 0; i < M; i++){
        sphereNormals[i] = normalize(vec3(spherePoints[i].x, spherePoints[i].y, spherePoints[i].z));
    }
}

void Sphere::initSphere(GLuint program){
    //prog = InitShader("sphere_vShader.glsl", "sphere_fShader.glsl");
    modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
    projectionMatrixLoc = glGetUniformLocation(program, "projectionMatrix");
    vPositionLoc = glGetAttribLocation(program, "vPosition");
    vNormalLoc = glGetAttribLocation(program, "vNormal");
    translationLoc = glGetUniformLocation(program, "translation");
    scaleLoc = glGetUniformLocation(program, "scale");

    glGenBuffers(1, &vbo);
    glGenVertexArraysAPPLE(1, &vao);
    glBindVertexArrayAPPLE(vao);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*M+sizeof(vec3)*M, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)*M, spherePoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec4)*M, sizeof(vec3)*M, sphereNormals);
    
    glEnableVertexAttribArray(vPositionLoc);
    glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vNormalLoc);
    glVertexAttribPointer(vNormalLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(vec3)*4));
    
    glBindVertexArrayAPPLE(0);
}

void Sphere::update(){
    center += normalize(direction)*0.1;
}


void Sphere::draw(mat4 modelMatrix, mat4 projectionMatrix, GLuint program){
    glUseProgram(program);
    
    glUniformMatrix4fv(modelMatrixLoc, 1, GL_TRUE, modelMatrix);
    glUniformMatrix4fv(projectionMatrixLoc, 1, GL_TRUE, projectionMatrix);
    
    glUniform3fv(translationLoc, 1, center);
    glUniform1f(scaleLoc, radius);
    
    
    glBindVertexArrayAPPLE(vao);
    glDrawArrays(GL_TRIANGLES, 0, M);
    glBindVertexArrayAPPLE(0);
}

void Sphere::stop(){
    direction = vec3(0,0,0);
}

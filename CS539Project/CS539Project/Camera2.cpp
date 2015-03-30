//
//  Camera2.cpp
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/29/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#include "Camera2.h"

const double Camera::TO_RADS = 3.141592654 / 180.0; // value of 1 degree in radians


Camera::Camera(){
    initCamera();
}

Camera::~Camera(){
    
}

const double Camera::toRads(const double &theAngleInDegrees) const{
    return theAngleInDegrees * TO_RADS;
}

void Camera::initCamera(){
    eye = vec3(0,2,8);
    at = vec3(1,2,8);
    rotation = vec3(0,0,0);
    angle = 0;
}

void Camera::moveForward(){
    eye.x += float(sin(toRads(rotation.y)));
    eye.y -= float(cos(toRads(rotation.y)));
    eye.z -= float(sin(toRads(rotation.x)));
}

void Camera::moveBackward(){
    eye.x -= float(sin(toRads(rotation.y)));
    eye.y += float(cos(toRads(rotation.y)));
    eye.z += float(sin(toRads(rotation.x)));
}

void Camera::turnRight(){
    eye.x += float(cos(toRads(rotation.y))) * 0.2;
    eye.z += float(sin(toRads(rotation.y))) * 0.2;
}

void Camera::turnLeft(){
    eye.x -= float(cos(toRads(rotation.y))) * 0.2;
    eye.z -= float(sin(toRads(rotation.y))) * 0.2;
}

void Camera::xup(){
    rotation.x += 1;
    if(rotation.x > 360)
        rotation.x-= 360;
}

void Camera::xdown(){
    rotation.x -= 1;
    if(rotation.x < 0)
        rotation.x += 360;
}

void Camera::mouseMovement(int x, int y){
    int diffx = x-lastx;
    int diffy = y-lasty;
    lastx = x;
    lasty = y;
    rotation.x += (float) diffy;
    rotation.y += (float) diffx;
    at.x = eye.x+rotation.x;
    at.y = eye.y+rotation.y;
}





//
//  Camera.cpp
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/28/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#include "Camera.h"

const double Camera::TO_RADS = 3.141592654 / 180.0; // value of 1 degree in radians

Camera::Camera(float theWindowWidth, float theWindowHeight){
    initCamera();
    windowWidth = theWindowWidth;
    windowHeight = theWindowHeight;
    
    windowMidX = windowWidth / 2.0f;
    windowMidY = windowHeight / 2.0f;
    
    //set mouse position to center of window??
}

Camera::~Camera(){}

void Camera::initCamera(){
    position = vec3(0,0,0);
    rotation = vec3(0,0,0);
    speed = vec3(0,0,0);
    at = vec3(1,0,0);
    
    movementSpeedFactor = 100.0;
    
    pitchSensitivity = 0.2;
    yawSensitivity = 0.2;
    
    holdingForward = false;
    holdingBackward = false;
    holdingLeftStrafe = false;
    holdingRightStrafe = false;
}

const double Camera::toRads(const double &theAngleInDegrees) const{
    return theAngleInDegrees * TO_RADS;
}

void Camera::handleMouseMove(int mouseX, int mouseY){
    double horizMovement = (mouseX-windowMidX+1)*yawSensitivity;
    double vertMovement  = (mouseY-windowMidY)*pitchSensitivity;
    
    std::cout << "Mid window values: " << windowMidX << "\t" << windowMidY << std::endl;
    std::cout << "Mouse values     : " << mouseX << "\t" << mouseY << std::endl;
    std::cout << horizMovement << "\t" << vertMovement << std::endl << std::endl;
    
    rotation.x += vertMovement;
    rotation.y += horizMovement;
    
    //limit looking up vertically
    if(rotation.x < -90){
        rotation.x = -90;
    }
    
    //limit looking down vertically
    if(rotation.x > 90){
        rotation.x = 90;
    }
    
    
    if(rotation.y <0){
        rotation.y += 360;
    }
    
    if(rotation.y >360){
        rotation.y -= 360;
    }
    
    at = rotation; //??
    
    // reset mouse position to center???
}

void Camera::move(double deltaTime){
    vec3 movement;
    vec3 atMovement;
    
    double sinXRot = sin(toRads(rotation.x));
    double cosXRot = cos(toRads(rotation.x));
    
    double sinYRot = sin(toRads(rotation.y));
    double cosYRot = cos(toRads(rotation.y));
    
    double pitchLimitFactor = cosXRot; // this cancels out moving on the z axis when we're looking up or down
    
    if(holdingForward) {
        movement.x += (sinYRot * pitchLimitFactor);
        movement.z += (-sinXRot);
        movement.y += (-sinYRot * pitchLimitFactor);
        atMovement.x += (sinYRot * pitchLimitFactor);
        atMovement.z += (-sinXRot);
        atMovement.y += (-sinYRot * pitchLimitFactor);

    }
    
    if(holdingBackward) {
        movement.x += (-sinYRot * pitchLimitFactor);
        movement.z += (sinXRot);
        movement.y += (sinYRot * pitchLimitFactor);
        atMovement.x += (-sinYRot * pitchLimitFactor);
        atMovement.z += (sinXRot);
        atMovement.y += (sinYRot * pitchLimitFactor);

    }
    
    if(holdingLeftStrafe) {
        movement.x += (-cosYRot);
        movement.z += (-sinYRot);
        atMovement.x += (-cosYRot);
        atMovement.z += (-sinYRot);

    }
    
    if(holdingRightStrafe) {
        movement.x += (cosYRot);
        movement.z += (sinYRot);
        atMovement.x += (-cosYRot);
        atMovement.z += (-sinYRot);

    }
    
    movement = normalize(movement);
    
    double framerateIndependentFactor = movementSpeedFactor * deltaTime;
    
    movement *= framerateIndependentFactor;
    
    position += movement;
}









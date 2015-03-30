//
//  Camera.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/28/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//
// Adapted from http://r3dux.org/2012/12/a-c-camera-class-for-simple-opengl-fps-controls/
//

#ifndef __CS539Project__Camera__
#define __CS539Project__Camera__

#include <stdio.h>
#include "Angel.h"
#include <math.h>


class Camera
{
protected:
    vec3 position;
    vec3 at;
    vec3 rotation;
    vec3 speed;
    
    double movementSpeedFactor;
    double pitchSensitivity;
    double yawSensitivity;
    
    int windowWidth;
    int windowHeight;
    int windowMidX;
    int windowMidY;
    
    void initCamera();
    
public:
    static const double TO_RADS; // value of 1 degree in radians
    
    bool holdingForward;
    bool holdingBackward;
    bool holdingLeftStrafe;
    bool holdingRightStrafe;
    
    Camera(float  windowWidth, float windowHeight);
    ~Camera();
    
    void handleMouseMove(int mouseX, int mouseY);
    
    const double toRads(const double &angleInDegrees) const;
    
    void move(double deltaTime);
    
    // ***** Inline Methods *****
    
    float getPitchSensitivity()            { return pitchSensitivity;  }
    void  setPitchSensitivity(float value) { pitchSensitivity = value; }
    float getYawSensitivity()              { return yawSensitivity;    }
    void  setYawSensitivity(float value)   { yawSensitivity   = value; }
    
    // Position getters
    vec3   getPosition()       const { return position;   }
    double getXPos()           const { return position.x; }
    double getYPos()           const { return position.y; }
    double getZPos()           const { return position.z; }
    
    // Rotation getters
    vec3   getRotation()       const { return rotation;   }
    double getXRot()           const { return rotation.x; }
    double getYRot()           const { return rotation.y; }
    double getZRot()           const { return rotation.z; }
    
    vec3 getAt()               const { return at;         }
    
};



#endif /* defined(__CS539Project__Camera__) */

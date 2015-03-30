//
//  Camera2.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/29/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#ifndef __CS539Project__Camera2__
#define __CS539Project__Camera2__

#include <stdio.h>
#include "Angel.h"
#include <math.h>

class Camera{
protected:
    vec3 eye;
    vec3 at;
    vec3 rotation;
    float angle;
    int lastx, lasty;
    
public:
    Camera();
    ~Camera();
    
    void initCamera();
    void moveForward();
    void moveBackward();
    void turnLeft();
    void strafeLeft();
    void turnRight();
    void strafeRight();
    void xup();
    void xdown();
    
    void mouseMovement(int x, int y);
    
    static const double TO_RADS; // value of 1 degree in radians
    const double toRads(const double &angleInDegrees) const;

    
    // *** Inline Methods ***
    vec3 getEye()   {   return eye;     }
    vec3 getAt()    {   return at;      }
    vec3 getRot()   {   return rotation;}
};






#endif /* defined(__CS539Project__Camera2__) */

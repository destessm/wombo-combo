//
//  Controls.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 4/19/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#ifndef __CS539Project__Controls__
#define __CS539Project__Controls__

#include <stdio.h>
#include "Angel.h"

class Controls{
    //unfinished
protected:
    vec3 position = vec3(0,0,0); // start position
    float horizontalAngle = 3.14f;
    float verticalAngle = 0.0f;
    float initialFoV = 45.0f;
    
    float speed = 3.0f; // 3 units / second
    float mouseSpeed = 0.005f;
    
    int xpos, ypos;
    
    
    
    
};


#endif /* defined(__CS539Project__Controls__) */

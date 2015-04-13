//
//  Plane.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 4/4/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#ifndef __CS539Project__Plane__
#define __CS539Project__Plane__

#include <stdio.h>
#include "Angel.h"

class Plane{
    
protected:
    float distance;
    vec3 normal;
public:
    Plane(){ distance = 0; normal = vec3(0,0,0); }
    Plane(float, vec3);
    Plane(vec3, vec3, vec3);
    
    float signedDistance(vec3);
    
    //inline methods:
    float getDistance() {   return distance;    }
    vec3 getNormal()    {   return normal;      }
};

#endif /* defined(__CS539Project__Plane__) */

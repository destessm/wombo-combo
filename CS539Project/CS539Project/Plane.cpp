//
//  Plane.cpp
//  CS539Project
//
//  Created by David Estes-Smargiassi on 4/4/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#include "Plane.h"


Plane::Plane(float d, vec3 n){
    distance = d;
    normal = n;
}

Plane::Plane(vec3 a, vec3 b, vec3 c){
    //    A
    //  B   C  (count clockwise order), normal going UP (off screen)
    
    // normal = AB x AC
    normal = cross(b-a, c-a);
    
    // point = (x,y,z), normal = (a, b, c)
    // 0 = ax + by + cz + d
    // d = -(ax + by + cz)
    distance = -(a.x*normal.x + a.y*normal.y + a.z*normal.z);
}

float Plane::signedDistance(vec3 pt){
    return normal.x*pt.x + normal.y*pt.y + normal.z*pt.z + distance;
//    vec3 norm;
//    if(sqrtf(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z) != 0){
//        norm = normalize(normal);
//    }
//    else{
//        norm = normal;
//    }
//    float answer =dot(norm, (pt - (norm*distance)));
//    return answer;
}

void Plane::update(float d, vec3 n){
    distance = d;
    normal = n;
}

void Plane::update(vec3 a, vec3 b, vec3 c){
    //    A
    //  B   C  (count clockwise order)
    
    // normal = BA x CA
    normal = cross(b-a, c-a);
    
    // point = (x,y,z), normal = (a, b, c)
    // 0 = ax + by + cz + d
    // d = -(ax + by + cz)
    distance = -(a.x*normal.x + a.y*normal.y + a.z*normal.z);

}
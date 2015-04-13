//
//  Frustum.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 4/12/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#include "Angel.h"
#include "Plane.h"
#include <vector>



#ifndef CS539Project_Frustum_h
#define CS539Project_Frustum_h

vec3 toVec3(vec4 v){
    return vec3(v.x, v.y, v.z);
}


class Frust{
public:
    Plane np, fp, lp, rp, tp, bp;
    int fov;
    float fratio;
    float nearDist;
    float farDist;
    
    float Hnear, Wnear, Hfar, Wfar;
    vec4 fc, ftl, ftr, fbl, fbr;
    vec4 nc, ntl, ntr, nbl, nbr;
    
    Frust(){}
    
    Frust(int fv, float rat, float near, float far, vec4 eye, vec4 at, vec4 up){
        fov = fv;
        fratio = rat;
        nearDist = near;
        farDist = far;
        float Hnear = 2*tan(fov / 2.0) * nearDist;
        float Wnear = Hnear * rat;
        float Hfar = 2*tan(fov / 2.0) * farDist;
        float Wfar = Hfar * rat;
        
        vec4 dir = normalize(at-eye);
        vec4 right = cross(up, dir);
        
        fc = eye + dir * farDist;
        ftl = fc + (up * Hfar/2.0) - (right * Wfar/2.0);
        ftr = fc + (up * Hfar/2) + (right * Wfar/2);
        fbl = fc - (up * Hfar/2) - (right * Wfar/2);
        fbr = fc - (up * Hfar/2) + (right * Wfar/2);
        
        nc = eye + dir * nearDist;
        ntl = nc + (up * Hnear/2) - (right * Wnear/2);
        ntr = nc + (up * Hnear/2) + (right * Wnear/2);
        nbl = nc - (up * Hnear/2) - (right * Wnear/2);
        nbr = nc - (up * Hnear/2) + (right * Wnear/2);
        
        np = Plane(toVec3(nbl), toVec3(ntl), toVec3(ntr));
        fp = Plane(toVec3(fbr), toVec3(ftr), toVec3(ftl));
        lp = Plane(toVec3(nbl), toVec3(fbl), toVec3(ftl));
        rp = Plane(toVec3(ntr), toVec3(ftr), toVec3(fbr));
        tp = Plane(toVec3(ntl), toVec3(ftl), toVec3(ftr));
        bp = Plane(toVec3(nbr), toVec3(fbr), toVec3(fbl));

    }
    
    void updateFrustum(vec4 eye, vec4 at, vec4 up){
        vec4 dir = normalize(at-eye);
        vec4 right = cross(up, dir);
        
        fc = eye + dir * farDist;
        ftl = fc + (up * Hfar/2.0) - (right * Wfar/2.0);
        ftr = fc + (up * Hfar/2) + (right * Wfar/2);
        fbl = fc - (up * Hfar/2) - (right * Wfar/2);
        fbr = fc - (up * Hfar/2) + (right * Wfar/2);
        
        nc = eye + dir * nearDist;
        ntl = nc + (up * Hnear/2) - (right * Wnear/2);
        ntr = nc + (up * Hnear/2) + (right * Wnear/2);
        nbl = nc - (up * Hnear/2) - (right * Wnear/2);
        nbr = nc - (up * Hnear/2) + (right * Wnear/2);
        
        np = Plane(toVec3(nbl), toVec3(ntl), toVec3(ntr));
        fp = Plane(toVec3(fbr), toVec3(ftr), toVec3(ftl));
        lp = Plane(toVec3(nbl), toVec3(fbl), toVec3(ftl));
        rp = Plane(toVec3(ntr), toVec3(ftr), toVec3(fbr));
        tp = Plane(toVec3(ntl), toVec3(ftl), toVec3(ftr));
        bp = Plane(toVec3(nbr), toVec3(fbr), toVec3(fbl));

    }
    
};

std::vector<Triangle> concatenate(std::vector<Triangle> a, std::vector<Triangle> b){
    std::vector<Triangle> ab;
    ab.reserve( a.size() + b.size() );
    ab.insert( ab.end(), a.begin(), a.end() );
    ab.insert( ab.end(), b.begin(), b.end() );
    return ab;
}



std::vector<Triangle> nodesToRender(OTNode root, Frust f){
    if((f.np.signedDistance(root.corners[0]) > 0 ||
        f.np.signedDistance(root.corners[1]) > 0 ||
        f.np.signedDistance(root.corners[2]) > 0 ||
        f.np.signedDistance(root.corners[3]) > 0 ||
        f.np.signedDistance(root.corners[4]) > 0 ||
        f.np.signedDistance(root.corners[5]) > 0 ||
        f.np.signedDistance(root.corners[6]) > 0 ||
        f.np.signedDistance(root.corners[7]) > 0) &&
       (f.fp.signedDistance(root.corners[0]) > 0 ||
        f.fp.signedDistance(root.corners[1]) > 0 ||
        f.fp.signedDistance(root.corners[2]) > 0 ||
        f.fp.signedDistance(root.corners[3]) > 0 ||
        f.fp.signedDistance(root.corners[4]) > 0 ||
        f.fp.signedDistance(root.corners[5]) > 0 ||
        f.fp.signedDistance(root.corners[6]) > 0 ||
        f.fp.signedDistance(root.corners[7]) > 0) &&
       (f.lp.signedDistance(root.corners[0]) > 0 ||
        f.lp.signedDistance(root.corners[1]) > 0 ||
        f.lp.signedDistance(root.corners[2]) > 0 ||
        f.lp.signedDistance(root.corners[3]) > 0 ||
        f.lp.signedDistance(root.corners[4]) > 0 ||
        f.lp.signedDistance(root.corners[5]) > 0 ||
        f.lp.signedDistance(root.corners[6]) > 0 ||
        f.lp.signedDistance(root.corners[7]) > 0) &&
       (f.rp.signedDistance(root.corners[0]) > 0 ||
        f.rp.signedDistance(root.corners[1]) > 0 ||
        f.rp.signedDistance(root.corners[2]) > 0 ||
        f.rp.signedDistance(root.corners[3]) > 0 ||
        f.rp.signedDistance(root.corners[4]) > 0 ||
        f.rp.signedDistance(root.corners[5]) > 0 ||
        f.rp.signedDistance(root.corners[6]) > 0 ||
        f.rp.signedDistance(root.corners[7]) > 0) &&
       (f.tp.signedDistance(root.corners[0]) > 0 ||
        f.tp.signedDistance(root.corners[1]) > 0 ||
        f.tp.signedDistance(root.corners[2]) > 0 ||
        f.tp.signedDistance(root.corners[3]) > 0 ||
        f.tp.signedDistance(root.corners[4]) > 0 ||
        f.tp.signedDistance(root.corners[5]) > 0 ||
        f.tp.signedDistance(root.corners[6]) > 0 ||
        f.tp.signedDistance(root.corners[7]) > 0) &&
       (f.bp.signedDistance(root.corners[0]) > 0 ||
        f.bp.signedDistance(root.corners[1]) > 0 ||
        f.bp.signedDistance(root.corners[2]) > 0 ||
        f.bp.signedDistance(root.corners[3]) > 0 ||
        f.bp.signedDistance(root.corners[4]) > 0 ||
        f.bp.signedDistance(root.corners[5]) > 0 ||
        f.bp.signedDistance(root.corners[6]) > 0 ||
        f.bp.signedDistance(root.corners[7]) > 0)){
           // at least one corner is inside the frustum
           if(root.hasChildren){
               //has chidlren
               std::vector<Triangle> c0 = nodesToRender(*root.children[0], f);
               std::vector<Triangle> c1 = nodesToRender(*root.children[1], f);
               std::vector<Triangle> c2 = nodesToRender(*root.children[2], f);
               std::vector<Triangle> c3 = nodesToRender(*root.children[3], f);
               std::vector<Triangle> c4 = nodesToRender(*root.children[4], f);
               std::vector<Triangle> c5 = nodesToRender(*root.children[5], f);
               std::vector<Triangle> c6 = nodesToRender(*root.children[6], f);
               std::vector<Triangle> c7 = nodesToRender(*root.children[7], f);
               std::vector<Triangle> c01 = concatenate(c0, c1);
               std::vector<Triangle> c23 = concatenate(c2, c3);
               std::vector<Triangle> c0123 = concatenate(c01, c23);
               std::vector<Triangle> c45 = concatenate(c4, c5);
               std::vector<Triangle> c67 = concatenate(c6, c7);
               std::vector<Triangle> c4567 = concatenate(c45, c67);
               return concatenate(c0123, c4567);
           }
           else{
               //has no children
               return root.data;
           }
       }
    else{
        std::vector<Triangle> r;
        return r;
    }

};


std::vector<int> cullAndRender(OTNode root, Frust f){
    std::vector<Triangle> allTris = nodesToRender(root, f);
    std::vector<int> allInds;
    for(int i=0; i<allTris.size(); i++){
        allInds.push_back(allTris[i].indices[0]);
        allInds.push_back(allTris[i].indices[1]);
        allInds.push_back(allTris[i].indices[2]);
    }
    return allInds;
}


#endif

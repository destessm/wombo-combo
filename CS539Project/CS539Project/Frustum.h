//
//  Frustum.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 4/12/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//




#ifndef CS539Project_Frustum_h
#define CS539Project_Frustum_h


#include "Angel.h"
#include "Plane.h"
#include <vector>
#include "Octree.h"

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
    
    vec4 lines[24];
    
    GLuint vPositionLoc, modelViewLoc, projectionLoc, vao, vbo;
    
    Frust(){}
    
    Frust(int fv, float rat, float near, float far, vec4 eye, vec4 dir, vec4 right, vec4 up, GLuint program){
        fov = fv;
        fratio = rat;
        nearDist = near;
        farDist = far;
        Hnear = 2*tan(fov / 2.0f) * nearDist;
        Wnear = Hnear * rat;
        Hfar = 2*tan(fov / 2.0f) * farDist;
        Wfar = Hfar * rat;
        
        //vec4 dir = normalize(at-eye);
        //vec4 right = cross(up, dir);
        
        fc = eye + dir * farDist;
        ftl = fc + (up * Hfar/1.2f) - (right * Wfar/1.2f);
        ftl.w = 1;
        ftr = fc + (up * Hfar/1.2f) + (right * Wfar/1.2f);
        ftr.w = 1;
        fbl = fc - (up * Hfar/1.2f) - (right * Wfar/1.2f);
        fbl.w = 1;
        fbr = fc - (up * Hfar/1.2f) + (right * Wfar/1.2f);
        fbr.w = 1;
        
        //        std::cout<<"ftl: "<<ftl<<std::endl;
        //        std::cout<<"ftr: "<<ftr<<std::endl;
        //        std::cout<<"fbl: "<<fbl<<std::endl;
        //        std::cout<<"fbr: "<<fbr<<std::endl;
        
        nc = eye + dir * nearDist;
        ntl = nc + (up * Hnear/1.2f) - (right * Wnear/1.2f);
        ntl.w = 1;
        ntr = nc + (up * Hnear/1.2f) + (right * Wnear/1.2f);
        ntr.w = 1;
        nbl = nc - (up * Hnear/1.2f) - (right * Wnear/1.2f);
        nbl.w = 1;
        nbr = nc - (up * Hnear/1.2f) + (right * Wnear/1.2f);
        nbr.w = 1;
        
        //        std::cout<<"ntl: "<<ntl<<std::endl;
        //        std::cout<<"ntr: "<<ntr<<std::endl;
        //        std::cout<<"nbl: "<<nbl<<std::endl;
        //        std::cout<<"nbr: "<<nbr<<std::endl;
        
        np = Plane(toVec3(nbl), toVec3(ntl), toVec3(ntr));
        fp = Plane(toVec3(fbr), toVec3(ftr), toVec3(ftl));
        lp = Plane(toVec3(nbl), toVec3(fbl), toVec3(ftl));
        rp = Plane(toVec3(ntr), toVec3(ftr), toVec3(fbr));
        tp = Plane(toVec3(ntl), toVec3(ftl), toVec3(ftr));
        bp = Plane(toVec3(nbr), toVec3(fbr), toVec3(fbl));
        
        
        modelViewLoc = glGetUniformLocation(program, "modelView");
        projectionLoc = glGetUniformLocation(program, "projection");
        vPositionLoc = glGetAttribLocation(program, "vPosition");
        
        glGenBuffers(1, &vbo);
        glGenVertexArraysAPPLE(1, &vao);
        glBindVertexArrayAPPLE(vao);
        
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec4)*24, NULL, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(vPositionLoc);
        glVertexAttribPointer(vPositionLoc, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glBindVertexArrayAPPLE(0);
        
        updateLines();


    }
    
    void updateFrustum(vec4 eye, vec4 dir, vec4 up, vec4 right){
        //vec4 dir = normalize(at-eye);
        //vec4 right = cross(up, dir);
        
        //vec4 dir = normalize(at-eye);
        //vec4 right = cross(up, dir);
        
        fc = eye + dir * farDist;
        ftl = fc + (up * Hfar/1.2f) - (right * Wfar/1.2f);
        ftl.w = 1;
        ftr = fc + (up * Hfar/1.2f) + (right * Wfar/1.2f);
        ftr.w = 1;
        fbl = fc - (up * Hfar/1.2f) - (right * Wfar/1.2f);
        fbl.w = 1;
        fbr = fc - (up * Hfar/1.2f) + (right * Wfar/1.2f);
        fbr.w = 1;
        
        //        std::cout<<"ftl: "<<ftl<<std::endl;
        //        std::cout<<"ftr: "<<ftr<<std::endl;
        //        std::cout<<"fbl: "<<fbl<<std::endl;
        //        std::cout<<"fbr: "<<fbr<<std::endl;
        
        nc = eye + dir * nearDist;
        ntl = nc + (up * Hnear/1.2f) - (right * Wnear/1.2f);
        ntl.w = 1;
        ntr = nc + (up * Hnear/1.2f) + (right * Wnear/1.2f);
        ntr.w = 1;
        nbl = nc - (up * Hnear/1.2f) - (right * Wnear/1.2f);
        nbl.w = 1;
        nbr = nc - (up * Hnear/1.2f) + (right * Wnear/1.2f);
        nbr.w = 1;
        
        //        std::cout<<"ntl: "<<ntl<<std::endl;
        //        std::cout<<"ntr: "<<ntr<<std::endl;
        //        std::cout<<"nbl: "<<nbl<<std::endl;
        //        std::cout<<"nbr: "<<nbr<<std::endl;

        np.update(toVec3(nbl), toVec3(ntl), toVec3(ntr));
        fp.update(toVec3(fbr), toVec3(ftr), toVec3(ftl));
        lp.update(toVec3(nbl), toVec3(fbl), toVec3(ftl));
        rp.update(toVec3(ntr), toVec3(ftr), toVec3(fbr));
        tp.update(toVec3(ntl), toVec3(ftl), toVec3(ftr));
        bp.update(toVec3(nbr), toVec3(fbr), toVec3(fbl));
        
        updateLines();
    }
    
    void updateLines(){
        lines[0] = nbr;
        lines[1] = nbl;
        
        lines[2] = nbl;
        lines[3] = ntl;
        
        lines[4] = ntl;
        lines[5] = ntr;
        
        lines[6] = ntr;
        lines[7] = nbr;
        
        lines[8] = fbr;
        lines[9] = fbl;
        
        lines[10]= fbl;
        lines[11]= ftl;
        
        lines[12]= ftl;
        lines[13]= ftr;
        
        lines[14]= ftr;
        lines[15]= fbr;
        
        lines[16]= nbl;
        lines[17]= fbl;
        
        lines[18]= ntl;
        lines[19]= ftl;
        
        lines[20]= ntr;
        lines[21]= ftr;
        
        lines[22]= nbr;
        lines[23]= fbr;
    }
    
    void draw(mat4 modelMatrix, mat4 projectionMatrix, GLuint program){
        glUseProgram(program);
        glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelMatrix);
        glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projectionMatrix);
        
        glBindVertexArrayAPPLE(vao);
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec4)*24, lines);

        glDrawArrays(GL_LINES, 0, 24);
        glBindVertexArrayAPPLE(0);

    }
};


std::vector<Triangle> concatenate(std::vector<Triangle> a, std::vector<Triangle> b, std::vector<Triangle> c, std::vector<Triangle> d, std::vector<Triangle> e, std::vector<Triangle> f, std::vector<Triangle> g, std::vector<Triangle> h){
    std::vector<Triangle> total;
    total.reserve(a.size() + b.size() + c.size() + d.size() + e.size() + f.size() + g.size() + h.size());
    total.insert(total.end(), a.begin(), a.end());
    total.insert(total.end(), b.begin(), b.end());
    total.insert(total.end(), c.begin(), c.end());
    total.insert(total.end(), d.begin(), d.end());
    total.insert(total.end(), e.begin(), e.end());
    total.insert(total.end(), f.begin(), f.end());
    total.insert(total.end(), g.begin(), g.end());
    total.insert(total.end(), h.begin(), h.end());
    return total;
}



std::vector<Triangle> nodesToRender(OTNode root, Frust f){
    std::vector<Triangle> returnData;
    if(root.hasChildren){
        std::vector<Triangle> total;
        std::vector<Triangle> current;
        for(int j=0; j<8; j++){
            current = nodesToRender(*root.children[j], f);
            total.reserve(total.size() + current.size());
            move(current.begin(), current.end(), inserter(total, total.end()));
        }
        returnData = total;
    }
    else{ // is leaf node
        bool intersect = false;
        Plane pls[] = {f.np, f.fp, f.lp, f.rp, f.tp, f.bp};
        for(int i=0; i<6; i++){ // loop through planes
            for(int j=0; j<8; j++){ //loop through corners
                //std::cout<<"Signed Distance: "<< pls[i].signedDistance(root.corners[j]) <<std::endl;
                if(pls[i].signedDistance(root.corners[j]) > 10){
                    //std::cout<<"Is In Frustum  ["<<j<<"]"<<std::endl;
                    intersect = true;
                    break;
                }
//                else{
//                    //std::cout<<"Not In Frustum ["<<j<<"]"<<std::endl;
//                }
            }
        }
        if(intersect){
            returnData = root.data;
            //std::cout<<"Return Data Size: " <<returnData.size()<<std::endl;
        }
        else{
            returnData.clear();
        }
    }
    return returnData;
};

std::vector<int> cullAndRender(OTNode root, Frust f){
    //Plane pls[] = {f.np, f.fp, f.lp, f.rp, f.tp, f.bp};
    std::vector<Triangle> allTris = nodesToRender(root, f);//root.checkFrustumCollision(pls);//nodesToRender(root, f);
    std::vector<int> allInds;
    for(int i=0; i<allTris.size(); i++){
        allInds.push_back(allTris[i].indices[0]);
        allInds.push_back(allTris[i].indices[1]);
        allInds.push_back(allTris[i].indices[2]);
    }
    //std::cout<<"Num Triangles = " << allInds.size()<<std::endl;
    return allInds;
}


#endif

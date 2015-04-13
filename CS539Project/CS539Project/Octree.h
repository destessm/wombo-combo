//
//  Octree.h
//  CS539Project
//
//  Created by David Estes-Smargiassi on 3/7/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

#ifndef __CS539Project__Octree__
#define __CS539Project__Octree__

#include <stdio.h>
#include "Angel.h"
#include <vector>

struct Triangle{
    int indices[3];
    int key; // to avoid duplications
    Triangle(int a, int b, int c, int k){
        indices[0] = a;
        indices[1] = b;
        indices[2] = c;
        key = k;
    }
};

class OTNode{
public:
    OTNode* children[8];
    OTNode* parent;
    vec3 center;
    float size;
    std::vector<Triangle> data;
    std::vector<vec3> drawData;
    vec3 cornerMin;
    vec3 cornerMax;
    vec3 corners[8];
    bool hasChildren;
    OTNode (){}
    OTNode(std::vector<Triangle> a, vec3 c, float s){
        data = a;
        center = c;
        size = s;
        setCorners(center, size);
        cornerMin = corners[6];
        cornerMax = corners[0];
        setDrawLines(center, size);
        for(int i = 0; i<8; i++){
            children[i] = NULL;
        }
        hasChildren = false;
    }
    OTNode(std::vector<Triangle> a, OTNode* par, vec3 c, float s){
        data = a;
        center = c;
        size = s;
        parent = par;
        setCorners(center, size);
        cornerMin = corners[6];
        cornerMax = corners[0];
        setDrawLines(center, size);
        for(int i = 0; i<8; i++){
            children[i] = NULL;
        }
        hasChildren = false;
    }
    
    void setCorners(vec3 center, float size){
        corners[0] = vec3(center.x+size, center.y+size, center.z+size);
        corners[1] = vec3(center.x-size, center.y+size, center.z+size);
        corners[2] = vec3(center.x-size, center.y+size, center.z-size);
        corners[3] = vec3(center.x+size, center.y+size, center.z-size);
        corners[4] = vec3(center.x+size, center.y-size, center.z+size);
        corners[5] = vec3(center.x-size, center.y-size, center.z+size);
        corners[6] = vec3(center.x-size, center.y-size, center.z-size);
        corners[7] = vec3(center.x+size, center.y-size, center.z-size);


    }
    
    void setDrawLines(vec3 center, float size){
        float rad = size/2.0;
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z+rad)); // 0
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z+rad)); // 1
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z+rad)); // 1
        drawData.push_back(vec3(center.x+rad, center.y+rad, center.z+rad)); // 2
        drawData.push_back(vec3(center.x+rad, center.y+rad, center.z+rad)); // 2
        drawData.push_back(vec3(center.x-rad, center.y+rad, center.z+rad)); // 3
        drawData.push_back(vec3(center.x-rad, center.y+rad, center.z+rad)); // 3
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z+rad)); // 0
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z+rad)); // 0
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z-rad)); // 4
        drawData.push_back(vec3(center.x-rad, center.y+rad, center.z+rad)); // 3
        drawData.push_back(vec3(center.x-rad, center.y+rad, center.z-rad)); // 5
        drawData.push_back(vec3(center.x+rad, center.y+rad, center.z+rad)); // 2
        drawData.push_back(vec3(center.x+rad, center.y+rad, center.z-rad)); // 6
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z+rad)); // 1
        drawData.push_back(vec3(center.x+rad, center.y-rad, center.z-rad)); // 7
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z-rad)); // 4
        drawData.push_back(vec3(center.x+rad, center.y-rad, center.z-rad)); // 7
        drawData.push_back(vec3(center.x+rad, center.y-rad, center.z-rad)); // 7
        drawData.push_back(vec3(center.x+rad, center.y+rad, center.z-rad)); // 6
        drawData.push_back(vec3(center.x+rad, center.y+rad, center.z-rad)); // 6
        drawData.push_back(vec3(center.x-rad, center.y+rad, center.z-rad)); // 5
        drawData.push_back(vec3(center.x-rad, center.y+rad, center.z-rad)); // 5
        drawData.push_back(vec3(center.x-rad, center.y-rad, center.z-rad)); // 4
    }
};


OTNode root;
int minTriangles = 600;

std::vector<Triangle> trianglulate(int indices[], int length){
    std::vector<Triangle> array;
    int index = 0;
    for(int i=0; i<length; i+=3){
        array.push_back(Triangle(indices[i], indices[i+1], indices[i+2], index));
        index++;
    }
    return array;
}

OTNode* subdivideTree(OTNode*, vec3[]);
void goThroughTree(OTNode*);

OTNode* genOctree(int indices[], int indLen, vec3 vertices[], vec3 center, float size){
    std::vector<Triangle> tri = trianglulate(indices, indLen);
    root = OTNode(tri, center, size);
    //goThroughTree(&root);
    return subdivideTree(&root, vertices);
}



OTNode* subdivideTree(OTNode* root, vec3 vertices[]){
    unsigned long curSize = root->data.size();
    //std::cout<<root->size <<std::endl;
    if(curSize <= minTriangles){
        std::cout<< "LEAF NODE! #Triangles = " << root->data.size() << std::endl;
        return root;
    }
    else{
        std::cout<<"INTERNAL NODE! #Triangles = " << root->data.size() << std::endl;
        std::vector<Triangle> child0Array, child1Array, child2Array, child3Array, child4Array, child5Array, child6Array, child7Array;
        float newSize = root->size*0.5;
        //std::cout<<root->size <<std::endl;
        //std::cout<<newSize<<std::endl;

        vec3 child0Center = vec3(root->center.x+newSize, root->center.y+newSize, root->center.z+newSize);
        
        vec3 child1Center = vec3(root->center.x-newSize, root->center.y+newSize, root->center.z+newSize);
        
        vec3 child2Center = vec3(root->center.x-newSize, root->center.y+newSize, root->center.z-newSize);
        
        vec3 child3Center = vec3(root->center.x+newSize, root->center.y+newSize, root->center.z-newSize);
        
        vec3 child4Center = vec3(root->center.x+newSize, root->center.y-newSize, root->center.z+newSize);
        
        vec3 child5Center = vec3(root->center.x-newSize, root->center.y-newSize, root->center.z+newSize);
        
        vec3 child6Center = vec3(root->center.x-newSize, root->center.y-newSize, root->center.z-newSize);
        
        vec3 child7Center = vec3(root->center.x+newSize, root->center.y-newSize, root->center.z-newSize);
        
        //std::cout<<root->size <<std::endl;

        
        for(int i =0; i < root->data.size(); i++){ // loop through Triangles in root
            for(int j=0; j<3; j++){ // loop through indices in each Triangle
                float x = vertices[root->data.at(i).indices[j]].x;
                float y = vertices[root->data.at(i).indices[j]].y;
                float z = vertices[root->data.at(i).indices[j]].z;
                
                // Check if in child 0
//                if(x < child0Center.x+root->size && x > child0Center.x-root->size
//                   && y < child0Center.y+root->size && y > child0Center.y-root->size
//                   && z < child0Center.z+root->size && z > child0Center.z-root->size){
//                    child0Array.push_back(root->data.at(i)); // push current Triangle to its array
//                    break;
//                }
//                
//
//
//                // Check if in child 1
//                if(x < child1Center.x+root->size && x > child1Center.x-root->size
//                   && y < child1Center.y+root->size && y > child1Center.y-root->size
//                   && z < child1Center.z+root->size && z > child1Center.z-root->size){
//                    child1Array.push_back(root->data.at(i)); // push current Triangle to its array
//                    break;
//                }
//                
//                // Check if in child 2
//                if(x < child2Center.x+root->size && x > child2Center.x-root->size
//                   && y < child2Center.y+root->size && y > child2Center.y-root->size
//                   && z < child2Center.z+root->size && z > child2Center.z-root->size){
//                    child2Array.push_back(root->data.at(i)); // push current Triangle to its array
//                    break;
//                }
//
//                // Check if in child 3
//                if(x < child3Center.x+root->size && x > child3Center.x-root->size
//                   && y < child3Center.y+root->size && y > child3Center.y-root->size
//                   && z < child3Center.z+root->size && z > child3Center.z-root->size){
//                    child3Array.push_back(root->data.at(i)); // push current Triangle to its array
//                    break;
//                }
//                
//                // Check if in child 4
//                if(x < child4Center.x+root->size && x > child4Center.x-root->size
//                   && y < child4Center.y+root->size && y > child4Center.y-root->size
//                   && z < child4Center.z+root->size && z > child4Center.z-root->size){
//                    child4Array.push_back(root->data.at(i)); // push current Triangle to its array
//                    break;
//                }
//
//                // Check if in child 5
//                if(x < child5Center.x+root->size && x > child5Center.x-root->size
//                   && y < child5Center.y+root->size && y > child5Center.y-root->size
//                   && z < child5Center.z+root->size && z > child5Center.z-root->size){
//                    child5Array.push_back(root->data.at(i)); // push current Triangle to its array
//                    break;
//                }
//
//                // Check if in child 6
//                if(x < child6Center.x+root->size && x > child6Center.x-root->size
//                   && y < child6Center.y+root->size && y > child6Center.y-root->size
//                   && z < child6Center.z+root->size && z > child6Center.z-root->size){
//                    child6Array.push_back(root->data.at(i)); // push current Triangle to its array
//                    break;
//                }
//
//                // Check if in child 7
//                if(x < child7Center.x+root->size && x > child7Center.x-root->size
//                   && y < child7Center.y+root->size && y > child7Center.y-root->size
//                   && z < child7Center.z+root->size && z > child7Center.z-root->size){
//                    child7Array.push_back(root->data.at(i)); // push current Triangle to its array
//                    break;
//                }
                
                // Check if in child 0
                if(x > root->center.x &&
                   y > root->center.y &&
                   z > root->center.z){
                    child0Array.push_back(root->data.at(i)); // push current Triangle to its array
                    break;
                }

                // Check if in child 1
                if(x < root->center.x &&
                   y > root->center.y &&
                   z > root->center.z){
                    child1Array.push_back(root->data.at(i)); // push current Triangle to its array
                    break;
                }
                
                // Check if in child 2
                if(x < root->center.x &&
                   y > root->center.y &&
                   z < root->center.z){
                    child2Array.push_back(root->data.at(i)); // push current Triangle to its array
                    break;
                }
                
                // Check if in child 3
                if(x > root->center.x &&
                   y > root->center.y &&
                   z < root->center.z){
                    child3Array.push_back(root->data.at(i)); // push current Triangle to its array
                    break;
                }
                
                // Check if in child 4
                if(x > root->center.x &&
                   y < root->center.y &&
                   z > root->center.z){
                    child4Array.push_back(root->data.at(i)); // push current Triangle to its array
                    break;
                }

                // Check if in child 5
                if(x < root->center.x &&
                   y < root->center.y &&
                   z > root->center.z){
                    child5Array.push_back(root->data.at(i)); // push current Triangle to its array
                    break;
                }
                
                // Check if in child 6
                if(x < root->center.x &&
                   y < root->center.y &&
                   z < root->center.z){
                    child6Array.push_back(root->data.at(i)); // push current Triangle to its array
                    break;
                }
                
                // Check if in child 7
                if(x > root->center.x &&
                   y < root->center.y &&
                   z < root->center.z){
                    child7Array.push_back(root->data.at(i)); // push current Triangle to its array
                    break;
                }
            }
        }
        
        //std::cout<<'\t';
        // assign to child 0
        root->children[0] = new OTNode(child0Array, root, child0Center, newSize);
        subdivideTree(root->children[0], vertices);
        
        //std::cout<<'\t';
        // make vector full of Triangles in child 1
        root->children[1] = new OTNode(child1Array, root, child1Center, newSize);
        subdivideTree(root->children[1], vertices);

        //std::cout<<'\t';
        // make vector full of Triangles in child 2
        root->children[2] = new OTNode(child2Array, root, child2Center, newSize);
        subdivideTree(root->children[2], vertices);

        //std::cout<<'\t';
        // make vector full of Triangles in child 3
        root->children[3] = new OTNode(child3Array, root, child3Center, newSize);
        subdivideTree(root->children[3], vertices);

        //std::cout<<'\t';
        // make vector full of Triangles in child 4
        root->children[4] = new OTNode(child4Array, root, child4Center, newSize);
        subdivideTree(root->children[4], vertices);

        //std::cout<<'\t';
        // make vector full of Triangles in child 5
        root->children[5] = new OTNode(child5Array, root, child5Center, newSize);
        subdivideTree(root->children[5], vertices);

        //std::cout<<'\t';
        // make vector full of Triangles in child 6
        root->children[6] = new OTNode(child6Array, root, child6Center, newSize);
        subdivideTree(root->children[6], vertices);

        //std::cout<<'\t';
        // make vector full of Triangles in child 7
        root->children[7] = new OTNode(child7Array, root, child7Center, newSize);
        subdivideTree(root->children[7], vertices);

        root->hasChildren = true;
        
        return root;
    }
}


void goThroughTree(OTNode* curNode){
    //curNode->children[0] == nullptr
    if(curNode->hasChildren == false){
        std::cout<< "LEAF NODE FOUND. " << std::endl;
        std::cout<< "  Center: " << curNode->center <<std::endl;
        std::cout<< "  Size: " << curNode->size <<std::endl;
        std::cout<< "  min: " << curNode->cornerMin << '\n' << "  max: " << curNode->cornerMax << std::endl;
        std::cout<< "  Num Triangles: " << curNode->data.size() <<std::endl;

    }
    else{
        std::cout<< "Interal Node. Children Are:" << std::endl;
        std::cout<< "  Num Triangles: " << curNode->data.size() <<std::endl;
//        std::cout<< '\t';
        goThroughTree(curNode->children[0]);
//        std::cout<< '\n';
//        std::cout<< '\t';
        goThroughTree(curNode->children[1]);
//        std::cout<< '\n';
//        std::cout<< '\t';
        goThroughTree(curNode->children[2]);
//        std::cout<< '\n';
//        std::cout<< '\t';
        goThroughTree(curNode->children[3]);
//        std::cout<< '\n';
//        std::cout<< '\t';
        goThroughTree(curNode->children[4]);
//        std::cout<< '\n';
//        std::cout<< '\t';
        goThroughTree(curNode->children[5]);
//        std::cout<< '\n';
//        std::cout<< '\t';
        goThroughTree(curNode->children[6]);
//        std::cout<< '\n';
//        std::cout<< '\t';
        goThroughTree(curNode->children[7]);
//        std::cout<< '\n';
    }
}


std::vector<vec3> generateVertices(OTNode* curNode){
    // this will create the vertices to draw the red cubes
    if(curNode->children[0]==NULL){
        return curNode->drawData;
    }
    else{
        
        std::vector<vec3> c0 = generateVertices(curNode->children[0]);
        std::vector<vec3> c1 = generateVertices(curNode->children[1]);
        std::vector<vec3> c2 = generateVertices(curNode->children[2]);
        std::vector<vec3> c3 = generateVertices(curNode->children[3]);
        std::vector<vec3> c4 = generateVertices(curNode->children[4]);
        std::vector<vec3> c5 = generateVertices(curNode->children[5]);
        std::vector<vec3> c6 = generateVertices(curNode->children[6]);
        std::vector<vec3> c7 = generateVertices(curNode->children[7]);

        std::vector<vec3> total;
        total.reserve(24*8);
        total.insert(total.end(), c0.begin(), c0.end());
        total.insert(total.end(), c1.begin(), c1.end());
        total.insert(total.end(), c2.begin(), c2.end());
        total.insert(total.end(), c3.begin(), c3.end());
        total.insert(total.end(), c4.begin(), c4.end());
        total.insert(total.end(), c5.begin(), c5.end());
        total.insert(total.end(), c6.begin(), c6.end());
        total.insert(total.end(), c7.begin(), c7.end());
        
        return total;
    }
}


#endif /* defined(__CS539Project__Octree__) */

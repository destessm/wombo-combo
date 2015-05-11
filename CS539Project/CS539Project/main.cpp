//  main.cpp
//  CS539Project
//
//  Created by David Estes-Smargiassi on 2/27/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

// ***** Import Statements *****

#include <iostream>
#include <vector>
#include "Angel.h"
#include "glm.h"
//#include "stb_image.h"
#include "Octree.h"
#include "Frustum.h"
#include "Flock.h"
#include "Sphere.h"
#include "Plane.h"
#include "Particles2.h"
#include "ApplicationServices/ApplicationServices.h"

using namespace std;

// ***** Fields *****

// window size
int windowWidth = 1024;
int windowHeight = 1024;


//I know this is bad, I just never got around to fixing it
const int heightMapWidth = 256;
const int heightMapHeight = 256;

//Camera *cam;

vector<Sphere> spheres;

OTNode* rt;

// HeightMap Height Values
float heightMapValues[heightMapWidth*heightMapHeight];
// HeightMap Vectors
vec3 heightMapVectors[heightMapWidth*heightMapHeight];
// HeightMap Colors
vec4 heightMapColors[heightMapWidth*heightMapHeight];
// HeightMap Triangle Normals
vec3 heightMapTriNormals[2*(heightMapWidth-1)*(heightMapHeight-1)];
// HeightMap Normal Averages
vec3 heightMapAvgNormals[heightMapWidth*heightMapHeight];


// size for the index buffer data
const int indexCount = 6*(heightMapWidth-1)*(heightMapHeight-1);
int indexCounter = indexCount;
int heightMapIndices[indexCount];
// Texture Coordinates
vec2 textureCoord[indexCount];

std::vector<vec3> redLineVertices;

//Flock myFlock;

//ParticleEffect fountain = ParticleEffect(15000, 10);
ParticleEffect effect;

GLuint programParticleEffect;


GLuint gVao[2], gVbo[2], gIbo, program, lineProgram;
GLuint textures[4];
GLuint modelViewLoc, projectionLoc, vPositionLoc, vNormalLoc, vTexCoordLoc, vPositionLineLoc, modelViewLineLoc, projectionLineLoc;


GLubyte* rockTexture;
GLubyte* grassTexture;
GLubyte* dirtTexture;
GLubyte* snowTexture;
GLuint rockTexLoc, grassTexLoc, dirtTexLoc, snowTexLoc;
mat4 modelView;
mat4 projection;

float horizontalAngle = 1.5*3.1415;
float verticalAngle = 3.1415f;//0.0f;

//vec4 at = vec4(8,0,8,0);
vec4 eye = vec4(8,5,8,1);
vec4 at =  vec4(cos(verticalAngle) * sin(horizontalAngle),
                sin(verticalAngle),
                cos(verticalAngle) * cos(horizontalAngle),
                0);//normalize(vec4(8,5,8,0) - eye);
vec4 rightVec = vec4(sin(horizontalAngle - 3.1415926f/2.0f),
                0,
                cos(horizontalAngle - 3.1415926f/2.0f), 1);

vec4 up = vec4(0,1,0,0);

Frust frustu;


// ***** Methods *****

void readHeightMap(char* filename, int width, int height){
    if(width != heightMapWidth || height != heightMapHeight){
        // just to make sure you don't have the wrong sized image
        cout << "ERROR: WRONG SIZE HEIGHTMAP. PLEASE CHANGE CONSTANTS" << endl;
        exit(0);
    }
    
    GLubyte *heightMapBytes = glmReadPPM(filename, heightMapWidth, heightMapHeight);
    
    
    int count = 0;
    for(int i = 0; i<heightMapHeight; i++){
        for(int j = 0; j<heightMapWidth; j++){
            heightMapValues[count] = (float) heightMapBytes[(i*heightMapHeight+j)*3];
            float curVal = heightMapValues[count]/64;
            float curCol = heightMapValues[count]/256;
            heightMapVectors[count] = vec3((float)j/16, curVal, (float)i/16);
            heightMapColors[count] = vec4(curCol, curCol, curCol, 1);
            count++;
        }
    }
    
    
    
    cout<< "done"<<endl;
    delete[] heightMapBytes;
}

GLuint programSphere;

void updateSpheres(){
    for(int i=0; i<spheres.size(); i++){
        spheres[i].update();
        std::cout << "sphere #" << i << " is at " << spheres[i].getCenter() << std::endl;
    }
}

void drawSpheres(mat4 model, mat4 projection){
    for(int i=0; i<spheres.size(); i++){
        spheres[i].draw(model, projection, programSphere);
    }
}



// **** COLLISION DETECTION ****
//  not finished at all

bool pointOutsideNode(OTNode curNode, vec3 pt){
    if(pt.x > root.cornerMax.x || pt.y > root.cornerMax.y || pt.z > root.cornerMax.z ||
       pt.x < root.cornerMin.x || pt.y < root.cornerMin.y || pt.z < root.cornerMin.z){
        //pt is outside node
        return true;
    }
    else{
        return false;
    }
}

bool pointInsideNode(OTNode root, vec3 pt){
    
    std::cout<<"Is point inside Node?"<<std::endl;
    std::cout<<"  Node: center = " << root.center << ", #triangles = " << root.data.size() << std::endl;
    std::cout<<"    corners: min = " << root.cornerMin << ", max = " << root.cornerMax << std::endl;
    std::cout<<"  Point: " << pt << std::endl;

    
    if((pt.x < root.cornerMax.x) && (pt.y < root.cornerMax.y) && (pt.z < root.cornerMax.z) &&
       (pt.x > root.cornerMin.x) && (pt.y > root.cornerMin.y) && (pt.z > root.cornerMin.z)){
        std::cout<< " --> Point was inside Node." <<std::endl;
        return true;
    }
    else{
        std::cout<< " --> Point was not inside Node." <<std::endl;
        return false;
    }
}


OTNode collisionNode(OTNode root, vec3 pt){
    if(!pointInsideNode(root, pt)){
        std::cout<<"*** Point was not inside Node ***"<<std::endl;
        std::cout<<"*** OOPS WAS NOT IN NODE! :( ***"<<std::endl;
        std::vector<Triangle> noTries;
        return OTNode(noTries, vec3(0,0,0), 0.0);
    }
    else{
        std::cout<<"*** Point was inside Node ***"<< '\n' << " check if has children"<<std::endl;
        //if node has no children
        if(root.hasChildren == false){
            std::cout<<"  Node has no children. Do collision detection on it."<<std::endl;
            return root; // return the correct root
        }
        else{
            std::cout<<" Check if in child 0"<<std::endl;
            if(pointInsideNode(*root.children[0], pt)){
                std::cout<<"  Point is inside child 0. Search further."<<std::endl;
                return collisionNode(*root.children[0], pt);
            }
            
            std::cout<<" Check if in child 1"<<std::endl;
            if(pointInsideNode(*root.children[1], pt)){
                std::cout<<"  Point is inside child 1. Search further."<<std::endl;
                return collisionNode(*root.children[0], pt);
            }
            
            std::cout<<" Check if in child 2"<<std::endl;
            if(pointInsideNode(*root.children[2], pt)){
                std::cout<<"  Point is inside child 2. Search further."<<std::endl;
                return collisionNode(*root.children[2], pt);
            }
            
            std::cout<<" Check if in child 3"<<std::endl;
            if(pointInsideNode(*root.children[3], pt)){
                std::cout<<"  Point is inside child 3. Search further."<<std::endl;
                return collisionNode(*root.children[3], pt);
            }
            
            std::cout<<" Check if in child 4"<<std::endl;
            if(pointInsideNode(*root.children[4], pt)){
                std::cout<<"  Point is inside child 4. Search further."<<std::endl;
                return collisionNode(*root.children[4], pt);
            }
            
            std::cout<<" Check if in child 5"<<std::endl;
            if(pointInsideNode(*root.children[5], pt)){
                std::cout<<"  Point is inside child 5. Search further."<<std::endl;
                return collisionNode(*root.children[5], pt);
            }
            
            std::cout<<" Check if in child 6"<<std::endl;
            if(pointInsideNode(*root.children[6], pt)){
                std::cout<<"  Point is inside child 6. Search further."<<std::endl;
                return collisionNode(*root.children[6], pt);
            }
            else{
                std::cout<<" Check if in child 7"<<std::endl;
                pointInsideNode(*root.children[7], pt);
                std::cout<<"  Point is inside child 7. Search further."<<std::endl;
                return collisionNode(*root.children[7], pt);
            }

        }
    }
}

bool sameSide(vec3 p1, vec3 p2, vec3 a, vec3 b){
    vec3 cp1 = cross(b-a, p1-a);
    vec3 cp2 = cross(b-a, p2-a);
    if( dot(cp1, cp2) >= 0)
        return true;
    else
        return false;
}


bool isCollidedWithTri(Sphere Sphr, Triangle tri){
    // convert triangle to plane
    
    vec3 a = heightMapVectors[tri.indices[0]];
    vec3 b = heightMapVectors[tri.indices[1]];
    vec3 c = heightMapVectors[tri.indices[2]];
    
    //std::cout<< "Triangle:" << '\n' << "  point 1: " << a << " point 2: " << b << " point :3 " << c << std::endl;
    
    Plane pl = Plane(a, b, c);
    
    //std::cout<<"Plane:" << '\n' << "  normal: " << pl.getNormal() << " distance: " << pl.getDistance() << std::endl;
    
    bool isCollided = false;
    
    // early out: normal directions
    // if collided with plane
    //std::cout << "Is collided with plane?" << std::endl;
    if(dot(pl.getNormal(),Sphr.getCenter())+pl.getDistance() < Sphr.getRadius()){
        //std::cout << "   yes." << std::endl;
        // move sphere back to test if the point of collision is actually in the triangle  [haven't done this yet]
        
        Sphr.setCenter(Sphr.getCenter()-normalize(Sphr.getDirection())*(Sphr.getRadius()-pl.getDistance())); //not sure if right
        
        float d = dot(pl.getNormal(), Sphr.getCenter())-pl.getDistance(); // gives distance from center of sphere to plane
        float t = (Sphr.getRadius()-pl.getDistance()) / dot(pl.getNormal(), Sphr.getVelocity()); // time of collision
        vec3 pt; // point of collision
        pt.x = Sphr.getCenter().x + (t*0.1) - (Sphr.getRadius()*pl.getNormal().x);
        pt.y = Sphr.getCenter().y + (t*0.1) - (Sphr.getRadius()*pl.getNormal().y);
        pt.z = Sphr.getCenter().z + (t*0.1) - (Sphr.getRadius()*pl.getNormal().z);

        
        //is point in triangle?
        if(sameSide(pt, a, b, c) &&
           sameSide(pt, b, a, c) &&
           sameSide(pt, c, a, b)){
            isCollided = true;
        }
    }
    
    // if you have a collision with the plane
    //float d  = dot(plane.normal, sphere.center)-plane.distanceFromCenter //gives distance from center of sphere to plane
    //float t = (sphere.radius – d) / dot(plane.normal, sphere.velocity) //time of collision
    //vec3 pt = sphere.center + (t*sphere.velocity)-(sphere.radius*plane.normal) //point of collision
    //std::cout << "is collided with triangle?" << '\n' << "   " << isCollided << std::endl;
    return isCollided;
}

bool isCollided(Sphere Sphr){
    bool isCollided = false;
    int inin = 0;
    for(int i = 0; i<indexCounter-2; i=i+3){
        //std::cout << "--triangle count-- " << i << std::endl;
        if(isCollidedWithTri(Sphr, Triangle(heightMapIndices[i], heightMapIndices[i+1], heightMapIndices[i+2], 0))){
            isCollided=true;
            inin = i;
            break;
        }
        inin = i;
    }
    std::cout<< "Ended at index: "<<inin<<std::endl;;
    return isCollided;
}

bool isCollidedOT(Sphere Sphr){
    bool isCollided = false;
    int inin = 0;
    OTNode colNode = collisionNode(root, Sphr.getCenter());
    for(int i = 0; i<colNode.data.size(); i++){
        //std::cout << "--triangle count-- " << i << std::endl;
        if(isCollidedWithTri(Sphr, colNode.data[i])){
            isCollided=true;
            inin = i;
            break;
        }
        inin = i;
    }
    std::cout<< "Ended at index: "<<inin<<std::endl;;
    return isCollided;
}

void collisionDetection(){
    if(spheres.size() != 0){
        //std::cout<< "Collision Detection Initiated" << std::endl;
        for(int i=0; i<spheres.size(); i++){
            //std::cout<< "Sphere #" << i << ":" << std::endl;
            if(spheres[i].getVelocity() != 0){
                if(isCollided(spheres[i])){
                    std::cout<< "Sphere stopped." << std::endl;
                    std::cout<< "current eye position: " << eye <<std::endl;
                    spheres[i].stop();
                }
            }
        }
    }
}
void collisionDetectionOT(){
    if(spheres.size() != 0){
        //std::cout<< "Collision Detection Initiated" << std::endl;
        for(int i=0; i<spheres.size(); i++){
            //std::cout<< "Sphere #" << i << ":" << std::endl;
            if(spheres[i].getVelocity() != 0){
                if(isCollidedOT(spheres[i])){
                    std::cout<< "Sphere stopped." << std::endl;
                    std::cout<< "current eye position: " << eye <<std::endl;
                    spheres[i].stop();
                }
            }
        }
    }
}




// ***** Generate Triangles and Starting Normals *****

void genTriangles(){
    int index=0;
    int normalIndex=0;
    // pointers to current points for normal calculation
    vec3 curVecStart; // starting point
    vec3 curVecA; // end of vector A
    vec3 curVecB; // end of vector B
    for(int i=0; i<heightMapWidth-1;i++){
        for(int j=0; j<heightMapHeight-1;j++){
            heightMapIndices[index] = j*heightMapWidth+i;
            curVecStart = heightMapVectors[heightMapIndices[index]];
            textureCoord[index] = vec2(0.0,0.0);
            index++;
            heightMapIndices[index] = j*heightMapWidth+i+heightMapWidth;
            curVecA = heightMapVectors[heightMapIndices[index]];
            textureCoord[index] = vec2(0.0,1.0);
            index++;
            heightMapIndices[index] = j*heightMapWidth+i+1;
            curVecB = heightMapVectors[heightMapIndices[index]];
            textureCoord[index] = vec2(1.0,1.0);
            index++;
            
            heightMapTriNormals[normalIndex] = normalize(cross((vec4(curVecA, 1)-vec4(curVecStart, 1)), (vec4(curVecB, 1)-vec4(curVecStart, 1))));
            normalIndex++;
            
            heightMapIndices[index] = j*heightMapWidth+i+1;
            curVecStart = heightMapVectors[heightMapIndices[index]];
            textureCoord[index] = vec2(0.0,0.0);
            index++;
            heightMapIndices[index] = j*heightMapWidth+i+heightMapWidth;
            curVecA = heightMapVectors[heightMapIndices[index]];
            textureCoord[index] = vec2(1.0,1.0);
            index++;
            heightMapIndices[index] = j*heightMapWidth+i+heightMapWidth+1;
            curVecB = heightMapVectors[heightMapIndices[index]];
            textureCoord[index] = vec2(1.0,0.0);
            index++;
            
            heightMapTriNormals[normalIndex] = normalize(cross((vec4(curVecA, 1)-vec4(curVecStart, 1)), (vec4(curVecB, 1)-vec4(curVecStart, 1))));
            normalIndex++;
        }
    }

}

// ***** Average Normals *****


void avgNormals(){
    
    /* 
     Cases:
     -Corner Cases
     --Corner1: handles the corners that are only influenced by one triangle
     --Corner2: handles the corners that are influenced by two triangles
     -Edge Cases
     --EdgeTop: handles the top-row cases that are influenced by three specific triangles
     --EdgeBottom: handles the bottom-row cases that are influenced by three specific triangles
     --EdgeSideL: handles the left-side-column cases that are influenced by three specific triangles
     --EdgeSideR: handles the right-side-column cases that are influenced by three specific triangles
     -Interior Case
     --Interior: handles all interior cases, influenced by six triangles.
     */
    
    // ** Corner1 Case **:
    // first normal is first triangle's normals
    heightMapAvgNormals[0] = heightMapTriNormals[0];
    // last normal is last triangle's normal
    heightMapAvgNormals[(heightMapWidth*heightMapHeight)-1] = heightMapTriNormals[(2*(heightMapWidth-1)*(heightMapHeight-1))-1];
    
    // ** Corner2 Case **:
    heightMapAvgNormals[heightMapWidth-1] = (heightMapTriNormals[2*(heightMapWidth-1)-1]+
                                             heightMapTriNormals[2*(heightMapWidth-1)-2])/2.0;
    heightMapAvgNormals[heightMapWidth*(heightMapHeight-1)] = (heightMapTriNormals[2*(heightMapWidth-1)*(heightMapHeight-2)]+
                                                               heightMapTriNormals[2*(heightMapWidth-1)*(heightMapHeight-2)+1])/2.0;
    
    // ** EdgeTop Case **
    for(int w=1; w<heightMapWidth; w++){
        heightMapAvgNormals[w] = (heightMapTriNormals[2*w-2]+
                                  heightMapTriNormals[2*w-1]+
                                  heightMapTriNormals[2*w])/3.0;
    }
    
    // ** EdgeBottom Case **
    for(int w=1; w<heightMapWidth; w++){
        heightMapAvgNormals[heightMapWidth*(heightMapHeight-1)+w] = (heightMapTriNormals[(2*(heightMapWidth-1)*(heightMapHeight-2)+w)-1]+
                                  heightMapTriNormals[(2*(heightMapWidth-1)*(heightMapHeight-2)+w)]+
                                  heightMapTriNormals[(2*(heightMapWidth-1)*(heightMapHeight-2)+w)+1])/3.0;
    }
    
    // ** EdgeSideL Case **
    for(int h=1; h<heightMapHeight; h++){
        heightMapAvgNormals[h*heightMapWidth] = (heightMapTriNormals[(2*(heightMapWidth-1)*(h-1))]+
                                                 heightMapTriNormals[(2*(heightMapWidth-1)*(h-1))+1]+
                                                 heightMapTriNormals[(2*(heightMapWidth-1)*h)])/3.0;
    }
    
    // ** EdgeSideR Case **
    for(int h=1; h<heightMapHeight; h++){
        heightMapAvgNormals[((h+1)*heightMapWidth)-1] = (heightMapTriNormals[(2*(heightMapWidth-1)*(h-1))-1]+
                                                 heightMapTriNormals[(2*(heightMapWidth-1)*h)-2]+
                                                 heightMapTriNormals[(2*(heightMapWidth-1)*h)-1])/3.0;
    }
    
    // ** Interior Case **
    for(int w=1; w<heightMapWidth; w++){
        for(int h=1; h<heightMapHeight; h++){
            heightMapAvgNormals[h*heightMapWidth+w] = (heightMapTriNormals[(2*(heightMapWidth-1)*(h-1)+w)-1]+
                                                       heightMapTriNormals[(2*(heightMapWidth-1)*(h-1)+w)]+
                                                       heightMapTriNormals[(2*(heightMapWidth-1)*(h-1)+w)+1]+
                                                       heightMapTriNormals[(2*(heightMapWidth-1)*h+w)-2]+
                                                       heightMapTriNormals[(2*(heightMapWidth-1)*h+w)-1]+
                                                       heightMapTriNormals[(2*(heightMapWidth-1)*h+w)])/6.0;
        }
    }
}




// ***** Initialization *****


void init(){
    
    //cam = new Camera();
    
    program = InitShader("HeightMap_vShader.glsl", "HeightMap_fShader.glsl");
    modelViewLoc = glGetUniformLocation(program, "modelView");
    projectionLoc = glGetUniformLocation(program, "projection");
    vPositionLoc = glGetAttribLocation(program, "vPosition");
    vNormalLoc = glGetAttribLocation(program, "vNormal");
    vTexCoordLoc = glGetAttribLocation(program, "vTexCoord");
    
    lineProgram = InitShader("vShaderRedLine.glsl", "fShaderRedLine.glsl");
    modelViewLineLoc = glGetUniformLocation(lineProgram, "modelView");
    projectionLineLoc = glGetUniformLocation(lineProgram, "projection");
    vPositionLineLoc = glGetAttribLocation(lineProgram, "vPosition");
    
    readHeightMap("heightmap256.ppm", 256, 256);
    genTriangles();
    avgNormals();
    
    programSphere = InitShader("sphere_vShader.glsl", "sphere_fShader.glsl");
    programParticleEffect = InitShader("particle2_vShader.glsl", "particle2_fShader.glsl");
    
    //fountain.initParticleEffet(programParticleEffect);
//    effect.init(programParticleEffect);

    
    rt = genOctree(heightMapIndices, indexCount, heightMapVectors, vec3(8,0,8), 8.0);
    //goThroughTree(rt);
    //redLineVertices = generateVertices(root);
    
    frustu = Frust(70, windowWidth/windowHeight, 0.1, 100, eye, at, rightVec, up, lineProgram);
    
    //myFlock =  Flock(50, vec3(16,4,16), vec3(0,2,0));
    //myFlock.initFlock();
    
    glGenBuffers(1, &gIbo);
    glGenBuffers(2, gVbo);
    
    glEnableVertexAttribArray(0);
    glBindVertexArrayAPPLE(0);
    glGenVertexArraysAPPLE(2, gVao);
    glBindVertexArrayAPPLE(gVao[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, gVbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(heightMapVectors) + sizeof(heightMapAvgNormals) + sizeof(textureCoord), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(heightMapVectors), heightMapVectors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(heightMapVectors), sizeof(heightMapAvgNormals), heightMapAvgNormals);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(heightMapVectors)+sizeof(heightMapAvgNormals), sizeof(textureCoord) , textureCoord);
    
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIbo);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(heightMapIndices), NULL, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(heightMapIndices), NULL, GL_STATIC_DRAW);

    
    glEnableVertexAttribArray(vPositionLoc);
    glVertexAttribPointer(vPositionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vNormalLoc);
    glVertexAttribPointer(vNormalLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(heightMapVectors)));
    glEnableVertexAttribArray(vTexCoordLoc);
    glVertexAttribPointer(vTexCoordLoc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(heightMapVectors)+sizeof(heightMapAvgNormals)));
    
    
    int rockWidth, rockHeight, rockChannels;
    rockTexture = glmReadPPM("rock.ppm", &rockWidth, &rockHeight);
    //rockTexture = stbi_load("rock.JPG", &rockWidth, &rockWidth, &rockChannels, 0);
    
    int grassWidth, grassHeight, grassChannels;
    grassTexture = glmReadPPM("grass.ppm", &grassWidth, &grassHeight);
    //grassTexture = stbi_load("grass.JPG", &grassWidth, &grassHeight, &grassChannels, 0);
    
    int dirtWidth, dirtHeight, dirtChannels;
    dirtTexture = glmReadPPM("dirt.ppm", &dirtWidth, &dirtHeight);
    //dirtTexture = stbi_load("dirt.JPG", &dirtWidth, &dirtHeight, &dirtChannels, 0);
    
    int snowWidth, snowHeight, snowChannels;
    snowTexture = glmReadPPM("snowB.ppm", &snowWidth, &snowHeight);
    //snowTexture = stbi_load("snow.JPG", &snowWidth, &snowHeight, &snowChannels, 0);
    
    //glUniform1i(glGetUniformLocation(program, "rockTex"), 0);
    //glUniform1i(glGetUniformLocation(program, "grassTex"), 1);
    //glUniform1i(glGetUniformLocation(program, "dirtTex"), 2);
    //glUniform1i(glGetUniformLocation(program, "snowTex"), 3);
    
    glGenTextures(4, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rockWidth, rockHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, rockTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassWidth, grassHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, grassTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dirtWidth, dirtHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, dirtTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snowWidth, snowHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, snowTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textures[3]);


    glBindVertexArrayAPPLE(0);
    glBindVertexArrayAPPLE(gVao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(&redLineVertices[0]), &redLineVertices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(vPositionLineLoc);
    glVertexAttribPointer(vPositionLineLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindVertexArrayAPPLE(0);

    glEnable(GL_DEPTH_TEST);
    //glClearColor(0.0, 0.5, 0.7, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    cout<<"done init"<<endl;
}

// ***** Display *****

float prevTime = 0;
bool pauseFrustum = false;
bool drawFrustum = false;
bool pauseMouse = false;
std::vector<int> indices;

Frust temp = frustu;

float fps;
float frame;
float fpsElapsedTime;
float fpsPrevTime;


void display(){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program);
    
    modelView = LookAt(eye, eye+at, up);
    frustu.updateFrustum(eye, at, up, rightVec);
    projection = Perspective(frustu.fov, frustu.fratio, frustu.nearDist, frustu.farDist);
    
    glUniform1i(glGetUniformLocation(program, "rockTex"), 0);
    glUniform1i(glGetUniformLocation(program, "grassTex"), 1);
    glUniform1i(glGetUniformLocation(program, "dirtTex"), 2);
    glUniform1i(glGetUniformLocation(program, "snowTex"), 3);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    
    
    glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);
    
    if(!pauseFrustum){
        std::vector<int> iboUpdates = cullAndRender(*rt, frustu);
        int iboIndices[indexCount];
        for(int i = 0;  i< iboUpdates.size(); i++){
            iboIndices[i] = iboUpdates[i];
        }
        for(int i = (int)iboUpdates.size(); i < indexCount; i++){
            iboIndices[i] = 0;
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIbo);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(iboIndices), iboIndices);
        
        temp = frustu;
    }

    
    glBindVertexArrayAPPLE(gVao[0]);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArrayAPPLE(0);
    
    if(pauseFrustum){
        temp.draw(modelView, projection, lineProgram);
    }

    

    
    /*
    //std::cout << "GL_ERROR: " << glGetError() << std::endl;
    
//    glUseProgram(lineProgram);
//    
//    glUniformMatrix4fv(modelViewLineLoc, 1, GL_TRUE, modelView);
//    glUniformMatrix4fv(projectionLineLoc, 1, GL_TRUE, projection);
//
//    glBindVertexArrayAPPLE(gVao[1]);
//    glDrawArrays(GL_LINES, 0, redLineVertices.size());
//    glBindVertexArrayAPPLE(0);
     //    collisionDetectionOT();
     //    updateSpheres();
     //    drawSpheres(modelView, projection);
     
//    collisionDetectionOT();
//    updateSpheres();
//    drawSpheres(modelView, projection);
    */
    

    /*
    //fountain.Update(deltaTime/5000.0);
    //std::cout<< "Delta Time: " << deltaTime/1000.0<<std::endl;
    //fountain.Draw(modelView, projection, programParticleEffect);
    
//    glUseProgram(programParticleEffect);
//    effect.update(deltaTime/1000.0, eye, at, up);
//    effect.render(programParticleEffect, modelView, projection);
    
    
    //glUseProgram(myFlock.program);
    //myFlock.updateFlock(time/1000.0);
    //myFlock.renderFlock(modelView, projection);
     */
    
    glutSwapBuffers();
}

// ***** Reshape *****

void reshape(GLsizei w, GLsizei h){
    glViewport(0, 0, w, h);
    windowWidth = w;
    windowHeight = h;
}

// ***** Keyboard & Mouse *****

float mouseSpeed = 0.5;

float currentTimeMouse, lastTimeMouse, deltaTimeMouse;
int32_t deltaX, deltaY;
CGPoint center = CGPointMake(windowWidth/2.0f, windowHeight/2.0f);

/*
 adapted from http://www.opengl-tutorial.org/beginners-tutorials/tutorial-6-keyboard-and-mouse/
 */
void mouseMotion(int x, int y){
    if(!pauseMouse){
        currentTimeMouse = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
        deltaTimeMouse = currentTimeMouse - lastTimeMouse;
        lastTimeMouse = currentTimeMouse;
        
        CGGetLastMouseDelta(&deltaX, &deltaY);
        //move pointer to center
        CGWarpMouseCursorPosition(center);
        
        horizontalAngle -= mouseSpeed * float(deltaX) * deltaTimeMouse * mouseSpeed; //* deltaTimeMouse * float(windowWidth/2 - x);
        verticalAngle   -= mouseSpeed * float(deltaY)  * deltaTimeMouse * mouseSpeed; //deltaTimeMouse * float(windowHeight/2 - y);
        
        at = vec4(cos(verticalAngle) * sin(horizontalAngle),
                  sin(verticalAngle),
                  cos(verticalAngle) * cos(horizontalAngle),
                  0);
        
        rightVec = vec4(sin(horizontalAngle - 3.1415926f/2.0f),
                        0,
                        cos(horizontalAngle - 3.1415926f/2.0f), 1);
        
        up = cross(rightVec, at);
        
        
        glutPostRedisplay();
    }
}


float currentTimeKey, lastTimeKey, deltaTimeKey;
float speed = 0.6f;

void keyboard(unsigned char key, int x, int y){
    
    currentTimeKey = glutGet(GLUT_ELAPSED_TIME)/1000.0f;
    deltaTimeKey = currentTimeKey - lastTimeKey;
    lastTimeKey = currentTimeKey;

    
    switch (key){
        case 'q': case 'Q': case 033: // esc key
            exit( EXIT_SUCCESS);
            break;
//        case 032: case 'i':
//            spheres.push_back(Sphere(vec3(eye.x, eye.y, eye.z), 0.1,(at-eye), programSphere));
//            break;
        case 'w':
            eye += at * speed * deltaTimeKey;
            //mouseMotion(x, y);
            glutPostRedisplay();
            break;
        case 's':
            eye -= at * speed * deltaTimeKey;
            //mouseMotion(x, y);
            glutPostRedisplay();
            break;
        case 'd':
            eye += rightVec * speed * deltaTimeKey;
            //at += rightVec * speed;
            //mouseMotion(x,y);
            glutPostRedisplay();
            break;
        case 'a':
            eye -= rightVec * speed * deltaTimeKey;
            //at -= rightVec * speed;
            //mouseMotion(x,y);
            glutPostRedisplay();
            break;
        case 'W':
            eye += at * speed * 2* deltaTimeKey;
            //mouseMotion(x, y);
            glutPostRedisplay();
            break;
        case 'S':
            eye -= at * speed * 2* deltaTimeKey;
            //mouseMotion(x, y);
            glutPostRedisplay();
            break;
        case 'D':
            eye += rightVec * speed * 2 * deltaTimeKey;
            //at += rightVec * speed;
            //mouseMotion(x,y);
            glutPostRedisplay();
            break;
        case 'A':
            eye -= rightVec * speed * 2 * deltaTimeKey;
            //at -= rightVec * speed;
            //mouseMotion(x,y);
            glutPostRedisplay();
            break;
        case 'b': case 'B':
            pauseFrustum = !pauseFrustum;
            glutPostRedisplay();
            break;
        case 'x': case 'X':
            std::cout<<"Eye: " <<eye<<std::endl;
            break;
        case 'h': case 'H':
            eye = vec4(8,5,8,1);
            glutPostRedisplay();
            break;
        case 'p': case 'P':
            pauseMouse = !pauseMouse;
            glutPostRedisplay();
            break;
        default:
            mouseMotion(x, y);
            break;
    }
}


void handleIdle(){
    frame++;
    fpsElapsedTime = glutGet(GLUT_ELAPSED_TIME);
    if(fpsElapsedTime - fpsPrevTime > 1000){
        fps = frame*1000/(fpsElapsedTime-fpsPrevTime);
        fpsPrevTime = fpsElapsedTime;
        frame = 0;
        std::cout<<"fps: "<<fps<<std::endl;
        
    }

    
    glutPostRedisplay();
}


// ***** Main *****

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Height Map Terrain Generation");
    
    init();
    glutSetCursor(GLUT_CURSOR_NONE);
    //glutWarpPointer(windowWidth/2.0f, windowHeight/2.0f);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMotion);
    //glutMouseFunc(mouseMotion);
    glutIdleFunc(handleIdle);

    glutMainLoop();
    return 0;
}

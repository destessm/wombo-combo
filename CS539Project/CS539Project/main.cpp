//
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
#include "stb_image.h"
#include "Octree.h"
#include "Flock.h"
#include "Camera2.h"
#include "Sphere.h"

using namespace std;

// ***** Fields *****

// window size
int windowWidth = 512;
int windowHeight = 512;


//I know this is bad, I just never got around to fixing it
const int heightMapWidth = 512;
const int heightMapHeight = 512;

Camera *cam;

vector<Sphere> spheres;

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
int heightMapIndices[indexCount];
// Texture Coordinates
vec2 textureCoord[indexCount];

std::vector<vec3> redLineVertices;

//Flock myFlock;


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

vec4 at = vec4(8,0,8,0);
vec4 eye = vec4(3,5,3,1);
vec4 up = vec4(0,1,0,0);



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
            heightMapVectors[count] = vec3((float)j/32, curVal, (float)i/32);
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


bool isCollidedWithTri(Sphere Sphr, Triangle tri){
    // convert triangle to plane
    // early out: normal directions
    // if collided with plane
    //    move sphere back to test if the point of collision is actually in the triangle
    
    // if you have a collision with the plane
    //float d  = dot(plane.normal, sphere.center)-plane.distanceFromCenter //gives distance from center of sphere to plane
    //float t = (sphere.radius – d) / dot(plane.normal, sphere.velocity) //time of collision
    //vec3 pt = sphere.center + (t*sphere.velocity)-(sphere.radius*plane.normal) //point of collision
    return false;
}

bool isCollided(Sphere Sphr){
    bool isCollided = false;
    for(int i = 0; i<(heightMapWidth*heightMapHeight); i+=3){
        if(isCollidedWithTri(Sphr, Triangle(i, i, i, 0))){ // THIS LINE ISN'T RIGHT AT ALL
            isCollided=true;
        }
    }
    return isCollided;
}

void collisionDetection(){
    for(int i=0; i<spheres.size(); i++){
        if(isCollided(spheres[i])){
            spheres[i].stop();
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
    //delete [] curVecStart;
    //delete [] curVecA;
    //delete [] curVecB;

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
    
    cam = new Camera();
    
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
    
    readHeightMap("heightmap512_1.ppm", 512, 512);
    genTriangles();
    avgNormals();
    
    //OTNode* root = genOctree(heightMapIndices, indexCount, heightMapVectors, vec3(8,0,8), 8.0);
    //goThroughTree(root);
    //redLineVertices = generateVertices(root);
    
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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(heightMapIndices), heightMapIndices, GL_STATIC_DRAW);
    
    
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
    
    
    
    glUniform1i(glGetUniformLocation(program, "rockTex"), 0);
    glUniform1i(glGetUniformLocation(program, "grassTex"), 1);
    glUniform1i(glGetUniformLocation(program, "dirtTex"), 2);
    glUniform1i(glGetUniformLocation(program, "snowTex"), 3);
    

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

void display(){
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(program);
    
    modelView = LookAt(eye, at, up);
    projection = Perspective(70, windowWidth/windowHeight, 0.1, 100);
    
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
    
    glBindVertexArrayAPPLE(gVao[0]);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArrayAPPLE(0);
    
    
    
    glUseProgram(lineProgram);
    
    glUniformMatrix4fv(modelViewLineLoc, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(projectionLineLoc, 1, GL_TRUE, projection);

    glBindVertexArrayAPPLE(gVao[1]);
    glDrawArrays(GL_LINES, 0, redLineVertices.size());
    glBindVertexArrayAPPLE(0);
    

    updateSpheres();
    drawSpheres(modelView, projection);
    
    
    float time = glutGet(GLUT_ELAPSED_TIME);
    
    //glUseProgram(myFlock.program);
    //myFlock.updateFlock(time/1000.0);
    //myFlock.renderFlock(modelView, projection);
    
    glutSwapBuffers();
}

// ***** Reshape *****

void reshape(GLsizei w, GLsizei h){
    glViewport(0, 0, w, h);
    windowWidth = w;
    windowHeight = h;
}

// ***** Keyboard *****

void keyboard(unsigned char key, int x, int y){
    switch (key){
        case 'q': case 'Q': case 033: // esc key
            exit( EXIT_SUCCESS);
            break;
        case 032: case 'i':
            spheres.push_back(Sphere(vec3(eye.x, eye.y, eye.z), 5,(at-eye), programSphere));
            break;
//        case 'w': case 'W':
//            cam->moveForward();
//            glutPostRedisplay();
//            break;
//        case 's': case 'S':
//            cam->moveBackward();
//            glutPostRedisplay();
//        case 'a': case 'A':
//            cam->turnLeft();
//            glutPostRedisplay();
//        case 'd': case 'D':
//            cam->turnRight();
//            glutPostRedisplay();
//        case 'z': case 'Z':
//            cam->xup();
//            glutPostRedisplay();
//        case 'c': case 'C':
//            cam->xdown();
//            glutPostRedisplay();
        case 'z':
            eye.x -= 0.2;
            glutPostRedisplay();
            break;
        case 'Z':
            eye.x += 0.2;
            glutPostRedisplay();
            break;
        case 'x':
            eye.y -= 0.2;
            glutPostRedisplay();
            break;
        case 'X':
            eye.y += 0.2;
            glutPostRedisplay();
            break;
        case 'c':
            eye.z -= 0.2;
            glutPostRedisplay();
            break;
        case 'C':
            eye.z += 0.2;
            glutPostRedisplay();
            break;
        default:
            break;
    }
}
/*
void keyboardUp(unsigned char key, int x, int y){
    switch (key){
        case 'w': case 'W':
            cam->holdingForward = false;
            glutPostRedisplay();
            break;
        case 's': case 'S':
            cam->holdingBackward = false;
            glutPostRedisplay();
        case 'a': case 'A':
            cam->holdingLeftStrafe = false;
            glutPostRedisplay();
        case 'd': case 'D':
            cam->holdingRightStrafe = false;
            glutPostRedisplay();
        default:
            break;
    }
}*/

void handleMouseMove(int mouseX, int mouseY){
    cam->mouseMovement(mouseX, mouseY);
}

// ***** Main *****

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Height Map Terrain Generation");
    
    init();
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(handleMouseMove);
    glutIdleFunc(glutPostRedisplay);

    glutMainLoop();
    return 0;
}

//
//  main.cpp
//  CS539Project
//
//  Created by David Estes-Smargiassi on 2/27/15.
//  Copyright (c) 2015 David Estes-Smargiassi. All rights reserved.
//

// ***** Import Statements *****

#include <iostream>
#include "Angel.h"
#include "glm.h"
#include "stb_image.h"

using namespace std;

// ***** Fields *****

// window size
int windowWidth = 512;
int windowHeight = 512;


const int heightMapWidth = 257;
const int heightMapHeight = 257;


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


GLuint gVao, gVbo, gIbo, program, gVaoTex;
GLuint textures[4];
GLuint modelViewLoc, projectionLoc, vPositionLoc, vNormalLoc, vTexCoordLoc;

GLubyte* rockTexture;
GLubyte* grassTexture;
GLubyte* dirtTexture;
GLubyte* snowTexture;
GLuint rockTexLoc, grassTexLoc, dirtTexLoc, snowTexLoc;
mat4 modelView;
mat4 projection;

vec4 at = vec4(8,0,8,0);
vec4 eye = vec4(-3,9,0,1);
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
            heightMapVectors[count] = vec3((float)j/16, curVal, (float)i/16);
            heightMapColors[count] = vec4(curCol, curCol, curCol, 1);
            count++;
        }
    }
    
    
    
    cout<< "done"<<endl;
    delete[] heightMapBytes;
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
            
            heightMapTriNormals[normalIndex] = normalize(cross((curVecA-curVecStart), (curVecB-curVecStart)));
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
            
            heightMapTriNormals[normalIndex] = normalize(cross((curVecA-curVecStart), (curVecB-curVecStart)));
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
    
    program = InitShader("HeightMap_vShader.glsl", "HeightMap_fShader.glsl");
    modelViewLoc = glGetUniformLocation(program, "modelView");
    projectionLoc = glGetUniformLocation(program, "projection");
    vPositionLoc = glGetAttribLocation(program, "vPosition");
    vNormalLoc = glGetAttribLocation(program, "vNormal");
    vTexCoordLoc = glGetUniformLocation(program, "vTexCoord");
    
    readHeightMap("heightmapWiki.ppm", 257, 257);
    genTriangles();
    avgNormals();
    
    
    glGenBuffers(1, &gIbo);
    glGenBuffers(1, &gVbo);
    
    glEnableVertexAttribArray(0);
    glBindVertexArrayAPPLE(0);
    glGenVertexArraysAPPLE(1, &gVao);
    glBindVertexArrayAPPLE(gVao);
    
    glBindBuffer(GL_ARRAY_BUFFER, gVbo);
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
    rockTexture = glmReadPPM("rock.ppm", &rockWidth, &rockWidth);
    //rockTexture = stbi_load("rock.JPG", &rockWidth, &rockWidth, &rockChannels, 0);
    
    int grassWidth, grassHeight, grassChannels;
    grassTexture = glmReadPPM("grass.ppm", &grassWidth, &grassHeight);
    //grassTexture = stbi_load("grass.JPG", &grassWidth, &grassHeight, &grassChannels, 0);
    
    int dirtWidth, dirtHeight, dirtChannels;
    dirtTexture = glmReadPPM("dirt.ppm", &dirtWidth, &dirtHeight);
    //dirtTexture = stbi_load("dirt.JPG", &dirtWidth, &dirtHeight, &dirtChannels, 0);
    
    int snowWidth, snowHeight, snowChannels;
    snowTexture = glmReadPPM("snow.ppm", &snowWidth, &snowHeight);
    //snowTexture = stbi_load("snow.JPG", &snowWidth, &snowHeight, &snowChannels, 0);
    
    glGenTextures(4, textures);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rockWidth, rockHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, rockTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    
    glUniform1i(glGetUniformLocation(program, "Texture0"), 0);

    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grassWidth, grassHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, grassTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    
    glUniform1i(glGetUniformLocation(program, "Texture1"), 1);

    
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, dirtWidth, dirtHeight, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, dirtTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    
    glUniform1i(glGetUniformLocation(program, "Texture2"), 2);

    
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, textures[3]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, rockTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    
    glUniform1i(glGetUniformLocation(program, "Texture3"), 3);

    
    

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
    projection = Perspective(70, windowWidth/windowHeight, 1.0, 100);
    
    
    glUniformMatrix4fv(modelViewLoc, 1, GL_TRUE, modelView);
    glUniformMatrix4fv(projectionLoc, 1, GL_TRUE, projection);
    
    glBindVertexArrayAPPLE(gVao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArrayAPPLE(0);
    
    
    
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
    
    glutMainLoop();
    return 0;
}

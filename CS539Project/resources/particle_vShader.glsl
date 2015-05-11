#version 120

//attribute vec3 vLife;
varying vec4 color;
uniform mat4 modelMatrix, projectionMatrix;
uniform vec3 initVel, initPos, accel;
attribute float life;

void main(){
    
    color = vec4(0,0,1,1);
    
//    vec3 pos = initPos + initVel*vLife+ (0.5)*accel*vLife*vLife;
    vec3 pos = initPos + initVel*life + (0.5)*accel*life*life;
    
    gl_Position = projectionMatrix*modelMatrix*vec4(pos,1.0);
}
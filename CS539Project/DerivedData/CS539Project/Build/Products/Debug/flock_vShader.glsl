#version 120

attribute vec3 vPosition;
attribute vec4 vColor;
varying vec4 fColor;
uniform mat4 modelMatrix, projectionMatrix;
uniform mat4 translation, rotation;

void main(){
    
    fColor = vColor;
    
    gl_Position = projectionMatrix*translation*modelMatrix*vec4(vPosition,0.0);
}
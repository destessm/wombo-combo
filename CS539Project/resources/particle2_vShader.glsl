#version 120

attribute vec3 vPosition;
attribute vec4 vColor;
attribute vec2 vTexCoords;
uniform mat4 modelMatrix, projectionMatrix;
varying vec4 color;
varying vec2 texCoords;

void main(){
    
    color = vColor;
    texCoords = vTexCoords;
    gl_Position = projectionMatrix*modelMatrix*vec4(vPosition,1.0);
    
}
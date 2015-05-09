#version 120

attribute vec4 vPosition;
varying vec4 color;
uniform mat4 modelView, projection;

void main(){
    
    color = vec4(1,0,0,1);
    
    gl_Position = projection*modelView*vPosition;
}
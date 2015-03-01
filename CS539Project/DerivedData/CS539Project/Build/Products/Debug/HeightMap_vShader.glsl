#version 120

attribute vec3 vPosition;
attribute vec3 vNormal;
varying vec3 fPosition;
varying vec3 fNormal;
uniform mat4 modelView, projection;

void main() 
{
    fPosition = vPosition;
    fNormal = vNormal;
    gl_Position = projection*modelView*vec4(vPosition,1.0);
}
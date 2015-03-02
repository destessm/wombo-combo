#version 120

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexCoord;
varying vec3 fPosition;
varying vec3 fNormal;
varying vec2 fTexCoord;
uniform mat4 modelView, projection;

void main() 
{
    fPosition = vPosition;
    fNormal = vNormal;
    fTexCoord = vTexCoord;
    gl_Position = projection*modelView*vec4(vPosition,1.0);
}
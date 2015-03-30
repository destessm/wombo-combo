attribute  vec4 vPosition;
attribute  vec3 vNormal;

varying vec3 fNormal;
varying vec4 fPosition;

uniform mat4 projectionMatrix;
uniform mat4 modelMatrix;
uniform mat4 translation;
uniform mat4 scale;

void main() 
{
    fNormal = vNormal;
    fPosition = modelMatrix*vPosition;
    gl_Position = projectionMatrix*modelMatrix*translation*scale*vPosition;
} 

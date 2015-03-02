#version 120
varying vec3 fPosition;
varying vec3 fNormal;  //get the interpolated color from the vertex shader
varying vec2 fTexCoord;

uniform sampler2D Texture0;
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform sampler2D Texture3;


vec4 materialAmbient = vec4(0.3,0.3,0.3,1.0);
vec4 materialDiffuse = vec4(1.0,1.0,1.0,1.0);
vec4 materialSpecular = vec4(1.0,0.0,0.0,1.0);
float materialShininess = 25.0;

vec4 lightPosition = vec4(8.0,1.0,8.0,1.0);
vec4 lightAmbient = vec4(0.5,0.5,0.5,1.0);
vec4 lightDiffuse = vec4(1.0,1.0,1.0,1.0);
vec4 lightSpecular = vec4(0.0,0.3,7.0,1.0);

struct lightSource
{
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constantAttenuation, linearAttenuation, quadraticAttenuation;
    float spotCutoff, spotExponent;
    vec3 spotDirection;
};

lightSource light = lightSource(lightPosition,
                                lightAmbient,
                                lightDiffuse,
                                lightSpecular,
                                0.0, 0.3, 0.0,
                                180.0, 00.0,
                                vec3(0,0,0));

mat4 v_inv;

vec4 totalLighting;

void main()
{
    totalLighting = materialAmbient*lightAmbient;
    
    vec3 N = normalize(fNormal);
    vec3 P = normalize(fPosition);
    
//    float Ks = pow(max(dot(N,H),0.0),materialShininess);
//    vec4 specular = Ks*materialSpecular*lightSpecular;
    
    vec4 viewDirection = normalize(v_inv*vec4(0,0,0,1) - vec4(fPosition,1.0));
    vec3 posToLight = fPosition-lightPosition.xyz;
    float dist = length(posToLight);
    vec3 direction = normalize(posToLight);
    float totalAtten = 1.0/dist;
    vec4 diffuse = totalAtten*lightDiffuse*materialDiffuse*max(dot(N,direction),0.0);
    vec4 specular = totalAtten*materialSpecular*lightSpecular*pow(max(dot(reflect(-direction,N),viewDirection.xyz),0.0),materialShininess);
    
//     disregard specular light if the light is behind the vertex
    if(dot(N,direction) < 0.0){
        specular = vec4(0.0,0.0,0.0,1.0);
    }
    
    //**TEXTURE STUFF**
    float heightHigh = fPosition.y;
    float heightLow = 1.0-heightHigh;
    float steepnessHigh = (acos((dot(vec3(0,1,0), fNormal)))*180.0/3.1415926)/90.0;
    float steepnessLow = 1.0-steepnessHigh;
    
    vec4 grass = texture2D(Texture1, fTexCoord);
    grass.a = (0.7*heightLow) + (0.3*steepnessLow);
    vec4 dirt = texture2D(Texture2, fTexCoord);
    dirt.a = (0.5*heightLow) + (0.5*steepnessHigh);
    vec4 rock = texture2D(Texture0, fTexCoord);
    rock.a = (0.4*heightHigh) + (0.6*steepnessHigh);
    vec4 snow = texture2D(Texture3, fTexCoord);
    snow.a = (0.7*heightHigh) + (0.3*steepnessLow);
    
    vec4 mix1 = mix(grass, dirt, 0.5);
    vec4 mix2 = mix(rock, snow, 0.5);
    
    totalLighting = totalLighting+diffuse+specular;

    gl_FragColor = totalLighting;// * mix(mix1, mix2, 0.5);  //apply the uniform color to the fragment
   // gl_FragColor.a = 1.0;
} 


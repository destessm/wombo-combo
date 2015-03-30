varying vec3 fNormal;
varying vec4 fPosition;

vec4 materialAmbient = vec4(0.3,0.3,0.3,1);
vec4 materialDiffuse = vec4(0.8,0.8,0.8,1);
vec4 materialSpecular = vec4(1.0,1.0,1.0,1);
float materialShine = 25.0;

vec4 sceneAmb = vec4(0.5,0.5,0.5,1.0);

mat4 v_inv;

struct lightSource
{
    vec4 position;
    vec4 diffuse;
    vec4 specular;
    float constantAttenuation, linearAttenuation, quadraticAttenuation;
    float spotCutoff, spotExponent;
    vec3 spotDirection;
};

lightSource light0 = lightSource(vec4(0.0,5.0,2.0,1.0),
                                 vec4(1.0, 1.0, 1.0,1.0),
                                 vec4(1.0, 1.0, 1.0, 1.0),
                                 0.0, 0.7, 0.0,
                                 180.0, 00.0,
                                 vec3(0.0,0.0,0.0));

vec4 diffuse;
vec4 specular;
vec4 totalLighting;

void main() 
{
    vec3 N = normalize(fNormal);
    vec3 P = normalize(fPosition).xyz;
    
    vec3 lightDirection;
    float totalAtten;
    
    
    totalLighting =  sceneAmb * materialAmbient;

    
    if(0.0 == light0.position.w){
        totalAtten = 1.0;
        lightDirection = normalize(vec3(light0.position));
    }
    else{
        vec3 positionToLight = vec3(light0.position-fPosition);
        float distance = length(positionToLight);
        lightDirection = normalize(positionToLight);
        totalAtten = 1.0 / (light0.constantAttenuation
                            + light0.linearAttenuation * distance
                            + light0.quadraticAttenuation * distance * distance);
        
        if(light0.spotCutoff <= 90.0){ //spotlight?
            float clampedCosine = max(0.0, dot(-lightDirection, normalize(light0.spotDirection)));
            if(clampedCosine < cos(radians(light0.spotCutoff))){ //outside spotlight cone?
                totalAtten = 0.0;
            }
            else{
                totalAtten = totalAtten * pow(clampedCosine, light0.spotExponent);
            }
            
        }
    }
    
    
      vec4 viewDirection = normalize(v_inv*vec4(0,0,0,1)-fPosition);
    
    diffuse = totalAtten * light0.diffuse * materialDiffuse * max(0.0, dot(N, lightDirection));
    
    
    if(dot(N, lightDirection) < 0.0){//light source on the wrong side?
        specular = vec4(0.0,0.0,0.0,1.0); //no specular
    }
    else{
        specular = totalAtten * light0.specular * materialSpecular * pow(max(0.0,dot(reflect(-lightDirection,N),viewDirection.xyz)), materialShine);
    }
    
    totalLighting = totalLighting + diffuse + specular;

    gl_FragColor = totalLighting;
} 


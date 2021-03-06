#version 450

#include "../HEADER.glsl"

#extension GL_ARB_separate_shader_objects : enable

#define LIGHTSOURCE_NONE 0
#define LIGHTSOURCE_DIR 1
#define LIGHTSOURCE_POINT 2
#define LIGHTSOURCE_SPOT 3

#define MAX_LIGHTS_NUM 150

struct Light{ //size = 64
	int type; //size = 4, offset = 0
    float range; // size = 4, offset = 4
	float intensity; //size = 4, offset = 8
	float spot_angle; //size = 4, offset = 12
	vec3 pos; //size = 16, offset = 16 
	vec3 dir; //size = 16, offset = 32
	vec3 color; //size = 16, offset = 48
};

layout( push_constant ) uniform constants
{
    int CameraIndex;
} PushConstants;

layout(std140, set = 0, binding = 2) readonly buffer CamsBuffer{   
	CameraInfo cameras[];
} camsBuffer;


layout (std140, binding = 1) uniform Lights{
    uniform Light lights[MAX_LIGHTS_NUM];
    uniform int lights_amount;
    uniform vec3 ambient_light_color;
};


layout(location = 0) in vec2 UVCoord;
layout(location = 1) in vec3 FragPos;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2D tDiffuse;
layout(set = 1, binding = 1) uniform sampler2D tNormal;
layout(set = 1, binding = 2) uniform sampler2D tPos;
layout(set = 1, binding = 3) uniform sampler2D tSpec;
layout(set = 1, binding = 4) uniform sampler2D tMasks;

void main() {
	vec4 Diffuse = texture(tDiffuse, UVCoord);
    vec3 FragPos = texture(tPos, UVCoord).rgb;
	vec3 Normal = texture(tNormal, UVCoord).rgb;
    vec4 Specular = texture(tSpec, UVCoord);
	vec4 Masks = texture(tMasks, UVCoord);  

    vec3 result = Diffuse.xyz;

    vec3 CamPos = camsBuffer.cameras[PushConstants.CameraIndex].Pos;

    //Check, if fragment isn't skybox
    if(Masks.r == 1){
        result *= (1 - Masks.g);
        result *= ambient_light_color;
        
        bool Shadowed = Masks.g > 0.02; 

        float specularFactor = Specular.r * 255.f; //Get factor in A channel
        vec3 camToFragDirection = normalize(CamPos - FragPos);
    
        for(int lg = 0; lg < lights_amount; lg ++){
        
            if(lights[lg].type == LIGHTSOURCE_DIR){
                float lightcoeff = max(dot(Normal, normalize(lights[lg].dir)), 0.0) * lights[lg].intensity;
                vec3 rlight = lightcoeff * lights[lg].color;
                //Specular calculation
                vec3 lightDirReflected = reflect(normalize(-lights[lg].dir), Normal);
                float angle = max(dot(camToFragDirection, lightDirReflected), 0.0);
                if(!Shadowed)
                    rlight += pow(angle, 32) * specularFactor * lights[lg].color;
			    //add light to result color
                result += rlight;
            }
            if(lights[lg].type == LIGHTSOURCE_POINT){
                //distance from light to fragment
                float Dist = length(lights[lg].pos - FragPos);
                vec3 Dir = normalize(lights[lg].pos - FragPos);
                //calculate factor of light
                float factor = 1.0 / ( 1.0 + 1.0 / lights[lg].range * Dist + 1.0 / lights[lg].range * Dist * Dist) * lights[lg].intensity;
                
                float lightcoeff = max(dot(Normal, normalize(Dir)), 0.0) * lights[lg].intensity;
                vec3 rlight = lightcoeff * lights[lg].color;
                //Specular calculation
                vec3 lightDirReflected = reflect(normalize(-Dir), Normal);
                float angle = max(dot(camToFragDirection, lightDirReflected), 0.0);
                rlight += pow(angle, 32) * specularFactor * lights[lg].color;
			    //add light to result color
                result += rlight * factor;
            }
            if(lights[lg].type == LIGHTSOURCE_SPOT){
                
                vec3 vec_dir = normalize(lights[lg].dir);
                vec3 vec_frag_light = (lights[lg].pos - FragPos);
                
                float vangle = dot(vec_dir, normalize(vec_frag_light));
                
                float spot_angle = cos(lights[lg].spot_angle);
                float spot_out_angle = cos(spot_angle + 0.2);
                
                
                float epsilon   = spot_angle - spot_out_angle;
                float intensity = clamp((vangle - spot_out_angle) / epsilon, 0.0, 1.0);
                intensity = intensity;
                result += lights[lg].color * intensity * lights[lg].intensity * (lights[lg].range / length(vec_frag_light));
                
            }
        }
    }
	outColor = vec4(result, 1);
}
#version 420 core

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
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec3 BloomColor;

layout(location = 0) in vec2 UVCoord;

//textures
layout(binding = 0) uniform sampler2D tDiffuse;
layout(binding = 1) uniform sampler2D tNormal;
layout(binding = 2) uniform sampler2D tPos;
layout(binding = 3) uniform sampler2D tSpec;
layout(binding = 4) uniform sampler2D tMasks;

layout (std140, binding = 1) uniform Lights{
    uniform Light lights[MAX_LIGHTS_NUM];
    uniform int lights_amount;
    uniform vec3 ambient_light_color;
};

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    //Camera position
    uniform vec3 cam_position;
};

void main(){

    vec4 Diffuse = texture(tDiffuse, UVCoord);
    vec3 FragPos = texture(tPos, UVCoord).rgb;
	vec3 Normal = texture(tNormal, UVCoord).rgb;
	vec4 Specular = texture(tSpec, UVCoord);
	vec4 Masks = texture(tMasks, UVCoord);   	

    vec3 result = Diffuse.xyz;

    //Check, if fragment isn't skybox
    if(Masks.r == 1){
        //Accept shadows
        result *= (1 - Masks.g);
        result *= ambient_light_color;
        
        bool Shadowed = Masks.g > 0.02; 

        float specularFactor = Specular.r * 255.f; //Get factor in A channel
        vec3 camToFragDirection = normalize(cam_position - FragPos);

        for(int lg = 0; lg < lights_amount; lg ++) {
        
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

	FragColor = vec4(result, 1);
    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BloomColor = FragColor.rgb;
    else
        BloomColor = vec3(0.0, 0.0, 0.0);
}

#version 430 core

#define TEXTURES_AMOUNT 12

layout (location = 1) out vec3 tNormal;
layout (location = 2) out vec3 tPos;
layout (location = 4) out vec4 tMasks;
layout (location = 0) out vec4 FragColor;

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 InNormal;
layout(location = 2) in vec2 UVCoord;
layout(location = 3) in mat3 TBN;
layout(location = 7) in vec3 _id;

//textures
layout(binding = 0) uniform sampler2D diffuse0;
layout(binding = 1) uniform sampler2D diffuse1;
layout(binding = 2) uniform sampler2D diffuse2;
layout(binding = 3) uniform sampler2D diffuse3;
layout(binding = 4) uniform sampler2D diffuse4;
layout(binding = 5) uniform sampler2D diffuse5;
layout(binding = 6) uniform sampler2D diffuse6;
layout(binding = 7) uniform sampler2D diffuse7;
layout(binding = 8) uniform sampler2D diffuse8;
layout(binding = 9) uniform sampler2D diffuse9;
layout(binding = 10) uniform sampler2D diffuse10;
layout(binding = 11) uniform sampler2D diffuse11;

layout(binding = 12) uniform sampler2D normal0;
layout(binding = 13) uniform sampler2D normal1;
layout(binding = 14) uniform sampler2D normal2;
layout(binding = 15) uniform sampler2D normal3;
layout(binding = 16) uniform sampler2D normal4;
layout(binding = 17) uniform sampler2D normal5;
layout(binding = 18) uniform sampler2D normal6;
layout(binding = 19) uniform sampler2D normal7;
layout(binding = 20) uniform sampler2D normal8;
layout(binding = 21) uniform sampler2D normal9;
layout(binding = 22) uniform sampler2D normal10;
layout(binding = 23) uniform sampler2D normal11;

layout(binding = 24) uniform sampler2D texture_mask;
layout(binding = 25) uniform sampler2D texture_mask1;
layout(binding = 26) uniform sampler2D texture_mask2;

//Shadowmapping stuff
layout(binding = 6) uniform sampler2DArray shadow_map;

layout (std140, binding = 3) uniform TerrainData{
//Shadowmapping stuff
    bool hasDiffuse[12];
    bool hasNormal[12];
    uniform int isPicking;
    uniform int terrain_Width;
    uniform int terrain_Height;
};

layout (std140, binding = 0) uniform CamMatrices{
    uniform mat4 cam_projection;
    uniform mat4 cam_view;
    uniform mat4 object_transform;
    uniform vec3 cam_position;
};

layout (std140, binding = 2) uniform ShadowData{
//Shadowmapping stuff
    uniform float ShadowBias; //4
    uniform int ShadowmapSize; //4
    uniform bool HasShadowMap; //4
    uniform int CascadesNum; //4
    uniform int PcfPassNum; // 4
    uniform float ShadowStrength; // 4
    //32
    uniform mat4 LightProjViewMat0; // 16 * 4
    uniform mat4 LightProjViewMat1; // 16 * 4
    uniform mat4 LightProjViewMat2; // 16 * 4
    uniform mat4 LightProjViewMat3; // 16 * 4
    uniform mat4 LightProjViewMat4; // 16 * 4
    uniform mat4 LightProjViewMat5; // 16 * 4
    //416   
    uniform int CasterDistance0; //4
    uniform int CasterDistance1; //4
    uniform int CasterDistance2; //4
    uniform int CasterDistance3; //4
    uniform int CasterDistance4; //4
    uniform int CasterDistance5; //4
};

float getFactor(int id, vec2 uv){
    vec4 mask;
	int maskid = id / 4;
	
	switch(maskid){
        case 0:{
            mask = texture(texture_mask, uv);
            break;
        }
        case 1:{
            mask = texture(texture_mask1, uv);
            break;
        }
        case 2:{
            mask = texture(texture_mask2, uv);
            break;
        }
	}
    
    float factor = 0;
    int factorid = id % 4;
    
    switch(factorid){
        case 0:{
            factor = mask.r;
            break;
        }
        case 1:{
            factor = mask.g;
            break;
        }
        case 2:{
            factor = mask.b;
            break;
        }
        case 3:{
            factor = mask.a;
            break;
        }
    
    }

    return factor;
}

vec3 getDiffuse(int id, vec2 uv){
    switch(id){
        case 0:{
            return texture(diffuse0, uv).xyz;
            break;
        }
        case 1:{
            return texture(diffuse1, uv).xyz;
            break;
        }
        case 2:{
            return texture(diffuse2, uv).xyz;
            break;
        }
        case 3:{
            return texture(diffuse3, uv).xyz;
            break;
        }
        case 4:{
            return texture(diffuse4, uv).xyz;
            break;
        }
        case 5:{
            return texture(diffuse5, uv).xyz;
            break;
        }
        case 6:{
            return texture(diffuse6, uv).xyz;
            break;
        }
        case 7:{
            return texture(diffuse7, uv).xyz;
            break;
        }case 8:{
            return texture(diffuse8, uv).xyz;
            break;
        }case 9:{
            return texture(diffuse9, uv).xyz;
            break;
        }
        case 10:{
            return texture(diffuse10, uv).xyz;
            break;
        }
        case 11:{
            return texture(diffuse11, uv).xyz;
            break;
        }
        
    }
}

vec3 getNormal(int id, vec2 uv){
    switch(id){
        case 0:{
            return texture(normal0, uv).xyz;
            break;
        }
        case 1:{
            return texture(normal1, uv).xyz;
            break;
        }
        case 2:{
            return texture(normal2, uv).xyz;
            break;
        }
        case 3:{
            return texture(normal3, uv).xyz;
            break;
        }
        case 4:{
            return texture(normal4, uv).xyz;
            break;
        }
        case 5:{
            return texture(normal5, uv).xyz;
            break;
        }
        case 6:{
            return texture(normal6, uv).xyz;
            break;
        }
        case 7:{
            return texture(normal7, uv).xyz;
            break;
        }
        case 8:{
            return texture(normal8, uv).xyz;
            break;
        }
        case 9:{
            return texture(normal9, uv).xyz;
            break;
        }
        case 10:{
            return texture(normal10, uv).xyz;
            break;
        }
        case 11:{
            return texture(normal11, uv).xyz;
            break;
        }
        
    }
}

vec3 getFragment(vec2 uv){
    vec3 result = vec3(0,0,0);

    vec2 nuv = uv;
    nuv.x *= terrain_Width / 50;
    nuv.y *= terrain_Height / 50;

    for(int i = 0; i < TEXTURES_AMOUNT; i ++){
        if(hasDiffuse[i] == false) continue;
    
        float factor = getFactor(i, uv);
        vec3 diffuse = getDiffuse(i, nuv);
        
        result = mix(result, diffuse, factor);
    }
        
    return result;
}

vec3 getFragmentNormal(vec2 uv){
    vec3 result = InNormal;

    vec2 nuv = uv;
    nuv.x *= terrain_Width / 50;
    nuv.y *= terrain_Height / 50;

    for(int i = 0; i < TEXTURES_AMOUNT; i ++){
        if(hasNormal[i] == false) continue;
        float factor = getFactor(i, uv);
        vec3 normal = getNormal(i, nuv);
        
        normal = normalize(normal * 2 - 1);
		normal = normalize(TBN * normal);
        
        result = mix(result, normal, factor);
    }
        
    return result;
}

void processShadows(){
    float dist = length(FragPos - cam_position);

    vec4 objPosLightSpace = vec4(0,0,0,0);
    if(dist < CasterDistance1){
        objPosLightSpace = LightProjViewMat0 * vec4(FragPos, 1);
    }else if(dist < CasterDistance2){
        objPosLightSpace = LightProjViewMat1 * vec4(FragPos, 1);
    }else if(dist < CasterDistance3){
        objPosLightSpace = LightProjViewMat2 * vec4(FragPos, 1);
    }else if(dist < CasterDistance4){
        objPosLightSpace = LightProjViewMat3 * vec4(FragPos, 1);
    }else if(dist < CasterDistance5){
        objPosLightSpace = LightProjViewMat4 * vec4(FragPos, 1);
    }else{
        objPosLightSpace = LightProjViewMat5 * vec4(FragPos, 1);
    }

    vec3 ShadowProjection = (objPosLightSpace.xyz / objPosLightSpace.w) / 2.0 + 0.5;
	
    float real_depth = ShadowProjection.z;

    float ShadowFactor = ShadowStrength / (PcfPassNum * PcfPassNum);

    for(int x = 0; x < PcfPassNum; x ++){
        for(int y = 0; y < PcfPassNum; y ++){
            vec2 offset = vec2(x, y);

            offset.x /= ShadowmapSize;
            offset.y /= ShadowmapSize;

            vec2 uvoffset = ShadowProjection.xy + offset;

            vec4 shadowmap = vec4(1,1,1,1);
            if(dist < CasterDistance1){
                shadowmap = texture(shadow_map, vec3(uvoffset, 0));
            }else if(dist < CasterDistance2){
                shadowmap = texture(shadow_map, vec3(uvoffset, 1));
            }else if(dist < CasterDistance3){
                shadowmap = texture(shadow_map, vec3(uvoffset, 2));
            }else if(dist < CasterDistance4){
                shadowmap = texture(shadow_map, vec3(uvoffset, 3));
            }else if(dist < CasterDistance5){
                shadowmap = texture(shadow_map, vec3(uvoffset, 4));
            }else{
                shadowmap = texture(shadow_map, vec3(uvoffset, 5));
            }
           
            float texture_depth = shadowmap.r;
            tMasks.g += (real_depth - ShadowBias > texture_depth) ? ShadowFactor : 0.0;
        }
    }
        
    if(real_depth > 1.0) tMasks.g = 0.0;
}

void main(){

	vec2 uv = UVCoord;
	
	vec3 result = vec3(1.0, 1.0, 1.0); //Default value
	
	if(isPicking == 1){
        float id_r = _id.r / terrain_Width;
        float id_b = _id.b / terrain_Height;

		FragColor = vec4(id_r, 0, id_b, 1);
	}
    if(isPicking == 0){
	   
	    tPos = FragPos;
	    tNormal = getFragmentNormal(uv); //defaultly, use normals from mesh
	    tMasks = vec4(1.0, 0, 0, 0);
        
        processShadows();
		FragColor = vec4(getFragment(uv), 0);
	}	
}

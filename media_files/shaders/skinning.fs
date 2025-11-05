#version 330 core

in vec2 TexCoord0;
in vec3 Normal0;
// in vec3 LocalPos0;
// flat in ivec4 BoneIDs0;
// in vec4 Weights0;

out vec4 FragColor;

uniform sampler2D texture_diffuse1;

uniform int gDisplayBoneIndex;

void main() {
	vec3 normal = normalize(Normal0);
	bool found = false;

    // for (int i = 0 ; i < 4 ; i++) {
    //     if (BoneIDs0[i] == gDisplayBoneIndex) {
    //        if (Weights0[i] >= 0.7) {
    //            FragColor = vec4(1.0, 0.0, 0.0, 0.0) * Weights0[i];
    //        } else if (Weights0[i] >= 0.4 && Weights0[i] <= 0.6) {
    //            FragColor = vec4(0.0, 1.0, 0.0, 0.0) * Weights0[i];
    //        } else if (Weights0[i] >= 0.1) {
    //            FragColor = vec4(1.0, 1.0, 0.0, 0.0) * Weights0[i];
    //        }

    //        found = true;
    //        break;
    //     }
    // }

	// if(!found) FragColor = vec4(1.0, 1.0, 1.0, 1.0) * dot(normal, vec3(-1, -1, 0)) * 0.7;
	// if(!found) FragColor =  dot(normal, vec3(-1, -1, 0)) * .55 + vec4(0.0, 0.0, 1.0, 1.0);
	// if(!found) FragColor = vec4(0.0, 0.0, 1.0, 1.0) + dot(normal, vec3(-1, -1, 0)) * .55;
    // FragColor = vec4(0.0, 0.0, .8, 1.0) + dot(normal, vec3(-1, -1, 0)) * .55; // blue

    // float light = dot(normal, vec3(-1, -1, 0)) * 1.9;
    // FragColor = texture(texture_diffuse1, TexCoord0.xy) * light * vec4(0.65, 0.6, 0.01, 1.0);

    // FragColor = texture2D(texture_diffuse1, TexCoord0.xy) * dot(normal, vec3(-1, -1, 0)) * 0.9;
    // FragColor = texture(texture_diffuse1, TexCoord0.xy) * 1.6 * vec4(0.65, 0.6, 0.01, 1.0); // flat light
    // FragColor = texture(texture_diffuse1, TexCoord0) * dot(normal, vec3(-1, -1, 0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0);
    // FragColor = dot(normal, vec3(-1, -1, 0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0);

    // FragColor = dot(normal, vec3(-1, -1, 1)) * 0.7 * vec4(1, 1, 1, 1.0);
    // FragColor = texture(texture_diffuse1, TexCoord0) * dot(normal, vec3(-1, -1, .0)) * 0.7 * vec4(1, 1, 1, 1.0);
    FragColor = texture(texture_diffuse1, TexCoord0) * dot(normal, vec3(-1, -1, .0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0);
}
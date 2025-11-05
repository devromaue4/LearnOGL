#version 430 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tex;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in ivec4 boneIDs;
layout (location = 6) in vec4 weights;

uniform mat4 mProj;
uniform mat4 mView;
uniform mat4 mModel;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesTransform[MAX_BONES];

out vec2 TexCoord;
out vec3 Normal;

void main() {
	vec4 totalPosition = vec4(0.0f);
	vec3 normalOut = vec3(1.0);
	for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
		if(boneIDs[i] == -1) continue;
		if(boneIDs[i] >= MAX_BONES) {
			totalPosition = vec4(pos, 1.0f);
			break;
		}
		vec4 localPosition = finalBonesTransform[boneIDs[i]] * vec4(pos, 1.0f);
		totalPosition += localPosition * weights[i];
		normalOut = mat3(finalBonesTransform[boneIDs[i]]) * norm;
		// normalOut += mat3(finalBonesTransform[boneIDs[i]]) * norm;
	}

	// mat4 viewModel = mView * mModel;
	// gl_Position = mProj * viewModel * totalPosition;
	gl_Position = mProj * mView * mModel * totalPosition;
	TexCoord = tex;
	Normal = normalOut;
}
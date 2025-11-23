#version 430 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
// layout (location = 3) in vec3 tangent;
// layout (location = 4) in vec3 bitangent;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

out vec3 NormalO;
out vec2 TexCoordO;

uniform mat4 gBones[200];
uniform mat4 mPVM;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProj;

void main() {
	mat4 boneTransform = gBones[BoneIDs[0]] * Weights[0];
	boneTransform += gBones[BoneIDs[1]] * Weights[1];
	boneTransform += gBones[BoneIDs[2]] * Weights[2];
	boneTransform += gBones[BoneIDs[3]] * Weights[3];
	vec4 PosL = boneTransform * vec4(Pos, 1.0);

	vec3 normalOut = normalize(mat3(boneTransform) * Normal);

	// vec4 totalPosition = vec4(0.0f);
	// vec3 normalOut = vec3(1.0);
	// for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
	// 	if(boneIDs[i] == -1) continue;
	// 	if(boneIDs[i] >= MAX_BONES) {
	// 		totalPosition = vec4(pos, 1.0f);
	// 		break;
	// 	}
	// 	vec4 localPosition = finalBonesTransform[boneIDs[i]] * vec4(pos, 1.0f);
	// 	totalPosition += localPosition * weights[i];
	// 	normalOut = mat3(finalBonesTransform[boneIDs[i]]) * norm;
	// 	// normalOut += mat3(finalBonesTransform[boneIDs[i]]) * norm;
	// }

	// gl_Position = mPVM * PosL;
	gl_Position = mProj * mView * mModel * PosL;
	// gl_Position = mProj * mView * mModel * vec4(Pos, 1.0);
	// gl_Position = mPVM * vec4(Pos, 1.0);
	// gl_Position = vec4(Pos, 1.0);
	// NormalO = Normal;
	NormalO = normalOut;
	TexCoordO = TexCoord;
}
#version 430 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoords;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

out vec3 NormalO;
out vec2 TexCoordsO;

// uniform mat4 mPVM;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProj;

const int MAX_BONES = 100;
uniform mat4 mBonesMatrices[MAX_BONES];

void main() {
	mat4 BoneTransform = mBonesMatrices[BoneIDs[0]] * Weights[0];
	BoneTransform += mBonesMatrices[BoneIDs[1]] * Weights[1];
	BoneTransform += mBonesMatrices[BoneIDs[2]] * Weights[2];
	BoneTransform += mBonesMatrices[BoneIDs[3]] * Weights[3];
	vec4 totalPosition = BoneTransform * vec4(Pos, 1.0f);

	// vec3 normalOut = mat3(mBonesMatrices[BoneIDs[0]]) * Normal;
	// normalOut += mat3(mBonesMatrices[BoneIDs[1]]) * Normal;
	// normalOut += mat3(mBonesMatrices[BoneIDs[2]]) * Normal;
	// normalOut += mat3(mBonesMatrices[BoneIDs[3]]) * Normal;
	
	// vec4 totalPosition = vec4(0.0f);
	// for(int i = 0; i < 4; i++) {
	// 	// if(BoneIDs[i] == -1) continue;
	// 	if(Weights[i] == 0.0f) continue;
	// 	if(BoneIDs[i] >= MAX_BONES) {
	// 		totalPosition = vec4(Pos, 1.0f);
	// 		break;
	// 	}
	// 	vec4 localPosition = mBonesMatrices[BoneIDs[i]] * vec4(Pos, 1.0f);
	// 	totalPosition += localPosition * Weights[i];
	// }

	gl_Position = mProj * mView * mModel * totalPosition;
	// NormalO = normalOut;
	NormalO = Normal;
	TexCoordsO = TexCoords;
}
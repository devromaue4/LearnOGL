#version 430 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

out vec3 NormalO;
out vec2 TexCoordO;

uniform mat4 gBones[200];
uniform mat4 mPVM;

void main() {
	mat4 boneTransform = gBones[BoneIDs[0]] * Weights[0];
	boneTransform += gBones[BoneIDs[1]] * Weights[1];
	boneTransform += gBones[BoneIDs[2]] * Weights[2];
	boneTransform += gBones[BoneIDs[3]] * Weights[3];
	vec4 PosL = boneTransform * vec4(Pos, 1.0);

	vec3 normalOut = normalize(mat3(boneTransform) * Normal);

	gl_Position = mPVM * PosL;
	// gl_Position = mPVM * vec4(Pos, 1.0);
	// gl_Position = vec4(Pos, 1.0);
	// NormalO = Normal;
	NormalO = normalOut;
	TexCoordO = TexCoord;
}
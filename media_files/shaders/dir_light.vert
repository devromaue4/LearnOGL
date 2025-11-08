#version 430 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec3 NormalO;
out vec2 TexCoordO;

uniform mat4 mPVM;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProj;

void main() {
	vec3 normalOut = normalize(Normal);
	gl_Position = mProj * mView * mModel * vec4(Pos, 1.0);
	// gl_Position = mProj * mView * mWorld * mModel * vec4(Pos, 1.0);

	NormalO = normalOut;
	TexCoordO = TexCoord;
}
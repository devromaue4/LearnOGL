#version 460 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec3 NormalO;
out vec2 TexCoordO;
out vec3 crntPos;

// uniform mat4 mPVM;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProj;

void main() {

	crntPos = vec3(mModel * vec4(Pos, 1.0));
	gl_Position = mProj * mView * vec4(crntPos, 1.0);

	NormalO = normalize(Normal);
	TexCoordO = TexCoord;

}
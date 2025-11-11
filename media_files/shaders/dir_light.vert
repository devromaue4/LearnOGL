#version 460 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec3 NormalO;
out vec2 TexCoordO;
out vec4 ColorO;

// uniform mat4 mPVM;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProj;

void main() {
	vec3 normalOut = normalize(Normal);
	gl_Position = mProj * mView * mModel * vec4(Pos, 1.0);
	// // gl_Position = mProj * mView * mWorld * mModel * vec4(Pos, 1.0);

	// NormalO = normalOut;
	TexCoordO = TexCoord;

	ColorO = dot(normalOut, vec3(-1, -1, .0)) * 1.7 * vec4(0.65, 0.6, 0.01, 1.0);
}
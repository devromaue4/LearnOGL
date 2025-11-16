#version 460 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec3 NormalO;
out vec2 TexCoordO;
out vec3 modelPos;

// uniform mat4 mPVM;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProj;

void main() {
	gl_Position = mProj * mView * mModel * vec4(Pos, 1.0);
	modelPos = vec3(mModel * vec4(Pos, 1.0));
	// NormalO = normalize(vec3(mModel * vec4(Normal, 1.0f)));
	NormalO = normalize(transpose(inverse(mat3(mModel))) * Normal);
	//NormalO = mat3(mModel) * Normal;
	TexCoordO = TexCoord;
}

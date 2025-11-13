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
	// vec3 NormalL = vec3(mModel * vec4(Normal, 0.0f));
	vec3 normalOut = normalize(mat3(mModel) * Normal);
	// vec3 NormalL = vec3(mat3(mModel) * Normal);
	// vec3 NormalL = mat3(transpose(inverse(mModel))) * Normal;
	// vec3 NormalL = mat3(transpose(inverse(mModel))) * Normal;

	// gl_Position = mProj * mView * vec4(crntPos, 1.0);
	gl_Position = mProj * mView * mModel * vec4(Pos, 1.0);

	NormalO = normalOut;
	TexCoordO = TexCoord;

}
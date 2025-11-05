#version 330 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
// layout (location = 2) in vec2 aTexCoord;
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform float gScale;
// out vec3 ourColor; // specify a color output to the fragment shader
// out vec3 ourPos;
out vec2 vTexCoord;
uniform mat4 mModel;
uniform mat4 mView;
uniform mat4 mProj;
// uniform mat4 mPVM;

void main() {
	gl_Position = mProj * mView * mModel * vec4(aPos, 1.0);
	// gl_Position = mPVM * vec4(aPos, 1.0);
	// gl_Position = vec4(aPos, 1.0) * mTransform;
	// gl_Position = vec4(aPos, 1.0);
	// gl_Position = vec4(gScale * aPos.x, gScale * aPos.y, aPos.z, 1.0); // multiply (scale)
	// gl_Position = vec4(aPos.x + aPos.x * gScale, aPos.y + aPos.y * gScale, aPos.z + aPos.z * gScale, 1.0); // add (move)
	// ourColor = aColor; // set ourColor to the input color we got from the vertex data
	// ourPos = aPos;
	vTexCoord = aTexCoord;
}
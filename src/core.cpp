#include "core.h"

void error_callback(int, const char* err_str) { throw glfw_error(err_str); }

glsl_compile_error::glsl_compile_error(const char* s, GLuint shaderID) : glsl_error(s) {
	GLint length;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
	std::vector<char> shaderLogV(length);
	glGetShaderInfoLog(shaderID, length, nullptr, shaderLogV.data());
	shaderLog.assign(shaderLogV.begin(), shaderLogV.end());
	std::cout << shaderLog << std::endl;
}
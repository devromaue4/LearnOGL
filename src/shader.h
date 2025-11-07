#pragma once
#include "core.h"

//#include <unordered_map>
//#include <string>
//#include <vector>

class Shader {
	std::unordered_map<std::string, int> m_uniformLocations;
public:
	GLuint m_ID;

	Shader();
	~Shader() { glDeleteProgram(m_ID); }
	Shader(const char* vertFile, const char* fragFile);

	void Use();
	void Delete();

	void setBool(const std::string& name, bool value);
	void setInt(const std::string& name, int value);
	void setFloat(const std::string& name, float value);
	void setVec4(const std::string& name, const glm::vec4& value);
	void setMat4(const std::string& name, const glm::mat4& value);
	//void setMat4(const std::string& name, const my::mat4& value);
	//void setMat4(const std::string& name, const glm::mat4* value) const;

private:
	void AddShader(GLuint shaderProgram, const char* shaderText, GLenum shaderType = GL_VERTEX_SHADER);
	void CheckLinkShader(GLuint shaderProg);
};


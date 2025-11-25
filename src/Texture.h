#pragma once
#include "core.h"

class Texture {

	GLuint m_ID;
	GLenum m_TexType;
	GLenum m_TexUnit;

	GLenum m_pixelType;
	GLenum m_format;

	std::string m_fileName;

public:
	Texture();
	Texture(const char* imgName, GLenum slot = GL_TEXTURE0, GLenum format = GL_RGB, GLenum pixelType = GL_UNSIGNED_BYTE, GLenum texType = GL_TEXTURE_2D);
	~Texture() { Delete(); }

	GLuint Load();
	GLuint Load(uint bufferSize, void* pData);

	GLuint LoadFromMemory(void* pData, uint bufferSize);

	void setTexUnit(class Shader& shader, const char* uniform, GLuint unit);
	void Bind();
	void Bind(GLenum texUnit);
	void Unbind()	{ glBindTexture(GL_TEXTURE_2D, 0); }
	void Delete()	{ glDeleteTextures(1, &m_ID); }
};

extern std::map<std::string, GLuint> gGlobalTexStorage;
//static std::map<std::string, int> gGlobalTexStorage;
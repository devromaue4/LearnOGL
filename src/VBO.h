#pragma once
#include "core.h"

class VBO {
public:
	GLuint m_ID;

	VBO(GLfloat* vertices, GLsizeiptr size);
	~VBO() { glDeleteBuffers(1, &m_ID); }

	void Bind()		{ glBindBuffer(GL_ARRAY_BUFFER, m_ID); }
	void Unbind()	{ glBindBuffer(GL_ARRAY_BUFFER, 0); }
	void Delete()	{ glDeleteBuffers(1, &m_ID); }
};
#pragma once
#include "core.h"

class EBO {
public:
	GLuint m_ID;

	EBO(GLuint* vertices, GLsizeiptr size);
	~EBO() { glDeleteBuffers(1, &m_ID); }

	void Bind()		{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID); }
	void Unbind()	{ glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
	void Delete()	{ glDeleteBuffers(1, &m_ID); }
};

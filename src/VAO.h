#pragma once
#include "core.h"
#include "VBO.h"

class VAO {
public:
	GLuint m_ID;

	VAO() { glGenVertexArrays(1, &m_ID); }
	~VAO() { glDeleteVertexArrays(1, &m_ID); }

	void linkAttrib(VBO& vbo, GLuint layout, GLint numComp, GLenum type, GLsizei stride, const void* offset);

	void Bind()		{ glBindVertexArray(m_ID); }
	void Unbind()	{ glBindVertexArray(0); }
	void Delete()	{ glDeleteVertexArrays(1, &m_ID); }
};

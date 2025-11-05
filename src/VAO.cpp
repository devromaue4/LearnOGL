#include "VAO.h"

void VAO::linkAttrib(VBO& vbo, GLuint layout, GLint numComp, GLenum type, GLsizei stride, const void* offset) {
	vbo.Bind();
	glVertexAttribPointer(layout, numComp, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	vbo.Unbind();
}
#include "stdafx.h"
#include "Shader.h"

GLuint getShader(const char *filepath, int shaderType) {
	GLuint vertexShaderObj = glCreateShader(shaderType);

	ifstream fin(filepath);
	char st[4096];
	char c;
	int n = 0;

	while (1){
		fin.get(c);
		if (fin.eof()) break;
		st[n] = c; n++;
	}
	st[n] = '\0';
	fin.close();

	const GLchar* p[1];
	p[0] = st;
	GLint Lengths[1];
	Lengths[0] = strlen(st);
	glShaderSource(vertexShaderObj, 1, p, Lengths);
	glCompileShader(vertexShaderObj);

	GLint success;
	glGetShaderiv(vertexShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(vertexShaderObj, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", vertexShaderObj, InfoLog);
		return 0;
	}

	return vertexShaderObj;
}
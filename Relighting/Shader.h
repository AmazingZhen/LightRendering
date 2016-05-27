#ifndef _SHADER_H_
#define _SHADER_H_

#include <string>
#include <iostream>
#include <fstream>
#include <GL\glew.h>
#include <GL\glut.h>

using namespace std;

GLuint getShader(const char *filepath, int shaderType);

#endif // _SHADER_H_
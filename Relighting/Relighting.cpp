// Relighting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>

#include <GL\glew.h>
#include <GL\glut.h>
#include <GL\SOIL.h>

#include <OpenMesh\Core\IO\MeshIO.hh>
#include <OpenMesh\Core\Mesh\TriMesh_ArrayKernelT.hh>

#include "FileReading.h"
#include "Shader.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define FILE_FOLDER "dataset\\"

#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "SOIL.lib")
#pragma comment (lib, "OpenMeshCored.lib")
#pragma comment (lib, "OpenMeshToolsd.lib")

GLuint skyboxTexture;
GLuint skyboxVAO, skyboxVBO;
GLuint skyboxShaderProgram;

vector<GLuint> modelVAO;
vector<GLuint> modelVBO;
vector<GLint> numOfVertex;
GLuint modelShaderProgram;

GLuint cubeVAO, cubeVBO;

GLfloat skyboxVertices[] = {
	// Positions          
	-2.0f, 2.0f, -2.0f,
	-2.0f, -2.0f, -2.0f,
	2.0f, -2.0f, -2.0f,
	2.0f, -2.0f, -2.0f,
	2.0f, 2.0f, -2.0f,
	-2.0f, 2.0f, -2.0f,

	-2.0f, -2.0f, 2.0f,
	-2.0f, -2.0f, -2.0f,
	-2.0f, 2.0f, -2.0f,
	-2.0f, 2.0f, -2.0f,
	-2.0f, 2.0f, 2.0f,
	-2.0f, -2.0f, 2.0f,

	2.0f, -2.0f, -2.0f,
	2.0f, -2.0f, 2.0f,
	2.0f, 2.0f, 2.0f,
	2.0f, 2.0f, 2.0f,
	2.0f, 2.0f, -2.0f,
	2.0f, -2.0f, -2.0f,

	-2.0f, -2.0f, 2.0f,
	-2.0f, 2.0f, 2.0f,
	2.0f, 2.0f, 2.0f,
	2.0f, 2.0f, 2.0f,
	2.0f, -2.0f, 2.0f,
	-2.0f, -2.0f, 2.0f,

	-2.0f, 2.0f, -2.0f,
	2.0f, 2.0f, -2.0f,
	2.0f, 2.0f, 2.0f,
	2.0f, 2.0f, 2.0f,
	-2.0f, 2.0f, 2.0f,
	-2.0f, 2.0f, -2.0f,

	-2.0f, -2.0f, -2.0f,
	-2.0f, -2.0f, 2.0f,
	2.0f, -2.0f, -2.0f,
	2.0f, -2.0f, -2.0f,
	-2.0f, -2.0f, 2.0f,
	2.0f, -2.0f, 2.0f
};

// A simple cube for testing.
GLfloat cubeVertices[] = {
	// Positions          // Normals
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
};

typedef OpenMesh::TriMesh_ArrayKernelT<> MyMesh;

vector<GLfloat> xRotate;
vector<GLfloat> yRotate;
vector<GLfloat> oldXPos, oldYPos;

vector<GLfloat> xOffset;
vector<GLfloat> yOffset;

vector<GLfloat> scale;

int chosenIndex = 0;
int mode = 1;

void initSkybox() {
	// Setup skybox VAO
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	// Create a cubemap texture.
	glGenTextures(1, &skyboxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

	// Loading six faces.
	vector<const GLchar*> faces;
	faces.push_back("skybox\\right.jpg");
	faces.push_back("skybox\\left.jpg");
	faces.push_back("skybox\\top.jpg");
	faces.push_back("skybox\\bottom.jpg");
	faces.push_back("skybox\\back.jpg");
	faces.push_back("skybox\\front.jpg");

	int width, height;
	unsigned char* image;
	for (GLuint i = 0; i < faces.size(); i++) {
		image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
			);

		SOIL_free_image_data(image);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void initSkyboxShaderProgram() {
	skyboxShaderProgram = glCreateProgram();

	GLuint vertexShaderObj = getShader("skyboxVertexShader.vert", GL_VERTEX_SHADER);
	glAttachShader(skyboxShaderProgram, vertexShaderObj);
	GLuint fragmentShaderObj = getShader("skyboxFragmentShader.frag", GL_FRAGMENT_SHADER);
	glAttachShader(skyboxShaderProgram, fragmentShaderObj);

	glLinkProgram(skyboxShaderProgram);
	GLint success;
	glGetProgramiv(skyboxShaderProgram, GL_LINK_STATUS, &success);
	if (success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(skyboxShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}

	glValidateProgram(skyboxShaderProgram);
}

void initModelShaderProgram() {
	modelShaderProgram = glCreateProgram();

	GLuint vertexShaderObj = getShader("modelVertexShader.vert", GL_VERTEX_SHADER);
	glAttachShader(modelShaderProgram, vertexShaderObj);
	GLuint fragmentShaderObj = getShader("modelFragmentShader.frag", GL_FRAGMENT_SHADER);
	glAttachShader(modelShaderProgram, fragmentShaderObj);

	glLinkProgram(modelShaderProgram);
	GLint success;
	glGetProgramiv(modelShaderProgram, GL_LINK_STATUS, &success);
	if (success == 0) {
		GLchar ErrorLog[1024];
		glGetProgramInfoLog(modelShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
	}

	glValidateProgram(modelShaderProgram);
}

void initMesh() {
	string file_folder(FILE_FOLDER);
	vector<string> mesh_files;

	getAllFiles(file_folder, mesh_files);
	vector<MyMesh> mesh = vector<MyMesh>(mesh_files.size());

	xRotate = vector<GLfloat>(mesh_files.size(), 0.0f);
	yRotate = vector<GLfloat>(mesh_files.size(), 0.0f);
	oldXPos = vector<GLfloat>(mesh_files.size());
	oldYPos = vector<GLfloat>(mesh_files.size());;

	xOffset = vector<GLfloat>(mesh_files.size(), 0.0f);
	yOffset = vector<GLfloat>(mesh_files.size(), 0.0f);
	scale = vector<GLfloat>(mesh_files.size(), 1.0f);

	for (int i = 0; i < mesh_files.size(); i++) {
		// request vertex normals, so the mesh reader can use normal information  
		// if available  
		mesh[i].request_vertex_normals();
		// assure we have vertex normals  
		if (!mesh[i].has_vertex_normals())
		{
			std::cerr << "ERROR: Standard vertex property 'Normals' not available!\n";
			return;
		}

		// Read mesh 
		OpenMesh::IO::Options opt;
		if (!OpenMesh::IO::read_mesh(mesh[i], mesh_files[i], opt))
		{
			cout << "Error: Cannot read mesh from " << mesh_files[i] << endl;
			return;
		}

		// If the file did not provide vertex normals, then calculate them  
		if (!opt.check(OpenMesh::IO::Options::VertexNormal))
		{
			// we need face normals to update the vertex normals  
			mesh[i].request_face_normals();

			// let the mesh update the normals  
			mesh[i].update_normals();

			// dispose the face normals, as we don't need them anymore  
			mesh[i].release_face_normals();
		}
	}

	vector<vector<GLfloat> > vertex_and_normal(mesh.size());
	modelVAO = vector<GLuint>(mesh.size());
	modelVBO = vector<GLuint>(mesh.size());
	numOfVertex = vector<GLint>(mesh.size());

	for (int i = 0; i < mesh.size(); i++) {
		for (MyMesh::FaceIter f_it = mesh[i].faces_begin(); f_it != mesh[i].faces_end(); ++f_it) {
			for (MyMesh::FaceVertexIter fv_it = mesh[i].fv_iter(*f_it); fv_it.is_valid(); ++fv_it) {
				const GLfloat *t = mesh[i].point(*fv_it).data();
				vertex_and_normal[i].push_back(t[0]);
				vertex_and_normal[i].push_back(t[1]);
				vertex_and_normal[i].push_back(t[2]);

				t = mesh[i].normal(*fv_it).data();
				vertex_and_normal[i].push_back(t[0]);
				vertex_and_normal[i].push_back(t[1]);
				vertex_and_normal[i].push_back(t[2]);
			}
		}

		int length = vertex_and_normal[i].size() * sizeof(GLfloat);
		numOfVertex[i] = vertex_and_normal[i].size();

		glGenVertexArrays(1, &(modelVAO[i]));
		glGenBuffers(1, &(modelVBO[i]));
		glBindVertexArray(modelVAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER, modelVBO[i]);
		glBufferData(GL_ARRAY_BUFFER, length, vertex_and_normal[i].data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);

	}

}

void initCube() {
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

void initGL() {
	// Setup some OpenGL options

	initSkybox();
	initSkyboxShaderProgram();
	initModelShaderProgram();
	initMesh();
	initCube();
}

void drawSkybox() {
	glDepthFunc(GL_LEQUAL);
	glUseProgram(skyboxShaderProgram);

	GLfloat view[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, view);
	GLfloat projection[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection);

	glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "view"), 1, GL_FALSE, view);
	glUniformMatrix4fv(glGetUniformLocation(skyboxShaderProgram, "projection"), 1, GL_FALSE, projection);

	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glUseProgram(0);
	glDepthFunc(GL_LESS);
}

void drawModel() {
	glShadeModel(GL_SMOOTH);

	glUseProgram(modelShaderProgram);

	GLfloat projection[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection);
	GLfloat model_view[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, model_view);

	glUniformMatrix4fv(glGetUniformLocation(modelShaderProgram, "model_view"), 1, GL_FALSE, model_view);
	glUniformMatrix4fv(glGetUniformLocation(modelShaderProgram, "projection"), 1, GL_FALSE, projection);
	glUniform1i(glGetUniformLocation(modelShaderProgram, "mode"), mode);

	glBindVertexArray(modelVAO[chosenIndex]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, numOfVertex[chosenIndex]);
	glBindVertexArray(0);

	glUseProgram(0);
}

void drawCube() {
	glUseProgram(modelShaderProgram);

	GLfloat projection[16];
	glGetFloatv(GL_PROJECTION_MATRIX, projection);
	GLfloat model_view[16];
	glGetFloatv(GL_MODELVIEW_MATRIX, model_view);

	glUniformMatrix4fv(glGetUniformLocation(modelShaderProgram, "model_view"), 1, GL_FALSE, model_view);
	glUniformMatrix4fv(glGetUniformLocation(modelShaderProgram, "projection"), 1, GL_FALSE, projection);

	glBindVertexArray(cubeVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glUseProgram(0);
}

void myDisplay() {
	// Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// Projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLfloat)WINDOW_WIDTH / WINDOW_HEIGHT, 1.0, 10.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, -2, 0, 0, 0, 0, 1, 0);

	// View matrix
	glTranslatef(xOffset[chosenIndex], yOffset[chosenIndex], 0);
	glRotatef(xRotate[chosenIndex], 1.0f, 0.0f, 0.0f);
	glRotatef(yRotate[chosenIndex], 0.0f, 1.0f, 0.0f);

	// Model matrix
	glScalef(scale[chosenIndex], scale[chosenIndex], scale[chosenIndex]);

	drawModel();

	// Test cube
	// drawCube();

	drawSkybox();

	glutSwapBuffers();
}

void mySpecial(int key, int x, int y) {
	switch (key) {
	case GLUT_KEY_RIGHT:
		chosenIndex++;

		if (chosenIndex == modelVAO.size()) {
			chosenIndex = 0;
		}
		break;
	case GLUT_KEY_LEFT:
		chosenIndex--;

		if (chosenIndex == -1) {
			chosenIndex = modelVAO.size() - 1;
		}
		break;
	case GLUT_KEY_DOWN:
		scale[chosenIndex] *= 0.9;
		break;
	case GLUT_KEY_UP:
		scale[chosenIndex] /= 0.9;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void myKeyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'W':
		yOffset[chosenIndex] += 0.1f;
		break;
	case 'S':
		yOffset[chosenIndex] -= 0.1f;
		break;
	case 'A':
		xOffset[chosenIndex] -= 0.1f;
		break;
	case 'D':
		xOffset[chosenIndex] += 0.1f;
		break;
	case '1':
		mode = 1;
		break;
	case '2':
		mode = 2;
		break;
	default:
		break;
	}
	cout << yOffset[chosenIndex] << endl;
	glutPostRedisplay();
}

void myMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		oldXPos[chosenIndex] = x;
		oldYPos[chosenIndex] = y;
	}
}

void myMotion(int x, int y) {
	GLint deltax = oldXPos[chosenIndex] - x;
	GLint deltay = oldYPos[chosenIndex] - y;
	
	yRotate[chosenIndex] -= 360 * (GLfloat)deltax / (GLfloat)WINDOW_WIDTH;
	xRotate[chosenIndex] -= 360 * (GLfloat)deltay / (GLfloat)WINDOW_HEIGHT;
	oldXPos[chosenIndex] = x;
	oldYPos[chosenIndex] = y;
	
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("All-frequency Relighting");
	glewInit();
	initGL();

	glutKeyboardFunc(&myKeyboard);
	glutSpecialFunc(&mySpecial);
	glutDisplayFunc(&myDisplay);
	glutMouseFunc(&myMouse);
	glutMotionFunc(&myMotion);

	glutMainLoop();

	return 0;
}


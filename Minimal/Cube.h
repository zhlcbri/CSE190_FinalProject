#pragma once

#ifndef Cube_h
#define Cube_h

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string> //

#include <Windows.h> //

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"
#include "Shader.h"

using namespace std;
using namespace glm;

float deg = 0.5f;
float angle_r = 0.0f;

class Cube {
private:
	char * myID = nullptr; // X, Y, A, B
	mat4 T = mat4(1.0f); // translation 
	mat4 R = mat4(1.0f); // rotation 
	mat4 S = mat4(1.0f); // scale
	mat4 toWorld = mat4(1.0f); // assign to T * R * S every frame

public:
	bool cullFront = false;
	unsigned int myTexture;

	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;

	Cube(vector<string> faces, bool cullFrontFace, char * ID = nullptr, mat4 model = mat4(1.0f))
	{
		cullFront = cullFrontFace;

		if (!cullFront && ID != nullptr) {
			myID = ID;
		}

		myTexture = loadCubemap(faces);
		toWorld = model;

		// cube VAO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Enable the usage of layout location 0 (check the vertex shader to see what this is)
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		// Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// NEVER unbind the element array buffer associated with a VAO
		glBindVertexArray(0);

	};

	~Cube()
	{
		// cube VAO
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	};

	char* getID() {
		return myID;
	}

	void translate(mat4 translationMatrix) {
		T = translationMatrix;
	}

	void rotate(mat4 rotationMatrix) {
		R = rotationMatrix;
	}

	void scale(mat4 scaleMatrix) {
		S = scaleMatrix;
	}

	void spin() {
		R = glm::rotate(mat4(1.0f), angle_r / 180.0f*pi<float>(), vec3(0.0, 1.0, 0.0));
	}

	void update() {
		spin();
		toWorld = T * R * S;
	}

	unsigned int loadCubemap(vector<string> faces)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		int width, height, nrChannels;
		for (unsigned int i = 0; i < faces.size(); i++)
		{
			unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
			if (data)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
				);
			}
			else
			{
				cout << "Cubemap texture failed to load at path: " << faces[i] << endl;
			}
			stbi_image_free(data);

		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // seamless edge

		return textureID;
	};

	//=============== draw using toWorld every frame ================
	void draw(Shader shader, const glm::mat4 & projection, const glm::mat4 & view)
	{
		update(); // get toWorld for this frame

		glEnable(GL_CULL_FACE);

		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", toWorld);

		glBindVertexArray(VAO);

		if (cullFront) {
			glCullFace(GL_FRONT); // If drawing skybox cull front face
		}
		else {
			glCullFace(GL_BACK); // otherwise cull back face
		}

		shader.setInt("myTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, myTexture);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);

		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Draw triangles
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS); // set depth function back to default
		glCullFace(GL_BACK); // set default cull backface 
	};

	//=============== draw using designated model matrix ================
	void draw(Shader shader, const glm::mat4 & projection, const glm::mat4 & view, const glm::mat4 & model)
	{
		glEnable(GL_CULL_FACE);

		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		glBindVertexArray(VAO);

		if (cullFront) {
			glCullFace(GL_FRONT); // If drawing skybox cull front face
		}
		else {
			glCullFace(GL_BACK); // otherwise cull back face
		}		

		shader.setInt("myTex", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, myTexture);

		// Enable depth test
		glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		glDepthFunc(GL_LESS);

		// Draw triangles
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glDepthFunc(GL_LESS); // set depth function back to default
		glCullFace(GL_BACK); // set default cull backface 
	};
	
	const GLfloat vertices[8][3] = {
		//"Front" vertices
		{ -1.0, -1.0,  1.0 },{ 1.0, -1.0,  1.0 },{ 1.0,  1.0,  1.0 },{ -1.0,  1.0,  1.0 },

		//"Back" vertices
	    { -1.0, -1.0, -1.0 },{ 1.0, -1.0, -1.0 },{ 1.0,  1.0, -1.0 },{ -1.0,  1.0, -1.0 }
	};

	const GLuint indices[6][6] = {
		// Front face
		{ 0, 1, 2, 2, 3, 0 },
		// Top face
	    { 1, 5, 6, 6, 2, 1 },
	    // Back face
	    { 7, 6, 5, 5, 4, 7 },
	    // Bottom face
	    { 4, 0, 3, 3, 7, 4 },
	    // Left face
	    { 4, 5, 1, 1, 0, 4 },
	    // Right face
	    { 3, 2, 6, 6, 7, 3 }
	};


};

#endif
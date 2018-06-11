#pragma once

#ifndef Score_h
#define Score_h

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>

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
#include "Model.h"

using namespace std;
using namespace glm;

class Score {
private:
	int num = 0;
	string text = "";
	vector<Model*> text_model;

	//unsigned int myTexture;
	GLuint VBO, VAO, EBO;

public:

	Score() {

		// once have .obj models, initialize VAO, VBO and EBO
		// no need for textures; use lighted shaders instead
	}

	~Score() {
		// delete every Model* in vector
	}

	int getNum() {
		return num;
	}

	void increment(int val) {
		num += val;
	}

	// first let's try printing to console
	void update() {
		// update score and text as string form
		// clear model vector and push_back corresponding text objects
		
	}

	void draw(Shader shader, const mat4 & projection, const mat4 & view, const mat4 & model) {
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		// send lighting information to shader
		vec3 lightPos = vec3(0.0f, 1.0f, 0.0f);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", vec3(view[3].x, view[3].y, view[3].z));

		// draw each model in vector
		for (Model* model : text_model) {
			model->Draw(shader);
		}
	}
};

#endif
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

bool hit = false;

class Score {
private:
	int num = 0;
	vector<Model*> text_model; // use vector for easier insertion and deletion
	Model* zero;
	Model* one;
	Model* two;
	Model* three;
	Model* four;
	Model* five;
	Model* six;
	Model* seven;
	Model* eight;
	Model* nine;

	// add more paths once have obj files ready
	string const PATH_TWO = "Models/num_two/two.obj";
	string const PATH_THREE = "Models/num_three/three.obj";

public:

	Score() {
		zero = new Model(PATH_TWO);
		one = new Model(PATH_THREE);
		two = new Model(PATH_TWO);
		three = new Model(PATH_THREE);
		four = new Model(PATH_TWO);
		five = new Model(PATH_THREE);
		six = new Model(PATH_TWO);
		seven = new Model(PATH_THREE);
		eight = new Model(PATH_TWO);
		nine = new Model(PATH_THREE);
	}

	~Score() {
		// delete every Model* in vector
		delete(zero);
		delete(one);
		delete(two);
		delete(three);
		delete(four);
		delete(five);
		delete(six);
		delete(seven);
		delete(eight);
		delete(nine);
	}

	int getNum() {
		return num;
	}

	void increment(int val) {
		num += val;
	}

	// first let's try printing to console
	void update() {
		// once a cube is hit, update score and text vector
		if (hit) {
			cout << "Perfect hit! Score: " << num << endl;

			increment(1);

			text_model.clear();
			string score_text = to_string(num);
			cout << "score text: " << score_text << endl;
			
			for (unsigned int i = 0; i < score_text.length(); i++) {

				char c = score_text[i];

				switch (c) {
				case '0':
					text_model.push_back(zero);
					break;

				case '1':
					text_model.push_back(one);
					break;

				case '2':
					text_model.push_back(two);
					break;

				case '3':
					text_model.push_back(three);
					break;

				case '4':
					text_model.push_back(four);
					break;

				case '5':
					text_model.push_back(five);
					break;

				case '6':
					text_model.push_back(six);
					break;

				case '7':
					text_model.push_back(seven);
					break;

				case '8':
					text_model.push_back(eight);
					break;

				case '9':
					text_model.push_back(nine);
					break;

				default:
					break;
				}
			}
		}
	}

	void draw(Shader shader, const mat4 & projection, const mat4 & view, const vec3 & start_pos) {
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);

		mat4 R = mat4(1.0f);
		mat4 S = scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
		vec3 pos = start_pos;	

		// send lighting information to shader
		/*vec3 lightPos = vec3(0.0f, 1.0f, 0.0f);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("viewPos", vec3(view[3].x, view[3].y, view[3].z));*/

		// draw each model in vector with shifted x position
		for (Model* text : text_model) {
			mat4 T = translate(mat4(1.0f), vec3(pos));
			mat4 M = T * R * S;
			shader.setMat4("model", M);
			text->Draw(shader);
			pos.x += 0.2f;
		}
	}
};

#endif
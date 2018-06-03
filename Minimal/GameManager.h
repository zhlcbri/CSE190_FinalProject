#pragma once

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Model.h"
#include "Cube.h"

using namespace std;
using namespace glm;


//string const path_hand = "Models/baseball_bat/bat.obj"; // &

string const path_hand = "Models/soccer_ball/soccer_ball.obj";

const char * MODEL_VERT = "model_loading.vert";
const char * MODEL_FRAG = "model_loading.frag";

const char * CUBE_VERT = "shader_cube.vert";
const char * CUBE_FRAG = "shader_cube.frag";

class GameManager
{
public:
	Model * hand = new Model(path_hand, false);
	Shader shader_model = Shader(MODEL_VERT, MODEL_FRAG, nullptr);
	Shader shader_cube = Shader(CUBE_VERT, CUBE_FRAG);

	vector<string> faces_skybox = {
		"Skybox/front.ppm",
		"Skybox/back.ppm",
		"Skybox/up.ppm",
		"Skybox/down.ppm",
		"Skybox/right.ppm",
		"Skybox/left.ppm",
	};

	Cube * skybox = new Cube(faces_skybox, true);

	GameManager() {

	}

	~GameManager() {
		delete(hand);
	}

	void renderHand(mat4 projection, mat4 view, mat4 model) {
		shader_model.use();
		shader_model.setMat4("projection", projection);
		shader_model.setMat4("view", view);
		shader_model.setMat4("model", model);
		hand->Draw(shader_model);
	}

	void renderSkybox(mat4 projection, mat4 view, mat4 model) {
		shader_cube.use();
		skybox->draw(shader_cube, projection, view, model);
	}

	void renderGame() {
	}

};

#endif
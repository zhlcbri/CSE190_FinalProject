#pragma once

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Model.h"
using namespace std;
using namespace glm;


//string const path_hand = "Models/baseball_bat/bat.obj"; // &

string const path_hand = "Models/soccer_ball/soccer_ball.obj"; // &

const char * MODEL_VERT = "model_loading.vert";
const char * MODEL_FRAG = "model_loading.frag";

class GameManager
{
public:
	Model * hand = new Model(path_hand, false);
	Shader shader_model = Shader(MODEL_VERT, MODEL_FRAG, nullptr);

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

	void renderGame() {


	}

};

#endif
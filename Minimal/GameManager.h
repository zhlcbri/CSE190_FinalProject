#pragma once

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <random>
#include "Model.h"
#include "Cube.h"
#include "Particles.h"
#include "Music.h"
#include "OpenGLText/OpenGLText.h"

#include <cstdlib>
#include <ctime>
using namespace std;
using namespace glm;

//string const path_hand = "Models/baseball_bat/bat.obj"; // &
string const PATH_HAND = "Models/soccer_ball/soccer_ball.obj";

const char * SOUND_1 = "Audio/my_heart_will_go_on.wav";

const char * MODEL_VERT = "model_loading.vert";
const char * MODEL_FRAG = "model_loading.frag";

const char * CUBE_VERT = "shader_cube.vert";
const char * CUBE_FRAG = "shader_cube.frag";

const char * PARTICLE_VERT = "shader_particle.vert";
const char * PARTICLE_FRAG = "shader_particle.frag";

vec3 cube_track = vec3(0, 2.0, -0.5);
mat4 T_cubeX = translate(mat4(1.0f), cube_track);
mat4 S_cubeX = scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));

mat4 fall = glm::translate(glm::mat4(1.0f), vec3(0.0f, -0.1f, 0.0f));

class GameManager
{
public:
	int counter = 0;
	Model * hand;
	Music * music;
	OpenGLText scoreText;

	Shader shader_model = Shader(MODEL_VERT, MODEL_FRAG);
	Shader shader_cube = Shader(CUBE_VERT, CUBE_FRAG);
	Shader shader_particle = Shader(PARTICLE_VERT, PARTICLE_FRAG);

	vector<string> faces_skybox = {
		"Skybox/front.ppm",
		"Skybox/back.ppm",
		"Skybox/up.ppm",
		"Skybox/down.ppm",
		"Skybox/right.ppm",
		"Skybox/left.ppm",
		"Skybox/left.ppm",
	};

	vector<string> faces_X = {
		"CubeTex/img_X.ppm",
		"CubeTex/img_X.ppm",
		"CubeTex/img_X.ppm",
		"CubeTex/img_X.ppm",
		"CubeTex/img_X.ppm",
		"CubeTex/img_X.ppm",
	};

	vector<string> faces_Y = {
		"CubeTex/img_Y.ppm",
		"CubeTex/img_Y.ppm",
		"CubeTex/img_Y.ppm",
		"CubeTex/img_Y.ppm",
		"CubeTex/img_Y.ppm",
		"CubeTex/img_Y.ppm",
	};

	vector<string> faces_A = {
		"CubeTex/img_A.ppm",
		"CubeTex/img_A.ppm",
		"CubeTex/img_A.ppm",
		"CubeTex/img_A.ppm",
		"CubeTex/img_A.ppm",
		"CubeTex/img_A.ppm",
	};

	vector<string> faces_B = {
		"CubeTex/img_B.ppm",
		"CubeTex/img_B.ppm",
		"CubeTex/img_B.ppm",
		"CubeTex/img_B.ppm",
		"CubeTex/img_B.ppm",
		"CubeTex/img_B.ppm",
	};

	Cube * skybox;
	Cube * cube_X;
	Cube * cube_Y;
	Cube * cube_A;
	Cube * cube_B;
	Particles * particles;

	const char *FONT_NAME = "baub_16";
	int CANVAS_WIDTH = 10;
	int CANVAS_HEIGHT = 10;
	
	GameManager() {
		hand = new Model(PATH_HAND, false);
		music = new Music(SOUND_1);

		skybox = new Cube(faces_skybox, true);
		cube_X = new Cube(faces_X, false, "X");
		cube_Y = new Cube(faces_X, false, "Y");
		cube_A = new Cube(faces_X, false, "A");
		cube_B = new Cube(faces_X, false, "B");

		particles = new Particles();
	}

	bool init() {
		if (!scoreText.init(FONT_NAME, CANVAS_WIDTH, CANVAS_HEIGHT)) {
			return false;
		}
		return true;
	}


	~GameManager() {
		delete(hand);
		delete(music);
		delete(skybox);
		delete(cube_X);
		delete(cube_Y);
		delete(cube_A);
		delete(cube_B);
		delete(particles);
	}

	void renderGame() {
	}

	vec3 gogoHand(vec3 handPos, vec3 torsoPos, vec3 handForward) {
		float threshold = 0.50f; // D
		float coeff = 7.5f; // k

		float distFromTorso = length(handPos - torsoPos); // R_r
		if (distFromTorso > threshold) {
			distFromTorso = distFromTorso + coeff * powf((distFromTorso - threshold), 2);
		}

		vec3 res = handPos + (handForward * distFromTorso);
		return res;
	}

	void renderHand(mat4 projection, mat4 view, mat4 model) {
		shader_model.use();
		shader_model.setMat4("projection", projection);
		shader_model.setMat4("view", view);
		shader_model.setMat4("model", model);
		hand->Draw(shader_model);
	}

	void renderSkybox(mat4 projection, mat4 view, mat4 model) {
		skybox->draw(shader_cube, projection, view, model);
	}

	void renderParticles(mat4 projection, mat4 view, mat4 model) {
		srand(time(0)); // particles have strange but cool effect
		int index = rand() % 11;
		particles->draw(shader_particle, projection, view, model, index);
		particles->update();
	}

	bool colliding(vec3 hand_pos, vec3 obj_pos, float scale) {
		float threshold = (float)sqrt(2) * scale;
		if (glm::distance(hand_pos, obj_pos) <= threshold) {
			cout << "colliding" << endl;
			return true;
		}
		return false;
	}


	//============== Sound stuff =================

	void playSound() {
		music->play();
	}

	void closeSound() {
		music->close();
	}


	//============ Cube stuff ================
	Cube* getRandomCube() {
		return cube_X;
	}

	void dropCubes(mat4 T, mat4 S, mat4 projection, mat4 view) {
		Cube* cube = getRandomCube();

		cube->translate(T);
		cube->scale(S);

		cube->draw(shader_cube, projection, view);
	}

	void renderCubes(mat4 projection, mat4 view) {
		dropCubes(T_cubeX, S_cubeX, projection, view);
	}

	bool duplicate(float z, vector<float> v) {
		for (int i = 0; i < v.size(); i++) {
			cout << "z is " << z << "v is " << v[i] << endl;
			if (abs(v[i]- z)<0.1)
				return false;
		}
		//using binary search if this is not enough
		return true;
	}

	void rainCubes(mat4 projection, mat4 view) {	
		mat4 S = scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));
	
		// randomize T matrix
		for (int i = 0; i < 10; i++) {
			if (i < 5)
				cube_track.x = -i * 0.4;
			else {
				cube_track.x = i * 0.4;
			}
			mat4 T = translate(mat4(1.0f), cube_track);
			dropCubes(T, S, projection, view);
			
		}
	}

	//============== Text Stuff ==================
	void renderScore() {
		// temp init(); place elsewhere later
		//init();
		if (!scoreText.init(FONT_NAME, CANVAS_WIDTH, CANVAS_HEIGHT)) {
			//cout << "init failed" << endl;
			return;
		}
		scoreText.beginString();
		float* bbStr = new float[2];
		char *tmpStr = "Hello World";
		scoreText.stringSize(tmpStr, bbStr);
		// 0.0 are place holders for xPos and yPos
		scoreText.drawString(10.0 - bbStr[0]*0.5, 5.0 - bbStr[1], tmpStr, 0, 0xF0F0F0F0);

		//
		scoreText.endString(); // will render the whole at once
	}
};

#endif
#pragma once

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <random>
#include "Model.h"
#include "Cube.h"
#include "Particles.h"
#include "Music.h"
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace glm;

//string const path_hand = "Models/baseball_bat/bat.obj"; // &
string const PATH_HAND = "Models/soccer_ball/soccer_ball.obj";

const char * SOUND_1 = "Audio/flowers_of_fire.wav";

const char * MODEL_VERT = "model_loading.vert";
const char * MODEL_FRAG = "model_loading.frag";

const char * CUBE_VERT = "shader_cube.vert";
const char * CUBE_FRAG = "shader_cube.frag";

const char * PARTICLE_VERT = "shader_particle.vert";
const char * PARTICLE_FRAG = "shader_particle.frag";

int num_instance = 4;
vec3 cube_track = vec3(0, 2.0, -0.5);
vector<vec3> cube_pos;
double speed = 1.0;
mat4 fall = glm::translate(glm::mat4(1.0f), vec3(0.0f, -0.1f, 0.0f));

// button states
bool button_X = false;
bool button_Y = false;
bool button_A = false;
bool button_B = false;
bool isPressed = false;

bool doOnce = true; // temp

class GameManager
{
public:
	int counter = 0;
	Model * hand = new Model(PATH_HAND, false);
	Music * music = new Music(SOUND_1);

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
	Particles * particles;

	Cube * cube_X;
	Cube * cube_Y;
	Cube * cube_A;
	Cube * cube_B;
	Cube * cube_curr;
	
	GameManager() {
		skybox = new Cube(faces_skybox, true);
		particles = new Particles();

		cube_X = new Cube(faces_X, false, "X");
		cube_Y = new Cube(faces_Y, false, "Y");
		cube_A = new Cube(faces_A, false, "A");
		cube_B = new Cube(faces_X, false, "B");

		cube_curr = new Cube(faces_B, false, "B");
	}

	~GameManager() {
		delete(hand);
		delete(music);
		delete(skybox);
		delete(particles);
		delete(cube_X);
		delete(cube_Y);
		delete(cube_A);
		delete(cube_B);
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
			//cout << "colliding" << endl;
			return true;
		}
		return false;
	}

	bool hit() {
		if (colliding(gogoPos, cube_curr->getToWorld()[3], 0.2f)) {
			if (button_X && cube_curr->getID() == "X") return true;
			else if ((button_Y && cube_curr->getID() == "Y")) return true;
			else if ((button_A && cube_curr->getID() == "A")) return true;
			else if ((button_B && cube_curr->getID() == "B")) return true;
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
	void renderCubes(mat4 projection, mat4 view, mat4 model) {
		cube_X->draw(shader_cube, projection, view, model);
	}

	Cube* getRandomCube() {
		int min = 0;
		int max = 3;
		int output = min + (rand() % static_cast<int>(max - min + 1));
		cout << "out: " << output << endl;
		switch (output) {
		case 0:
			return cube_X;
			break;

		case 1:
			return cube_Y;
			break;

		case 2:
			return cube_A;
			break;

		default:
			return cube_B;
			break;
		}
	}

	void dropCubes(mat4 T, mat4 S, mat4 projection, mat4 view) {
		//Cube* cube = getRandomCube();

		cube_curr->translate(T);
		cube_curr->scale(S);

		cube_curr->draw(shader_cube, projection, view);
	}


	void calculate() {
		double y = cube_track.y;
		cout << y << endl;
		srand(time(0));
		int num = rand() % 25;
		for (double i = 1; i <= num_instance; i++) {
			if (i < 3) {
				cube_track = vec3(-0.1*num + i, y, -0.1*i*num);
			}
			else {
				cube_track = vec3(0.1*num + i, y, -0.1*(i-1.6)*num);
			}

			cube_pos.push_back(cube_track);
			cout << cube_pos.size() << endl;
		}

		cube_curr = getRandomCube();
	}


	void rainCubes(mat4 projection, mat4 view) {
		
		mat4 S = scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));
		cout << cube_pos.size() << endl;
		for (int i = 0; i < num_instance; i++) {
			vec3 track = vec3(cube_pos[i].x, cube_track.y, cube_pos[i].z);
			mat4 T = translate(mat4(1.0f), track);
			//cube_curr = getRandomCube();//
			dropCubes(T, S, projection, view);
		}
		
		cube_track.y -= speed*0.002;
	}
};

#endif
#pragma once

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <random>
#include "Model.h"
#include "Cube.h"
#include "Particles.h"
#include "Music.h"
#include "Score.h"
#include <cstdlib>
#include <ctime>
#include <map>
using namespace std;
using namespace glm;

//string const path_hand = "Models/baseball_bat/bat.obj"; // 
string const PATH_HAND = "Models/soccer_ball/soccer_ball.obj";
//string const PATH_HAND = "Models/num_three/three.obj";

const char * SOUND_1 = "Audio/my_heart_will_go_on.wav";

const char * MODEL_VERT = "model_loading.vert";
const char * MODEL_FRAG = "model_loading.frag";

const char * CUBE_VERT = "shader_cube.vert";
const char * CUBE_FRAG = "shader_cube.frag";

const char * PARTICLE_VERT = "shader_particle.vert";
const char * PARTICLE_FRAG = "shader_particle.frag";

const char * SCORE_VERT = "shader_score.vert";
const char * SCORE_FRAG = "shader_score.frag";


vec3 cube_track = vec3(0, 2.0, -0.5);
vector<vec3> cube_pos;
vector<Cube*> cube_queue;
vector<Cube*> cube_queue_copy;
vector<Particles*> particles_vec;
double speed = 1.0;
mat4 fall = glm::translate(glm::mat4(1.0f), vec3(0.0f, -0.1f, 0.0f));

// button states
bool button_X = false;
bool button_Y = false;
bool button_A = false;
bool button_B = false;
bool isPressed = false;

bool doOnce = true; // temp

// some colors I like
vec3 red = vec3(1.000, 0.000, 0.000);
vec3 pink = vec3(1.000, 0.078, 0.576); // deep pink
vec3 yellow = vec3(1.000, 1.000, 0.000);
vec3 purple = vec3(0.502, 0.000, 0.502);
vec3 indigo = vec3(0.294, 0.000, 0.510); 
vec3 lime = vec3(0.000, 1.000, 0.000);
vec3 aqua = vec3(0.000, 1.000, 1.000); 
vec3 aquamarine = vec3(0.498, 1.000, 0.831); 
vec3 sky = vec3(0.000, 0.749, 1.000); // deep sky blue
vec3 blue = vec3(0.000, 0.000, 1.000); 
vec3 lavender = vec3(1.000, 0.941, 0.961); // lavender blush

class GameManager
{
public:

	//int counter = 0;
	Model * hand;
	Music * music;
	Score * my_score;

	Shader shader_model = Shader(MODEL_VERT, MODEL_FRAG);
	Shader shader_cube = Shader(CUBE_VERT, CUBE_FRAG);
	Shader shader_particle = Shader(PARTICLE_VERT, PARTICLE_FRAG);
	Shader shader_score = Shader(SCORE_VERT, SCORE_FRAG);
	
	vector<string> faces_cry = {
		"CubeTex/emoji_cry.ppm",
		"CubeTex/emoji_cry.ppm",
		"CubeTex/emoji_cry.ppm",
		"CubeTex/emoji_cry.ppm",
		"CubeTex/emoji_cry.ppm",
		"CubeTex/emoji_cry.ppm",
	};

	vector<string> faces_wink = {
		"CubeTex/emoji_wink.ppm",
		"CubeTex/emoji_wink.ppm",
		"CubeTex/emoji_wink.ppm",
		"CubeTex/emoji_wink.ppm",
		"CubeTex/emoji_wink.ppm",
		"CubeTex/emoji_wink.ppm",
	};

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
		"CubeTex/X.ppm",
		"CubeTex/X.ppm",
		"CubeTex/X.ppm",
		"CubeTex/X.ppm",
		"CubeTex/X.ppm",
		"CubeTex/X.ppm",
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
		"CubeTex/A.ppm",
		"CubeTex/A.ppm",
		"CubeTex/A.ppm",
		"CubeTex/A.ppm",
		"CubeTex/A.ppm",
		"CubeTex/A.ppm",
	};

	vector<string> faces_B = {
		"CubeTex/B.ppm",
		"CubeTex/B.ppm",
		"CubeTex/B.ppm",
		"CubeTex/B.ppm",
		"CubeTex/B.ppm",
		"CubeTex/B.ppm",
	};

	Cube * skybox;
	Particles * particles;
	Particles * particles_1;
	Particles * particles_2;
	Particles * particles_3;
	Particles * particles_4;
	Cube * cube_X;
	Cube * cube_Y;
	Cube * cube_A;
	Cube * cube_B;

	Cube * head_cry;
	Cube * head_wink;

	GameManager() {
		hand = new Model(PATH_HAND, false);
		music = new Music(SOUND_1);

		skybox = new Cube(faces_skybox, true);

		particles_1 = new Particles(pink, aqua);
		particles_2 = new Particles(lime, pink);
		particles_3 = new Particles(yellow, purple);
		particles_4 = new Particles(aquamarine, red);

		cube_X = new Cube(faces_B, false, "X");
		cube_Y = new Cube(faces_Y, false, "Y");
		cube_A = new Cube(faces_A, false, "A");
		cube_B = new Cube(faces_B, false, "B");

		cube_queue.push_back(cube_X);
		cube_queue.push_back(cube_Y);
		cube_queue.push_back(cube_A);
		cube_queue.push_back(cube_B);

		particles_vec.push_back(particles_1);
		particles_vec.push_back(particles_2);
		particles_vec.push_back(particles_3);
		particles_vec.push_back(particles_4);

		cube_pos.push_back(vec3(0, 0, 0));
		cube_pos.push_back(vec3(0, 0, 0));
		cube_pos.push_back(vec3(0, 0, 0));
		cube_pos.push_back(vec3(0, 0, 0));
		cube_queue_copy = cube_queue;

		head_cry = new Cube(faces_cry, false);
		head_wink = new Cube(faces_wink, false);

		my_score = new Score();
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

	void update() {
		//hit = hitting();
		//cout << "hit: " << hit << endl;
		my_score->update();
	}

	//===================== head ======================
	void renderHead(mat4 projection, mat4 view, mat4 model, bool flag = false) {
		if (flag) {
			head_cry->draw(shader_cube, projection, view, model);
		}
		else {
			head_wink->draw(shader_cube, projection, view, model);
		}
	}

	//===================== calculate controller position ===================
	vec3 gogoHand(vec3 handPos, vec3 torsoPos, vec3 handForward) {
		float threshold = 0.50f; // D
		float coeff = 2.5f; // k

		float distFromTorso = length(handPos - torsoPos); // R_r
		if (distFromTorso > threshold) {
			distFromTorso = distFromTorso + coeff * powf((distFromTorso - threshold), 2);
		}

		vec3 res = handPos + (handForward * distFromTorso);
		return res;
	}

	//===================== controller ========================
	void renderHand(mat4 projection, mat4 view, mat4 model) {
		shader_model.use();
		shader_model.setMat4("projection", projection);
		shader_model.setMat4("view", view);
		shader_model.setMat4("model", model);
		hand->Draw(shader_model);
	}

	//======================= skybox ==========================
	void renderSkybox(mat4 projection, mat4 view, mat4 model) {
		skybox->draw(shader_cube, projection, view, model);
	}

	//===================== particles =========================
	void renderParticles(Particles * p, mat4 projection, mat4 view, mat4 model) {
		srand(time(0)); // particles have strange but cool effect
		int index = rand() % 11;

		

		p->draw(shader_particle, projection, view, model, index);
		p->update();
	}

	//-------------- true when controller collides with a cube ---------------
	bool colliding(vec3 hand_pos, vec3 obj_pos, float scale) {
		float threshold = (float)sqrt(3) * scale;
		if (glm::distance(hand_pos, obj_pos) <= threshold) {
			// << "colliding" << endl;
			return true;
		}
		return false;
	}

	// delete later; use global bool hit to trigger particles and score increment
	bool hitting() {
		for (int i = 0; i < cube_queue.size(); i++) {
			if (colliding(gogoPos, cube_queue[i]->getToWorld()[3], 0.3f)
				|| colliding(gogoPos_right, cube_queue[i]->getToWorld()[3], 0.3f)) {

				if (button_X && cube_queue[i]->getID() == "X") {
					cube_queue[i]->is_hit = true;
					hit = true;
					return true;
				}
				else if ((button_Y && cube_queue[i]->getID() == "Y")) {
					cube_queue[i]->is_hit = true;
					hit = true;
					return true;
				}
				else if ((button_A && cube_queue[i]->getID() == "A")) {
					cube_queue[i]->is_hit = true;
					hit = true;
					return true;
				}
				else if ((button_B && cube_queue[i]->getID() == "B")) {
					cube_queue[i]->is_hit = true;
					hit = true;
					return true;
				}
				else {
					hit = false;
				}
			}
		}
		return false;
	}

	//=================== Sound =====================
	void playSound() {
		music->play();
	}

	void closeSound() {
		music->close();
	}


	//=================== Cube =====================
	void renderCubes(mat4 projection, mat4 view, mat4 model) {
		cube_X->draw(shader_cube, projection, view, model);
	}

	Cube* getRandomCube() {
		int min = 0;
		int max = 3;
		int output = min + (rand() % static_cast<int>(max - min + 1));
		//cout << "out: " << output << endl;
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

	int getRandom() {
		int min = 0;
		int max = 3;
		int output = min + (rand() % static_cast<int>(max - min + 1));
		return output;
	}

	//----------------- drop a single random cube ----------------- 
	void dropCubes(Cube * cube, mat4 T, mat4 S, mat4 projection, mat4 view) {
		//Cube* cube = getRandomCube();

		cube->translate(T);
		cube->scale(S);

		cube->draw(shader_cube, projection, view);
	}

	//------------- calculate cube positions ------------------
	void calculate() {
		double y = cube_track.y;
		int num = rand() % 15;
		srand(time(0));

		int color_index = 0;

		for (int i = 0; i < cube_queue.size(); i++) {
			int cube_i = rand() % 4;
					
			//////////////////////////
			if (cube_queue_copy[cube_i]->getID() == "X") {
				Cube * copy = new Cube(faces_X, false, "X");
				cube_queue[i] = copy;
				color_index = 0;
			}
			else if ((cube_queue_copy[cube_i]->getID() == "Y")) {
				Cube * copy = new Cube(faces_Y, false, "Y");
				cube_queue[i] = copy;
				color_index = 1;
			}
			else if ((cube_queue_copy[cube_i]->getID() == "A")) {
				Cube * copy = new Cube(faces_A, false, "A");
				cube_queue[i] = copy;	
				color_index = 2;
			}
			else if ((cube_queue_copy[cube_i]->getID() == "B")) {
				Cube * copy = new Cube(faces_B, false, "B");
				cube_queue[i] = copy;
				color_index = 3;
			}
			//////////////////////////

			double x = 0.1*(i*num + num);
			if (i % 2 == 0) {
				cube_track = vec3(x, y, -0.1*(i - 1.6)*num);
			}
			else {
				cube_track = vec3(-x, y, -0.1*(i - 1.6)*num);
			}
			cube_queue[i]->is_hit = false;
			cube_pos[i] = (cube_track);

			//============= use different colors on cubes ===============
			vec3 color_1 = vec3(1.0f, 1.0f, 1.0f);
			vec3 color_2 = vec3(1.0f, 1.0f, 1.0f);
			color_1 = aqua;
			color_2 = aqua;
			/*if (color_index == 0) {
				cout << " x " << color_index << endl;
				color_1 = aqua;
				color_2 = aqua;
			}
			else if (color_index == 1) {
				cout << " y " << color_index << endl;
				color_1 = red;
				color_2 = red;
			}
			else if (color_index == 2) {
				cout << " A " << color_index << endl;
				color_1 = yellow;
				color_2 = purple;
			}
			else if (color_index == 3) {
				cout << " b " << color_index << endl;
				color_1 = aquamarine;
				color_2 = red;
			}*/

			Particles * p_copy = new Particles(color_1, color_2);
			particles_vec[i] = (p_copy);

			cube_queue[i]->is_hit = false;
			hit = false;
		}
	}

	//--------------- drop a lot of cubes along designated track ----------------
	void rainCubes(mat4 projection, mat4 view) {
		hitting();
		mat4 S = scale(mat4(1.0f), vec3(0.1f, 0.1f, 0.1f));
		
		vec3 track;
		for (int i = 0; i < cube_queue.size(); i++) {
			track = vec3(cube_pos[i].x, cube_track.y, cube_pos[i].z);
			mat4 T = translate(mat4(1.0f), track);		

			if (cube_queue[i]->is_hit == false) {
			
				dropCubes(cube_queue[i], T, S, projection, view);
			}
			else {
				vec3 temp = cube_queue[i]->getToWorld()[3];
				
				renderParticles(particles_vec[i], projection, view, cube_queue[i]->getToWorld());
			}
		}		

		cube_track.y -= speed * 0.004;
	}


	//================== Score ====================
	void renderScore(mat4 projection, mat4 view) {

		// use model shader for now
		vec3 score_pos = vec3(0.0f, 0.0f, -3.0f);
		my_score->draw(shader_model, projection, view, score_pos);
	}

};

#endif
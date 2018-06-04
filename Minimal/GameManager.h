#pragma once

#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "Model.h"
#include "Cube.h"
#include "Particles.h"
#include "Music.h"

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

class GameManager
{
public:
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
		"CubeTex/x.ppm",
		"CubeTex/x.ppm",
		"CubeTex/x.ppm",
		"CubeTex/x.ppm",
		"CubeTex/x.ppm",
		"CubeTex/x.ppm",
	};

	Cube * skybox = new Cube(faces_skybox, true);
	Particles * particles = new Particles();

	Cube * cube_X = new Cube(faces_X, false, "X");

	GameManager() {
		
	}

	~GameManager() {
		delete(hand);
		delete(music);
		delete(skybox);
		delete(particles);
	}

	vec3 gogoHand(vec3 handPos, vec3 torsoPos, vec3 handForward) {
		float threshold = 0.50f;
		float coeff = 15.0f;

		float distFromTorso = length(handPos - torsoPos);
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

	void renderCubes(mat4 projection, mat4 view, mat4 model) {
		cube_X->draw(shader_cube, projection, view, model);
	}

	void renderParticles(mat4 projection, mat4 view, mat4 model) {
		particles->draw(shader_particle, projection, view, model);
		particles->update();
	}

	bool colliding(vec3 hand_pos, vec3 obj_pos, float scale) {
		float threshold = sqrt(2) * scale;
		if (glm::distance(hand_pos, obj_pos) <= threshold) {
			cout << "colliding" << endl;
			return true;
		}
		return false;
	}

	void playSound() {
		music->play();
	}

	void closeSound() {
		music->close();
	}

	void renderGame() {
	}

};

#endif
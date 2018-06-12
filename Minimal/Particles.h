#ifndef Particles_h
#define Particles_h

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h> // Needed for a new class

// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <cstdlib>

#include "Shader.h"

using namespace std;
using namespace glm;

class Particles
{
private:


public:
	Particles(vec3, vec3);
    ~Particles();
    
    glm::vec3 translations[10000];

    void update();
    int life;
    int visible;
    
    void draw(Shader, const glm::mat4&, const glm::mat4&, const glm::mat4&, int);
    void setVisible();

    // These variables are needed for the shader program
    GLuint quadVAO, quadVBO, instVBO, transVBO;
    GLuint uProjection, uModelview, uView, uModel;
};
#endif

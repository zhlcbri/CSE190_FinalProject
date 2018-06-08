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

class Particles
{
public:
    Particles();
    ~Particles();
    
    glm::vec3 translations[10000];

    void update();
    int life;
    int visible;
    
    void draw(Shader, const glm::mat4&, const glm::mat4&, const glm::mat4&);
    void setVisible();

    // These variables are needed for the shader program
    GLuint quadVAO, quadVBO, instVBO, transVBO;
    GLuint uProjection, uModelview, uView, uModel;

	// beautiful colors sent to shader randomly
	vector<glm::vec3> colors;
};

static const GLfloat quadv[] =
{
    -0.05f,  0.05f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.05f, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.05f, -0.05f, 0.0f,  1.0f, 0.0f, 0.0f,
    
    -0.05f,  0.05f, 0.0f,  1.0f, 0.0f, 0.0f,
    0.05f, -0.05f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.05f,  0.05f,0.0f,  1.0f, 0.0f, 0.0f
};
#endif

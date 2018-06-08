#include <stdio.h>
#include <iostream>
#include <algorithm>
#include "Particles.h"
#include <math.h>
#include "glm/ext.hpp"

using namespace std;
using namespace glm;

Particles::Particles()
{   
    int index = 0;
    float offset = 0.5f;
    for (int y = -100; y < 100; y += 2)
    {
        for (int x = -100; x < 100; x += 2)
        {
            glm::vec3 translation;
            translation.x = (float)x / 50.0f + offset + (float) rand()/ (float) RAND_MAX * 4.0-2.0;
            translation.y = (float)y / 50.0f + offset + (float) rand()/ (float) RAND_MAX * 4.0-2.0;
            translation.z = 0.0f;
            translations[index++] = translation;
        }
    }

    life = 1;
    visible = 1;
    
    glGenBuffers(1, &instVBO);

    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(quadVAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadv), quadv, GL_STATIC_DRAW);

    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, instVBO); // this attribute comes from a different vertex buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 10000, &translations[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(2, 1);

    // Unbind the VAO now so we don't accidentally tamper with it.
    glBindVertexArray(0);

	// some colors I like
	colors.push_back(vec3(1.000, 0.000, 0.000)); // red
	colors.push_back(vec3(1.000, 0.078, 0.576)); // deep pink
	colors.push_back(vec3(1.000, 1.000, 0.000)); // yellow
	colors.push_back(vec3(0.502, 0.000, 0.502)); // purple
	colors.push_back(vec3(0.294, 0.000, 0.510)); // indigo
	colors.push_back(vec3(0.000, 1.000, 0.000)); // lime
	colors.push_back(vec3(0.000, 1.000, 1.000)); // aqua
	colors.push_back(vec3(0.498, 1.000, 0.831)); // aquamarine
	colors.push_back(vec3(0.000, 0.749, 1.000)); // deep sky blue
	colors.push_back(vec3(0.000, 0.000, 1.000)); // blue
	colors.push_back(vec3(1.000, 0.941, 0.961)); // lavender blush
}

void Particles::draw(Shader shader, const mat4 & projection, const mat4 & view, const mat4 & model, int index)
{
	if (visible)
	{
		shader.use();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);

		if (index >= 0 && index < colors.size() - 1) {
			shader.setVec3("color", colors.at(index));
		}
		// set random color
		//int index = rand() % colors.size();
		shader.setVec3("color", colors.at(1));

		glBindVertexArray(quadVAO);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 10000);
		glBindVertexArray(0);
	}
}


Particles::~Particles()
{
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &instVBO);
    glDeleteBuffers(1, &quadVBO);
}


void Particles::update()
{
    if(life % 2000 != 0) // shorten lifetime
    {
        for (int i = 0; i < 10000; i++)
        {
            translations[i].x += rand()/ (float) RAND_MAX/10.0;
            translations[i].y += rand()/ (float) RAND_MAX/10.0;
            translations[i].z += rand()/ (float) RAND_MAX/10.0;
            translations[i].x -= rand()/ (float) RAND_MAX/10.0;
            translations[i].y -= rand()/ (float) RAND_MAX/10.0;
            translations[i].z -= rand()/ (float) RAND_MAX/10.0;
        }
        
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, instVBO); // this attribute comes from a different vertex buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 10000, &translations[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glVertexAttribDivisor(2, 1);
        life++;
    }
    else
    {
        visible = 0;
    }
}

void Particles::setVisible()
{
    visible = 1;
    life++;
}

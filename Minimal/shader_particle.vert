#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 acolor;
layout (location = 2) in vec3 aoffset;

uniform mat4 projection;
// uniform mat4 modelview;

uniform mat4 view;
uniform mat4 model;

out float sampleExtraOutput;
out vec3 fcolor;

void main()
{
    // gl_Position = projection * modelview * vec4(position + aoffset, 1.0);
    
	gl_Position = projection * view * model * vec4(position + aoffset, 1.0);

	fcolor = acolor;
    sampleExtraOutput = 1.0f;
}

#version 330 core
in float sampleExtraOutput;
in vec3 fcolor;

out vec4 color;

void main()
{
    color = vec4(fcolor.x, fcolor.y, fcolor.z, sampleExtraOutput);
}

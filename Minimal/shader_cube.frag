#version 330 core

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube myTex;

void main()
{
    FragColor = texture(myTex, TexCoords);
}

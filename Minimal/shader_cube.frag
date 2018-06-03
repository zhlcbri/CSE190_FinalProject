#version 330 core

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube myTex;

void main()
{
    // FragColor = vec4(0.19f, 0.42f, 0.37f, 1.0f);

    FragColor = texture(myTex, TexCoords);
}

#version 330 core

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube myTex;
uniform samplerCube myTex_X;

// 0 for skybox texture, 1 for X
uniform int tex_mode; 

void main()
{

    FragColor = texture(myTex, TexCoords);

    // FragColor = vec4(0.19f, 0.42f, 0.37f, 1.0f);

    //if (tex_mode == 0) FragColor = texture(myTex, TexCoords);

	//else if (tex_mode == 1) FragColor = texture(myTex_X, TexCoords);
}

#version 330 core

out vec4 FragColor;
in vec3 ourColor; // the input variable from the vertex shader (same name and same type)
in vec2 texCoord; //coming from vertex shader

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    FragColor = texture(texture1, texCoord);
}
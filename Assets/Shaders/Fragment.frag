#version 330 core

out vec4 FragColor;

in vec2 texCoord; //coming from vertex shader
in vec4 color;
in float texIndex;

uniform sampler2D textures[16]; //"16" should be dynamic somehow

void main()
{
    int tIndex = int(texIndex);
    FragColor = texture(textures[tIndex], texCoord) * color;
}
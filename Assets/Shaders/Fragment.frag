#version 330 core

out vec4 FragColor;

in vec2 texCoord; //coming from vertex shader
in vec4 color;
in float texIndex;

uniform sampler2D textures[2];

void main()
{
    int tIndex = int(texIndex);
    FragColor = texture(texIndex == 0.0? textures[tIndex] : textures[tIndex], texCoord) * color;
}
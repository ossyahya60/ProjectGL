#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"
#include <filesystem>

class Texture
{
public:
	int width;
	int height;
	int numberOfChannels;

	Texture();
	Texture(const char* name, bool isPng);
	bool loadTexture(const char* name, bool isPng);
	void setTextureWrapping(int textureWrapH, int textureWrapV);
	void setTextureFiltering(int minFilter, int maxFilter);
	void bindTexture();
	void deleteTexture();
	~Texture();
private:
	unsigned int textureID;
};

#endif


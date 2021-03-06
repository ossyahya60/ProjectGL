#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"
#include <filesystem>
#include <glm/glm.hpp>

class Texture
{
public:
	static int nextFreeID;

	int width;
	int height;
	int numberOfChannels;
	unsigned int color = 0xffffffff;

	Texture();
	Texture(const char* name, bool isPng);
	GLuint getTexID();
	void setTexID(unsigned int texID);
	bool loadTexture(const char* name, bool isPng);
	void setTextureWrapping(int textureWrapH, int textureWrapV);
	void setTextureFiltering(int minFilter, int maxFilter);
	void bindTexture();
	void deleteTexture();
	~Texture();
private:
	unsigned int textureID;
	unsigned int assignedTexID;
};

#endif


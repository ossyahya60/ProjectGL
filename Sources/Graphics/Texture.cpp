#include "Texture.h"

int Texture::nextFreeID = 1;

Texture::Texture()
{
	width = 1;
	height = 1;
	numberOfChannels = 3;
	textureID = 0;
	assignedTexID = 0;

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID); //for 2D textures?

	//texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //mipmap option => tex filtering: linear, mipmap: linear between two
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
}

Texture::Texture(const char* name, bool isPng)
{
	width = 1;
	height = 1;
	numberOfChannels = isPng? 4 : 3;
	textureID = 0;
	assignedTexID = 0;

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID); //for 2D textures?

	//texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //mipmap option => tex filtering: linear, mipmap: linear between two
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	loadTexture(name, isPng);
}

GLuint Texture::getTexID()
{
	return assignedTexID;
}

void Texture::setTexID(unsigned int texID)
{
	assignedTexID = texID;
}

bool Texture::loadTexture(const char* name, bool isPng)
{
	std::string projectPath = std::filesystem::current_path().string();
	std::replace(projectPath.begin(), projectPath.end(), '\\', '/');

	stbi_set_flip_vertically_on_load(true); //flip images
	unsigned char* data = stbi_load((projectPath + name).c_str(), &width, &height, &numberOfChannels, 0);

	if (data) //error checking
	{
		if(isPng)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data); //note jpg is RGB
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); //note jpg is RGB

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return false;
	}

	//free image memory (we don't need it anymore)
	stbi_image_free(data);

	return true;
}

void Texture::setTextureWrapping(int textureWrapH, int textureWrapV)
{
	glBindTexture(GL_TEXTURE_2D, textureID); //for 2D textures?

	//texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrapH);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrapV);
}

void Texture::setTextureFiltering(int minFilter, int maxFilter)
{
	glBindTexture(GL_TEXTURE_2D, textureID); //for 2D textures?

	//texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter); //mipmap option => tex filtering: linear, mipmap: linear between two
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, maxFilter);
}

void Texture::bindTexture()
{
	glBindTexture(GL_TEXTURE_2D, textureID); //for 2D textures?
}

void Texture::deleteTexture() //don't call this unless you won't use it anymore
{
	glDeleteTextures(1, &textureID);
} 

Texture::~Texture()
{
	deleteTexture();
}

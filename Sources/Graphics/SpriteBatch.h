#ifndef SPRITE_BATCH
#define SPRITE_BATCH

#include <glm/glm.hpp>
#include "Texture.h"

struct Vertex //keep this order!!
{
	glm::vec3 position;
	glm::vec2 texCoord;
	glm::vec4 color;
	float texID;
};

//this batch renderer is specific for 2d rendering only!, it's based on the Cherno implementation of a 2D batch renderer
//link: https://www.youtube.com/watch?v=KyCQBQzaBOM&ab_channel=TheCherno
class SpriteBatch
{
private:
	static bool initCalled;

	//helper functions
	static Vertex* createQuad(Vertex* target, float x, float y, float texID, const glm::vec4& color, const glm::vec2& size);
	static void bindTextureGivenIndex(int index);
public:
	static void init();
	static void shutDown();

	static void begin();
	static void end();
	static void flush();

	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
	static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, Texture* tex);

	//stats
	struct Stats
	{
		unsigned int drawCount;
		unsigned int quadCount;
	};

	static const Stats& getStats();
	static void resetStats();
};

#endif


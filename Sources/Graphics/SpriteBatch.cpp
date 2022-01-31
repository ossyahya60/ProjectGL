#include "SpriteBatch.h"
#include <array>
#include <glad/glad.h>
#include <map>
#include <iostream>

using namespace std;

static const size_t maxQuadCount = 1000;
static const size_t maxVerticesCount = maxQuadCount * 4;
static const size_t maxIndexCount = maxQuadCount * 6;
static const size_t maxTextureCount = 16; //this should be queried from GPU, since each gpu differ in the amount of textures it can store

struct RendererData
{
    GLuint quadVA = 0; //vertex array
    GLuint quadVB = 0; //vertex buffer
    GLuint quadIB = 0; //index buffer

    Texture* whiteTexture = nullptr; //this texture is used to draw colored quads, it's reserved by the renderer
    unsigned int whiteTextureSlot = 0; //reserved as first texture in texture slots

    GLuint indexCount = 0; //actual number of indices uses from the reserved number (maxIndexCount)

    Vertex* quadBuffer = nullptr; //this a pointer to the buffer holding the quads
    Vertex* quadBufferPtr = nullptr; //this is a pointer to where we are in the buffer (reset every frame)

    unsigned int* indices = nullptr;

    std::map<Texture*, Texture*> textureSlots;
    unsigned int textureSlotIndex = 1; //next free space to insert a new texture

    SpriteBatch::Stats renderStats; //this is just some stats
};

static RendererData rData;

bool SpriteBatch::initCalled = false;

void SpriteBatch::init()
{
    if (SpriteBatch::initCalled) //avoid calling init multiple times by mistake, as this would cause memory leak! (I handled it)
        return;

    SpriteBatch::initCalled = true;

    //allocate memory
    rData.quadBuffer = new Vertex[maxVerticesCount];

    //vertex array
    glGenVertexArrays(1, &rData.quadVA);
    glBindVertexArray(rData.quadVA);

    //vertex buffer
    glGenBuffers(1, &rData.quadVB);
    glBindBuffer(GL_ARRAY_BUFFER, rData.quadVB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * maxVerticesCount, nullptr, GL_DYNAMIC_DRAW); //reserving data only without initializing them

    //pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    //color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

    // texture coord attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texCoord));

    // texture id attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, texID));

    //index buffer
    rData.indices = new unsigned int[maxIndexCount];
    int offset = 0;
    for (size_t i = 0; i < maxIndexCount; i += 6)
    {
        rData.indices[i] = offset;
        rData.indices[i + 1] = offset + 1;
        rData.indices[i + 2] = offset + 2;
        rData.indices[i + 3] = offset + 2;
        rData.indices[i + 4] = offset + 3;
        rData.indices[i + 5] = offset + 0;

        offset += 4;
    }

    glGenBuffers(1, &rData.quadIB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rData.quadIB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndexCount * sizeof(unsigned int), rData.indices, GL_STATIC_DRAW); //note: indices buffer is static, data itself doesn't change

    rData.whiteTexture = new Texture();

    rData.textureSlots[rData.whiteTexture] = rData.whiteTexture; //assign white texture as the first texture in texture slots
    //for (unsigned int i = 1; i < maxTextureCount; i++) //initialize all places with 0 as default texture (white texture)
    //    rData.textureSlots[i] = *rData.whiteTexture;
}

void SpriteBatch::shutDown()
{
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &rData.quadVA);
    glDeleteBuffers(1, &rData.quadVB);
    glDeleteBuffers(1, &rData.quadIB);
    rData.whiteTexture->deleteTexture();

    delete rData.whiteTexture;
    delete[] rData.quadBuffer;
    delete[] rData.indices;
}

void SpriteBatch::begin()
{
    rData.quadBufferPtr = rData.quadBuffer; //reset pointer to begining if quad buffer
}

void SpriteBatch::end() //here, we submit data for rendering to GPU
{
    GLsizeiptr size = (uint8_t*)rData.quadBufferPtr - (uint8_t*)rData.quadBuffer; //size of the actual data used from the quad buffer
    glBindBuffer(GL_ARRAY_BUFFER, rData.quadVB);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, rData.quadBuffer);
}

void SpriteBatch::flush() //actual rendering of quads
{
    int iterator = 0;
    for (auto const& x : rData.textureSlots)
    {
        x.second->bindTexture();
        bindTextureGivenIndex(x.second->getTexID());
    }

    glBindVertexArray(rData.quadVA);
    glDrawElements(GL_TRIANGLES, rData.indexCount, GL_UNSIGNED_INT, 0);
    rData.renderStats.drawCount++; //gpu draw calls

    //reset
    rData.indexCount = 0;
    rData.textureSlotIndex = 1;
}

void SpriteBatch::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    if (rData.indexCount >= maxIndexCount) //reached maximum nuber of quads rendered in a single draw call
    {
        end();
        flush();
        begin();
    }

    rData.quadBufferPtr = createQuad(rData.quadBufferPtr, position.x, position.y, (float)rData.whiteTexture->getTexID(), color, size);
    rData.indexCount += 6; //a quad
    rData.renderStats.quadCount++;
}

void SpriteBatch::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, Texture* tex)
{
    if (rData.indexCount >= maxIndexCount || (rData.textureSlotIndex >= maxTextureCount && !rData.textureSlots.count(tex))) //reached maximum nuber of quads rendered in a single draw call
    {
        end();
        flush();
        begin();
    }

    float texIndex = 0.0f;
    if (rData.textureSlots.count(tex)) //texture exists
        texIndex = (float)tex->getTexID();
    else
    {
        texIndex = (float)rData.textureSlotIndex;
        rData.textureSlots[tex] = tex;
        rData.textureSlotIndex++;
    }

    rData.quadBufferPtr = createQuad(rData.quadBufferPtr, position.x, position.y, texIndex, color, size);
    rData.indexCount += 6; //a quad
    rData.renderStats.quadCount++;
}

const SpriteBatch::Stats& SpriteBatch::getStats()
{
    // TODO: insert return statement here
    return rData.renderStats;
}

void SpriteBatch::resetStats()
{
    memset(&rData.renderStats, 0, sizeof(Stats));
}

/////////////////////////////////
Vertex* SpriteBatch::createQuad(Vertex* target, float x, float y, float texID, const glm::vec4& color, const glm::vec2& size)
{
    target->position = { x, y, 0.0f };
    target->texCoord = { 0.0f, 0.0f };
    target->color = color;
    target->texID = texID;
    target++;

    target->position = { x + size.x,  y, 0.0f };
    target->texCoord = { 1.0f, 0.0f };
    target->color = color;
    target->texID = texID;
    target++;

    target->position = { x + size.x,  y + size.y, 0.0f };
    target->texCoord = { 1.0f, 1.0f };
    target->color = color;
    target->texID = texID;
    target++;

    target->position = { x,  y + size.y, 0.0f };
    target->texCoord = { 0.0f, 1.0f };
    target->color = color;
    target->texID = texID;
    target++;

    return target;
}

void SpriteBatch::bindTextureGivenIndex(int index)
{
    switch (index)
    {
    case 0:
        glActiveTexture(GL_TEXTURE0);
        break;
    case 1:
        glActiveTexture(GL_TEXTURE1);
        break;
    case 2:
        glActiveTexture(GL_TEXTURE2);
        break;
    case 3:
        glActiveTexture(GL_TEXTURE3);
        break;
    case 4:
        glActiveTexture(GL_TEXTURE4);
        break;
    case 5:
        glActiveTexture(GL_TEXTURE5);
        break;
    case 6:
        glActiveTexture(GL_TEXTURE6);
        break;
    case 7:
        glActiveTexture(GL_TEXTURE7);
        break;
    case 8:
        glActiveTexture(GL_TEXTURE8);
        break;
    case 9:
        glActiveTexture(GL_TEXTURE9);
        break;
    case 10:
        glActiveTexture(GL_TEXTURE10);
        break;
    case 11:
        glActiveTexture(GL_TEXTURE11);
        break;
    case 12:
        glActiveTexture(GL_TEXTURE12);
        break;
    case 13:
        glActiveTexture(GL_TEXTURE13);
        break;
    case 14:
        glActiveTexture(GL_TEXTURE14);
        break;
    case 15:
        glActiveTexture(GL_TEXTURE15);
        break;
    case 16:
        glActiveTexture(GL_TEXTURE16);
        break;
    case 17:
        glActiveTexture(GL_TEXTURE17);
        break;
    case 18:
        glActiveTexture(GL_TEXTURE18);
        break;
    case 19:
        glActiveTexture(GL_TEXTURE19);
        break;
    case 20:
        glActiveTexture(GL_TEXTURE20);
        break;
    case 21:
        glActiveTexture(GL_TEXTURE21);
        break;
    case 22:
        glActiveTexture(GL_TEXTURE22);
        break;
    case 23:
        glActiveTexture(GL_TEXTURE23);
        break;
    case 24:
        glActiveTexture(GL_TEXTURE24);
        break;
    case 25:
        glActiveTexture(GL_TEXTURE25);
        break;
    case 26:
        glActiveTexture(GL_TEXTURE26);
        break;
    case 27:
        glActiveTexture(GL_TEXTURE27);
        break;
    case 28:
        glActiveTexture(GL_TEXTURE28);
        break;
    case 29:
        glActiveTexture(GL_TEXTURE29);
        break;
    case 30:
        glActiveTexture(GL_TEXTURE30);
        break;
    case 31:
        glActiveTexture(GL_TEXTURE31);
        break;
    default:
        break;
    }
}

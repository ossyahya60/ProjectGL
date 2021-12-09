#ifndef SCENE_H
#define SCENE_H

#include <entt/entt.hpp>

class Scene
{
private:
	entt::registry reg;
public:
	std::string name;

	entt::entity createEntity()
	{
		//create an entity
		//assign a transform
		//initialize tag and name
	}

	friend class Entity;
};

#endif


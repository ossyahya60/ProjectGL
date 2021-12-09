#ifndef ENTITY_H
#define ENTITY_H

#include <entt/entt.hpp>
#include "Scene.h"

class Entity
{
private:
	Scene* scene = nullptr;
public:
	entt::entity id;
	std::string name;
	std::string tag;

	Entity();
	~Entity();

	template <typename T, typename... Args>
	T& addComponent(Args&&... args)
	{
		if (!hasComponent<T>())
			scene->reg.emplace<T>(id, std::forward<Args>(args)...);
		else //raise exception?
			return nullptr;
	}

	template <typename T>
	int removeComponent() //retruns number of components removed
	{
		return scene->reg.remove<T>(id);
	}

	template <typename T>
	bool hasComponent()
	{
		return scene->reg.all_of<T>(id);
	}

	template <typename T>
	T& getComponent()
	{
		if (hasComponent<T>())
			scene->reg.get<T>(id);
		else //raise exception?
			return nullptr;
	}

	void destroy()
	{
		scene->reg.destroy(id);
	}
};

#endif


#pragma once

#include "Globals.h"
#include "Module.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>


class PhysBody;
class PhysicEntity;
class UIElement;

class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();
	void move();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void OnUIMouseClickEvent(UIElement* element);

	PhysicEntity* playerCar = nullptr;

public:
	std::vector<PhysicEntity*> entities;

	PhysBody* sensor;
	bool sensed;

	Texture2D circle;
	Texture2D box;

	vec2<int> ray;
	bool ray_on;	
};

#pragma once

#include "Globals.h"
#include "Module.h"
#include "ModulePhysics.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

#include "PhysicEntity.h"
#include "UIElement.h"

class PhysBody;

class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void OnUIMouseClickEvent(UIElement* element);

	PhysicEntity* playerCar = nullptr;

public:
	std::vector<PhysicEntity*> entities;
	b2Body* bodyCar;
	b2Vec2 positionCar;

	b2Vec2 posInit = { 1800, 670 };
	bool mapLoad = false;

	PhysBody* sensor;
	bool sensed;

	vec2<int> ray;
	bool ray_on;	
};

#pragma once
#include "PhysicEntity.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"


class IACar : public PhysicEntity
{
public:
	IACar(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, Texture2D _wheeltexture)
		: PhysicEntity(physics->CreateCar(_x, _y, BodyType::IACAR), _listener), texture(_texture), wheel(_wheeltexture), physics(physics) {}

	void Update() override;
	void Draw() override;
	int RayHit(vec2<int> ray, vec2<int> pos, vec2<float>& normal) override;

public:
	std::vector<b2Vec2> waypoints;
	int currentWaypoint = 0;
	float laneOffset = 0.0f; //To separate the cars      

	float stuckTimer = 0.0f;
	bool isStuck = false;

private:
	Texture2D texture;
	Texture2D wheel;
	ModulePhysics* physics;
};
#pragma once
#include "p2Point.h"
#include "ModulePhysics.h"

class PhysicEntity
{
protected:
	PhysicEntity(PhysBody* _body, Module* _listener)
		: body(_body)
		, listener(_listener)
	{
		body->listener = listener;
		app = _listener->App;
	}

public:
	virtual ~PhysicEntity() = default;
	virtual void Update() = 0;
	virtual void Draw() = 0;

	virtual void DeletePhysBody() = 0;

	virtual int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal)
	{
		return 0;
	}

public:
	Application* app;
	PhysBody* body;
	Module* listener;
};
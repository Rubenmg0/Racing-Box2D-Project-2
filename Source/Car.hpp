#pragma once
#include "PhysicEntity.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"


class Car : public PhysicEntity
{
public:
	Car(ModulePhysics* physics, int _x, int _y, Module* _listener, Texture2D _texture, Texture2D _wheeltexture)
		: PhysicEntity(physics->CreateCar(_x, _y, BodyType::CAR), _listener), texture(_texture), wheel(_wheeltexture) {}

	void Update() override;
	void Draw() override;
	int RayHit(vec2<int> ray, vec2<int> mouse, vec2<float>& normal) override;


private:
	Texture2D texture;
	Texture2D wheel;
	ModulePhysics* physics;
};
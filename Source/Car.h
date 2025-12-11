#pragma once
#include "ModulePhysics.h"
#include "Module.h"
#include "Timer.h"
#include "ModuleMap.h"
#include <iostream>
#include <vector>

using namespace std;

enum class STATES
{
	DRIVING,
	DRIFTING,
	END_DRIFTING,
};
enum class TypeCar
{
	PLAYER,
	IA_1,
	IA_2,
	IA_3
};

class Carr
{
public:

	Carr(Application* app, bool start_enabled = true);
	~Carr();

	Carr(ModulePhysics* physics, int _x, int _y,  Module* _listener,TypeCar car)
	{
		switch (car)
		{
		case TypeCar::PLAYER:
			texture = LoadTexture("Assets/Cars/BurgerCar/burger_car.png");
			break;
		case TypeCar::IA_1:
			texture = LoadTexture("Assets/Cars/NormalCars/normal_car_cian.png");
			break;
		case TypeCar::IA_2:
			texture = LoadTexture("Assets/Cars/NormalCars/normal_car_yellow.png");
			break;
		case TypeCar::IA_3:
			texture = LoadTexture("Assets/Cars/NormalCars/normal_car_green.png");
			break;
		default:
			break;
		}

	}

	//CAR FUNCTIONS////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	float normalizeAngle(float angle);

	//FRICTION
	b2Vec2 GetFriction(b2Vec2 vec) const;
	void ApplyFriction();

	//MOVEMENT
	void MoveForward() ;
	void MoveBackwards() ;

	//GETTERS
	b2Vec2 GetAcceleration();
	float GetAngleOfVector(const b2Vec2& vec);

	//DRIFT ROTATION AND MOVEMENT
	float lerpAngle(float from, float to, float t);

	//SETTERS
	void SetAcceleration(b2Vec2 Acc) { Acceleration = Acc; }
	void SetFriction(float FrQuo) { FrQueficient = FrQuo; }
	void SetRotFriction(float RotFr) { RotFriction = RotFr; }

	//APPLY THE VECTOR
	void ApplyMovement();

	//ROAD LIMITS
	void RoadLimits();

public:
	
	//The vector that the Car will follow
	b2Vec2 mainVec;
	//The direction of the vector
	float direction = 0;

	//Car Acceleration
	b2Vec2 Acceleration;

	//Car Brake
	b2Vec2 brake = { 10,10};

	//Vector Lerps rotation
	float DriftLerp = 0.01;
	float EndDriftLerp = 0.05;
	float NormalLerp = 1;

	//
	float RotForce;

	//Car state
	STATES state = STATES::DRIVING;

	//Delta time
	float dt  = GetFrameTime();

private:

	//Texture
	Texture2D texture;

	//Frictions
	float FrQueficient = 0.2;
	float RotFriction = 0.1;

};

class Player : public Carr
{
public:

	//Player constructor
	Player();

	//Player functions

	void ApplyMovement();
	
	// Method to get the car's position
	Vector2 GetPosition();

	vector<b2Vec2> trackPoints;

};
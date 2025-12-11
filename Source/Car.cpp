#include "Car.h"

Carr::Carr(Application* app, bool start_enabled)
{
}

Carr::~Carr()
{
}


b2Vec2 Carr::GetFriction(b2Vec2 vec) const
{
	b2Vec2 Force = { 0,0 };

	//Check the velocity on the different axis
	if (vec.x > 0.1 || vec.x < 0.1)
	{
		Force.x = -vec.x * FrQueficient * dt;
	}
	if (vec.y > 0.1 || vec.y < -0.1)
	{
		Force.y = -vec.y * FrQueficient * dt;
	}

	//Get the friction force
	return Force;
}
void Carr::ApplyFriction()
{
	//Apply the friction to the Car
	mainVec.x += GetFriction(mainVec).x;
	mainVec.y += GetFriction(mainVec).y;
}

void Carr::MoveForward() 
{
	//Accelerate the car
	mainVec.x += Acceleration.x*dt;
	mainVec.y += Acceleration.y*dt;
}
void Carr::MoveBackwards() 
{
	//Slow down the car
	mainVec.x -= brake.x*dt;
	mainVec.y -= brake.y*dt;
}

float Carr::GetAngleOfVector(const b2Vec2& vec)
{
	return atan2(vec.y, vec.x); // atan2 returns the angle in radians
}
b2Vec2 Carr::GetAcceleration()  
{ 
	return Acceleration; 
}

float Carr::lerpAngle(float from, float to, float t) {


	// Geth The short path, if not it rotattes wirrd
	float diff = fmod(to - from, 2 * PI);
	if (diff > PI) {
		diff -= 2 * PI;
	}

	//Return the lerp angle
	return fmod(from + diff * t, 2 * PI);
}




//void Car::ApplyMovement()
//{
//	dt = GetFrameTime();
//	//Apply the friction to the car
//	ApplyFriction();
//
//	//Check Road Limits
//	//RoadLimits();
//	
//	//get the angle of the car inside the 360 degree range
//	float angle = normalizeAngle(body->body->GetAngle());
//
//	switch (state)
//	{
//	case STATES::DRIVING:
//
//		direction = lerpAngle(GetAngleOfVector(body->GetVelocity()), angle, NormalLerp);
//		RotForce = 1;
//
//		break;
//	case STATES::DRIFTING:
//
//		direction = lerpAngle(GetAngleOfVector(body->GetVelocity()), angle, DriftLerp);
//		RotForce = 2;
//
//		break;
//	case STATES::END_DRIFTING:
//
//		direction = lerpAngle(GetAngleOfVector(body->GetVelocity()), angle, EndDriftLerp);
//
//		//This checks if the angles are as close together to be considered the same
//		if (std::fabs(GetAngleOfVector(body->GetVelocity()) - angle) <= EPS)
//		{
//			state = STATES::DRIVING;
//		}
//
//		RotForce = 1;
//
//		break;
//	default:
//		break;
//	}
//
//	//Set all the values to the car with a vector
//	b2Vec2 vector;
//	vector.x = cos(direction) * mainVec.x;
//	vector.y = sin(direction) * mainVec.y;
//	body->body->SetLinearVelocity(vector);
//}
//
//void Car::RoadLimits()
//{
//	if (body->entity->OnRoad) 
//	{
//		body->body->SetLinearDamping(0);
//	}
//	else 
//	{
//		body->body->SetLinearDamping(50);
//	}
//}
//
//
//
//float Car::normalizeAngle(float angle) {
//
//	float twoPi = 2.0f * PI; //==360 degrees
//
//	angle = fmod(angle, twoPi); //Thiis makes the angle be inside the 360 degree range
//
//	if (angle < 0) {
//		angle += twoPi;  //Then set the angle to the positives
//	}
//
//	//All of this is maded beacouse the rotation in Box2D is in radians and it goes over the 360 degrees, so when we are
//	//doing the lerp if this is not maded the car will rotate weirdly
//
//	return angle;
//}
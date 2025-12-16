#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"

#include "p2Point.h"

#include <math.h>

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// Needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureA()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}


PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, b2Vec2 initialVelocity, float mass)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = mass / (b2_pi * radius * radius);

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = pbody->height = radius;
	b->SetLinearVelocity(initialVelocity);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 0.0f;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	pbody->body = b;
	pbody->width = width;
	pbody->height = height;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, const int* points, int size)
{
	PhysBody* pbody = new PhysBody();

	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for (int i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	pbody->body = b;
	pbody->width = pbody->height = 0;

	return pbody;
}

PhysBody* ModulePhysics::CreateCar(int x, int y, int mass)
{
	//Create PhysBody
	PhysBody* pbody = new PhysBody();

	//Create Chasis
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.bullet = true;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.userData.pointer = reinterpret_cast<uintptr_t>(pbody);

	body.angularDamping = 3.0f;
	body.linearDamping = 0.3f;

	b2Body* chassis = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(carWidth/2), PIXEL_TO_METERS(carHeigh/2));

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = (float)mass / (carHeigh * carHeigh);	
	fixture.isSensor = false;

	chassis->CreateFixture(&fixture);

	//Physic Body variables
	pbody->body = chassis;
	pbody->width = carWidth;
	pbody->height = carHeigh;


	//Create Wheels
	b2Vec2 flWheelPos = { (float)x - carWidth / 2, (float)y - carHeigh / 2 }; //Front Left
	b2Vec2 frWheelPos = { (float)x + carWidth / 2, (float)y - carHeigh / 2 }; //Front Right
	b2Vec2 blWheelPos = { (float)x - carWidth / 2, (float)y + carHeigh / 2 }; //Back Left
	b2Vec2 brWheelPos = { (float)x + carWidth / 2, (float)y + carHeigh / 2 }; //Back Right

	b2Body* flWheel = CreateWheels(flWheelPos.x, flWheelPos.y);
	b2Body* frWheel = CreateWheels(frWheelPos.x, frWheelPos.y);
	b2Body* blWheel = CreateWheels(blWheelPos.x, blWheelPos.y);
	b2Body* brWheel = CreateWheels(brWheelPos.x, brWheelPos.y);

	//Save each wheel
	pbody->wheels[0] = flWheel;
	pbody->wheels[1] = frWheel; 
	pbody->wheels[2] = blWheel; 
	pbody->wheels[3] = brWheel; 

	//Create Joints
	b2RevoluteJointDef joint;
	joint.bodyA = chassis;

	joint.collideConnected = false;

	float halfWidthMeters = PIXEL_TO_METERS(carWidth / 2);
	float halfHeightMeters = PIXEL_TO_METERS(carHeigh / 2);

	joint.enableLimit = true;
	joint.lowerAngle = -45 * DEG2RAD; // -45 degrees in rads
	joint.upperAngle = 45 * DEG2RAD;  //  45 degrees in rads

	joint.enableMotor = true;
	joint.maxMotorTorque = 2.0f; // Max motor rotation speed

	// Front Left
	joint.localAnchorA.Set(-halfWidthMeters, -halfHeightMeters);
	joint.bodyB = flWheel; 
	pbody->motorJoints[0] = (b2RevoluteJoint*)world->CreateJoint(&joint);

	// Front Right
	joint.localAnchorA.Set(halfWidthMeters, -halfHeightMeters);
	joint.bodyB = frWheel;
	pbody->motorJoints[1] = (b2RevoluteJoint*)world->CreateJoint(&joint);

	//We block back wheels
	joint.enableLimit = true;
	joint.lowerAngle = 0;
	joint.upperAngle = 0;
	joint.enableMotor = false;

	// Back Left
	joint.localAnchorA.Set(-halfWidthMeters, halfHeightMeters);
	joint.bodyB = blWheel;
	world->CreateJoint(&joint);

	// Back Right
	joint.localAnchorA.Set(halfWidthMeters, halfHeightMeters);
	joint.bodyB = brWheel;
	world->CreateJoint(&joint);

	return pbody;
}

b2Body* ModulePhysics::CreateWheels(int x, int y)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angularDamping = 5.0f;
	body.linearDamping = 0.3f;


	b2Body* wheel = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(carWidth / 8), PIXEL_TO_METERS(carHeigh / 8));

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.friction = 0.9f;
	fixture.filter.groupIndex = -1;

	wheel->CreateFixture(&fixture);

	return wheel;
}

void ModulePhysics::DeleteBody(PhysBody* body)
{
	world->DestroyBody(body->body);
}

update_status ModulePhysics::PostUpdate()
{
	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
	{
		return UPDATE_CONTINUE;
	}

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();

				DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{ 0, 0, 0, 128 });
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->m_count;
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), RED);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), GREEN);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), BLUE);
			}
			break;
			}
		}
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");
	b2Body* body = world->GetBodyList();
	while (body != nullptr){
		b2BodyUserData& data = body->GetUserData();
		PhysBody* pBody = (PhysBody*)data.pointer;
		if (pBody != nullptr){
			delete pBody;
		}
		body = body->GetNext();
	}
	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPhysicPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x);
	y = METERS_TO_PIXELS(pos.y);
}

float PhysBody::GetRotation() const
{
	return body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

void ModulePhysics::KillLateralVelocity(b2Body* body)
{
	// Get the right vector from the car's perspective
	b2Vec2 right = body->GetWorldVector(b2Vec2(1, 0));

	// Get the sideways velocity
	float lateralSpeed = b2Dot(body->GetLinearVelocity(), right);

	// Remove the sideways speed component
	b2Vec2 lateralVel = lateralSpeed * right;
	body->SetLinearVelocity(body->GetLinearVelocity() - lateralVel);
}

void ModulePhysics::MoveCar(PhysBody* car)
{
	// Check if playerCar exists
	if (car == nullptr) return;

	float acceleration = 0.15f;
	float steerSpeed = 1.5f; //w (rad/s)
	b2Vec2 forward = car->wheels[0]->GetWorldVector(b2Vec2(0, -1));

	//Turbo
	if (IsKeyDown(KEY_SPACE))
	{
		acceleration *= 10;
	}

	//Accelerate
	if (IsKeyDown(KEY_W))
	{
		car->body->ApplyForceToCenter(acceleration * forward, true);
	}
	//Slow Down
	else if (IsKeyDown(KEY_S))
	{
		car->body->ApplyForceToCenter(-acceleration * forward, true);
	}

	float steer = 0.0f;
	if (IsKeyDown(KEY_A))
	{
		steer = -steerSpeed;
	}
	else if (IsKeyDown(KEY_D))
	{
		steer = steerSpeed;
	}
	else if (car->motorJoints[0]->GetJointAngle() != 0)
	{
		steer = -car->motorJoints[0]->GetJointAngle() * 3;
	}

	car->motorJoints[0]->SetMotorSpeed(steer);
	car->motorJoints[1]->SetMotorSpeed(steer);
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = (float)(x2 - x1);
			float fy = (float)(y2 - y1);
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return (int)(output.fraction * dist);
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}
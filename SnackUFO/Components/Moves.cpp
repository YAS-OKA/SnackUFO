#include "../stdafx.h"
#include "Moves.h"

using namespace snk;

void BaseMove::reset()
{
	vel = init_vel;
	acc = init_acc;
}

void BaseMove::start()
{
	transform = GetComponent<Transform>();
}

void BaseMove::update(double dt)
{
	transform->moveBy(vel * dt);
	vel += acc * dt;
}

Vec2 BaseMove::get_vel()
{
	return vel;
}

Vec2 BaseMove::get_acc()
{
	return acc;
}

void BaseMove::set_vel(const Vec2& v)
{
	vel = v;
}

void BaseMove::set_acc(const Vec2& a)
{
	acc = a;
}

void StandardMove::start_action()
{
	reset();
	active = true;
}

void StandardMove::end_action()
{
	active = false;
}

bool StandardMove::is_working()
{
	return active;
}

void StandardMove::update(double dt)
{
	if (not active)return;

	time += dt;

	if (maxSpeed and vel.length() > *maxSpeed)vel.setLength(*maxSpeed);

	BaseMove::update(dt);
}

void StandardMove::reset()
{
	BaseMove::reset();
	active = false;
	time = 0;
}

double StandardMove::get_time()
{
	return time;
}

void FuncMove::update(double dt)
{
	vel = Vt(dt, this);
	acc = At(dt, this);
	StandardMove::update(dt);
}

//void MixedMove::start_action()
//{
//	StandardMove::start_action();
//	for (auto& move : moves)
//	{
//		move->start_action();
//		vel += move->get_vel();
//		acc += move->get_acc();
//	}
//	
//}
//
//void MixedMove::update(double dt)
//{
//}

DecelerateMove::DecelerateMove(double staticFriction)
	:staticFriction(staticFriction)
{
	
}

void DecelerateMove::update(double dt)
{
	if (transform->getVel().length() < staticFriction*dt)acc = -transform->getVel();
	else acc = transform->getVel().setLength(-staticFriction);
	StandardMove::update(dt);
}

AdvanceMove::AdvanceMove(double speed)
	:speed(speed)
{

}

void AdvanceMove::update(double dt)
{
	vel = speed * Vec2{ Cos(transform->getDirection()),Sin(transform->getDirection()) };
	StandardMove::update(dt);
}

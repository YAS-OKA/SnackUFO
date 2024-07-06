#include "../stdafx.h"
#include "GameAction.h"
#include"../GameObject/Gun.h"

using namespace snk;

Wait::Wait(double t, double timeScale, double updatePriority, double drawPriority, const String& id)
	: Action(t, timeScale, updatePriority, drawPriority, id)
{
}

Wait::Wait(Action* waitTarget)
	:Action(new prg::FuncCondition([=]() {return waitTarget->getEnded(); }))
{
}

GunShot::GunShot(IGun* gun, double t, double timeScale, double updatePriority, double drawPriority, const String& id)
	:gun(gun), Action(t, timeScale, updatePriority, drawPriority, id)
{

}

void GunShot::update(double dt)
{
	Action::update(dt);
	gun->shot();
}

//void FuncDraw::update(double dt)
//{
//	Action::update(dt);
//	FuncDraw::dt = dt;
//	updateFunction(this);
//}
//
//void FuncDraw::draw()const
//{
//	if (visible)drawFunction(this);
//}

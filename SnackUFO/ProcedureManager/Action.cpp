#include "../stdafx.h"
#include "Action.h"
#include"Condition.h"
#include"Actions.h"

using namespace prg;

Action::Action(double t, double timeScale,double updatePriority, double drawPriority,const String& id)
	:startCondition(new Condition()), timeScale(timeScale), updatePriority(updatePriority), drawPriority(drawPriority)
{
	time = 0;
	setTime(t);
}

Action::Action(Condition* end, double timeScale, double updatePriority, double drawPriority, const String& id)
	:startCondition(new Condition()), endCondition(end), timeScale(timeScale), updatePriority(updatePriority), drawPriority(drawPriority)
{
	time = 0;
}

Action::Action(Condition* start, Condition* end, double timeScale, double updatePriority, double drawPriority, const String& id)
	:startCondition(start),endCondition(end),timeScale(timeScale), updatePriority(updatePriority), drawPriority(drawPriority)
{
	time = 0;
}

Action::~Action()
{
	if (startCondition != nullptr) delete startCondition;
	if (endCondition != nullptr) delete endCondition;
}

bool Action::getStarted()
{
	return started;
}

void Action::setTime(double t)
{
	if (endCondition != nullptr)delete endCondition;
	endCondition = new FuncCondition([=]() {return time >= t; });
}

bool Action::getEnded()
{
	return ended;
}

void Action::update(double dt)
{
	if (startCondition == nullptr)startCondition = new Condition();
	if (endCondition == nullptr)endCondition = new Condition();

	time += dt * timeScale;
};

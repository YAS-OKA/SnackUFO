#include "../stdafx.h"
#include "Actions.h"
#include"Condition.h"
#include"../Components/Progress.h"

using namespace prg;

ManageActions::ManageActions(double timeScale, double updatePriority, double drawPriority, const String& id)
	:Action(new Condition(), new FuncCondition([=]() {return isAllFinished(); }), timeScale, updatePriority, drawPriority, id)
{
	time = 0;	
}

ManageActions::ManageActions(Condition* start, Condition* end, double timeScale, double updatePriority, double drawPriority,const String& id)
	:Action(start, end, timeScale, updatePriority, drawPriority, id)
{
	time = 0;
}

ManageActions::~ManageActions()
{
	for (auto& action : update_list)delete action;
}

void ManageActions::setStartCondition(Condition* condition)
{
	if (startCondition != nullptr)delete startCondition;
	startCondition = condition;
}

void ManageActions::setEndCondition(Condition* condition)
{
	if (endCondition != nullptr)delete endCondition;
	endCondition = condition;
}
//他のオーバーロードは結局このaddを呼び出してる
Action* ManageActions::add(Action* action, Condition* start, Condition* end)
{
	if (action->startCondition != nullptr and action->endCondition != start)delete action->startCondition;
	if (action->endCondition != nullptr and action->endCondition != end)delete action->endCondition;
	action->startCondition = start;
	action->endCondition = end;

	endTimeList[action] = none;
	update_list << action;
	draw_list << action;

	activeNum++;

	return action;
}

Action* ManageActions::add(Action* action, double start_time, Condition* end)
{
	return add(action, new FuncCondition([this, start_time]() {return time >= start_time; }), end);
}

Action* ManageActions::add(Action* action, Condition* start, double time)
{
	return add(action, start, new FuncCondition([time, action]() {return action->time > time; }));
}

Action* ManageActions::add(Action* action, double start_time, double time)
{
	return add(action,
		new FuncCondition([this, start_time]() {return ManageActions::time >= start_time; }),
		new FuncCondition([time, action]() {return action->time > time; })
	);
}

Action* ManageActions::add(Action* action, double offsetTime)
{
	if (update_list.isEmpty())
	{
		return add(action, offsetTime,action->endCondition);
	}

	Action* preAction = update_list.back();
	return add(action, new FuncCondition([this,preAction, offsetTime]() {
		return preAction->getEnded() and (*endTimeList[preAction] >= offsetTime);})
		, action->endCondition);
}

Action* ManageActions::add(Action* action,Action* preAction, double offsetTime)
{
	if (not endTimeList.contains(preAction))
	{
		throw Error{ U"preActionがありません" };
	}

	return add(action, new FuncCondition([this, preAction, offsetTime]() {
		return preAction->getEnded() and (*endTimeList[preAction] >= offsetTime); })
		, action->endCondition);
}

Action* ManageActions::addParallel(Action* action, double offsetTime)
{
	if (update_list.isEmpty())
	{
		return add(action, offsetTime, action->endCondition);
	}

	Action* preAction = update_list.back();
	return add(action, new FuncCondition([this, preAction, offsetTime]() {
		double additionalTime = endTimeList[preAction].has_value() ? *endTimeList[preAction] : 0;
		return preAction->getStarted() and (preAction->time + additionalTime >= offsetTime);
		}), action->endCondition);
}

Action* ManageActions::addParallel(Action* action,Action* preAction, double offsetTime)
{
	if (not endTimeList.contains(preAction))
	{
		throw Error{ U"preActionがありません" };
	}

	return add(action, new FuncCondition([this, preAction, offsetTime]() {
		double additionalTime = endTimeList[preAction].has_value() ? *endTimeList[preAction] : 0;
		return preAction->getStarted() and (preAction->time + additionalTime >= offsetTime);
		}), action->endCondition);
}

Action* ManageActions::getAction(const String& id)
{
	for (auto& action : update_list)if (action->id == id)return action;
	//見つからなければ
	return nullptr;
}

int32 ManageActions::getLoopCount()const
{
	return loopCount;
}

bool ManageActions::isActive()
{
	return started and (not ended);
}

void ManageActions::restart()
{
	reset();
	start();
}

void ManageActions::start()
{
	Action::start();
	//最初のアクションをスタートさせる
	for (auto& action : update_list)
	{
		if ((not action->started) and (not action->ended) and action->startCondition->check())
		{
			action->start();
		}
	}
}

void ManageActions::reset()
{
	Action::reset();
	activeNum = update_list.size();
	loopCount = 0;
	for (auto& action : update_list)
	{
		action->reset();
		endTimeList[action] = none;
	}
}

bool ManageActions::isAllFinished()
{
	return activeNum == 0;
}

void ManageActions::end()
{
	Action::end();
	//実行中のアクションは終了させる
	for (auto& action : update_list)
	{
		if (action->getStarted() and not action->getEnded())action->end();
	}

	if (loop)
	{
		int32 temp = ++loopCount;
		reset();
		start();
		loopCount = Min(temp, maxLoopCount);
		return;
	}
	if (init)reset();
}

void ManageActions::update(double dt)
{
	if (stopped)return;

	dt *= timeScale;
	Action::update(dt);
	//優先度でソート
	std::sort(update_list.begin(), update_list.end(), [this](const Action* ac1, const Action* ac2) {
		return ac1->updatePriority < ac2->updatePriority;
	});
	std::sort(draw_list.begin(), draw_list.end(), [this](const Action* ac1, const Action* ac2) {
		return ac1->drawPriority > ac2->drawPriority;
	});

	for (auto& endTime : endTimeList)if (endTime.second.has_value()) *endTime.second += dt;
	
	for (auto& action : update_list)
	{
		if ((not action->started) and (not action->ended) and action->startCondition->check())
		{
			action->start();
		}

		if ((not action->started) or action->ended)continue;

		action->update(dt);

		if (action->endCondition->check())
		{
			action->end();//actionがループアクションの場合ここでgetEndedがfalseにならないので下の処理は行わない。
			if (action->getEnded())
			{
				endTimeList[action] = 0;
				activeNum--;
			}
		}
	}
}

void ManageActions::draw()const
{
	if (stopped)return;

	for (const auto& action : draw_list)
	{
		if ((not action->started) or action->ended)continue;
		action->draw();
	}
}


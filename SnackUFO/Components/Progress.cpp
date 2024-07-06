#include "../stdafx.h"
#include "Progress.h"

using namespace snk;

void Progress::startAllAction(HashSet<String> exept)
{
	for (auto& action : actions)for (auto& act : action.second)if (not exept.contains(action.first))act->start();
}

void Progress::stopAllAction(HashSet<String> exept)
{
	for (auto& action : actions)for (auto& act : action.second)if (not exept.contains(action.first))act->stopped = true;
}

void Progress::cancelAllActionStopped()
{
	for (auto& action : actions)for (auto& act : action.second)act->stopped = false;
}

void Progress::resetAllAction(HashSet<String> exept)
{
	for (auto& action : actions)for (auto& act : action.second)if(not exept.contains(action.first))act->reset();
}

void Progress::update(double dt)
{
	for (auto& action : actions)
	{
		for (auto& act : action.second) {
			//スタート
			if ((not act->started) and (not act->ended) and act->startCondition->check())act->start();
			//アップデート
			if (act->started and (not act->ended))
			{
				act->update(dt);
				//エンド
				if (act->isAllFinished() or act->endCondition->check())act->end();
			}
		}
	}
}

prg::ManageActions* Progress::setAction(prg::ManageActions* action, const String& actName)
{
	if (actions.contains(actName)) {
		actions[actName].clear();
		actions[actName] << std::unique_ptr<prg::ManageActions>(action);
	}
	else {
		actions[actName] << std::unique_ptr<prg::ManageActions>(action);
	}
	return action;
}

prg::ManageActions* Progress::addAction(prg::ManageActions* action, const String& actName)
{
	actions[actName] << std::unique_ptr<prg::ManageActions>(action);

	return action;
}

prg::ManageActions* Progress::getAction(const String& actName) const
{
	return actions.at(actName)[0].get();
}

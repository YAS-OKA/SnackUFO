#include "../stdafx.h"
#include"Collider.h"
#include "HitCallback.h"

using namespace snk;

void HitCallback::start()
{
	mainObject = dynamic_cast<Object*>(Parent);
	manager = mainObject->objectManager;
}

void HitCallback::registerCallback(const ObjectTag& tag, const HitMethod& method)
{
	if (callbacks.contains(tag))callbacks[tag] = method;
	else callbacks.emplace(tag, method);
	//最初に登録した場合はenableをtrueにする
	if (not enable.contains(tag))enable.emplace(tag, true);
}

void HitCallback::registerCallback(HashSet<ObjectTag> tags, const HitMethod& method)
{
	for (auto& tag : tags)registerCallback(tag, method);
}

void HitCallback::allHitMethodEnabled()
{
	for (auto& e : enable)
	{
		e.second = true;
	}
}

void HitCallback::allHitMethodStoped()
{
	for (auto& e : enable)
	{
		e.second = false;
	}
}

void HitCallback::check()
{
	if (mainObject->GetComponent<Collider>() == nullptr)return;
	auto collider = mainObject->GetComponent<Collider>();
	//ここはほぼ一重ループ?
	for (auto& [targetTag, method] : callbacks) {
		//相手の衝突を判定するかどうか
		if (not enable[targetTag]) continue;

		for (auto& other : manager->getGameObjects(targetTag))
		{
			auto otherCollider = other->GetComponent<Collider>();
			if (otherCollider != nullptr and collider->intersects(*otherCollider))
			{
				method(mainObject, other);
			}
		}
	}
}

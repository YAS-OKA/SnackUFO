#include"../stdafx.h"
#include"MakeObject.h"
#include"../GameObject/Bullet.h"
#include"HitCallback.h"
using namespace snk;

void ObjectBehavior::start()
{
	MainObject = dynamic_cast<Object*>(Parent);
	transform = MainObject->transform;
	progress = GetComponent<Progress>();
	hitCallback = GetComponent<HitCallback>();
};

void ObjectBehavior::makeParent(const ObjectBehavior* parent)
{
	MainObject->makeParent(parent->MainObject);
}

void ObjectBehavior::makeChiled(ObjectBehavior* chiled)const
{
	MainObject->makeChiled(chiled->MainObject);
}

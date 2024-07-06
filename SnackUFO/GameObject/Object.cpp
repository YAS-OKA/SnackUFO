#include "../stdafx.h"
#include "Object.h"
#include"../GameUtils.h"
#include"../ProcedureManager/Init.h"
#include"../Components/HitCallback.h"
#include"Bullet.h"

using namespace snk;

Object::Object(GameObjectManager* manager)
	:objectManager(manager)
{
	tag = ObjectTag::none;
	parent = nullptr;
	objectManager->addGameObject(this);
	transform = AddComponentImmediately<Transform>();
	AddComponentImmediately<Progress>();
	AddComponentImmediately<HitCallback>();
	erase = false;
}
//
//void Object::affectedChildren()
//{	
//	for (auto& child : children)
//	{
//		child->transform->moveBy(transform->getDelta());
//		child->transform->rotateAt(transform->getPos(), transform->getDeltaDirection());
//		child->affectedChildren();
//	}
//}

void Object::makeParent(Object* p)
{
	p->makeChiled(this);
}

void Object::makeChiled(Object* c)
{
	children.emplace(c);
	c->parent = this;
}

void Object::relaseParent()
{
	if(parent!=nullptr) parent->relaseChild(this);
}

void Object::relaseChild(Object* obj)
{
	obj->parent = nullptr;
	children.erase(obj);
}

void Object::relaseChildren()
{
	for (auto it = children.begin(); it != children.end();)
	{
		(*it)->parent = nullptr;
		it = children.erase(it);
	}
}

void Object::eraseThis()
{
	erase = true;
	relaseChildren();
	relaseParent();
}

bool Object::getEraseFlag()
{
	return erase;
}

bool Object::hasParent()
{
	return parent != nullptr;
}

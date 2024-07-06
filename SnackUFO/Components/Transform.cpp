#include "../stdafx.h"
#include "Transform.h"
#include"../GameObject/Object.h"

using namespace snk;

Transform::Transform()
{
	preFramePos=vel=deltaPos=pos = prePos = { 0,0 };
	preFrameDir = deltaDirection = preDirection = direction = 0;
	objParent = nullptr;
}

void Transform::start()
{
	priority.setUpdate(Math::Inf);
	objParent = dynamic_cast<Object*>(Parent);
};

void Transform::setDirection(double dir)
{
	direction = dir;
	affectChildren();
}

void Transform::setDirAndPreDir(double dir)
{
	direction = preDirection = dir;
	preFrameDir = dir;
}

void Transform::setPosAndPrePos(const Vec2& p)
{
	pos = prePos = p;
	preFramePos = p;
}

void Transform::setPos(const Vec2& p)
{
	pos = p;
	affectChildren();
}

Vec2 Transform::getPos()const
{
	return pos;
}

Vec2 Transform::getVel()const
{
	return vel;
}

double Transform::getDirection()const
{
	return direction;
}

double Transform::getAngulerVel()const
{
	return angularVel;
}

void Transform::moveBy(const Vec2& delta)
{
	pos += delta;
	affectChildren();
}

void Transform::rotateAt(const Vec2& center, double rad)
{
	pos.rotateAt(center, rad);
	direction += rad;
	affectChildren();
}

void Transform::rotate(double rad)
{
	direction += rad;
	affectChildren();
}

void Transform::affectChildren()
{
	calculate();
	for (auto& child : objParent->children)
	{
		if (not child->followParent)continue;
		child->transform->moveBy(deltaPos);
		child->transform->rotateAt(pos, deltaDirection);
		child->transform->affectChildren();
	}
}

void Transform::calculate()
{
	deltaPos = pos - prePos;
	prePos = pos;
	deltaDirection = direction - preDirection;
	preDirection = direction;
}

void Transform::calUpdate(double dt)
{
	if (dt == 0)return;

	vel = (pos - preFramePos) / dt;
	angularVel = (direction - preFrameDir) / dt;
	preFramePos = pos;
	preFrameDir = direction;
}

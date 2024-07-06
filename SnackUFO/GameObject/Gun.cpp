#include "../stdafx.h"
#include "Gun.h"
#include"Bullet.h"
#include"../Components/Collider.h"
#include"../Components/Moves.h"

using namespace snk;

void IGun::eraseAllGuns()
{
	for (auto& gun : otherGuns)
	{
		gun->eraseAllGuns();
	}
	MainObject->eraseThis();
}

Array<Bullet*> IGun::getBullets()const
{
	Array<Bullet*>bulletList = bullets;

	for (const auto& gun : otherGuns)
	{
		bulletList.append(gun->getBullets());
	}

	return bulletList;
}

void IGun::shotOtherGun()
{
	for (auto& gun : otherGuns)
	{
		gun->shot();
	}
}

TouchUp::TouchUp()
{
	shotFunction = none;
}

void TouchUp::reset()
{
	shotFunction = none;
}

void TouchUp::operator =(const TouchUpBullet& f)
{
	reset();
	shotFunction = f;
}

TouchUpBullet TouchUp::operator +(const TouchUpBullet& f)
{
	TouchUpBullet func;
	if (shotFunction.has_value())
	{
		auto temp = *shotFunction;
		func = [=](IGun* gun, Bullet* b) {
				//入れ子にする
				return f(gun, temp(gun, b));
			};
		//shotFunction = [=](IGun* gun, Bullet* b) {
		//	//入れ子にする
		//	return f(gun, (*shotFunction)(gun, b));
		//};
	}
	else
	{
		func = f;
	}
	return func;
}

void TouchUp::operator +=(const TouchUpBullet& f)
{
	shotFunction = *this + f;
}

PrimitiveGun::~PrimitiveGun()
{
	for (auto& bullet : bullets)bullet->gun = nullptr;
}

void PrimitiveGun::shot()
{
	//球を発射
	Bullet* b=nullptr;
	if (touch.shotFunction.has_value())
		b=(*touch.shotFunction)(this, shotFunction(this, MainObject->objectManager));
	else
		b=shotFunction(this, MainObject->objectManager);
	b->gun = this;
	bullets << b;
}

void AutoAimingGun::update(double dt)
{
	if (target != nullptr) {
		if (target->getEraseFlag()) {
			target = nullptr;
		}
		else {
			transform->setDirection((target->GetComponent<Transform>()->getPos() - transform->getPos()).getAngle()-90_deg);
			for (auto& gun : otherGuns)gun->transform->setDirection((target->GetComponent<Transform>()->getPos() - gun->transform->getPos()).getAngle()-90_deg);
		}
	}
	PrimitiveGun::update(dt);
}

void RadiateGun::addArm(IGun* gun, double dir, double distance)
{
	gun->transform->setDirAndPreDir(dir);
	gun->transform->setPos(transform->getPos() + distance * Vec2{ Cos(dir),Sin(dir) });
	addGun(gun);
}

void RadiateGun::update(double dt)
{
	IGun::update(dt);
	for (auto& gun : otherGuns)
	{
		if(gun->transform->getPos() - transform->getPos()!=Vec2{0,0})gun->transform->setDirection((gun->transform->getPos() - transform->getPos()).getAngle() - 90_deg);
	}
}

void RadiateGun::shot()
{
	for (auto& gun : otherGuns)
	{
		if (gun->transform->getPos() - transform->getPos() != Vec2{ 0,0 })gun->transform->setDirection((gun->transform->getPos() - transform->getPos()).getAngle() - 90_deg);
		gun->shot();
	}
}

void RadiateGun::shot(int32 k)
{
	auto& gun = otherGuns[k];
	if (gun->transform->getPos() - transform->getPos() != Vec2{ 0,0 })gun->transform->setDirection((gun->transform->getPos() - transform->getPos()).getAngle() - 90_deg);
	gun->shot();
}

void RegulerRadiateGun::start()
{
	IGun::start();
	radiate_gun = AddComponentImmediately<RadiateGun>();
	otherGuns << radiate_gun;
}

void RegulerRadiateGun::addArm(IGun* gun, double distance)
{
	radiate_gun->addArm(gun, 0, distance);
	regulered();
}

void RegulerRadiateGun::update(double dt)
{
	IGun::update(dt);
	regulered();
}

void RegulerRadiateGun::shot()
{
	regulered();
	for (auto& gun : radiate_gun->otherGuns)
	{
		gun->shot();
	}
}

void RegulerRadiateGun::regulered()
{
	if (symmetrical) {
		double direction = MainObject->transform->getDirection() - range / 2;
		for (auto& gun : radiate_gun->otherGuns)
		{
			if (radiate_gun->otherGuns.size() == 1)direction += range / 2;
			double dis = (radiate_gun->transform->getPos() - gun->transform->getPos()).length();
			gun->transform->setPos(
				radiate_gun->transform->getPos() + dis * Vec2{ Cos(direction),Sin(direction) }
			);
			gun->transform->setDirection(direction);
			if (radiate_gun->otherGuns.size() != 1)direction += range / (radiate_gun->otherGuns.size() - 1);
		}
	}
	else {
		double direction = MainObject->transform->getDirection() - range / 2;
		for (auto& gun : radiate_gun->otherGuns)
		{
			double dis = (radiate_gun->transform->getPos() - gun->transform->getPos()).length();
			gun->transform->setPos(
				radiate_gun->transform->getPos() + dis * Vec2{ Cos(direction),Sin(direction) }
			);
			gun->transform->setDirection(direction);
			direction += range / (radiate_gun->otherGuns.size());
		}
	}
}

void RegulerRadiateGun::shot(int32 k)
{
	radiate_gun->shot(k);
}

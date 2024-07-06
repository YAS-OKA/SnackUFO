#include "../stdafx.h"
#include "Collider.h"
#include"../GameObject/Object.h"

using namespace snk;

void Collider::set_hitbox(const Figure& figure,const Vec2& relative)
{
	hitbox = figure;
	pos = relative;
	hitbox.moveBy(transform->getPos() - figure.center() + pos);
}

void Collider::setScale(double scale)
{
	hitbox.scaleAt(transform->getPos() + pos, 1/Collider::scale);//元の大きさに戻す
	hitbox.scaleAt(transform->getPos() + pos, scale);
	Collider::scale = scale;
}

void Collider::addScale(double ds)
{
	setScale(scale + ds);
}

double Collider::getScale()const
{
	return scale;
}

Figure Collider::getHitbox()const
{
	return hitbox;
}

void Collider::start()
{
	transform = GetComponent<snk::Transform>();
}

bool Collider::intersects(const Collider& other)const
{
	if (not active)return false;

	return other.intersects(hitbox);
}

bool Collider::intersects(const Figure& other)const
{
	if (not active)return false;

	return other.intersects(hitbox);
}

void Collider::update(double dt)
{
	hitbox.setCenter(transform->getPos()+pos);
	//hitbox.moveBy(transform->getVel()*dt);
	hitbox.rotateAt(hitbox.center()+rotateCenter, transform->getAngulerVel()*dt);
}

void Collider::draw()const
{
	if (draw_hitbox)
	{
		hitbox.draw(color);
	}
	if (draw_hitbox_outline)
	{
		hitbox.drawFrame(1.0,outline_color);
	}
}

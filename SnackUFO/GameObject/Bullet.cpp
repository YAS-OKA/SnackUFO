#include "../stdafx.h"
#include "Bullet.h"
#include"../Components/Collider.h"
#include"Object.h"
#include"../Components/Moves.h"
#include"Gun.h"
#include"../Components/HitCallback.h"
#include"../Barrages.h"
//#include"../../Components/TextureDraw.h"

using namespace snk;

BulletElements BulletElements::setting(const ObjectTag& t, HashSet<ObjectTag> targetT, double d, const Figure& hitbox)
{
	tag = t;
	targetTags = targetT;
	damage = d;
	setFigure(hitbox);
	return *this;
}

void BulletElements::setFigure(const Figure& fig,const Vec2& p)
{
	figure=fig;
	if (bullet != nullptr)bullet->GetComponent<Collider>()->set_hitbox(fig,p);
}

Figure BulletElements::getFigure()const
{
	return figure;
}

Bullet::~Bullet()
{
	if (gun != nullptr)gun->bullets.remove(this);
}

void Bullet::start()
{
	ObjectBehavior::start();
	setElements(elems);
	MainObject->tag = elems.tag;
	//	消滅するアクションをセット
	util::setDissapearAction(this, 1.2, 0.4);
}

void Bullet::update(double dt)
{
	//	フィールドの外に出たら消去
	if(not GetComponent<Collider>()->getHitbox().intersects(MainObject->objectManager->getGameObjects(ObjectTag::bulletField)[0]->GetComponent<Collider>()->getHitbox()))
		MainObject->eraseThis();
}

void Bullet::setHitbox(const Figure& fig,const Vec2& p)
{
	elems.setFigure(fig,p);
}

void Bullet::setElements(BulletElements elems)
{
	if (elems.bullet == nullptr)elems.bullet = this;
	MainObject->tag = elems.tag;
	if (GetComponent<Collider>() == nullptr) AddComponentImmediately<Collider>()->set_hitbox(elems.getFigure());
	else GetComponent<Collider>()->set_hitbox(elems.getFigure());
	Bullet::elems = elems;
	//ヒットコールバックの設定
	hitCallback->registerCallback(elems.targetTags, sc::hitCallbackRegistring().get(U"disappear"));
}

BulletElements* Bullet::getPElems()
{
	return &elems;
}

BulletElements Bullet::getElements()const
{
	return elems;
}

void LenearLaser::start()
{
	bullet = GetComponent<Bullet>();
	bullet->MainObject->name = U"laser";
	firstPos = bullet->transform->getPos();
	baseDamage = bullet->elems.damage;
	//レーザーを縮める
	bullet->hitCallback->registerCallback(bullet->elems.targetTags,
		[=](Object* self,Object* other){
			auto otherHitbox=other->GetComponent<Collider>();
			auto hitbox = self->GetComponent<Collider>();
			auto points = otherHitbox->getHitbox().intersectsAt(hitbox->getHitbox());
			if (not points)return;
			Vec2 nearPoint{ Math::Inf,0 };
			Vec2 rightPoint = self->GetComponent<LenearLaser>()->firstPos;
			for (const auto& point : *points)
			{
				//もしnearPointよりpointのほうが近ければ
				if ((nearPoint - rightPoint).length() > (point - rightPoint).length())
					nearPoint = point;
			}
			self->GetComponent<LenearLaser>()->disLength = (self->transform->getPos() - nearPoint).length();
		});
}

void LenearLaser::update(double dt)
{
	bullet->elems.damage = baseDamage * dt;

	auto vec = firstPos - bullet->transform->getPos();
	//長さが0いかになったら消去
	if (vec.length() - disLength < 0) {
		bullet->MainObject->eraseThis();
		return;
	}
	auto temp = vec;
	temp.setLength(vec.length() - disLength);
	bullet->setHitbox(RectF{ Arg::leftCenter(Vec2{disLength,0}),vec.length()-disLength,thickness}.rotatedAt(Vec2{0,0}, vec.getAngle() - Vec2{1,0}.getAngle()),vec-temp/2);

	if (not releaseFirstPos) {
		//このレーザーを放っている銃が消されたら
		if (gun->MainObject->getEraseFlag())releaseFirstPos = true;
		return;
	}

	firstPos += bullet->transform->getVel() * dt;
}

void LenearLaser::setThickness(double t)
{
	thickness = t;
}

prg::ManageActions* util::setDissapearAction(ObjectBehavior* behavior, double maxScale, double time)
{
	auto action = new prg::ManageActions;
	action->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act)
		{
			//拡大し透明化する
			behavior->GetComponent<DrawTexture>()->color.a = (time - act->time) / time;
			behavior->GetComponent<DrawTexture>()->scale = (maxScale - 1.0) / time * act->time + 1;
		},
		[=]
		{
			//動きを止める　めちゃくちゃなことしてる
			for (auto& [_, compTable] : behavior->MainObject->getAllComponents())
			{
				for (auto& [str, comp] : compTable)
				{
					auto a = dynamic_cast<StandardMove*>(comp);
					if (a != nullptr)a->end_action();
				}
			}
			//消滅アクション以外のアクションを停止させる
			behavior->progress->stopAllAction({ U"dissapearAction" });

			//当たり判定を無効
			if(behavior->GetComponent<Collider>()!=nullptr)behavior->GetComponent<Collider>()->active = false;
		},
			[=]
		{
			behavior->MainObject->eraseThis();
		}, time
	));

	behavior->progress->setAction(action,U"dissapearAction");

	return action;
}

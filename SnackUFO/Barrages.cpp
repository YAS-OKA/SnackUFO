#include "stdafx.h"
#include "Barrages.h"
#include"GameObject/Bullet.h"
#include"GameObject/Gun.h"
#include"GameObject/Actors.h"
#include"Components/Draw.h"
#include"Components/Collider.h"

using namespace snk;

using Gpm = GameParameterManager;
using Gd = GameDataModel;

BulletElements sc::circleBullet(ObjectTag tag, double r,double damage)
{
	Circle c{ 0,0,r };
	switch (tag)
	{
	case ObjectTag::playerBullet:
		return BulletElements().setting(tag, { ObjectTag::enemy,ObjectTag::enemyBullet }, damage, c);
	case ObjectTag::enemyBullet:
		return BulletElements().setting(tag, { ObjectTag::player,ObjectTag::playerBullet }, damage, c);
	default:
		return BulletElements();
	}
}

double sc::aimDirection(GameObjectManager* manager,Object* obj, ObjectTag targetTag)
{
	double dir = 0;
	auto targets = manager->getGameObjects(targetTag);
	if (not targets.isEmpty())dir = (targets[0]->transform->getPos() - obj->transform->getPos()).getAngle() - 90_deg;
	return dir;
}

sc::hitCallbackRegistring::hitCallbackRegistring()
{
	//自分を消す
	callbackDist[U"disappear"] = [](Object* self, const Object* other)
	{
		auto progress = self->GetComponent<Bullet>()->progress;
		if (progress != nullptr)
		{
			progress->startAction(U"dissapearAction");
		}
	};
}

void sc::hitCallbackRegistring::set(HitCallback* callback, ObjectTag target, const String& callbackID)const
{
	if (not callbackDist.contains(callbackID))throw Error{ U"callbackID :" + callbackID + U" がありません" };
	callback->registerCallback(target, callbackDist.at(callbackID));
}

HitMethod sc::hitCallbackRegistring::get(const String& callbackID)const
{
	return callbackDist.at(callbackID);
}

prg::ManageActions* util::repeat(prg::ManageActions* action, int32 time)
{
	auto repeatedAction = new prg::ManageActions;

	repeatedAction->add(action);
	//指定した回数リスタートさせるアクション
	double count = 1;

	repeatedAction->add(new prg::FuncAction([=]()mutable
		{
			if (count < time)
			{
				repeatedAction->reset();
				repeatedAction->start();
			}
			else {
				count = 0;
			}
			++count;
		}
	));

	return repeatedAction;
}

prg::ManageActions* util::repeat(std::function<prg::ManageActions*()> buildFunction, int32 time)
{
	auto repeatedAction = new prg::ManageActions;

	for (const auto& k : step(time))
	{
		repeatedAction->add(buildFunction());
	}

	return repeatedAction;
}

Act util::rectRandomTransform(ObjectBehavior* actor,Vec2 pos, double width, double height,bool fit,double agility, double speedly, double widely, TransFunction ft)
{
	auto action = new prg::ManageActions();
	Gpm p = Gpm::createManager(U"common", U"GameScene");
	RectF rect{ p.params<Vec2>(U"mainFieldPos"),p.params<double>(U"mainFieldWidth"),p.params<double>(U"mainFieldHeight") };
	RectF range;
	if (fit) range = RectF{ Max(pos.x,p.params<Vec2>(U"mainFieldPos").x),Max(pos.y,p.params<Vec2>(U"mainFieldPos").y),	Min(width,p.params<Vec2>(U"mainFieldPos").x + p.params<double>(U"mainFieldWidth") - pos.x),Min(height,p.params<Vec2>(U"mainFieldPos").y + p.params<double>(U"mainFieldHeight") - pos.y) };
	else range = RectF{ pos,width,height };

	double span = 20 / agility; double transTime = 7.5 / speedly; double maxDistance = 300 * widely / 5;
	double time = span - transTime;
	Optional<Vec2> dist = none;
	Optional<Vec2> p0 = none;
	action->add(new prg::FuncAction([=](double dt,prg::FuncAction*)mutable
		{
			//range.draw(ColorF{ Palette::White,0.3});
			for (time += dt; time > transTime + span; time -= (transTime + span))
			{
				dist = RandomVec2(range);
				p0 = actor->transform->getPos();
				if ((*dist - *p0).length() > maxDistance)*dist = *p0 + (*dist - *p0).setLength(maxDistance);
			}
			if (time >= transTime and dist)actor->transform->setPos(*dist);
			else if (dist) actor->transform->setPos(ft(time / transTime, *p0, *dist - *p0));
		}, new prg::Condition()));

	return action;
}

Act util::rectRandomTransform(Object* actor, Vec2 rectCenterPos, double width, double height, double time, TransFunction ft)
{
	std::shared_ptr<Vec2> p0(new Vec2{});
	std::shared_ptr<Vec2> dist(new Vec2{});
	return new prg::FuncAction(
		[=](double, prg::FuncAction* act)
		{//アップデート
			actor->transform->setPos(ft(act->time / time, *p0, *dist - *p0));
		},
		[=]()mutable
		{//開始
			auto transformRange = RectF{ Arg::center(rectCenterPos),width,height };
			*p0 = actor->transform->getPos();
			*dist = RandomVec2(transformRange);
		},
			[=]()
		{//終了
			actor->transform->setPos(*dist);
		}
		, time
		);
}

Act util::rectRandomTransform(Object* actor, Object* rangeActor, double width, double height, double time, Vec2 offset,TransFunction ft)
{
	std::shared_ptr<Vec2> p0(new Vec2{});
	std::shared_ptr<Vec2> dist(new Vec2{});
	return new prg::FuncAction(
		[=](double, prg::FuncAction* act)
		{//アップデート
			actor->transform->setPos(ft(act->time / time, *p0, *dist - *p0));
		},
		[=]()mutable
		{//開始
			auto rangeTransform = rangeActor->GetComponent<Transform>();
			auto transformRange = RectF{ Arg::center(rangeTransform->getPos()),width,height }.rotatedAt(rangeTransform->getPos(), rangeTransform->getDirection());
			transformRange.moveBy(offset);
			*p0 = actor->transform->getPos();
			*dist = RandomVec2(transformRange);
		},
		[=]()
		{//終了
			actor->transform->setPos(*dist);
		}
		,time
	);
}

Act util::transformPosition(ObjectBehavior* actor,Array<Vec2> positions, double timeSpan, double transTime, bool loop, TransFunction ft)
{
	auto action = new prg::ManageActions();

	std::shared_ptr<Vec2> p0(new Vec2{});
	for (const auto& pos : positions)
	{
		action->add(new prg::FuncAction([=](double dt, prg::FuncAction* act)
			{//アップデート
				actor->transform->setPos(ft(act->time / transTime, *p0, pos - *p0));
			},
			[=]()
			{//スタート				
				*p0 = actor->transform->getPos();
			},
			[=]()
			{//エンド
				actor->transform->setPos(pos);
			}, transTime
		));
		action->add(new snk::Wait(timeSpan));
	}

	action->loop = loop;

	return action;
}

Act util::aimingTarget(ObjectBehavior* actor, ObjectBehavior* target, Optional<double> time)
{
	auto action = new prg::ManageActions();
	//time==noneだったらずっとアクションを続ける
	if (not time)
	{
		action->loop = true;
		time = 0;
	}

	action->add(new prg::FuncAction([=](double, prg::FuncAction*) {
		actor->transform->setDirection((target->transform->getPos() - actor->transform->getPos()).getAngle() - 90_deg);
		},*time));

	return action;
}

prg::ManageActions* util::rotational(Object* actor, double time, Optional<int32> numCycle)
{
	auto action = new prg::ManageActions;
	std::shared_ptr<double> firstRad(new double);
	action->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act) {
			actor->transform->setDirection(*firstRad + 2 * Math::Pi * act->time / time);
		},
		[=] {
			*firstRad = actor->transform->getDirection();
		},time
	));
	if (numCycle)return util::repeat(action, *numCycle);
	else
	{
		action->loop = true;
		return action;
	}
}

void util::bulletSetting(Bullet* bullet, BulletType type, double scale)
{
	auto tex=bullet->AddComponentImmediately<DrawTexture>();

	tex->gameCamera = bullet->MainObject->objectManager->camera;

	auto p = GameParameterManager::createManager(U"common", U"Barrage");

	double t = 7 + Random<double>(-5, 2);
	switch (type)
	{
	case snk::BulletType::cake:
		tex->setTexture(U"cakeBullet", scale);
		bullet->progress->setAction(mitame::rotational(bullet->GetComponent<DrawTexture>(), t), U"rotation");
		bullet->progress->startAction(U"rotation");
		bullet->getPElems()->damage *= p.params<double>(U"cakeDamage");
		break;
	case snk::BulletType::cup:
		tex->setTexture(U"cupBullet", scale);
		bullet->progress->setAction(mitame::rotational(bullet->GetComponent<DrawTexture>(), t), U"rotation");
		bullet->progress->startAction(U"rotation");
		bullet->getPElems()->damage *= p.params<double>(U"cupDamage");
		break;
	case snk::BulletType::choco:
		tex->setTexture(U"choco", scale);
		bullet->progress->setAction(mitame::rotational(bullet->GetComponent<DrawTexture>(), t), U"rotation");
		bullet->progress->startAction(U"rotation");
		bullet->getPElems()->damage *=p.params<double>(U"chocoDamage");
		break;
	case snk::BulletType::ufo:
		tex->setTexture(U"chocoUFO", scale*0.8);
		bullet->progress->setAction(mitame::vibrateDirectin(bullet->GetComponent<DrawTexture>(), 20_deg, 2.5), U"yureru");
		bullet->progress->startAction(U"yureru");
		bullet->getPElems()->damage *= p.params<double>(U"chocoUfoDamage");
		break;
	case snk::BulletType::bat:
		tex->setTexture(U"bat", scale);
		break;
	case snk::BulletType::none:
		break;
	default:
		break;
	}
}

void util::touchUpGun(PrimitiveGun* gun, HashTable<BulletType, double> bulletDistribution,double scale)
{
	Array<BulletType> options;
	Array<double> values;
	for (auto& [key, value] : bulletDistribution)
	{
		options << key; values << value;
	}
	if (options.isEmpty())return;
	
	gun->touch += [=](IGun*, Bullet* bullet)
	{
		//確率分布を生成
		DiscreteDistribution distribution(values);
		util::bulletSetting(bullet, DiscreteSample(options, distribution), scale);
		return bullet;
	};
}

PrimitiveGun* util::createPrimitiveGun(GameObjectManager* manager, double bulletSpeed, const BulletElements& b, TouchUpBullet touchUp)
{
	return createPrimitiveGun(manager,
		[=](IGun* gun, Bullet* bullet) {
			bullet->setElements(b);
			bullet->AddComponentImmediately<StandardMove>(new StandardMove(Vec2{ Cos(gun->transform->getDirection()),Sin(gun->transform->getDirection()) }*bulletSpeed))->start_action();
			return touchUp(gun,bullet);
		});
}

PrimitiveGun* util::createPrimitiveGun(GameObjectManager* manager, double bulletSpeed, const BulletElements& b)
{
	return createPrimitiveGun(manager,
		[=](IGun* gun, Bullet* bullet) {
			bullet->setElements(b);
			bullet->AddComponentImmediately<StandardMove>(new StandardMove(Vec2{ Cos(gun->transform->getDirection()),Sin(gun->transform->getDirection()) }*bulletSpeed))->start_action();
			return bullet;
		});
}
//結局この関数を呼び出す
PrimitiveGun* util::createPrimitiveGun(GameObjectManager* manager, TouchUpBullet b)
{
	auto gun = (new Object(manager))->AddComponentImmediately<PrimitiveGun>();

	//弾を撃つ処理
	gun->shotFunction = [=](IGun* gun, GameObjectManager* m)
	{
		auto bullet = (new Object(m))->AddComponentImmediately<Bullet>();
		bullet->transform->setPosAndPrePos(gun->transform->getPos());
		return b(gun, bullet);
	};

	return gun;
}

PrimitiveGun* util::createLaserGun(Object* obj, double speed, double thick, const BulletElements& elems)
{
	auto laserGun=obj->AddComponentImmediately<PrimitiveGun>();

	laserGun->shotFunction = [=](IGun* gun, GameObjectManager* m) {
		auto bullet = (new Object(m))->AddComponentImmediately<Bullet>();
		bullet->transform->setPosAndPrePos(gun->transform->getPos());
		bullet->setElements(elems);
		double dir = gun->transform->getDirection();
		bullet->AddComponentImmediately<StandardMove>(new StandardMove(Vec2{ Cos(dir),Sin(dir) }*speed))->start_action();
		auto laser = bullet->AddComponentImmediately<LenearLaser>();
		laser->setThickness(thick);
		laser->firstPos = gun->transform->getPos();
		laser->gun = gun;

		auto tmp = new prg::ManageActions;
		tmp->add(new prg::FuncAction([=]() mutable
			{
				laser->releaseFirstPos = true;
			}));

		laserGun->progress->setAction(tmp, U"stopLaserShot");

		return bullet;
	};

	auto a = new prg::ManageActions;
	a->add(new prg::FuncAction([=] {laserGun->shot(); }));
	laserGun->progress->setAction(a, U"laserShot");

	return laserGun;
}

PrimitiveGun* util::createLaserGun(GameObjectManager* manager, double speed, double thick, const BulletElements& elems)
{
	return createLaserGun(new Object(manager), speed, thick,elems);
}

Barrages::Barrages(GameObjectManager* manager, Object* shooter, double difficalty)
	:manager(manager), actor(shooter->GetComponent<ObjectBehavior>()), difficalty(difficalty)
{
	//ゲーム難易度の取得
	auto mode = Gd::getInstance().getData().mini_mode;
	//パラメータを生成
	Gpm pGameScene = Gpm::createManager(U"common", U"GameScene");
	Gpm pManager = Gpm::createManager(U"common", U"Barrages");
	//ラムダ式の中でactorを使うとthisのコピーからactorを参照してしまうのでそれを阻止
	auto actor = Barrages::actor;

	HashTable<BulletType, double> defaultDist = { { BulletType::cake,1 }, { BulletType::cup,1 },{BulletType::choco,1 } };

	//以下、各弾幕の開始処理

	//ランダムな方向に弾を発射
	String barrageName = U"SimpleRandomDirection";
	barrageDictionaly[barrageName] = [=]
	{
		//パラメータの取得はラムダ式の中で書こう
		Gpm p = pManager.getManager(barrageName);
		auto aManager = new prg::ManageActions;
		int32 GunNum = p.params<double>(U"numGun") + Barrages::difficalty * p.params<double>(U"numGunCoe");
		for (int32 i = 0; i < GunNum; i++)
		{
			//銃を生成
			auto gun = util::createPrimitiveGun(manager, p.params<double>(U"bulletSpeed") + p.params<double>(U"bulletSpeedCoe") * difficalty
			, BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 1, Circle{ 0,0,6 }));
			//ケーキ、カップケーキを放つ銃
			util::touchUpGun(gun, defaultDist);

			auto mainWeapon = actor->GetComponent<MainWeapon>();
			//生成した銃をメインウェポンに追加
			mainWeapon->addGun(gun);

			//銃の動きを設定する
			auto shotAction = new prg::ManageActions;
			//銃を移動
			shotAction->add(util::rectRandomTransform(gun->MainObject, mainWeapon->gun->MainObject, p.params<Point>(U"transformRange").x, p.params<Point>(U"transformRange").y, 0));
			//メインウェポンを基準にランダムな方向に銃を向ける
			shotAction->add(randomDirection(gun, mainWeapon->gun, Min(-40_deg / difficalty, -20_deg), Max(40_deg / difficalty, 20_deg)));
			//shot
			shotAction->add(new GunShot(gun));
			//数秒待つ
			shotAction->add(new Wait(p.params<double>(U"timeSpan") - difficalty * p.params<double>(U"timeCoe")));

			aManager->addParallel(shotAction);
		}

		aManager->loop = true;
		return aManager;
	};
	//ランダムな方向に一定時間弾を連射
	barrageName = U"ConstantRandomDirection";
	barrageDictionaly[barrageName] = [=]
	{
		Gpm p = pManager.getManager(barrageName);
		auto aManager = new prg::ManageActions;
		int32 numGun = p.params<double>(U"numGun") + Barrages::difficalty * p.params<double>(U"numGunCoe");
		auto mainWeapon = actor->GetComponent<MainWeapon>();
		double bulletNum = p.params<int32>(U"bulletNum") + difficalty * p.params<double>(U"bulletNumCoe");
		double shotSpan = p.params<double>(U"shotSpan") + p.params<double>(U"shotSpanCoe") / difficalty;
		auto createGun = [=]() {
			auto shotAction = new prg::ManageActions;
			auto radiate = (new Object(manager))->AddComponentImmediately<RegulerRadiateGun>();
			radiate->transform->setPosAndPrePos(mainWeapon->gun->transform->getPos());
			mainWeapon->gun->addGun(radiate);
			radiate->range = Min(80_deg, numGun * 12.5_deg);
			for (auto& k : step(numGun))
			{
				//銃生成
				auto gun = util::createPrimitiveGun(manager, p.params<double>(U"bulletSpeed") + p.params<double>(U"bulletSpeedCoe") * difficalty, BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 1, Circle{ 0,0,6 }));

				util::touchUpGun(gun, defaultDist);
				//RegulerRadiateGunに追加
				radiate->addArm(gun);
			}
			//メインウェポンをプレイヤーの向きに向ける
			shotAction->add(new prg::FuncAction([](double, prg::FuncAction*) {}, [=] {
				if (manager->getGameObjects(ObjectTag::player).size() != 1)return;
				auto player = manager->getGameObjects(ObjectTag::player)[0];
				mainWeapon->gun->transform->setDirection((player->transform->getPos() - actor->transform->getPos()).getAngle() - 90_deg);
				//放射状弾幕銃の向きをメインウェポンと一致させる
				radiate->transform->setDirection(mainWeapon->gun->transform->getDirection());
				}));
			//ランダムに銃を回転
			shotAction->add(new prg::FuncAction([](double, prg::FuncAction*) {}, [=] {
				radiate->transform->rotate(Random<double>(-12_deg, 12_deg));
				}));
			//銃を連射する
			shotAction->add(constantFire(radiate, bulletNum, shotSpan));
			//数秒待つ
			shotAction->add(new Wait(p.params<double>(U"timeSpan") + difficalty * p.params<double>(U"timeCoe")));

			shotAction->loop = true;
			return shotAction;
		};
		//上の関数で銃を生成
		aManager->add(createGun());
		//難易度がNormal以上なら
		if (mode != Easy_Mode)
		{//銃を追加
			aManager->addParallel(createGun(), (bulletNum * shotSpan + p.params<double>(U"timeSpan") + difficalty * p.params<double>(U"timeCoe")) / 2);
		}
		return aManager;
	};
	//発射した弾が後にプレイヤーの方向へ進む(UFO)
	barrageName = U"ShotFollowTarget";
	barrageDictionaly[barrageName] = [=]
	{
		Gpm p = pManager.getManager(barrageName);
		auto aManager = new prg::ManageActions;

		int32 numGun = p.params<double>(U"numGun") + Barrages::difficalty * p.params<double>(U"numGunCoe");
		double bulletFriction = 120;
		auto radiate = (new Object(manager))->AddComponentImmediately<RadiateGun>();
		radiate->transform->setPosAndPrePos(actor->transform->getPos());

		auto bulletTouchUp = [=](IGun* gun, Bullet* b) {
			b->AddComponentImmediately<DecelerateMove>(new DecelerateMove(bulletFriction))->start_action();
			auto bulletAction = new prg::ManageActions();
			auto followPlayer = new prg::FuncAction([=]
			{
				if (manager->getGameObjects(ObjectTag::player).size() != 1)return;
				auto player = manager->getGameObjects(ObjectTag::player)[0];
				auto dir = (player->transform->getPos() - b->transform->getPos()).getAngle() - 90_deg;
				b->GetComponent<DecelerateMove>()->removeSelf<DecelerateMove>();
				b->GetComponent<StandardMove>()->removeSelf<StandardMove>();
				b->AddComponentImmediately<StandardMove>(new StandardMove(Vec2{ Cos(dir),Sin(dir) }*(200 + 40 * difficalty)))->start_action();
				b->progress->startAction(U"mawaru");
				b->progress->stopAction(U"yureru");
			});
			bulletAction->add(followPlayer, 3);
			b->progress->setAction(bulletAction, U"advanceToTarget")->start();
			
			return b;
		};

		auto bulbuilder = BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 1, Circle{ 0,0,8 });

		for (const auto& k : step(numGun))
		{
			auto upper_gun = util::createPrimitiveGun(manager, 200, bulbuilder, bulletTouchUp);
			auto lower_gun = util::createPrimitiveGun(manager, 200, bulbuilder, bulletTouchUp);

			util::touchUpGun(upper_gun, { {BulletType::ufo,1} });
			util::touchUpGun(lower_gun, { {BulletType::ufo,1} });
			upper_gun->touch += [](IGun*, Bullet* b) {
				b->progress->setAction(mitame::rotational(b->GetComponent<DrawTexture>(), 0.7), U"mawaru");
				return b; };
			lower_gun->touch += [](IGun*, Bullet* b) {
				b->progress->setAction(mitame::rotational(b->GetComponent<DrawTexture>(), 0.7), U"mawaru");
				return b;
			};

			radiate->addArm(upper_gun, -70_deg - k * 25_deg);
			radiate->addArm(lower_gun, 70_deg + k * 25_deg);

			aManager->add(new prg::FuncAction([](double, prg::FuncAction*) {},
				[=] {
					//左を向かせてから打つ
					radiate->transform->setDirection(-180_deg);
					upper_gun->shot();
					lower_gun->shot();
				}), 0.5);
		}

		aManager->add(new Wait(2.5));

		aManager->loop = true;

		return aManager;
	};
	//らせん状弾幕
	barrageName = U"Spiral01";
	barrageDictionaly[barrageName] = [=]
	{
		Gpm p = pManager.getManager(barrageName);
		double spinTime = p.params<double>(U"spinTime") + p.params<double>(U"spinTimeCoe") / difficalty;
		int32 numGun = p.params<double>(U"numGun") + Barrages::difficalty * p.params<double>(U"numGunCoe");
		auto aManager = new prg::ManageActions;
		auto mainWeapon = actor->GetComponent<MainWeapon>();
		int32 s = 1;
		if (mode == Hard_Mode)s = 2;
		bool clockWise = false;//反時計回り
		for (const auto& kk : step(s)) {
			auto radiate = (new Object(manager))->AddComponentImmediately<RegulerRadiateGun>();
			radiate->transform->setPosAndPrePos(mainWeapon->gun->transform->getPos());
			radiate->transform->setDirAndPreDir(180_deg);
			mainWeapon->addGun(radiate);
			for (const auto& k : step(numGun))
			{
				auto gun = util::createPrimitiveGun(manager, p.params<double>(U"bulletSpeed") + p.params<double>(U"bulletSpeedCoe") * difficalty, BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 1, Circle{ 0,0,6 }));
				util::touchUpGun(gun, defaultDist);
				radiate->addArm(gun);
			}

			//放射状弾幕銃の動きを設定
			auto shotAction = new prg::ManageActions();

			shotAction->add(new prg::FuncAction([=](double dt, prg::FuncAction*) {
				if (not clockWise)dt = -dt;
				radiate->transform->rotate(2 * Math::Pi / spinTime * dt);
				}, new prg::Condition()));

			shotAction->addParallel(constantFire(radiate, p.params<double>(U"shotSpan") + p.params<double>(U"shotSpanCoe") / difficalty));

			aManager->addParallel(shotAction);

			clockWise = not(clockWise);
		}

		return aManager;
	};
	//散らばる弾幕
	barrageName = U"Scatter01";
	barrageDictionaly[barrageName] = [=]
	{
		Gpm p = pManager.getManager(barrageName);

		auto aManager = new prg::ManageActions;

		auto mainWeapon = actor->GetComponent<MainWeapon>();

		aManager = util::repeat(
		[=]
		{//散らばる弾幕を作る部分
			auto action = new prg::ManageActions;

			auto gun = util::createPrimitiveGun(manager, 0, BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 1, Circle{ 0,0,6 }));

			mainWeapon->addGun(gun);

			gun->transform->setPosAndPrePos(mainWeapon->gun->transform->getPos());

			Vec2 rectPos{ pGameScene.params<Vec2>(U"mainFieldPos").x + pGameScene.params<double>(U"mainFieldWidth") - 200,pGameScene.params<double>(U"mainFieldHeight") / 2 };

			action->add(util::rectRandomTransform(actor->MainObject, rectPos, p.params<double>(U"rangeW"), p.params<double>(U"rangeH"), p.params<double>(U"shotTime")));

			int32 bulletNum = int32(p.params<int32>(U"numBullet") + p.params<double>(U"numBulletCoe") * difficalty);
			double range = p.params<double>(U"range") * 1_deg;
			double highSpeed = p.params<double>(U"highSpeed") + p.params<double>(U"highSpeedCoe") * difficalty;
			double lowSpeed = p.params<double>(U"lowSpeed");
			//ランダムな方向にランダムな速度で放つ
			gun->touch = [=](IGun* gun, Bullet* b) {
				double d = sc::aimDirection(manager, actor->MainObject, ObjectTag::player) + Random(-range, range);
				b->AddComponentImmediately<StandardMove>(new StandardMove(Vec2{ Cos(d),Sin(d) }*Random(lowSpeed, highSpeed)))->start_action();
				return b;
			};
			//弾の見た目を付与
			util::touchUpGun(gun, defaultDist);
			//shot
			action->add(new prg::FuncAction([=] {for (const auto& k : step(bulletNum))gun->shot(); }));

			return action;
		}//３回繰り返す
		,3);

		Vec2 basePos = Vec2{ pGameScene.params<Vec2>(U"mainFieldPos").x + pGameScene.params<double>(U"mainFieldWidth") - 200,pGameScene.params<double>(U"mainFieldHeight") / 2 };

		aManager->add(util::transformPosition(actor, { basePos }, 0, 2, false));

		//雑魚敵を放つ↓
		{
			p = p.getManager(U"Laser");

			auto radiate = (new Object(manager))->AddComponentImmediately<RegulerRadiateGun>();

			radiate->transform->setPosAndPrePos(basePos);

			radiate->MainObject->followParent = false;

			radiate->range = 240_deg;

			actor->GetComponent<MainWeapon>()->addGun(radiate);

			int32 numEnemy = p.params<double>(U"laserEnemyNum") + p.params<double>(U"laserEnemyNumCoe") * difficalty;

			int32 laserThick = p.params<int32>(U"laserThick");

			double laserShotTime = p.params<double>(U"laserTime") + p.params<double>(U"laserTimeCoe") * difficalty;

			double laserSpeed = 160 + difficalty * p.params<double>(U"laserSpeedCoe");

			auto laserRadiateAction = new prg::ManageActions;

			//撃った銃(弾)を知っているクラス
			struct Shot {
				Array<PrimitiveGun*> gun;
			};
			std::shared_ptr<Shot> shot(new Shot);

			//雑魚敵を放つ銃を配置
			for (const auto& k : step(numEnemy))
			{
				//レーザー銃(雑魚敵)を放つ銃
				auto gun = util::createPrimitiveGun(manager, 200, sc::circleBullet(ObjectTag::none, 6, 1),
					[=](IGun* gun, Bullet* b) {
					b->AddComponentImmediately<DecelerateMove>(new DecelerateMove(180))->start_action();
					auto laserGun = util::createLaserGun(b->MainObject, laserSpeed, laserThick, sc::circleBullet(ObjectTag::enemyBullet, 2, 2));
					gun->addGun(laserGun);
					auto stopLaser = new prg::ManageActions;
					stopLaser->add(new prg::FuncAction([=] {laserGun->progress->startAction(U"stopLaserShot"); }), laserShotTime);
					laserGun->progress->setAction(stopLaser, U"stopLaser");
					laserGun->touch += [=](IGun* gun, Bullet* b) {
						gun->progress->resetAction(U"stopLaser");
						gun->progress->startAction(U"stopLaser");
						//レーザーの見た目
						auto tex=b->AddComponentImmediately<DrawTexture>();
						tex->scale = 0.3;
						tex->gameCamera = manager->camera;
						ColorF color = Sample({ Palette::Purple,Palette::Mediumspringgreen,Palette::Brown });
						tex->color = color;
						auto fig = b->AddComponentImmediately<DrawFigure>();
						auto action = new prg::ManageActions;
						action->add(new prg::FuncAction([=](double, prg::FuncAction*)
							{
								auto laser = b->GetComponent<LenearLaser>();
								Vec2 vec = b->transform->getPos() - laser->firstPos;
								vec.setLength(vec.length() - laser->disLength);
								tex->pos = Vec2{ -laser->disLength,0 };//x軸だけ変えればいいみたい　謎
								RectF rect = RectF{ Arg::leftCenter(Vec2{laser->disLength,0}), vec.length(), laser->thickness };
								fig->setFigure(rect, color);
								b->transform->setDirection(b->GetComponent<StandardMove>()->get_vel().getAngle() + 90_deg);
							}, [=] {
								b->progress->startAction(U"animation");
							}, 
							Math::InfF)
						);
						//b->progress->setAction(createAnimation(tex, 0.15, FileSystem::DirectoryContents(U"resource/water")), U"animation");
						b->progress->setAction(createAnimation(tex, 0.15, getAssetNames(U"resource/water/",6)), U"animation");
						b->progress->setAction(action, U"shot")->start();
						return b;
					};
					//銃の見た目はUFO
					auto tex = laserGun->AddComponentImmediately<DrawTexture>();
					tex->setTexture(U"chocoUFO", 0.1);
					tex->affectedByTransformsDirection = false;
					tex->gameCamera = manager->camera;
					laserGun->progress->setAction(mitame::vibrateDirectin(tex, 15_deg, 5), U"yureru")->start();

					for (auto& gun : shot->gun)	if (gun == nullptr) {
						gun = laserGun;
						break;
					}

					return b;
				});

				shot->gun << nullptr;

				radiate->addArm(gun);
			}

			double shotSpan = 0.1;
			double accum = 0;
			laserRadiateAction->add(new prg::FuncAction([=](double dt, prg::FuncAction* act) mutable {
				for (auto& enemy : shot->gun)
				{//雑魚敵が死んだら
					if (enemy != nullptr and enemy->MainObject->getEraseFlag())enemy = nullptr;
				}
				//一定時間たったら
				for (accum += dt; accum >= shotSpan; accum -= shotSpan)
				{
					int32 k = 0;
					for (auto& enemy : shot->gun)
					{
						if (enemy == nullptr) {
							radiate->shot(k);
							break;
						}
						++k;
					}
				}
				}));
			laserRadiateAction->loop = true;

			aManager->add(new prg::FuncAction([=] {radiate->progress->startAction(U"Filling"); }));

			radiate->progress->setAction(laserRadiateAction, U"Filling");

			laserRadiateAction = new prg::ManageActions;

			double laserShotSpan = p.params<double>(U"laserShotSpan");
			int32 numEnemyShot = 0;
			//レーザーを撃つ
			laserRadiateAction->add(new prg::FuncAction([=](double dt, prg::FuncAction* act) mutable {
				if (aManager->getLoopCount() < 1) {
					act->endCondition->forcedFire();
					return;
				}
				bool flag = false;
				for (accum += dt; accum >= laserShotSpan; accum -= laserShotSpan) {
					auto& uEnemy = shot->gun[numEnemyShot];	auto& dEnemy = shot->gun[shot->gun.size() - 1 - numEnemyShot];
					if (uEnemy != nullptr) {
						uEnemy->transform->setDirection(sc::aimDirection(manager, uEnemy->MainObject, ObjectTag::player));
						uEnemy->shot();
					}
					if (dEnemy != nullptr and uEnemy != dEnemy) {
						dEnemy->transform->setDirection(sc::aimDirection(manager, dEnemy->MainObject, ObjectTag::player));
						dEnemy->shot();
					}
					if (++numEnemyShot > shot->gun.size() / 2.0) { numEnemyShot = 0; act->endCondition->forcedFire(); }
				}
				}, [] {},
					[=]
				{//終わったらリセット
					laserRadiateAction->reset();
				}, Math::InfF));

			radiate->progress->setAction(laserRadiateAction, U"laserShot");

			aManager->add(new prg::FuncAction([=] {radiate->progress->startAction(U"laserShot"); }));
		}
		aManager->addParallel(new Wait(2));

		aManager->loop = true;

		return aManager;
	};

	Vec2 mainFieldPos = pGameScene.params<Vec2>(U"mainFieldPos");
	double mainFieldW = pGameScene.params<double>(U"mainFieldWidth");
	double mainFieldH = pGameScene.params<double>(U"mainFieldHeight");

	//二つの銃が回って放射状に弾を放つ　連星
	barrageName = U"binalyCircle";
	barrageDictionaly[barrageName] = [=]
	{
		Gpm p = pManager.getManager(barrageName);

		auto aManager = new prg::ManageActions;

		auto mainWeapon = actor->GetComponent<MainWeapon>();

		auto smallUfo = (new Object(manager))->AddComponentImmediately<RegulerRadiateGun>();

		smallUfo->transform->setPos(actor->transform->getPos());

		smallUfo->symmetrical = false;

		auto ufoTex = smallUfo->AddComponentImmediately<DrawTexture>();

		ufoTex->setTexture(U"chocoUFO", 0.1);

		ufoTex->affectedByTransformsDirection = false;

		ufoTex->gameCamera = manager->camera;

		smallUfo->progress->setAction(mitame::vibrateDirectin(ufoTex, 10_deg, 1.8), U"vib")->start();

		auto gun = (new Object(manager))->AddComponentImmediately<RegulerRadiateGun>();

		gun->transform->setPos(actor->transform->getPos());

		gun->symmetrical = false;

		mainWeapon->addGun(smallUfo);

		mainWeapon->addGun(gun);

		int32 numGun = p.params<int32>(U"numGun") + p.params<double>(U"numGunCoe") * difficalty;

		for (auto k : step(numGun))
		{
			auto g1=util::createPrimitiveGun(manager, 45, BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 1, Circle{ 0,0,6 }));
			auto g2=util::createPrimitiveGun(manager, 45, BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 1, Circle{ 0,0,6 }));
			util::touchUpGun(g1, defaultDist);
			util::touchUpGun(g2,defaultDist);
			smallUfo->addArm(g1);
			gun->addArm(g2);
		}

		//円軌道半径
		double r = 100;
		//周期
		double t = 20;

		auto centerObj = (new Object(manager))->AddComponentImmediately<ObjectBehavior>();

		centerObj->transform->setPos({ actor->transform->getPos() });

		double shotSpan = p.params<double>(U"shotSpan") + p.params<double>(U"shotSpanCoe") / difficalty;
		//centerから離れて回る
		double disTime = 2.4;//中心から離れるのにかかる時間
		aManager->add(new prg::FuncAction([=](double, prg::FuncAction* act) {
				double ra = Min(r, r * (act->time / disTime));
				//回しながら離す
				actor->transform->setPos(centerObj->transform->getPos() +ra * Vec2{ Cos(2 * Math::Pi * act->time / t), Sin(2 * Math::Pi * act->time / t) });
				smallUfo->transform->setPos(actor->transform->getPos() + 2 * (centerObj->transform->getPos() - actor->transform->getPos()));
			}, [=] {
				auto shotAction = new prg::ManageActions;
				shotAction->add(constantFire(smallUfo, shotSpan), 1.6);
				smallUfo->progress->setAction(shotAction, U"constantShot")->start();
				shotAction = new prg::ManageActions;
				shotAction->add(constantFire(gun, shotSpan), 1.6);
				gun->progress->setAction(shotAction, U"constantShot")->start();
				auto randomWalk = new prg::ManageActions;
				randomWalk->add(util::rectRandomTransform(centerObj->MainObject, mainFieldPos+Vec2{ mainFieldW * 3 / 4-100.0,mainFieldH / 2.0 }, mainFieldW /4.0, mainFieldH / 2.0, 5), 8);
				randomWalk->loop = true;
				centerObj->progress->setAction(randomWalk, U"random")->start();
			}, Math::Inf
		));
		return aManager;
	};
	barrageName = U"combinationBarrage";
	barrageDictionaly[barrageName] = [=]
	{
		Gpm p = pManager.getManager(barrageName);

		auto aManager = new prg::ManageActions;

		auto mainWeapon = actor->GetComponent<MainWeapon>();

		//放射状銃の設定
		double radiateShotSpanCoe = p.params<double>(U"radiateShotSpanCoe");
		double radiateShotSpan = 0.6 - radiateShotSpanCoe*difficalty;
		auto radiate = (new Object(manager))->AddComponentImmediately<RegulerRadiateGun>();
		radiate->symmetrical = false;
		radiate->progress->setAction(util::rotational(radiate->MainObject, radiateShotSpan/0.6*8), U"rotate");//回転させる(最初はゆっくり)
		auto tmp = new prg::ManageActions;
		tmp->add(constantFire(radiate, radiateShotSpan));//連射する
		radiate->progress->setAction(tmp, U"constant");
		for (auto k : step(2)) {
			auto gun = util::createPrimitiveGun(manager, 80, sc::circleBullet(ObjectTag::enemyBullet, 6, 1));
			util::touchUpGun(gun, defaultDist);
			gun->touch += [](IGun* gun,Bullet* b) {
				b->GetComponent<StandardMove>()->set_acc(b->GetComponent<StandardMove>()->get_vel()*1.5);
				return b;
			};
			radiate->addArm(gun);
		}
		radiate->transform->setPos(actor->transform->getPos());
		mainWeapon->addGun(radiate);

		double timeSpan = 0.15 + 1.3 / difficalty;
		double whipSpan = 1 + 6 / difficalty;
		double whipShotSpan = 0.05+0.5/difficalty;
		aManager->add(new prg::FuncAction(
			[=] {
				//子分ufoを生成し、放つ
				for (auto k : step(2))
				{
					std::shared_ptr<double> speed(new double);
					double atten = 30+difficalty*8;
					auto aiming = util::createPrimitiveGun(manager,0,sc::circleBullet(ObjectTag::enemyBullet,6,1),[=](IGun* gun, Bullet* b)mutable {
						b->GetComponent<StandardMove>()->removeSelf<StandardMove>();
						double dir = gun->transform->getDirection();
						b->AddComponentImmediately<StandardMove>(new StandardMove(Vec2{ Cos(dir),Sin(dir) }*(*speed)))->start_action();
						*speed += atten;
						return b;
					});
					auto tmp=aiming->AddComponentImmediately<AutoAimingGun>();
					tmp->target = manager->getGameObjects(ObjectTag::player)[0];
					int32 numBullet = 6; std::shared_ptr<int32> count(new int32(0));
					double accum = 0;
					auto shot = new prg::ManageActions;
					shot->add(new prg::FuncAction(
						[=](double dt, prg::FuncAction*)mutable {
							for (accum += dt; accum > whipShotSpan; accum -= whipShotSpan)
							{
								aiming->shot();
								++(*count);
							}
							if (*count > numBullet)shot->endCondition->forcedFire();
						}, [=]()mutable {
							*speed = 70+difficalty*10;
							*count = 0;
							accum = 0;
						},
						Math::Inf
					),whipSpan*k/2.0);
					aiming->progress->setAction(shot, U"shot");
					aiming->transform->setPos(actor->transform->getPos());
					mainWeapon->addGun(aiming);
					auto tex=aiming->AddComponentImmediately<DrawTexture>();
					tex->setTexture(U"chocoUFO", 0.1);
					tex->gameCamera = manager->camera;
					tex->affectedByTransformsDirection = false;
					aiming->progress->setAction(mitame::vibrateDirectin(tex, 10_deg, 1.7), U"yureru")->start();
					aiming->AddComponentImmediately<DecelerateMove>(new DecelerateMove(150))->start_action();
					aiming->AddComponentImmediately<StandardMove>(new StandardMove(Vec2{ 0,-1 + k * 2 }*200))->start_action();
					aiming->Parent->name = U"aimingShotUfo";
					util::touchUpGun(aiming, defaultDist);
					util::setDissapearAction(aiming, 1.6, 0.2);					
				}
			},
			[=] {
				for (auto& gun : mainWeapon->gun->otherGuns)
				{
					if (gun->MainObject->get_name() == U"aimingShotUfo") {
						gun->GetComponent<DecelerateMove>()->removeSelf<DecelerateMove>();
						gun->GetComponent<StandardMove>()->removeSelf<StandardMove>();
					}
				}
			},2.5
		));
		//whipShotを３回(×２)
		auto shotAction = new prg::ManageActions;
		shotAction->add(new prg::FuncAction([=] {
			for (auto& gun : mainWeapon->gun->otherGuns)if (gun->MainObject->get_name() == U"aimingShotUfo")gun->progress->restartAction(U"shot");
		}, whipSpan));
		aManager->add(util::repeat(shotAction,3));
		//回転放射状弾幕
		double t = 10;//この秒数行う
		aManager->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction* act) {
				double s = 3*Periodic::Triangle1_1(t, (act->time + t / 4.0));
				s = Max(s, -1.0);
				s = Min(s, 1.0);
				s *= 7;
				radiate->progress->getAction(U"rotate")->timeScale = s;
				radiate->progress->getAction(U"constant")->timeScale = Math::Abs(s) + 1;
			}, [=] {
				radiate->progress->restartAction(U"rotate");
				radiate->progress->restartAction(U"constant");
			}, [=] {
				radiate->progress->stopAction(U"rotate");
				radiate->progress->stopAction(U"constant");
			}, t
		));

		double shotSpanCoe = p.params<double>(U"expTimeSpanCoe")*difficalty;
		double numBullet = 30 + p.params<double>(U"numBulletCoe") * difficalty;
		aManager->add(new prg::FuncAction(
			[=]{
				//爆発するアクション
				auto explAction = new prg::ManageActions;
				Vec2 pos{};

				for (auto& gun : mainWeapon->gun->otherGuns)
				{
					if (gun->Parent->get_name() != U"aimingShotUfo")continue;
					pos = manager->getGameObjects(ObjectTag::player)[0]->transform->getPos();
					//移動し、消滅するアクション
					auto tmp = new prg::ManageActions;
					tmp->add(util::transformPosition(gun, { manager->getGameObjects(ObjectTag::player)[0]->transform->getPos() }, 0, 2, false));
					tmp->add(new prg::FuncAction(
						[=] {
							for (auto it = mainWeapon->gun->otherGuns.begin(); it != mainWeapon->gun->otherGuns.end();)
							{
								if (*it == gun) it = mainWeapon->gun->otherGuns.erase(it);
								else ++it;
							}
							gun->progress->startAction(U"dissapearAction");
						}
					));
					explAction->addParallel(tmp);
				}

				//消滅and銃設置
				explAction->add(new prg::FuncAction(
					[=] {
						//爆発
						auto expGun = util::createPrimitiveGun(manager, 1, sc::circleBullet(ObjectTag::enemyBullet, 6, 1));
						util::touchUpGun(expGun, defaultDist);
						expGun->transform->setPos(pos);
						expGun->touch += [](IGun* gun, Bullet* b)
						{
							b->GetComponent<StandardMove>()->set_vel(b->GetComponent<StandardMove>()->get_vel() * Random<double>(40, 120));
							return b;
						};
						auto shotAction = new prg::ManageActions;
						shotAction->add(randomDirectionShot(expGun, mainWeapon->gun, 0.01-shotSpanCoe, 0, 360_deg));
						auto expGunAction = new prg::ManageActions;
						expGunAction->add(util::repeat(shotAction, numBullet));
						expGunAction->add(new prg::FuncAction(
							[=] {
								//爆発した後は銃を消す
								for (auto it = mainWeapon->gun->otherGuns.begin(); it != mainWeapon->gun->otherGuns.end();)
								{
									if ((*it)==expGun) {
										expGun->MainObject->eraseThis();
										it = mainWeapon->gun->otherGuns.erase(it);
									}
									else ++it;
								}
							}
						));
						mainWeapon->addGun(expGun);
						expGun->progress->setAction(expGunAction, U"constant")->start();
					}
				));
				actor->progress->setAction(explAction, U"explosion")->start();
			}
		));

		aManager->add(new Wait(1));

		aManager->loop = true;

		return aManager;
	};

	barrageName = U"shootingStar";
	barrageDictionaly[barrageName] = [=]
	{
		Gpm p = pManager.getManager(barrageName);
		auto aManager = new prg::ManageActions;

		auto gun = util::createPrimitiveGun(manager, 100, BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 2, Circle{ 0,0,8*2}));

		util::touchUpGun(gun, { { BulletType::ufo,1 } }, 0.3);

		std::shared_ptr<double> dir(new double);

		gun->touch += [=](IGun* gun, Bullet* b)
		{
			auto vec = Vec2{ Cos(*dir),Sin(*dir) };

			b->GetComponent<StandardMove>()->set_vel(vec * 50);

			b->GetComponent<StandardMove>()->set_acc(vec * 600);

			b->GetComponent<StandardMove>()->maxSpeed = 370+14*difficalty;

			return b;
		};

		auto mainWeapon = actor->GetComponent<MainWeapon>();

		gun->transform->setPos(actor->transform->getPos());

		mainWeapon->addGun(gun);

		double shotTimeSpan = 0;

		double interval = 200;

		double range = 60_deg;

		auto player = manager->getGameObjects(ObjectTag::player)[0];

		auto field = manager->getGameObjects(ObjectTag::bulletField)[0]->GetComponent<Collider>()->getHitbox().asPolygon();

		Vec2 rectPos = actor->transform->getPos();

		double accum = 0;

		aManager->add(new prg::FuncAction(
			[=] (double dt,prg::FuncAction*)mutable{
				*dir = 180_deg - 0.3*(actor->transform->getPos().y - mainFieldH / 2) / mainFieldH * 2 * Math::Pi;
				for (accum += dt; accum >= shotTimeSpan; accum -= shotTimeSpan) {
					double theta = (*dir + 180_deg) + Random<double>(-range / 2, range / 2);
					Line generateLine{ mainFieldPos + Vec2{0,mainFieldH / 2},mainFieldPos + Vec2{0,mainFieldH / 2} + (mainFieldH + mainFieldW) * Vec2 { Cos(theta),Sin(theta) } };
					auto points = generateLine.intersectsAt(field);
					gun->transform->setPos((*points)[0]);
					gun->shot();
					shotTimeSpan = 2/difficalty+Random(-0.15,0.1)*9/difficalty;
					if (shotTimeSpan <= 0)shotTimeSpan = 0.04;
				}
			}, [=] {
				auto randomTransform = new prg::ManageActions();
				randomTransform->add(util::rectRandomTransform(actor->MainObject, rectPos, 100, 300, 2),5);
				actor->progress->setAction(randomTransform, U"transform")->loop = true;
				actor->progress->startAction(U"transform");
			}, Math::Inf
		));

		return aManager;
	};


	barrageName = U"sentakuki";
	barrageDictionaly[barrageName] = [=]
	{
		Gpm p = pManager.getManager(barrageName);
		auto aManager = new prg::ManageActions;

		auto mainWeapon = actor->GetComponent<MainWeapon>();

		auto radiate = (new Object(manager))->AddComponentImmediately<RegulerRadiateGun>();

		radiate->symmetrical = false;

		radiate->transform->setPos(actor->transform->getPos());

		for (auto k : step(5))
		{
			auto gun = util::createPrimitiveGun(manager, 200, BulletElements().setting(ObjectTag::enemyBullet, { ObjectTag::player, ObjectTag::playerBullet }, 1, Circle{ 0,0,6 }));

			util::touchUpGun(gun, { { BulletType::cake,1 } });

			radiate->addArm(gun,20);
		}
		mainWeapon->addGun(radiate);

		auto shotAction = new prg::ManageActions;

		double shotSpan = 0.07 * 5 / difficalty;
		shotAction->add(constantFire(radiate,shotSpan));

		radiate->progress->setAction(shotAction, U"shot");

		radiate->progress->setAction(util::rotational(radiate->MainObject,3*5/difficalty), U"rotation");

		auto rev = new prg::ManageActions;

		rev->add(new Wait(0.5*5/difficalty));
		//回転方向を逆転
		rev->add(new prg::FuncAction([=] {radiate->progress->getAction(U"rotation")->timeScale *= -1; }));

		rev->loop = true;

		radiate->progress->setAction(rev, U"revers");

		aManager->add(new prg::FuncAction(
			[=] {
				radiate->progress->restartAction(U"shot");
				radiate->progress->startAction(U"rotation",U"revers");
			},
			[=] {
				radiate->progress->stopAction(U"shot");
			},2
		));
		aManager->add(new Wait(4/difficalty));

		aManager->loop = true;

		return aManager;
	};
}

void snk::BarragesParamInit()
{
	using namespace params_setter;
	set_nest(U"Barrage");
	set_param(1.6, U"ケーキのダメージ", U"cakeDamage");
	set_param(1.1, U"カップケーキのダメージ", U"cupDamage");
	set_param(1, U"チョコufoのダメージ", U"chocoUfoDamage");
	set_param(0.8, U"chocoのダメージ", U"chocoDamage");
	//ランダムな方向に球を放つ
	{
		set_nest(U"Barrages", U"SimpleRandomDirection");
		set_param(Point(150, 350), U"銃が移動できる範囲(shooterのまわりにこの数値に従った長方形範囲が生成される。また、長方形は銃の向きに追従)", U"transformRange");
		set_param(3.0, U"ランダム弾を飛ばす時間間隔", U"timeSpan");
		set_param(0.29, U"ランダム弾を飛ばす時間間隔は(timeSpan-difficalty×これ)", U"timeCoe");
		set_param(0, U"銃の個数", U"numGun");
		set_param(0.6, U"銃の個数はnumGun+int(difficalty×これ)", U"numGunCoe");
		set_param(50, U"弾速", U"bulletSpeed");
		set_param(22, U"弾速はbulletSpeed+difficalty×これ", U"bulletSpeedCoe");
	}
	// 一定間隔で弾をランダムな方向に連射
	{
		set_nest(U"Barrages", U"ConstantRandomDirection");
		set_param(0.14, U"1連射が何秒か", U"shotSpan");
		set_param(0.3, U"1連射の秒数はshotSpan+これ/difficalty", U"shotSpanCoe");
		set_param(2.5, U"ランダム弾を飛ばす時間間隔", U"timeSpan");
		set_param(-0.24, U"ランダム弾を飛ばす時間間隔は(timeSpan+difficalty×これ)", U"timeCoe");
		set_param(1, U"銃の個数", U"numGun");
		set_param(0.39, U"銃の個数はnumGun+int(difficalty×これ)", U"numGunCoe");
		set_param(3, U"連射で発射する弾数", U"bulletNum");
		set_param(0.4, U"連射で発射する玉の個数はbulletNum+difficalty×これ", U"bulletNumCoe");
		set_param(160, U"弾速", U"bulletSpeed");
		set_param(22, U"弾速はbulletSpeed+difficalty×これ", U"bulletSpeedCoe");
	}
	//発射した弾が後にプレイヤーを追従する
	{
		set_nest(U"Barrages", U"ShotFollowTarget");
		set_param(1, U"銃の個数", U"numGun");
		set_param(0.39, U"銃の個数はnumGun+int(difficalty×これ)", U"numGunCoe");
	}
	//最初に出す螺旋弾幕 ショート
	{
		set_nest(U"Barrages", U"Spiral01");
		set_param(5, U"放射状銃に追加する銃の数", U"numGun");
		set_param(0.99, U"銃の個数はnumGun+int(difficalty×これ)", U"numGunCoe");
		set_param(0.2, U"1連射が何秒か", U"shotSpan");
		set_param(0.1, U"1連射の秒数はshotSpan+これ/difficalty", U"shotSpanCoe");
		set_param(1.4, U"放射状銃の１回転周期", U"spinTime");
		set_param(0.7, U"1回転周期はspinTime+これ/difficalty", U"spinTimeCoe");
		set_param(120, U"弾速", U"bulletSpeed");
		set_param(15, U"弾速はbulletSpeed+difficalty×これ", U"bulletSpeedCoe");
	}
	//散弾弾幕
	{
		set_nest(U"Barrages", U"Scatter01");
		set_param(340, U"動く範囲の横幅", U"rangeW");
		set_param(500, U"動く範囲の高さ", U"rangeH");
		set_param(0.7, U"動き出してから止まって銃を撃つまでの時間", U"shotTime");
		set_param(-1, U"散弾する弾の数", U"numBullet");
		set_param(4, U"散弾する弾の数はnumBullet+これ×difficalty", U"numBulletCoe");
		set_param(40, U"散弾する範囲は(-これ)～(これ)", U"range");
		set_param(50, U"弾速の最低速度", U"lowSpeed");
		set_param(150, U"弾速の最高速度", U"highSpeed");
		set_param(5, U"弾速の最高速度はhighSpeed+これ×difficalty", U"highSpeedCoe");
		//雑魚敵レーザー
		set_nest(U"Barrages", U"Scatter01", U"Laser");
		set_param(0 ,U"レーザーを放つ雑魚敵の数", U"laserEnemyNum");
		set_param(1.5, U"レーザーを放つ雑魚敵の数はlaserEnemyNum＋これ×difficalty", U"laserEnemyNumCoe");
		set_param(0.2, U"敵を一体放つ時間間隔", U"timeSpan");
		set_param(5, U"レーザーの太さ", U"laserThick");
		set_param(0.6, U"レーザーを撃つ時間間隔", U"laserShotSpan");
		set_param(1.4, U"レーザーを放つ時間", U"laserTime");
		set_param(-0.1, U"レーザーを放つ時間はlaserTime+これ×difficalty", U"laserTimeCoe");
		set_param(40, U"レーザーのスピードは120+これ×difficalty", U"laserSpeedCoe");
	}
	//連星重力波的弾幕
	{
		set_nest(U"Barrages", U"binalyCircle");
		set_param(8, U"銃の個数", U"numGun");
		set_param(0.8, U"銃の個数はnumGun+difficalty*これ", U"numGunCoe");
		set_param(1, U"1連射が何秒か", U"shotSpan");
		set_param(1.4, U"1連射の秒数はshotSpan+これ/difficalty", U"shotSpanCoe");
	}
	//いろいろな弾幕の組み合わせ
	{
		set_nest(U"Barrages", U"combinationBarrage");
		set_param(1, U"鞭弾幕の連射間隔0.7+これ*difficalty", U"shotTimeSpan");
		set_param(1, U"0.7+これ*difficalty", U"aimShotTimeSpanCoe");
		set_param(1, U"鞭弾幕を放つ時間間隔", U"shotSpan");
		set_param(0.05, U"らせん弾幕　0.6-これ*difficalty", U"radiateShotSpanCoe");
		set_param(0.001, U"爆発したとき、弾を放つ時間の間隔は0.01-これ*difficalty", U"expTimeSpanCoe");
		set_param(5, U"爆発したとき放つ玉の数は30+これ＊difficalty", U"numBulletCoe");
	}

}

Act Barrages::randomDirectionShot(IGun* gun,IGun* mainWeapon, double timeSpan,double minRad, double maxRad)
{
	auto shotAction = new prg::ManageActions();
	auto s = new prg::FuncAction(
		[=](double dt,prg::FuncAction*)
		{
			double rad = mainWeapon->transform->getDirection() + Random<double>(minRad, maxRad);

			gun->transform->setDirection(rad);

			gun->shot();
		}
	);
	shotAction->add(s);

	shotAction->add(new snk::Wait(timeSpan));

	return shotAction;
}

Act Barrages::randomDirection(IGun* gun, IGun* mainWeapon, double minRad, double maxRad)
{
	auto shotAction = new prg::ManageActions();
	auto s = new prg::FuncAction([=](double dt, prg::FuncAction*){},
		[=]
		{
			double rad = mainWeapon->transform->getDirection() + Random<double>(minRad, maxRad);
			gun->transform->setDirection(rad);
		});
	shotAction->add(s);

	return shotAction;
}

Act Barrages::constantFire(IGun* gun, int32 num, double timeSpan)
{
	auto shotAction = new prg::ManageActions();
	//shotしてからtimeSpan秒間待つアクションをnum回追加
	for (const auto& k : step(num)) {
		shotAction->add(new prg::FuncAction([](double, prg::FuncAction*) {}, [=]()
			{
				gun->shot();
			}
		, timeSpan));
	}

	return shotAction;
}

Act Barrages::constantFire(IGun* gun, double timeSpan)
{
	auto shotAction = new prg::ManageActions();
	
	shotAction->add(new prg::FuncAction([=] {gun->shot(); }), timeSpan);

	shotAction->loop = true;

	return shotAction;
}
//barrageDictionalyが呼び出された時点で銃などのオブジェクトの生成や、それらに対するアクションの生成がされる。
prg::ManageActions* Barrages::createBarrage(const String& barrageName)
{
	return barrageDictionaly[barrageName]();
}

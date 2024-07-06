#include "../stdafx.h"
#include "Actors.h"
#include"../Components/Collider.h"
#include"Object.h"
#include"../Components/Transform.h"
#include"../GameParameterManager/Init.h"
#include"../Components/Moves.h"
#include"../GameObject/Bullet.h"
#include"../Barrages.h"
#include"../Components/HitCallback.h"
#include"../Components/Draw.h"
#include"../Barrages.h"
#include"../Components/Draw.h"
#include"../GameObject/Camera.h"

using Gmp = GameParameterManager;

using namespace snk;

//ParamInit.hで記述するのは面倒臭いので、必要に応じてここにパラメータを書く
void maker::ActorsParamInit()
{
	using namespace params_setter;

	set_nest(U"Actors", U"MainActors");
	set_param<double>(1, U"MainActorが初期位置につくまでの時間", U"preparationTime");

	set_nest(U"Actors", U"MainActors", U"Player");
	set_param<double>(4, U"当たり判定の大きさ", U"radius");
	set_param<double>(260, U"通常速度", U"speed");
	set_param<double>(12, U"体力", U"hp");
	set_param<double>(1, U"敵に与えるダメージ", U"power");
	set_param<double>(2, U"無敵時間", U"unbeatableTime");
	set_param<double>(0.5, U"弾が自分のもとに戻ってから次に発射できるまでの時間", U"affterShotTime");
	set_param<double>(0.2, U"通常弾の大きくなりやすさ", U"damageCoe");
	set_param<double>(3, U"通常弾のマックススケール", U"maxScale");
	set_param<double>(2, U"必殺技ゲージをためる速さはこの値/sec", U"chargeVel");

	set_nest(U"Actors", U"MainActors", U"MainEnemy");
	set_param<double>(10, U"当たり判定の大きさ", U"radius");
	set_param<double>(10, U"体力", U"hp");
}

void MainWeapon::start()
{
	//銃をオブジェクトに追従させる
	gun = (new Object(dynamic_cast<Object*>(Parent)->objectManager))->AddComponentImmediately<PrimitiveGun>();
	dynamic_cast<Object*>(Parent)->makeChiled(gun->MainObject);
}

Object* maker::CreateActors::Actor(GameObjectManager* objectManager)
{
	auto obj = (new Object(objectManager));

	auto behavior = obj->AddComponentImmediately<ObjectBehavior>();

	obj->AddComponentImmediately<Collider>();

	obj->AddComponentImmediately<MainWeapon>();

	obj->AddComponentImmediately<ActorState>()->hp = 0;

	return obj;
}

Object* maker::CreateActors::MainActor(GameObjectManager* objectManager,const String& actorName)
{
	Object* mainActor = Actor(objectManager);

	auto behavior = mainActor->GetComponent<ObjectBehavior>();
	//パラメータを取得
	GameParameterManager manager = Gmp::createManager(U"common", U"Actors",U"MainActors");

	//アクションをセット
	{
		prg::ManageActions* actions = new prg::ManageActions;

		auto gameSceneParam = Gmp::createManager(U"common", U"GameScene");
		Vec2 firstPos;
		if (actorName == U"Player")firstPos = { 100+gameSceneParam.params<Vec2>(U"mainFieldPos").x,gameSceneParam.params<double>(U"mainFieldHeight") / 2};
		else if (actorName == U"MainEnemy")firstPos = { gameSceneParam.params<Vec2>(U"mainFieldPos").x+gameSceneParam.params<double>(U"mainFieldWidth") - 100,gameSceneParam.params<double>(U"mainFieldHeight") / 2 };

		//初期位置に向かう動きをセット
		auto Movement = new prg::ManageActions;
		Movement->add(util::transformPosition(behavior, { firstPos }, manager.params<double>(U"preparationTime"), manager.params<double>(U"preparationTime"), false));
		behavior->progress->setAction(Movement, U"preparationMove");
	}

	return mainActor;
}

Object* maker::CreateActors::Player(GameObjectManager* objectManager)
{
	Object* player = MainActor(objectManager, U"Player");
	player->tag = ObjectTag::player;

	//パラメータを取得
	Gmp manager = Gmp::createManager(U"common", U"Actors", U"MainActors", U"Player");
	Gmp gameSceneParam = Gmp::createManager(U"common", U"GameScene");

	auto behavior = player->GetComponent<ObjectBehavior>();

	auto tex = player->AddComponentImmediately<DrawTexture>();

	/*tex->setTexture(U"annna", 0.15);*/

	tex->baseScale = 0.15;

	tex->gameCamera = objectManager->camera;
	//通常アニメーション
	//behavior->progress->setAction(createAnimation(tex, 0.12, FileSystem::DirectoryContents(U"resource/annnaAnimation")), U"normalAni")->start();
	behavior->progress->setAction(createAnimation(tex, 0.12, getAssetNames(U"resource/annnaAnimation/",4)), U"normalAni")->start();
	//蝙蝠を放つときのアンナちゃんのアニメーしょん
	{
		double time = Gmp::createManager(U"common", U"Actors", U"MainActors", U"Player").params<double>(U"affterShotTime");
		auto attackAnimation = createAnimation(tex, time / 5.0, getAssetNames(U"resource/annnaAttackAnimation/",5),
			[=](int32 page) {return 2 * page / 5.0; });
		attackAnimation->add(new Wait(0.05));
		attackAnimation->add(new prg::FuncAction([] {}, [=] {
			behavior->progress->stopAction(U"attackAni");
			behavior->progress->restartAction(U"normalAni");
		}));
		attackAnimation->loop = false;

		behavior->progress->setAction(attackAnimation, U"attackAni");
	}
	//チャージアニメーション
	{
		auto chargeAnimation = new prg::ManageActions;
		auto animation1 = createAnimation(tex, 0.15, getAssetNames(U"resource/annnaChargeAnimation/",2));
		animation1->loop = false;
		animation1->setEndCondition(new prg::FuncCondition([=] {
			if (player->GetComponent<Status<chargeGage>>(U"chargeGage")->value.value >= 9)behavior->progress->restartAction(U"chargeAni2");
			return MouseR.up() or player->GetComponent<Status<chargeGage>>(U"chargeGage")->value.value >= 9; }));
		chargeAnimation->add(animation1);
		auto animation2 = createAnimation(tex, 0.15, { U"resource/annnaChargeAnimation/2.png",U"resource/annnaChargeAnimation/1.png" });
		animation2->loop = false;
		auto rChargeAnimation = new prg::ManageActions;
		rChargeAnimation->add(animation2);
		rChargeAnimation->add(new prg::FuncAction([] {},
			[=] {
				behavior->progress->restartAction(U"normalAni");
				behavior->progress->resetAction(U"attackAni");
			}));
		chargeAnimation->init = true;
		rChargeAnimation->init = true;
		behavior->progress->setAction(chargeAnimation, U"chargeAni");
		behavior->progress->setAction(rChargeAnimation, U"chargeAni2");
	}
	//必殺技チャージのときの点滅
	behavior->progress->setAction(mitame::constantFlashing(tex, ColorF{ 2,2,2,1 }, 1), U"flashing");

	player->GetComponent<ActorState>()->hp = manager.params<double>(U"hp");
	//衝突処理
	{
		player->GetComponent<HitCallback>()->registerCallback(
			ObjectTag::enemyBullet,
			[=](Object* obj, const Object* bullet)
			{
				auto status = obj->GetComponent<ActorState>();
				if (not status->unbeatable)
				{
					status->hp -= bullet->GetComponent<Bullet>()->getElements().damage;
					if (bullet->name != U"laser") {
						AudioAsset(U"eat").stop();
						AudioAsset(U"eat").setVolume(0.4 * GameDataModel::getData().Effect_volume);
						AudioAsset(U"eat").play();
					}
					else
					{
						AudioAsset(U"drinking").setVolume(0.7 * GameDataModel::getData().Effect_volume).setSpeed(0.6).play();
					}
				}
			}
		);
		auto a = Gmp::createManager(U"common", U"GameScene");
		player->GetComponent<Transform>()->setPosAndPrePos({ a.params<Vec2>(U"mainFieldPos").x - 150,100 });
		player->GetComponent<Collider>()->set_hitbox(Figure{ Circle{0,0,manager.params<double>(U"radius")} });
	}
	//蝙蝠を放つ銃
	auto batGun = util::createPrimitiveGun(objectManager, [=](IGun* gun, Bullet* b) {
		b->transform->setPosAndPrePos(player->transform->getPos() + Vec2{ 100,0 });

		b->setElements(sc::circleBullet(ObjectTag::playerBullet, 15, 1));

		//ヒットコールバック
		{
			double damageCoef = manager.params<double>(U"damageCoe");
			double maxScale = manager.params<double>(U"maxScale");
			//敵の弾にあたると大きくなる　敵に当たったら元の大きさに戻す
			b->hitCallback->registerCallback(ObjectTag::enemyBullet, [=](Object* self, const Object* other)
			{
				if (other->name != U"laser") {
					AudioAsset(U"eat").stop();
					AudioAsset(U"eat").setVolume(0.5*GameDataModel::getData().Effect_volume);
					AudioAsset(U"eat").play();
				}
				else
				{
					AudioAsset(U"drinking").setVolume(0.7* GameDataModel::getData().Effect_volume).setSpeed(0.6).play();
				}
				self->GetComponent<Collider>()->addScale(other->GetComponent<Bullet>()->getElements().damage * damageCoef);
				self->GetComponent<DrawTexture>()->scale += other->GetComponent<Bullet>()->getElements().damage * damageCoef;
				//スケールに制限をかける
				if (self->GetComponent<Collider>()->getScale() > maxScale) {
					self->GetComponent<Collider>()->setScale(maxScale);
					self->GetComponent<DrawTexture>()->scale = maxScale;
				}
			});
			//敵に当たったときの処理はbatの生成関数で書いてる
			b->hitCallback->registerCallback(ObjectTag::enemy, [](Object* self, Object*) {
				AudioAsset(U"batAttack").setVolume(GameDataModel::getData().Effect_volume).play();
				});
		}

		b->makeParent(behavior);

		return b;
	});
	//弾に見た目を付与
	util::touchUpGun(batGun, { {BulletType::bat,1} });
	//蝙蝠を生成
	batGun->shot();
	//生成したこーもりを取得
	auto bat = batGun->getBullets()[0];

	bat->GetComponent<Collider>()->active = false;//初期状態は当たり判定無効
	//蝙蝠の動き
	{
		auto action = new prg::ManageActions;

		//攻撃を出すまでのラグ　アニメーション
		action->add(new prg::FuncAction(
			[=] {
				//アニメーションの切り替え
				behavior->progress->restartAction(U"attackAni");
				behavior->progress->stopAction(U"normalAni");
				behavior->progress->resetAction(U"chargeAni2");
			}, [=] {
				//behavior->progress->stopAction(U"attackAni");
				//behavior->progress->restartAction(U"normalAni");
			}, new prg::FuncCondition(
			[=] {
				//アニメーションが終わるまで
				return behavior->progress->getAction(U"attackAni")->getEnded();
			}
			)));
		//放つ
		action->add(new prg::FuncAction(
			[=](double, prg::FuncAction*) {//アップデート
				//ダメージを蝙蝠の大きさに合わせる
				bat->GetComponent<Bullet>()->getPElems()->damage = bat->GetComponent<DrawTexture>()->scale;
				//蝙蝠の頭を敵の方向に向ける
				bat->GetComponent<DrawTexture>()->rad = 90_deg;
				bat->transform->setDirection(sc::aimDirection(bat->MainObject->objectManager, bat->MainObject, ObjectTag::enemy));
			},
			[=] {//開始
				bat->MainObject->followParent = false;
				bat->transform->priority.setUpdate(-Math::InfF);
				auto move = bat->AddComponentImmediately<AdvanceMove>();
				move->speed = 450;
				move->start_action();
				bat->GetComponent<Collider>()->active = true;
			},
				[=] {//終了
				bat->GetComponent<AdvanceMove>()->removeSelf<AdvanceMove>();
				bat->GetComponent<Collider>()->active = false;
			},
				new prg::FuncCondition(
				[=] {
					auto target = bat->MainObject->objectManager->getGameObjects(ObjectTag::enemy)[0];
					//敵にあたったら終了
					if (bat->GetComponent<Collider>()->intersects(*target->GetComponent<Collider>()))
					{
						bat->GetComponent<Collider>()->setScale(1);
						bat->GetComponent<DrawTexture>()->scale = 1;
						return true;
					}
					return false;
				})
		));
		//戻る
		action->add(new prg::FuncAction(
			[=](double, prg::FuncAction*) {
				bat->transform->setDirection(sc::aimDirection(bat->MainObject->objectManager, bat->MainObject, ObjectTag::player));
			},
			[=] {
				auto move = bat->AddComponentImmediately<AdvanceMove>();
				move->speed = 330;
				move->start_action();
			},
			[=] {
				bat->GetComponent<AdvanceMove>()->removeSelf<AdvanceMove>();
				bat->transform->setDirection(0_deg);
				bat->GetComponent<DrawTexture>()->rad = 0_deg;
				bat->MainObject->followParent = true;

			},
				new prg::FuncCondition(
				[=] {
					auto target = bat->MainObject->objectManager->getGameObjects(ObjectTag::player)[0];
					//プレイヤーにあたったら終了 batのcollider->active=falseとしているので、図形を取得して直接当たり判定を行っている
					return bat->GetComponent<Collider>()->getHitbox().intersects(target->GetComponent<Collider>()->getHitbox());
				})
				));
		//最初の場所に
		action->add(new prg::FuncAction(
			[=] {
				bat->AddComponent<StandardMove>(new StandardMove({ 150,0 }))->start_action();
			},
			[=] {
				auto target = bat->MainObject->objectManager->getGameObjects(ObjectTag::player)[0];

				bat->transform->setPos(target->transform->getPos() + Vec2{ 100,0 });

				bat->GetComponent<StandardMove>()->removeSelf<StandardMove>();
			},
				new prg::FuncCondition(
				[=] {
					auto target = bat->MainObject->objectManager->getGameObjects(ObjectTag::player)[0];

					return (target->transform->getPos() - bat->transform->getPos()).length() > 100;
				})
				), 0.2);

		bat->progress->setAction(action, U"shotAction");
	}
	bat->progress->setAction(createAnimation(bat->GetComponent<DrawTexture>(), 0.06, getAssetNames(U"resource/batAnimation/",9)), U"animation")->start();

	batGun->transform->setPosAndPrePos(player->transform->getPos());

	player->GetComponent<MainWeapon>()->addGun(batGun);
	//必殺技作成
	{
		auto bombGun = (new Object(objectManager))->AddComponentImmediately<RegulerRadiateGun>();
		bombGun->symmetrical = false;
		bombGun->transform->setPosAndPrePos(player->transform->getPos());
		player->GetComponent<MainWeapon>()->addGun(bombGun);
		Array<ColorF> colors{ ColorF{Palette::Pink,0.6},ColorF{Palette::Red,0.6},ColorF{Palette::Yellow,0.6},ColorF{Palette::Greenyellow,0.6},ColorF{Palette::Blue,0.6},ColorF{Palette::Purple,0.6} };
		//六方向に銃を設置
		for (auto& k : step(6))
		{
			bombGun->addArm(util::createPrimitiveGun(objectManager, [=](IGun* gun, Bullet* b)
				{
					b->GetComponent<Collider>()->draw_hitbox = true;

					b->transform->setDirection(gun->transform->getDirection());

					b->setElements(sc::circleBullet(ObjectTag::playerBullet, 24, 1));
					//敵の弾にあたっても影響を受けないようにする
					b->hitCallback->registerCallback({ ObjectTag::enemyBullet,ObjectTag::enemy }, [=](Object* self, const Object* other) {});

					auto tex = b->AddComponentImmediately<DrawTexture>();
					//向きは最初固定
					tex->affectedByTransformsDirection = false;

					tex->color = colors[k];

					tex->scale = 0.2;

					tex->gameCamera = objectManager->camera;

					auto oraobj = b->AddComponentImmediately<DrawFunc>();

					oraobj->drawingFunction = [=](const DrawFunc*) {
						for (auto kk : step(30))
						{
							Circle{ b->transform->getPos(),kk * 2 + 20 }.draw(ColorF{ 0.9,0.5,0.5,0.1 * (10 - kk) / 10.0 + 0.05 });
						}
					};

					oraobj->priority.setDraw(-1);

					b->progress->setAction(createAnimation(tex, 0.06, getAssetNames(U"resource/batAnimation1/",9)), U"animation")->start();

					return b;
				}
			));
		}
		//ボムのアクション
		double s1time = 0.7;
		double distance = 100;
		auto bombAction = new prg::ManageActions;
		//等加速度直線運動
		bombAction->add(
			new prg::FuncAction([=](double, prg::FuncAction* act) {//アップデート
				for (auto& bullet : bombGun->getBullets())
				{
					double progress = act->time / s1time;
					Vec2 dist = distance * Vec2{ Cos(bullet->transform->getDirection()),Sin(bullet->transform->getDirection()) };
					bullet->transform->setPos(dist * progress * (2 - progress) + player->transform->getPos());
				}
				}, [=] {//開始
					bombGun->shot();
				}, [=] {//最後にすべての弾を銃に追従させる
					for (auto& bullet : bombGun->getBullets())
					{
						bullet->makeParent(bombGun);
					}
				}, s1time));
		//回転しながら収縮
		double speed = 60;
		double angVel = 360_deg * 1.6;
		double damageCoef = manager.params<double>(U"damageCoe");
		bombAction->add(new prg::FuncAction([=](double dt, prg::FuncAction*)
			{
				bombGun->transform->rotate(angVel * dt);
			}, [=] {
				for (auto& bullet : bombGun->getBullets())
				{
					double d = (player->transform->getPos() - bullet->transform->getPos()).getAngle() - 90_deg;
					bullet->transform->setDirection(d);
					bullet->AddComponentImmediately<AdvanceMove>(new AdvanceMove(speed))->start_action();
				}
			}, [=] {
				for (auto& bullet : bombGun->getBullets())
				{
					bullet->GetComponent<AdvanceMove>()->removeSelf<AdvanceMove>();
				}
			}, distance / speed), 0.4);
		//発射
		bombAction->add(new prg::FuncAction(
			[=] {
				//上下たいしょう
				Array<double> rad{ 50_deg, 95_deg, 160_deg };
				Array<int32> speed{ 100,160,110 };
				auto bullets = bombGun->getBullets();
				for (auto& k : step(bullets.size() / 2))
				{
					bullets[2 * k]->AddComponentImmediately<FuncMove>(new FuncMove(
						Vec2{ Cos(rad[k]),Sin(rad[k]) }*speed[k]
					));
					bullets[2 * k + 1]->AddComponentImmediately<FuncMove>(new FuncMove(
						Vec2{ Cos(rad[k]),-Sin(rad[k]) }*speed[k]
					));
				}
				double limit = 7;//ボム弾は敵に当たらなくてもこの秒数がたったら消えてしまう。
				double maxScale = manager.params<double>(U"maxScale");
				for (auto& bullet : bullets)
				{
					//敵の弾にあたると大きくなるようにする　標的に当たったら消す
					bullet->hitCallback->registerCallback(ObjectTag::enemyBullet, [=](Object* self, const Object* other)
					{
						self->GetComponent<Collider>()->addScale(other->GetComponent<Bullet>()->getElements().damage * damageCoef);
						self->GetComponent<DrawTexture>()->scale += other->GetComponent<Bullet>()->getElements().damage * damageCoef;
						//スケールに制限をかける
						if (self->GetComponent<Collider>()->getScale() > maxScale) {
							self->GetComponent<Collider>()->setScale(maxScale);
							self->GetComponent<DrawTexture>()->scale = maxScale;
						}
					});

					bullet->hitCallback->registerCallback(ObjectTag::enemy, [=](Object* self, Object*) {
						self->eraseThis();
						self->objectManager->camera->progress->restartAction(U"shake");
					});
					//絵が進行方向を向くようにする
					auto tex = bullet->GetComponent<DrawTexture>();
					tex->affectedByTransformsDirection = true;
					tex->rad = 90_deg;

					bullet->MainObject->relaseParent();
					auto movement = bullet->GetComponent<FuncMove>();
					movement->Vt = [=](double dt, FuncMove* m)
					{
						Vec2 v = m->get_vel();
						double dir = sc::aimDirection(objectManager, bullet->MainObject, ObjectTag::enemy);
						double dis = (objectManager->getGameObjects(ObjectTag::enemy)[0]->transform->getPos() - bullet->transform->getPos()).length();
						if (dis < 3)return v;
						v += Vec2{ Cos(dir),Sin(dir) } / (Math::Sqrt(dis)) * dt * 10000 * (m->get_time());
						v.setLength(Min(v.length(), 300.0));
						//蝙蝠の向きを敵に向ける
						bullet->transform->setDirection(dir);
						//面倒くさいからここに自然消滅の処理を書く 実行されることはたぶんないけど
						if (m->get_time() > limit)bullet->MainObject->eraseThis();
						return v;
					};
					movement->start_action();
				}
			}
		));
		behavior->progress->setAction(bombAction, U"bomb");
	}

	//以下、アクションのセット
	//キー移動アクション
	{
		auto actions = new prg::ManageActions;
		Vec2 fieldPos = gameSceneParam.params<Vec2>(U"mainFieldPos");
		double w = gameSceneParam.params<double>(U"mainFieldWidth");
		double h = gameSceneParam.params<double>(U"mainFieldHeight");
		RectF mainField{ fieldPos,w,h };
		actions->add(new prg::FuncAction(
			[=, speed = manager.params<double>(U"speed")](double dt, prg::FuncAction*)
			{
				Vec2 dir{ 0,0 };
				if (GameDataModel::getData().KeyUp.pressed())
				{
					dir += {0, -1};
				}
				if (GameDataModel::getData().KeyDown.pressed())
				{
					dir += {0, 1};
				}
				if (GameDataModel::getData().KeyRight.pressed())
				{
					dir += {1, 0};
				}
				if (GameDataModel::getData().KeyLeft.pressed())
				{
					dir += {-1, 0};
				}
				dir.setLength(speed);
				player->transform->moveBy(dir * dt);
				Vec2 pos = player->transform->getPos();
				if (not mainField.contains(pos))//フィールドから出ないようにする
				{
					pos.x = Min(pos.x, fieldPos.x + w);
					pos.x = Max(pos.x, fieldPos.x);
					pos.y = Min(pos.y, fieldPos.y + h);
					pos.y = Max(pos.y, fieldPos.y);
					player->transform->setPos(pos);
				}
			}
		, new prg::Condition()));

		behavior->progress->setAction(actions, U"Move");
	}
	//蝙蝠を放つアクション
	{
		auto actions = new prg::ManageActions;
		actions->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction*)
			{
				if (MouseL.down() and (not MouseR.pressed()) and Cursor::OnClientRect())
				{
					auto action = bat->progress->getAction(U"shotAction");
					if (not(action->getStarted() and (not action->getEnded())))	bat->progress->restartAction(U"shotAction");
				}
			}
		, new prg::Condition()));
		behavior->progress->setAction(actions, U"Attack");
	}
	//必殺技をチャージ＆発動させるアクション
	{
		//チャージが完了したら回転する円の オーラ
		auto oraObj = (new Object(objectManager))->AddComponentImmediately<ObjectBehavior>();
		oraObj->transform->setPos(behavior->transform->getPos());
		oraObj->makeParent(behavior);
		auto ora = oraObj->AddComponentImmediately<DrawFigure>(U"chargeOra");
		auto ora2 = oraObj->AddComponentImmediately<DrawFigure>(U"chargeOra2");
		ora->setFigure(Rect{ Arg::center(0,0),60}, ColorF{Palette::White,0.8});
		ora-> visible= false;
		ora->GetComponent<ObjectBehavior>()->progress->setAction(mitame::rotational(ora, 0.2), U"mawaru")->start();
		ora2->setFigure(Rect{ Arg::center(0,0),100 }, ColorF{ Palette::Firebrick,0.2 },1,20_deg);
		ora2->visible = false;
		ora2->priority.setDraw(-1);
		ora2->GetComponent<ObjectBehavior>()->progress->setAction(mitame::rotational(ora2, 0.4), U"mawaru2")->start();
		oraObj->Parent->z = behavior->Parent->z - 1;
		//チャージエフェクト
		auto effect = (new Object(objectManager))->AddComponentImmediately<ObjectBehavior>();
		effect->makeParent(behavior);
		effect->transform->setPos(behavior->transform->getPos());
		auto effectAction = new prg::ManageActions;
		auto effectFigs = effect->AddComponentImmediately<DrawFigures>();
		double accum = 0;
		effectAction->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction*)mutable
			{//アップデート
				//0.17秒ごとに追加
				for (accum += dt; accum > 0.1; accum -= 0.1) {
					double dir = Random<double>(360_deg);
					auto fig = effectFigs->addFigure(U"obj", Circle{0,0,Random<double>(4,15)}, ColorF{Palette::White,Random<double>(0.6,0.95)}, Random<double>(1, 0.8), Random<double>(360_deg), Vec2{Cos(dir),Sin(dir)}*Random<double>(30,60));
					fig->AddComponentImmediately<AdvanceMove>(new AdvanceMove(100))->start_action();
					fig->transform->setDirection((behavior->transform->getPos() - fig->transform->getPos()).getAngle() - 90_deg);
				}
				//オーブを　生成　削除
				for (auto it=effectFigs->figures.begin();it!=effectFigs->figures.end();)
				{
					auto& figure = *it;
					Vec2 d1 = behavior->transform->getPos() - figure.second->transform->getPos();
					Vec2 d2 = Vec2{ Cos(figure.second->transform->getDirection()),Sin(figure.second->transform->getDirection()) };
					if (d1.x * d2.x + d1.y * d2.y<=0) {
						figure.second->GetComponent<ObjectBehavior>()->MainObject->eraseThis();
						it = effectFigs->figures.erase(it);
						continue;
					}
					else
					{
						figure.second->scale = Max(0.2,d1.length()/60.0);
						figure.second->transform->setDirection(d1.getAngle() - 90_deg);
					}
					++it;
				}
			},[=]{
				behavior->progress->restartAction(U"flashing");
			},
			[=]{//終了
				behavior->progress->getAction(U"flashing")->endCondition->forcedFire();
				behavior->progress->stopAction(U"flashing");
				effectFigs->clearFigures();
				effectAction->reset();
			},
				new prg::FuncCondition([=] {return MouseR.up(); })
		));
		behavior->progress->setAction(effectAction, U"chargeEffect");

		auto gage = player->AddComponentImmediately<Status<chargeGage>>(U"chargeGage");
		auto vel = manager.params<double>(U"chargeVel");
		auto actions = new prg::ManageActions;
		double maxHp= player->GetComponent<ActorState>()->hp;
		actions->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction*)mutable
			{
				gage->value.colision = false;
				if (MouseR.up())
				{
					behavior->progress->restartAction(U"chargeAni2");
				}
				//
				if (MouseR.pressed() and (not behavior->progress->getAction(U"attackAni")->isActive()))
				{//チャージ
					gage->value.value = Min(gage->value.value + vel * dt, maxHp - player->GetComponent<ActorState>()->hp);
					if (gage->value.value >= 9) {
						gage->value.value = 9;
						gage->value.ready = true;
					}
					//ゲージと針がぶつかっていたら
					gage->value.colision = gage->value.value == maxHp - player->GetComponent<ActorState>()->hp;

					if (not behavior->progress->getAction(U"chargeAni")->getStarted() and gage->value.value <9)
					{//チャージアニメーションを開始
						behavior->progress->restartAction(U"chargeEffect");
						behavior->progress->restartAction(U"chargeAni");
						behavior->progress->stopAction(U"normalAni");
					//	behavior->progress->getAction(U"attackAni")->endCondition->forcedFire();
						behavior->progress->stopAction(U"attackAni");
					}
					if (gage->value.value >= 9)
					{
						ora->visible = true;
						ora2->visible = true;
					}
				}
				else if (MouseR.up()and gage->value.value >= 9)
				{//発動
					behavior->progress->restartAction(U"bomb");
					behavior->GetComponent<ActorState>()->hp = maxHp;
					gage->value.ready = false;
					ora->visible = false; ora2->visible = false;
				}
				else
				{//チャージしていない
					gage->value.value = Max(gage->value.value - 360_deg*1.7 * dt, 0.0);
				}
			}
		, new prg::Condition()));
		behavior->progress->setAction(actions, U"bombAttack");
	}
	//hpに合わせて体の大きさを変えるアクション
	{
		auto actions = new prg::ManageActions;
		int32 max_hp = manager.params<double>(U"hp");
		double a = 0.2;//増え幅
		
		actions->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction*)
			{
				double scale = Min(1 + (max_hp - player->GetComponent<ActorState>()->hp) * a, 2.3);//(2 - player->GetComponent<ActorState>()->hp / max_hp);
				player->GetComponent<DrawTexture>()->scale = scale;
				player->GetComponent<Collider>()->setScale(scale);
			}
		, new prg::Condition()));
		behavior->progress->setAction(actions, U"life");
	}

	player->GetComponent<Collider>()->priority.setDraw(Math::InfF);
	return player;
}

Object* maker::CreateActors::MainEnemy(GameObjectManager* objectManager)
{
	Object* mainEnemy = MainActor(objectManager, U"MainEnemy");
	mainEnemy->tag = ObjectTag::enemy;

	Gmp manager = Gmp::createManager(U"common", U"Actors", U"MainActors", U"MainEnemy");

	//初期位置、当たり判定の設定
	mainEnemy->GetComponent<Transform>()->setPosAndPrePos({ Scene::Width() + 100,100 });
	mainEnemy->GetComponent<Collider>()->set_hitbox(Figure{ Circle{0,0,manager.params<double>(U"radius")} });

	auto behavior = mainEnemy->GetComponent<ObjectBehavior>();
	//体力
	behavior->GetComponent<ActorState>()->hp = manager.params<double>(U"hp");

	auto tex = behavior->AddComponentImmediately<DrawTexture>();

	behavior->progress->setAction(mitame::vibrateDirectin(tex, 10_deg, 3), U"yureru");
	behavior->progress->addAction(mitame::shaking(tex, 5, 2), U"yureru");

	tex->setTexture(U"UFO", 0.2);

	tex->gameCamera = objectManager->camera;

	{
		auto tamani = new prg::ManageActions;

		std::shared_ptr<double> eventTime(new double);
		auto genEvent = [] {
			return 240 + Random<double>(-180, 180);
		};
		double accum = 0;
		tamani->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction*)mutable {
				for (accum += dt; accum > *eventTime; accum = 0)
				{
					behavior->progress->restartAction(U"tamaniAnimation");
					*eventTime = genEvent();
				}
			},
			[=] {
				*eventTime = genEvent();
			}, Math::Inf
			));

		auto tamaniAnimation = new prg::ManageActions;

		tamaniAnimation->add(new prg::FuncAction([=] {tex->setAssetName(U"UFO1"); }, 0.2));
		tamaniAnimation->add(new prg::FuncAction([=] {tex->setAssetName(U"UFO2"); }, 2.5));
		tamaniAnimation->add(new prg::FuncAction([=] {tex->setAssetName(U"UFO1"); }, [=] {tex->setAssetName(U"UFO"); }, 0.1));

		behavior->progress->setAction(tamaniAnimation, U"tamaniAnimation");
		behavior->progress->setAction(tamani, U"tamani")->start();
	}
	behavior->progress->startAction(U"yureru");

	behavior->hitCallback->registerCallback({ ObjectTag::playerBullet },
		[](Object* self, Object* other)
		{
			self->GetComponent<ActorState>()->hp -= other->GetComponent<Bullet>()->getElements().damage;
		});

	auto effect = (new Object(objectManager))->AddComponentImmediately<ObjectBehavior>();
	effect->makeParent(behavior);
	effect->transform->setPos(behavior->transform->getPos());
	auto effectAction = new prg::ManageActions;
	auto effectFigs = effect->AddComponentImmediately<DrawFigures>();
	double accum = 0;
	effectAction->add(new prg::FuncAction(
		[=](double dt, prg::FuncAction*)mutable
		{//アップデート
			//0.17秒ごとに追加
			for (accum += dt; accum > 0.1; accum -= 0.1) {
				double dir = Random<double>(360_deg);
				auto fig = effectFigs->addFigure(U"obj", Circle{ 0,0,Random<double>(4,15) }, ColorF{ Palette::White,Random<double>(0.6,0.95) }, Random<double>(1, 0.8), Random<double>(360_deg), Vec2{ Cos(dir),Sin(dir) }*Random<double>(30, 60));
				fig->AddComponentImmediately<AdvanceMove>(new AdvanceMove(100))->start_action();
				fig->transform->setDirection((behavior->transform->getPos() - fig->transform->getPos()).getAngle() - 90_deg);
			}
			//オーブを　生成　削除
			for (auto it = effectFigs->figures.begin(); it != effectFigs->figures.end();)
			{
				auto& figure = *it;
				Vec2 d1 = behavior->transform->getPos() - figure.second->transform->getPos();
				Vec2 d2 = Vec2{ Cos(figure.second->transform->getDirection()),Sin(figure.second->transform->getDirection()) };
				if (d1.x * d2.x + d1.y * d2.y <= 0) {
					figure.second->GetComponent<ObjectBehavior>()->MainObject->eraseThis();
					it = effectFigs->figures.erase(it);
					continue;
				}
				else
				{
					figure.second->scale = Max(0.2, d1.length() / 60.0);
					figure.second->transform->setDirection(d1.getAngle() - 90_deg);
				}
				++it;
			}
		}, [] {
		},
		[=] {//終了
			effectFigs->clearFigures();
		},Math::Inf)
	);
	behavior->progress->setAction(effectAction, U"chargeEffect");

	util::setDissapearAction(behavior, 1.4, 0.3);

	return mainEnemy;
}

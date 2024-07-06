#include "stdafx.h"
#include "Game.h"
#include"GameObject/Actors.h"
#include"GameUtils.h"
#include"GameParameterManager/Init.h"
#include"Components/Transform.h"
#include"Components/Draw.h"
#include"Components/Collider.h"
#include"Barrages.h"

using namespace snk;

void snk::GameSceneParamInit()
{
	using namespace params_setter;
	set_nest(U"GameScene");
	double w, h, fw, fh; w = 950; h = 600; fw = w + 6 * 40; fh = h + 6 * 40;
	Vec2 p = { 1200 - w,0 };
	Vec2 fp = p - Vec2{ 120,120 };
	set_param(p, U"メインフィールドの左上座標", U"mainFieldPos");
	set_param(h, U"プレイヤーなどが動ける範囲の縦の長さ", U"mainFieldHeight");
	set_param(w, U"プレイヤーなどが動ける範囲の横の長さ", U"mainFieldWidth");
	set_param(fh, U"弾が存在できるフィールドの高さ", U"fieldHeight");
	set_param(fw, U"弾が存在できるフィールドの幅", U"fieldWidth");
	set_param(fp, U"弾が存在できるフィールドの左上座標", U"fieldPos");

	set_param(2.5, U"ゲーム開始までの時間", U"preparationTime");
	set_nest(U"GameScene", U"difficalty");
	set_param(2, U"easyモードのdifficaltyの値", U"1");
	set_param(5, U"normalモードのdifficaltyの値", U"2");
	set_param(9, U"hardモードのdifficaltyの値", U"3");

	set_nest(U"Barrages",U"SimpleRandomDirection");
	set_param(1, U"難易度調整係数(difficalty*difCoe)", U"difCoe");
	set_param(10, U"敵のhp", U"enemyHp");

	set_nest(U"Barrages", U"ConstantRandomDirection");
	set_param(1, U"難易度調整係数(difficalty*difCoe)", U"difCoe");
	set_param(8, U"敵のhp", U"enemyHp");

	set_nest(U"Barrages", U"ShotFollowTarget");
	set_param(1, U"難易度調整係数(difficalty*difCoe)", U"difCoe");
	set_param(10, U"敵のhp", U"enemyHp");

	set_nest(U"Barrages", U"Spiral01");
	set_param(1, U"難易度調整係数(difficalty*difCoe)", U"difCoe");
	set_param(10, U"敵のhp", U"enemyHp");

	set_nest(U"Barrages", U"Scatter01");
	set_param(1, U"難易度調整係数(difficalty*difCoe)", U"difCoe");
	set_param(17, U"敵のhp", U"enemyHp");

	set_nest(U"Barrages", U"binalyCircle");
	set_param(1, U"難易度調整係数", U"difCoe");
	set_param(14, U"敵のhp", U"enemyHp");

	set_nest(U"Barrages", U"combinationBarrage");
	set_param(1, U"難易度調整係数", U"difCoe");
	set_param(30, U"敵のhp", U"enemyHp");

	set_nest(U"Barrages", U"shootingStar");
	set_param(1, U"難易度調整係数", U"difCoe");
	set_param(18, U"敵のhp", U"enemyHp");

	set_nest(U"Barrages", U"sentakuki");
	set_param(1, U"難易度調整係数", U"difCoe");
}

Array<String> fileInDirectory(const String& directry,int32 size)
{
	Array<String> files;
	for (const auto& a : step(size))
	{
		files << directry+Format(a+1)+U".png";
	}
	return files;
}

void Game::registerGameAsset()
{
	r.type = RegisterResources::AssetType::texture;
	using namespace resource;
	//texture::Register(r(U"test"), U"example/siv3d-kun.png");
	texture::Register(r(U"cakeBullet"), U"resource/cakeBullet.png",TextureDesc::Mipped);
	texture::Register(r(U"cupBullet"), U"resource/cupCakeBullet.png", TextureDesc::Mipped);
	texture::Register(r(U"choco"), U"resource/choco.png", TextureDesc::Mipped);
	texture::Register(r(U"chocoUFO"), U"resource/chocoUFO.png", TextureDesc::Mipped);
	texture::Register(r(U"UFO"), U"resource/UFO.png", TextureDesc::Mipped);
	texture::Register(r(U"UFO1"), U"resource/UFO1.png", TextureDesc::Mipped);
	texture::Register(r(U"UFO2"), U"resource/UFO2.png", TextureDesc::Mipped);
	texture::Register(r(U"annna"), U"resource/annna.png", TextureDesc::Mipped);
	texture::Register(r(U"uiAnnna0"), U"resource/annnabl0.png", TextureDesc::Mipped);
	texture::Register(r(U"uiAnnna1"), U"resource/annnabl1.png", TextureDesc::Mipped);
	texture::Register(r(U"uiAnnna2"), U"resource/annnabl2.png", TextureDesc::Mipped);
	texture::Register(r(U"uiAnnna3"), U"resource/annnabl3.png", TextureDesc::Mipped);
	texture::Register(r(U"uiAnnna4"), U"resource/annnabl4.png", TextureDesc::Mipped);
	texture::Register(r(U"bat"), U"resource/bat.png", TextureDesc::Mipped);
	texture::Register(r(U"setumei"), U"resource/operating.png", TextureDesc::Mipped);
	texture::Register(r(U"selectUfo"), U"resource/selectUfo.png", TextureDesc::Mipped);
	texture::Register(r(U"buta"), U"resource/buta.png", TextureDesc::Mipped);
	for (const auto& filePath : fileInDirectory(U"resource/batAnimation/",9)) texture::Register(r(filePath), filePath, TextureDesc::Mipped);
	for (const auto& filePath : fileInDirectory(U"resource/batAnimation1/",9)) texture::Register(r(filePath), filePath, TextureDesc::Mipped);
	for (const auto& filePath : fileInDirectory(U"resource/water/",6)) texture::Register(r(filePath), filePath, TextureDesc::Mipped);
	for (const auto& filePath : fileInDirectory(U"resource/annnaAnimation/",4)) texture::Register(r(filePath), filePath, TextureDesc::Mipped);
	for (const auto& filePath : fileInDirectory(U"resource/annnaAttackAnimation/",5)) texture::Register(r(filePath), filePath, TextureDesc::Mipped);
	for (const auto& filePath : fileInDirectory(U"resource/annnaChargeAnimation/",2)) texture::Register(r(filePath), filePath, TextureDesc::Mipped);
	texture::Register(r(U"c1"), U"resource/cloud1.png", TextureDesc::Mipped);
	texture::Register(r(U"c2"), U"resource/cloud2.png", TextureDesc::Mipped);
	texture::Register(r(U"c3"), U"resource/cloud3.png", TextureDesc::Mipped);
	texture::Register(r(U"DoCloud"), U"resource/dcloud.png", TextureDesc::Mipped);
	texture::Register(r(U"AmCloud"), U"resource/acloud.png", TextureDesc::Mipped);
	texture::Register(r(U"PeCloud"), U"resource/pcloud.png", TextureDesc::Mipped);
	texture::Register(r(U"particle"), U"example/particle.png");
	r.type = RegisterResources::AssetType::audio;
	audio::Register(r(U"mainBgm"), U"resource/SnackUFO.mid", 0.5);
	AudioAsset::Register(r(U"eat"), Resource(U"resource/eat.mp3"));
	AudioAsset::Register(r(U"drinking"), Resource(U"resource/drinking.mp3"));
	AudioAsset::Register(r(U"batAttack"), Resource(U"resource/batAttack.mp3"));
	r.type = RegisterResources::AssetType::font;
	font::Register(r(U"pauseFont"), 30, Typeface::Bold);
	font::Register(r(U"loadingFont"), 20, Typeface::Medium);
	font::Register(r(U"creditFont"), 25, Typeface::Heavy);
}

Game::Game(const InitData& init)
	: IScene{ init }
{
	Init();
}

Game::~Game()
{
	//オブジェクトの状態を保存しておく必要があるならここにGameObjectManagerのdeleteは書かない(今回は1ステージだけなのでセーフ)
	delete manager;
	delete pauseObjectsManager;
	delete gameProgress;
	GameDataModel::Destroy();
}

void Game::Init()
{
	end = false;

	ableToPause = false;

	retry = false;
	//ゲーム進行アクションの生成
	gameProgress = new prg::ManageActions;

	//Scene::SetBackground(ColorF{ 204 / 256.0,229 / 256.0,255 / 256.0 });

	//ゲームデータのシングルトン
	GameDataModel::New(getData());

	//パラメータの取得
	auto param = GameParameterManager::createManager(U"common", U"GameScene");

	//難易度
	difficalty = param.getManager(U"difficalty").params<double>(Format(GameDataModel::getInstance().getData().mini_mode));

	//ゲームオブジェクトの管理クラスを生成
	manager = new GameObjectManager;
	pauseObjectsManager = new GameObjectManager;

	Vec2 mainFieldPos = param.params<Vec2>(U"mainFieldPos");
	double mainFieldW = param.params<double>(U"mainFieldWidth");
	double mainFieldH = param.params<double>(U"mainFieldHeight");

	//弾が存在できるフィールドを生成
	field = (new Object(manager));
	field->tag = ObjectTag::bulletField;
	auto field_hitbox = field->AddComponentImmediately<Collider>();
	field_hitbox->set_hitbox(RectF{ 0,0,param.params<double>(U"fieldWidth"),param.params<double>(U"fieldHeight") }, param.params<Vec2>(U"fieldPos") + Vec2{ param.params<double>(U"fieldWidth"),param.params<double>(U"fieldHeight") } / 2);
	field_hitbox->outline_color = Palette::White;
	field_hitbox->draw_hitbox = false;

	//プレイヤーなどが動けるフィールドを生成
	mainField = (new Object(manager));
	mainField->tag = ObjectTag::field;
	field_hitbox = mainField->AddComponentImmediately<Collider>();
	field_hitbox->set_hitbox(RectF{ 0,0,param.params<double>(U"mainFieldWidth"),param.params<double>(U"mainFieldHeight") }, param.params<Vec2>(U"mainFieldPos") + Vec2{ param.params<double>(U"mainFieldWidth"),param.params<double>(U"mainFieldHeight") } / 2);
	field_hitbox->outline_color = Palette::Yellow;
	field_hitbox->draw_hitbox = false;

	//カメラの生成
	camera = (new Object(manager))->AddComponentImmediately<Camera>();
	camera->scale = 1.0;
	camera->transform->setPosAndPrePos(Scene::Center());
	camera->center = Scene::Center();
	//カメラをセット
	manager->camera = camera;
	pauseObjectsManager->camera = camera;

	//ポーズ画面を構成
	{
		pauseObject = new Object(pauseObjectsManager);
		pauseObject->transform->setPosAndPrePos(param.params<Vec2>(U"mainFieldPos"));
		auto pauseBehavior = pauseObject->AddComponentImmediately<ObjectBehavior>();
		auto pauseWindow = pauseObject->AddComponentImmediately<DrawFigure>();
		pauseWindow->setFigure(RectF{ 0,0,param.params<double>(U"mainFieldWidth"),param.params<double>(U"mainFieldHeight") }, ColorF{ Palette::Black,0 });
		double time = 0.15;//fadeinのtime
		//フォントのベースカラー
		ColorF baseColor = Palette::Gray;
		//ポーズ画面またはゲームオーバー画面内のボタンを生成
		HashTable<String,Object*> buttons;
		HashTable<String, Object*> textObjects;
		Array<String> allTexts{ U"一時停止中（Enter->決定）",U"本当にいいですか？",U"ゲームオーバー（Enter->決定）"};
		Array<String> allButtonTexts{ U"一時停止を解除する",U"ゲームを終了する",U"最初からやり直す",U"ルール・攻略",U"戻る",U"はい",U"いいえ"};
		std::shared_ptr<Array<String>> buttonTexts(new Array<String>());
		std::shared_ptr<Array<String>> texts(new Array<String>());
		//テキストの共通設定
		for (const auto& text : allTexts)
		{
			auto textObject = new Object(pauseObjectsManager);
			auto font = textObject->AddComponentImmediately<DrawFont>();
			font->gameCamera = camera;
			font->center = false;
			font->setText(text);
			font->setAssetName(U"pauseFont");
			font->color = Palette::White; font->color.a = 0;
			auto behavior = textObject->AddComponentImmediately<ObjectBehavior>();
			auto fadeInOut = mitame::createFadeInAndOutAction(font, time);
			//fadeInアクション
			{
				behavior->progress->setAction(fadeInOut.fadeIn, U"fadeIn");
				auto slideAction = new prg::ManageActions;
				slideAction->add(new prg::FuncAction([=] {
					Vec2 nowPos = textObject->transform->getPos();
					auto slide = new prg::ManageActions;
					slide->add(util::transformPosition(behavior, { nowPos + Vec2{30,60} }, 0, time, false));
					behavior->progress->setAction(slide, U"slideOfFadeIn")->start();
					}));
				behavior->progress->addAction(slideAction, U"fadeIn");
			}
			//fadeOutアクション
			{
				behavior->progress->setAction(fadeInOut.fadeOut, U"fadeOut");
				auto slideAction = new prg::ManageActions;
				slideAction->add(new prg::FuncAction([=] {
					Vec2 nowPos = textObject->transform->getPos();
					auto slide = new prg::ManageActions;
					slide->add(util::transformPosition(behavior, { nowPos - Vec2{30,60} }, 0, time, false));
					behavior->progress->setAction(slide, U"slideOfFadeOut")->start();
					}));
				behavior->progress->addAction(slideAction, U"fadeOut");
			}

			textObjects.emplace(text, textObject);
		}
		//ボタンの共通設定
		for (const auto& text : allButtonTexts)
		{
			auto button = new Object(pauseObjectsManager);
			auto font = button->AddComponentImmediately<DrawFont>();
			font->gameCamera = camera;
			font->center = false;
			font->setText(text);
			font->setAssetName(U"pauseFont");
			font->color = baseColor; font->color.a = 0;
			auto behavior=button->AddComponentImmediately<ObjectBehavior>();
			auto fadeInOut = mitame::createFadeInAndOutAction(font, time);
			//ボタンのfadeInアクション
			{
				behavior->progress->setAction(fadeInOut.fadeIn, U"fadeIn");
				auto slideAction = new prg::ManageActions;
				slideAction->add(new prg::FuncAction([=] {
					Vec2 nowPos = button->transform->getPos();
					auto slide = new prg::ManageActions;
					slide->add(util::transformPosition(behavior, { nowPos + Vec2{-30,0} }, 0, time, false));
					behavior->progress->setAction(slide, U"slideOfFadeIn")->start();
				}));
				behavior->progress->addAction(slideAction, U"fadeIn");
			}
			//ボタンのfadeOutアクション
			{
				behavior->progress->setAction(fadeInOut.fadeOut, U"fadeOut");
				auto slideAction = new prg::ManageActions;
				slideAction->add(new prg::FuncAction([=] {
					Vec2 nowPos = button->transform->getPos();
					auto slide = new prg::ManageActions;
					slide->add(util::transformPosition(behavior, { nowPos + Vec2{30,0} }, 0, time, false));
					behavior->progress->setAction(slide, U"slideOfFadeOut")->start();
				}));
				behavior->progress->addAction(slideAction, U"fadeOut");
			}
			//当てられたときの動き 横に揺れて光る
			{
				auto shakeAction = mitame::shaking(font, 4, 0.05, 0_deg);
				shakeAction->loop = false;

				shakeAction->addParallel(new prg::FuncAction(
					[=] {
						font->color = Palette::White;
					},
					[=] {
						font->color = baseColor;
					},
					Math::Inf
				));
				shakeAction->init = true;
				behavior->progress->setAction(shakeAction, U"attention");
			}
			buttons.emplace(text, button);
		}
		
		const Vec2 basePos = param.params<Vec2>(U"mainFieldPos") + Vec2{ 150 ,param.params<double>(U"mainFieldHeight") * 4 / 7.0 };
		const double interval = param.params<double>(U"mainFieldHeight") * 3 / 35.0;

		//セレクトUFO
		auto ufoBehavior = (new Object(pauseObjectsManager))->AddComponentImmediately<ObjectBehavior>();
		auto ufoTex = ufoBehavior->AddComponentImmediately<DrawTexture>();
		ufoTex->gameCamera = camera;
		ufoTex->color = ColorF{ Palette::White,0 };
		ufoTex->setTexture(U"selectUfo", 0.3);
		auto ufoFadeInOut = mitame::createFadeInAndOutAction(ufoTex, time);
		ufoBehavior->transform->setPos(basePos - Vec2{ 90,70 } - Vec2{ 70,-15 });
		//fadeInアクション
		{
			ufoBehavior->progress->setAction(ufoFadeInOut.fadeIn, U"fadeIn");
			auto slideAction = new prg::ManageActions;
			slideAction->add(new prg::FuncAction([=] {
				Vec2 nowPos = ufoBehavior->transform->getPos();
				auto slide = new prg::ManageActions;
				slide->add(util::transformPosition(ufoBehavior, { nowPos + Vec2{90,70} }, 0, time, false));
				ufoBehavior->progress->setAction(slide, U"slideOfFadeIn")->start();
				}));
			ufoBehavior->progress->addAction(slideAction, U"fadeIn");
		}
		//fadeOutアクション
		{
			ufoBehavior->progress->setAction(ufoFadeInOut.fadeOut, U"fadeOut");
			auto slideAction = new prg::ManageActions;
			slideAction->add(new prg::FuncAction([=] {
				Vec2 nowPos = ufoBehavior->transform->getPos();
				auto slide = new prg::ManageActions;
				slide->add(util::transformPosition(ufoBehavior, { nowPos - Vec2{90,70} }, 0, time, false));
				ufoBehavior->progress->setAction(slide, U"slideOfFadeOut")->start();
				}));
			ufoBehavior->progress->addAction(slideAction, U"fadeOut");
		}

		//はいを押したときのコールバック
		std::shared_ptr<std::function<void()>> yesCallback(new std::function<void()>);
		//はい、いいえのフェードインフェードアウト
		{
			auto yesOrNoAction = new prg::ManageActions;
			std::shared_ptr<Array<String>> firstButtonSets(new Array<String>());
			yesOrNoAction->add(new prg::FuncAction(
				[=]()mutable {
					for (String text : *buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
					for (String text : *texts)textObjects[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
					ufoBehavior->progress->restartAction(U"fadeOut");
					*firstButtonSets=*buttonTexts;
					*buttonTexts = { U"はい",U"いいえ" };
					double interval2 = 150;
					int32 count = 0;
					for (String text : *buttonTexts) {
						buttons[text]->transform->setPosAndPrePos(mainFieldPos + Vec2{ mainFieldW,mainFieldH } / 2 + Vec2{ 30 - interval2 * (0.5 - count),0 });
						buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
						++count;
					}
					textObjects[U"本当にいいですか？"]->transform->setPosAndPrePos(mainFieldPos + Vec2{mainFieldW,mainFieldH} / 2 + Vec2{0,-100} - Vec2{90,60});
					textObjects[U"本当にいいですか？"]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
				}, time
			));
			std::shared_ptr<int32> selecting(new int32);
			//終了したらtrue
			yesOrNoAction->add(new prg::FuncAction(
				[=](double, prg::FuncAction* act)mutable {
					int32 preSelecting = *selecting;
					if (GameDataModel::getData().KeyLeft.down())--(*selecting);
					else if(GameDataModel::getData().KeyRight.down())++(*selecting);

					if (*selecting < 0)*selecting += 2;
					*selecting %= 2;

					if (KeyEnter.down())act->endCondition->forcedFire();//終了

					//対応したボタンを揺らす
					if (*selecting != preSelecting) {
						buttons[(*buttonTexts)[preSelecting]]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
						buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
					}
				},
				[=]()mutable{
					*selecting = 1;
					buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
				},
				Math::Inf
			));
			yesOrNoAction->add(new prg::FuncAction(
				[=]()mutable {//フェードアウトさせる
					for (String text : *buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
					textObjects[U"本当にいいですか？"]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
				}, [=]()mutable {
					*buttonTexts = *firstButtonSets;
					//元に戻す
					for (String text : *buttonTexts) {
						buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
					}
					for (String text : *texts) {
						textObjects[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
					}

					ufoBehavior->progress->restartAction(U"fadeIn");

					//コールバック実行
					if (*selecting == 0)(*yesCallback)();
				}, time
			));
			pauseBehavior->progress->setAction(yesOrNoAction, U"yesNo");
		};
		//ポーズ画面のフェードイン・アウト
		auto fadeInOut=mitame::createFadeInAndOutAction(pauseWindow, time, 0, 0.5);

		auto& fadeIn = fadeInOut.fadeIn;

		fadeIn->init = true;
		//ポーズ
		auto pauseButtonSetting = new prg::ManageActions;
		pauseButtonSetting->add(new prg::FuncAction([=]()mutable {
			*buttonTexts = { U"一時停止を解除する",U"ルール・攻略", U"ゲームを終了する", U"最初からやり直す"};
			*texts = { U"一時停止中（Enter->決定）" };
			},[=] {fadeIn->start();	}
		));
		//ポーズ開始条件のセット
		pauseButtonSetting->setStartCondition(new prg::FuncCondition([=] {return ((not GameDataModel::getInstance().pause) and KeyEscape.down() and ableToPause); }));

		auto gameOverTex = (new Object(pauseObjectsManager))->AddComponentImmediately<DrawTexture>();
		gameOverTex->setTexture(U"buta",0.75);
		gameOverTex->gameCamera = camera;
		gameOverTex->transform->setPos(mainFieldPos + Vec2{ mainFieldW * 4 / 5.0,mainFieldH * 3 / 4.0 });
		gameOverTex->color = ColorF{ 0.9,0.9,0.9,0 };
		gameOverTex->AddComponentImmediately<ObjectBehavior>()->progress->setAction(mitame::createFadeInAndOutAction(gameOverTex, time).fadeIn, U"fadeIn");
		std::shared_ptr<bool> gameOver(new bool(false));
		//ゲームオーバー
		auto gameOverButtonSetting = new prg::ManageActions;
		gameOverButtonSetting->add(new prg::FuncAction([=]()mutable {
			gameOverTex->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
			ableToPause = false;
			*gameOver = true;
			*buttonTexts = { U"ゲームを終了する", U"最初からやり直す",U"ルール・攻略"};
			*texts = { U"ゲームオーバー（Enter->決定）" };
			},[=] {	fadeIn->start();}
		));
		//ゲームオーバーの開始条件は体重計の処理で書いている(針の位置を上にセットしてからポーズに入るため)

		//ポーズ画面のフェードインと同時にボタンもフェードイン
		fadeIn->addParallel(new prg::FuncAction([=]()mutable {
			GameDataModel::getInstance().pause = true;
			AudioAsset(U"mainBgm").pause();
			ufoBehavior->progress->restartAction(U"fadeIn");
			pauseBehavior->progress->resetAction(U"pause", U"gameover");
			int32 count = 0;
			for (String text : *buttonTexts){
				buttons[text]->transform->setPosAndPrePos(basePos + Vec2{ 0,interval } * count);
				buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
				++count;
			}
			for (String text : *texts) {
				textObjects[text]->transform->setPosAndPrePos(basePos - Vec2{0,60} - Vec2{90,70});
				textObjects[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
			}
		}, time));
		//ルール・攻略の説明
		{
			auto a = (new Object(pauseObjectsManager));
			a->z = Math::Inf;
			auto image = a->AddComponentImmediately<DrawTexture>();
			image->transform->setPos(Scene::Center() + Vec2{ 0,-50 });
			image->gameCamera = camera;
			image->setTexture(U"slide", 0.9);
			image->visible = false;
			auto tmp = new prg::ManageActions;
			std::shared_ptr<Array<String>> firstButtonSets(new Array<String>());
			tmp->add(new prg::FuncAction(
				[=]()mutable {
					for (String text : *buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
					*firstButtonSets = *buttonTexts;
					*buttonTexts = { U"戻る" };
					int32 count = 0;
					for (String text : *buttonTexts) {
						buttons[text]->transform->setPosAndPrePos(basePos + Vec2{ -200,500 });
						buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
						++count;
					}
				}, 0.5
			));
			std::shared_ptr<int32> selecting(new int32);

			tmp->add(new prg::FuncAction(
				[=](double, prg::FuncAction* act)mutable {
					if (KeyEnter.down())act->endCondition->forcedFire();//終了
				},
				[=]()mutable {
					*selecting = 0;
					image->visible = true;
					buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
				},
					Math::Inf
					));
			tmp->add(new prg::FuncAction(
				[=]()mutable {//フェードアウトさせる
					buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
					for (String text : *buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
				}, [=]()mutable {
					*buttonTexts = *firstButtonSets;
					//元に戻す
					for (String text : *buttonTexts) {
						buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
					}
					image->visible = false;
				}, time
					));
			pauseBehavior->progress->setAction(tmp, U"exp");
		}
		//ポーズ画面の処理
		std::shared_ptr<int32> selecting(new int32);
		auto pauseSelectObj = (new Object(pauseObjectsManager))->AddComponentImmediately<ObjectBehavior>();
		auto selectAction = new prg::ManageActions;
		selectAction->add(new prg::FuncAction(
			[=](double, prg::FuncAction* act)mutable {
				//はいいいえの選択時は動かさない
				if (pauseBehavior->progress->getAction(U"yesNo")->isActive()
					or pauseBehavior->progress->getAction(U"exp")->isActive())return;

				ufoBehavior->transform->setPos({ ufoBehavior->transform->getPos().x ,buttons[(*buttonTexts)[*selecting]]->transform->getPos().y + 10 });

				int32 preSelect = *selecting;
				if (GameDataModel::getData().KeyDown.down()) ++(*selecting);
				if (GameDataModel::getData().KeyUp.down()) --(*selecting);
				//ボタンを巡回する
				if (*selecting < 0)*selecting += buttonTexts->size();
				*selecting %= buttonTexts->size();
				//Escを押したらポーズ解除
				if (KeyEscape.down() and not(*gameOver)) act->endCondition->forcedFire();
				
				const auto& text = (*buttonTexts)[*selecting];
				//EnterKeyを押したらボタンに対応した処理を行う
				if (KeyEnter.down()) {
					if (text == U"一時停止を解除する") {
						act->endCondition->forcedFire();
					}
					else if (text == U"ゲームを終了する") {
						*yesCallback = [=] {EndGame(false); };
						pauseBehavior->progress->restartAction(U"yesNo");

					}
					else if (text == U"最初からやり直す") {
						*yesCallback = [=] { retry = true; };
						pauseBehavior->progress->restartAction(U"yesNo");
					}
					else if (text == U"ルール・攻略")
					{
						pauseBehavior->progress->restartAction(U"exp");
					}
				}
				
				//対応したボタンを揺らす
				if (*selecting != preSelect) {
					buttons[(*buttonTexts)[preSelect]]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
					buttons[text]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
				}
			},
			[=]()mutable {//開始
				*selecting = 0;
				buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
			},
			[=]()mutable {//終了
				buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
				//フェードアウトを開始する
				pauseBehavior->progress->startAction(U"fadeOut");
			},
			Math::Inf
		));
		//ポーズ選択
		fadeIn->add(selectAction);

		auto& fadeOut = fadeInOut.fadeOut;

		fadeOut->init = true;
		//ポーズ画面のフェードアウトに合わせてボタンもフェードアウト
		//ポーズの終了処理
		fadeOut->addParallel(new prg::FuncAction([=]()mutable {
			for(String text:*buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
			for(String text:*texts)textObjects[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
			ufoBehavior->progress->restartAction(U"fadeOut");
			}, [=] {
				GameDataModel::getInstance().pause = false;
				AudioAsset(U"mainBgm").play();
			}, time));

		pauseBehavior->progress->setAction(fadeIn, U"fadeIn");
		pauseBehavior->progress->setAction(fadeOut, U"fadeOut");
		pauseBehavior->progress->setAction(pauseButtonSetting, U"pause");
		pauseBehavior->progress->setAction(gameOverButtonSetting, U"gameover");
	}

	//プレイヤーの生成
	player = maker::CreateActors::Player(manager);
	//敵の生成
	mainEnemy = maker::CreateActors::MainEnemy(manager);

	std::shared_ptr<double> enemyMaxHp(new double(0));

	//ゲーム画面外 カメラに追従するようにしている
	{
		auto frame = (new Object(manager))->AddComponentImmediately<ObjectBehavior>();
		frame->MainObject->tag = ObjectTag::ui;
		frame->makeParent(camera);
		frame->MainObject->z = 90;
		auto frameFig = frame->AddComponentImmediately<DrawFigure>();
		frameFig->setFigure(Geometry2D::Or(
			RectF{ {0,0},param.params<Vec2>(U"mainFieldPos").x,Scene::Height() }.asPolygon(),
			RectF{ {param.params<Vec2>(U"mainFieldPos").x,param.params<double>(U"mainFieldHeight")},Scene::Width() - param.params<Vec2>(U"mainFieldPos").x,Scene::Height() - param.params<double>(U"mainFieldHeight") }.asPolygon())[0]
			, Palette::Wheat);

		Vec2 center{ 460,38 };//体重計の中心
		double scale = 0.82;//全体の大きさ
		//プレイヤーの体力などを表すゲージ
		auto gage = (new Object(manager))->AddComponentImmediately<ObjectBehavior>();
		gage->MainObject->tag = ObjectTag::ui;
		gage->makeParent(camera);
		gage->MainObject->z = 101;
		auto fig = gage->AddComponentImmediately<DrawFigures>();
		//吹き出し
		auto tr = Triangle{ {0,0}, Vec2{50,0}, Vec2{50,30} }.rotatedAt({ 0,0 }, 0_deg).moveBy({ 320,3 });
		auto ci = Circle{ center,110 };
		auto f=fig->addFigure(U"hukidashi", Geometry2D::Or(tr.asPolygon(), ci.asPolygon(80))[0], Palette::White, scale);
		f->drawFrame = true;f->frameThick = 4.0;f->frameColor = Palette::Paleturquoise;
		//体重計 針なし
		auto outCi = Circle{ center,90 };
		auto inCi = Circle{ center,78 };
		auto dangerZone = inCi.pieAsPolygon(270_deg,90_deg);
		auto centerCi = Circle{ center,8 };
		fig->addFigure(U"taijuukeiFrame", Geometry2D::Subtract(outCi.asPolygon(80), inCi.asPolygon(70))[0], Palette::Gray, scale);
		fig->addFigure(U"dangerZone", dangerZone, Palette::Yellow, scale);
		fig->addFigure(U"centerCi", centerCi, Palette::Red, scale);
		for (auto& k : s3d::step(12))
		{
			RectF l{ Arg::leftCenter(center + Vec2{Cos(k * 30_deg),Sin(k * 30_deg)}*63),8,4 };
			fig->addFigure(U"memori", l.rotatedAt(center + Vec2{ Cos(k * 30_deg),Sin(k * 30_deg) }*63, k * 30_deg), Palette::Darkgray, scale);
		}
		//体重計の針
		fig->addFigure(U"hari", Triangle{ Vec2{-4.5,0},Vec2{4.5,0},Vec2{0,-60} }.movedBy(center), Palette::Red, scale);
		//針の動き
		auto gageAction = new prg::ManageActions;
		double playerMaxHp = GameParameterManager::createManager(U"common", U"Actors", U"MainActors", U"Player").params<double>(U"hp");
		const double maxOffset = 10_deg;
		double offset = 0;//dRadのoffsetCoef倍通りすぎる
		const double offsetCoef = 0.7;//振動の減衰率
		const double v = 360_deg * 0.7;//針が動く速さ
		const double vibThreadhold = 0.5;//|hp-prehp|がこの値より大きくなったら揺らす
		const double threadhold = 0.8_deg;//針の揺れがこの値より小さければ止める
		double preHp = playerMaxHp;
		gageAction->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction*)mutable {
				double dRad = 360_deg * (1 - player->GetComponent<ActorState>()->hp / playerMaxHp) - fig->figures[U"hari"]->rad;
				if (player->GetComponent<ActorState>()->hp <= 0)
				{
					fig->figures[U"hari"]->rad = 360_deg;
					pauseObject->GetComponent<ObjectBehavior>()->progress->startAction(U"gameover");
					return;
				}
				//動かす
				fig->figures[U"hari"]->rad += v * Math::Sign(offset + dRad) * dt;
				//揺らすかどうか
				if (Math::Abs(player->GetComponent<ActorState>()->hp - preHp) > vibThreadhold)
				{
					offset = maxOffset;//ずれを代入
					offset *= Math::Sign(dRad);//目的地の方向へ
				}
				preHp = player->GetComponent<ActorState>()->hp;
				//ずれた分を過ぎたら
				if (Math::Abs(offset + dRad) <= Math::Abs(dRad) and Math::Abs(offset) <= Math::Abs(dRad))
				{
					offset *= -Math::Sign(offset);//反転
					offset *= offsetCoef;//減衰
				}
				//offsetが閾値を下回ったら
				if (Math::Abs(offset) < threadhold)
				{
					offset = 0;
					fig->figures[U"hari"]->rad = 360_deg * (1 - player->GetComponent<ActorState>()->hp / playerMaxHp);
				}
			},
			[=] {
				fig->figures[U"hari"]->rotateCenter = center;
			},
				Math::Inf
				));
		gage->progress->addAction(gageAction, U"hariMovement")->start();
		//ゲージがたまったときに点滅させる
		auto readyCharge = new prg::ManageActions;
		readyCharge->add(new prg::FuncAction(
			[=] {
				//figureに点滅アクションをセット
				for (auto it = fig->figures.begin(); it != fig->figures.end();++it)
				{
					auto& figure = *it;
					if (figure.first.split(U'-')[0] == U"spGage")
					{
						gage->progress->addAction(mitame::constantFlashing(figure.second, ColorF{ 0.3,0.3,0.7,1 }, 1), U"flashing");
					}
				}
				gage->progress->startAction(U"flashing");
			},
			[=] {
				gage->progress->setAction(new prg::ManageActions, U"flashing");//消す
			},
			new prg::FuncCondition([=] {
				//必殺技が放たれたら終了
				return not player->GetComponent<Status<chargeGage>>(U"chargeGage")->value.ready;
			})
		));
		readyCharge->init = true;
		gage->progress->setAction(readyCharge, U"chargeFlashing");
		auto colision = new prg::ManageActions;
		auto hibana=gage->AddComponentImmediately<DrawFunc>();
		hibana->priority.setDraw(Math::Inf);
		std::shared_ptr<mitame::SparkParticle> spark(new mitame::SparkParticle);
		colision->add(new prg::FuncAction(
			[=] {
				//はりの位置に火花を散らせる
				double rad = fig->figures[U"hari"]->rad-80_deg;
				spark->setPosition(50 * Vec2{ Cos(rad),Sin(rad) }*scale + fig->figures[U"centerCi"]->transform->getPos()+center*scale);
				spark->setRate(20);
				spark->setSpeed(50);
				spark->setDirection(rad/1_deg);
				hibana->drawingFunction =
				[=](const DrawFunc*) {
					spark->draw();
				};
			},
			[=] {
				gage->progress->setAction(new prg::ManageActions, U"flashing");//消す
				spark->setRate(0);
			},
			new prg::FuncCondition([=] {
				//ゲージが針と衝突していなければ終了
				return not player->GetComponent<Status<chargeGage>>(U"chargeGage")->value.colision;
			})
		));
		colision->init = true;
		gage->progress->setAction(colision, U"hibana");
		//必殺技ゲージの動き
		auto spGageAction = new prg::ManageActions;
		double radStep = 1.5_deg;
		spGageAction->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction*) {
				spark->update(dt);//火花のアップデート
				if (player->GetComponent<Status<chargeGage>>(U"chargeGage")->value.ready) {
					gage->progress->startAction(U"chargeFlashing");
					return;
				}
				if (player->GetComponent<Status<chargeGage>>(U"chargeGage")->value.colision){
					gage->progress->startAction(U"hibana");
					return;
				}
				for (auto it = fig->figures.begin(); it != fig->figures.end();)
				{
					auto& figure = *it;
					if (figure.first.split(U'-')[0] == U"spGage")
					{
						figure.second->GetComponent<ObjectBehavior>()->MainObject->eraseThis();
						it = fig->figures.erase(it);
					}
					else
					{
						++it;
					}
				}
				for (auto i : s3d::step(int32(player->GetComponent<Status<chargeGage>>(U"chargeGage")->value.value / playerMaxHp * 360_deg / radStep)))
				{
					fig->addFigure(U"spGage-" + Format(i), Circle{ center,70 }.arcAsPolygon(i* radStep, radStep, 15, 5), HSV{ i * radStep / 1_deg }, scale)->priority.setDraw(0);
				}
			},
			Math::InfF
		));

		gage->progress->addAction(spGageAction, U"spGageMovement")->start();

		gage->transform->setPos({ -10,Scene::Height() - 130 });

		auto annna = (new Object(manager))->AddComponentImmediately<ObjectBehavior>();

		annna->MainObject->tag = ObjectTag::ui;

		annna->makeParent(camera);

		auto tex = annna->AddComponentImmediately<DrawTexture>();

		tex->Parent->z = 100;

		tex->gameCamera = camera;

		tex->setTexture(U"uiAnnna0", 0.52);

		annna->transform->setPos({ 155,430 });
		//アンナちゃんの表情を変える
		auto annnaVariation = new prg::ManageActions;
		annnaVariation->add(new prg::FuncAction([=](double,prg::FuncAction*) {
			double hpRate = 1-player->GetComponent<ActorState>()->hp / playerMaxHp;
			int32 num=int32(hpRate * 4.0);
			tex->setAssetName(U"uiAnnna" + Format(num));
		},Math::Inf));
		//操作説明の絵
		auto sousa = (new Object(manager));
		sousa->tag = ObjectTag::ui;
		auto sousaTex=sousa->AddComponentImmediately<DrawTexture>(U"sousa");
		sousa->makeParent(camera->MainObject);
		sousa->z = 100;
		sousa->transform->setPos(Vec2{ Scene::Size() } - Vec2{ 360,70 });
		sousaTex->setTexture(U"setumei", 0.5);
		sousaTex->gameCamera = camera;
		gage->progress->setAction(annnaVariation, U"variation")->start();
	}

	//以下ゲームの進行を設定
	//ローディング画面の作成
	auto loadScene = (new Object(manager));
	auto cake = (new Object(manager));
	gameProgress->add(new prg::FuncAction(
		[=] {
			loadScene->z = 300;
			cake->z = 301;
			auto rect = loadScene->AddComponentImmediately<DrawFigure>();
			rect->setFigure(Rect{ 0,0,Scene::Size() },Palette::Black);
			cake->transform->setPos(Scene::Size() - Vec2{ 170,150 });
			auto tex = cake->AddComponentImmediately<DrawTexture>();
			//ケーキの絵をロード
			TextureAsset::Load(U"cakeBullet");
			tex->setTexture(U"cakeBullet",0.4);
			tex->gameCamera = camera;
			//ケーキの絵を回転させる
			auto loadBehavior = loadScene->AddComponentImmediately<ObjectBehavior>();
			//文字
			FontAsset::Load(U"loadingFont");
			auto font = loadScene->AddComponentImmediately<DrawFont>();
			font->setText(U"なうろーりんぐ");
			font->setAssetName(U"loadingFont");
			font->pos = Scene::Size() - Vec2{ 170,230 };
			font->gameCamera = camera;
			//フェードアウト
			auto fadeOut = mitame::createFadeInAndOutAction(rect, 0.8).fadeOut;
			fadeOut->addParallel(new prg::FuncAction([=] {font->visible = false; }));
			fadeOut->add(new prg::FuncAction([=] {loadScene->eraseThis(); }),0.8);
			loadBehavior->progress->setAction(fadeOut, U"fadeOut");
			util::setDissapearAction(cake->AddComponentImmediately<ObjectBehavior>(), 1.4, 0.5)
				->add(new prg::FuncAction([=]
				{
					loadBehavior->progress->restartAction(U"fadeOut");
				}));
			cake->GetComponent<ObjectBehavior>()->progress->setAction(mitame::rotational(tex, 0.4), U"rotateCake")->start();			
		}
	));
	//ローディング 
	registerGameAsset();
	std::shared_ptr<bool> completeLoading(new bool(false));
	gameProgress->add(new prg::FuncAction(
		[=](double dt, prg::FuncAction*)
		{
			for (const auto& name : r.textures)
			{
				if (not TextureAsset::IsReady(name)) {
					*completeLoading = false;
					return;
				}
			}
			for (const auto& name : r.fonts)
			{
				if (not FontAsset::IsReady(name)) {
					*completeLoading = false;
					return;
				}
			}
			for (const auto& name : r.audios)
			{
				if (not AudioAsset::IsReady(name)) {
					*completeLoading = false;
					return;
				}
			}
			
			*completeLoading = true;
		},
		[=] {
			for (const auto& name : r.textures)
			{
				TextureAsset::LoadAsync(name);
			}
			for (const auto& name : r.fonts)
			{
				FontAsset::LoadAsync(name);
			}
			for (const auto& name : r.audios)
			{
				AudioAsset::LoadAsync(name);
			}
		},
		[=] {
			//loadScene->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
			cake->GetComponent<ObjectBehavior>()->progress->restartAction(U"dissapearAction");
		},
		new prg::FuncCondition([=]() {
			return *completeLoading;
		}
	)));

	//キャラクター登場
	{
		auto preparation = new prg::FuncAction([&]
			{
				player->GetComponent<ObjectBehavior>()->progress->startAction(U"preparationMove");
				mainEnemy->GetComponent<ObjectBehavior>()->progress->startAction(U"preparationMove");
			});
		gameProgress->add(preparation);
		gameProgress->add(new prg::FuncAction([&] {
				//音楽再生
				AudioAsset(U"mainBgm").seekTime(0.5);
				AudioAsset(U"mainBgm").setVolume(GameDataModel::getData().BGM_volume*2).play();
			}), 0.6);
	}
	//ゲーム開始
	{
		auto startGame = new prg::FuncAction(
			[&] {
				auto playerBehavior = player->GetComponent<ObjectBehavior>();
				playerBehavior->progress->startAction(U"Move");
				playerBehavior->progress->startAction(U"Attack");
				playerBehavior->progress->startAction(U"bombAttack");
				playerBehavior->progress->startAction(U"life");
				
				//ポーズを可能に
				ableToPause = true;
			});
		gameProgress->add(startGame, param.params<double>(U"preparationTime"));
	}
	GameParameterManager barragePManager = GameParameterManager::createManager(U"common", U"Barrages");
	//メインウェポン取得
	auto mainWeapon = mainEnemy->GetComponent<MainWeapon>();
	//Behaviorを取得
	auto behavior = mainEnemy->GetComponent<ObjectBehavior>();

	mainWeapon->gun->transform->setPosAndPrePos(behavior->transform->getPos());

	auto setEnemyHp = [=](double maxHp) {
		double dif = difficalty * 0.1 + 0.2;
		*enemyMaxHp = maxHp * dif;
		mainEnemy->GetComponent<ActorState>()->hp = maxHp*dif;
	};
	//銃を消す　弾は消さない
	auto eraseEnemyAllGuns = [=] {
		for (auto& gun : mainEnemy->GetComponent<MainWeapon>()->gun->otherGuns)gun->MainObject->eraseThis();
		mainEnemy->GetComponent<MainWeapon>()->gun->otherGuns.clear();
	};
	//最初の弾幕
	{
		String barrageName = U"SimpleRandomDirection";
		auto pManager = barragePManager.getManager(barrageName);
		auto barrageAction = new prg::ManageActions;
		double maxHp = pManager.params<double>(U"enemyHp");
		//アクションをセット
		barrageAction->add(new prg::FuncAction(
			[=](double, prg::FuncAction*act)
			{//更新処理
				mainWeapon->gun->transform->setDirection((player->transform->getPos() - mainEnemy->transform->getPos()).getAngle() - 90_deg);
				if (mainEnemy->GetComponent<ActorState>()->hp <= 0) barrageAction->endCondition->forcedFire();
			},
			[=]()
			{//開始処理
				setEnemyHp(maxHp);
				//弾幕を準備
				behavior->progress->setAction(startUpBarrage(barrageName), barrageName);
				//メインウェポンを敵の位置にセット
				mainWeapon->gun->transform->setPosAndPrePos(behavior->transform->getPos());
				//敵の動きをセットし、開始する
				Point range = { 200,500 };
				auto m = new prg::ManageActions;
				m->add(util::rectRandomTransform(behavior, Vec2{ param.params<Vec2>(U"mainFieldPos").x+param.params<double>(U"mainFieldWidth") - (50 + range.x),(param.params<double>(U"mainFieldHeight") - range.y) / 2}, range.x, range.y, true, 4, 2, 8));
				behavior->progress->setAction(m, U"Movement");
				behavior->progress->startAction(barrageName, U"Movement");
			},
			[=]()
			{//終了処理
				eraseEnemyAllGuns();
				behavior->progress->resetAllAction(HashSet<String>{ U"yureru" });
			},
			Math::Inf));
	
		gameProgress->add(barrageAction);
	}
	//連射
	{
		String barrageName = U"ConstantRandomDirection";
		auto pManager = barragePManager.getManager(barrageName);
		double maxHp = pManager.params<double>(U"enemyHp");
		auto barrageAction = new prg::ManageActions;
		//アクションをセット
		barrageAction->add(new prg::FuncAction(
			[=]()
			{//スタート
				setEnemyHp(maxHp);
				//メインウェポンをリセット(弾は消えない)
				auto mainWeapon = mainEnemy->GetComponent<MainWeapon>();
				mainWeapon->gun->eraseAllGuns();
				mainWeapon->gun = (new Object(manager))->AddComponentImmediately<PrimitiveGun>();
				mainWeapon->gun->transform->setPosAndPrePos(mainEnemy->transform->getPos());
				mainWeapon->gun->makeParent(mainEnemy->GetComponent<ObjectBehavior>());
				
				//初期位置に移動
				behavior->progress->startAction(U"preparationMove");
				behavior->progress->getAction(U"preparationMove")->timeScale=0.7;
			}
		));
		//移動が終わるまで待つ
		barrageAction->add(new Wait(behavior->progress->getAction(U"preparationMove")));
		//弾幕を開始
		barrageAction->add(new prg::FuncAction(
			[=](double,prg::FuncAction*){
				if (mainEnemy->GetComponent<ActorState>()->hp <= 0) barrageAction->endCondition->forcedFire();
			},
			[=]()
			{//開始処理
				//弾幕を開始
				behavior->progress->setAction(startUpBarrage(barrageName), barrageName)->start();
			},Math::Inf));

		gameProgress->add(barrageAction);
	}
	//自機狙い
	{
		String barrageName = U"ShotFollowTarget";
		auto pManager = barragePManager.getManager(barrageName);
		double maxHp = pManager.params<double>(U"enemyHp");
		auto barrageAction = new prg::ManageActions;
		//アクションをセット
		barrageAction->add(new prg::FuncAction([=] { setEnemyHp(maxHp); }));
		barrageAction->add(util::transformPosition(behavior, { Vec2{param.params<Vec2>(U"mainFieldPos").x+param.params<double>(U"mainFieldWidth") - 150,param.params<double>(U"mainFieldHeight") / 2}}, 1, 1, false));
		barrageAction->add(new prg::FuncAction(
			[=](double, prg::FuncAction*) {
				if (mainEnemy->GetComponent<ActorState>()->hp <= 0) barrageAction->endCondition->forcedFire();
			},
			[=]
			{//開始処理
				//弾幕を準備
				behavior->progress->setAction(startUpBarrage(barrageName), barrageName);
				behavior->progress->startAction(barrageName);
			},
			[=]
			{
				behavior->progress->stopAllAction({U"yureru",U"tamani"});
				eraseEnemyAllGuns();
			},
			Math::Inf));
		gameProgress->add(barrageAction);
	}
	//渦巻弾幕
	{
		String barrageName = U"Spiral01";
		auto pManager = barragePManager.getManager(barrageName);
		double maxHp = pManager.params<double>(U"enemyHp");
		auto barrageAction = new prg::ManageActions;
		//敵のhpを回復し、2秒待つ
		gameProgress->add(new prg::FuncAction([=] {setEnemyHp(maxHp); }, 2));
		barrageAction->add(new prg::FuncAction(
			[=](double, prg::FuncAction*) {
				if (mainEnemy->GetComponent<ActorState>()->hp <= 0) barrageAction->endCondition->forcedFire();
			},
			[=] {//開始処理
				behavior->progress->setAction(startUpBarrage(barrageName), barrageName);
				behavior->progress->startAction(barrageName);
			},
			[=] {//終了処理
				behavior->progress->stopAllAction({U"yureru",U"tamani" });
				eraseEnemyAllGuns();
			}, Math::Inf));
		
		gameProgress->add(barrageAction);
	}
	//ランダム散弾
	{
		String barrageName = U"Scatter01";
		auto pManager = barragePManager.getManager(barrageName);
		auto barrageAction = new prg::ManageActions;
		double maxHp = pManager.params<double>(U"enemyHp");
		barrageAction->add(new prg::FuncAction([=] {setEnemyHp(maxHp);}));
		barrageAction->add(new prg::FuncAction(
			[=](double, prg::FuncAction*) {
				if (mainEnemy->GetComponent<ActorState>()->hp <= 0) barrageAction->endCondition->forcedFire();
			},[=] {
				behavior->progress->setAction(startUpBarrage(barrageName), barrageName);
				behavior->progress->startAction(barrageName);
			}, [=] {
				for (auto& a : behavior->GetComponent<MainWeapon>()->gun->otherGuns)
				{
					for(auto& b:a->getBullets())if(b->GetComponent<DrawTexture>()!=nullptr)	util::setDissapearAction(b, 1.2, 0.4)->start();
					a->MainObject->eraseThis();
				}
				behavior->GetComponent<MainWeapon>()->gun->otherGuns.clear();
				behavior->progress->stopAllAction({ U"yureru",U"tamani" });
				behavior->progress->startAction(U"preparationMove");
				behavior->progress->getAction(U"preparationMove")->timeScale=0.7;
			}, Math::Inf)
		, 3);
		gameProgress->add(barrageAction);
	}
	//円
	{
		String barrageName = U"binalyCircle";
		auto pManager = barragePManager.getManager(barrageName);
		auto barrageAction = new prg::ManageActions;
		double maxHp = pManager.params<double>(U"enemyHp");
		barrageAction->add(new prg::FuncAction([=] {setEnemyHp(maxHp);}));
		auto prep = new prg::ManageActions;
		barrageAction->add(new prg::FuncAction([=] {
				prep->add(util::transformPosition(behavior, { mainFieldPos + Vec2{mainFieldW * 3 / 4,mainFieldH / 2.0} }, 0, 1, false));
				behavior->progress->setAction(prep, U"preparatePosition")->start();
			},[]{}, new prg::FuncCondition([=] {return prep->getEnded(); })));
		barrageAction->add(new prg::FuncAction(
			[=](double, prg::FuncAction*){
				if (mainEnemy->GetComponent<ActorState>()->hp <= 0) barrageAction->endCondition->forcedFire();
			},[=] {
				behavior->progress->setAction(startUpBarrage(barrageName), barrageName)->start();
			},
			[=] {
				behavior->progress->stopAllAction({ U"yureru",U"tamani" });
				for (auto& a : behavior->GetComponent<MainWeapon>()->gun->otherGuns)
				{
					for(auto& b:a->getBullets())if(b->GetComponent<DrawTexture>()!=nullptr)	util::setDissapearAction(b, 1.2, 0.4)->start();
					a->MainObject->eraseThis();
				}
				behavior->GetComponent<MainWeapon>()->gun->otherGuns.clear();

			},Math::Inf
		));

		gameProgress->add(barrageAction);
	}
	//いろいろ弾幕
	{
		String barrageName = U"combinationBarrage";
		auto pManager = barragePManager.getManager(barrageName);
		auto barrageAction = new prg::ManageActions;
		double maxHp = pManager.params<double>(U"enemyHp");
		barrageAction->add(new prg::FuncAction([=] {setEnemyHp(maxHp); }));
		barrageAction->add(new prg::FuncAction([=] {
			behavior->progress->restartAction(U"preparationMove");
			}, [] {}, new prg::FuncCondition([=] {return behavior->progress->getAction(U"preparationMove")->getEnded(); })));
		barrageAction->add(new prg::FuncAction([=](double, prg::FuncAction*) {
				if (mainEnemy->GetComponent<ActorState>()->hp <= 0) barrageAction->endCondition->forcedFire();
			}, [=] {
				behavior->progress->setAction(startUpBarrage(barrageName), barrageName)->start();
			},[=] {
				behavior->progress->stopAllAction({ U"yureru",U"tamani" });
				eraseEnemyAllGuns();
				behavior->GetComponent<MainWeapon>()->gun->otherGuns.clear();
			}, Math::Inf
		));

		gameProgress->add(barrageAction);
	}
	//大量のUFOが流れてくる
	{
		String barrageName = U"shootingStar";
		auto pManager = barragePManager.getManager(barrageName);
		auto barrageAction = new prg::ManageActions;
		double maxHp = pManager.params<double>(U"enemyHp");
		barrageAction->add(new prg::FuncAction([=] {setEnemyHp(maxHp); }));
		barrageAction->add(new prg::FuncAction([=](double, prg::FuncAction*) {
			if (mainEnemy->GetComponent<ActorState>()->hp <= 0) barrageAction->endCondition->forcedFire();
			}, [=] {
				behavior->progress->setAction(startUpBarrage(barrageName), barrageName)->start();
			}, [=] {
				behavior->progress->stopAllAction({ U"yureru",U"tamani" });
				for (auto a : behavior->GetComponent<MainWeapon>()->gun->otherGuns)
				{
					for (auto b : a->getBullets())	if (b->GetComponent<DrawTexture>() != nullptr)	util::setDissapearAction(b, 1.2, 0.4)->start();
					a->MainObject->eraseThis();
				}
				behavior->GetComponent<MainWeapon>()->gun->otherGuns.clear();
			}, Math::Inf
		));
		auto sentakuki = new prg::ManageActions;

		sentakuki->add(new prg::FuncAction([=] {behavior->progress->setAction(startUpBarrage(U"sentakuki"), U"sentakuki")->start();	}));

		barrageAction->addParallel(sentakuki);

		sentakuki->setStartCondition(new prg::FuncCondition([=] {return mainEnemy->GetComponent<ActorState>()->hp <= maxHp * 7.0/12 ; }));

		gameProgress->add(barrageAction);
	}
	//敵のhpゲージ
	auto hp = (new Object(manager));
	hp->AddComponentImmediately<DrawFunc>()->drawingFunction = [=](const DrawFunc*) {
		auto pos = mainEnemy->transform->getPos();
		double lifeRate = mainEnemy->GetComponent<ActorState>()->hp / *enemyMaxHp;
		Circle{ pos,100 }.drawArc(0_deg, -lifeRate * 2 * Math::Pi, 7, 0);
	};
	//クリア
	{
		//ufo爆発
		gameProgress->add(new prg::FuncAction([=]
			{
				mainEnemy->GetComponent<ObjectBehavior>()->progress->startAction(U"chargeEffect");
				GameDataModel::getInstance().timeScale = 0.3;
			}, [=] {
				mainEnemy->GetComponent<ObjectBehavior>()->progress->getAction(U"chargeEffect")->endCondition->forcedFire();
				//mainEnemy->GetComponent<ObjectBehavior>()->progress->getAction(U"chargeEffect")->
				mainEnemy->GetComponent<ObjectBehavior>()->progress->startAction(U"dissapearAction");
				player->GetComponent<ObjectBehavior>()->progress->stopAction(U"Attack",U"bombAttack");
				hp->eraseThis();
				for (auto& gun : mainEnemy->GetComponent<MainWeapon>()->gun->otherGuns)
					for (auto& bullet : gun->getBullets())bullet->progress->restartAction(U"dissapearAction");
				behavior->GetComponent<MainWeapon>()->gun->otherGuns.clear();
				GameDataModel::getInstance().timeScale = 1;
			}, 1
		));
		auto fontObj = (new Object(manager));
		auto f = fontObj->AddComponentImmediately<DrawFont>();
		f->color = Palette::Yellow;
		f->gameCamera = camera;
		f->transform->setPos(mainFieldPos + Vec2{ mainFieldW / 2,300 });
		f->visible = false;
		f->setAssetName(U"pauseFont");
		gameProgress->add(new prg::FuncAction([=] {
			f->visible = true;
			f->setText(U"クリア");
		},5),2.5);
		gameProgress->add(new prg::FuncAction([=] {
			f->setAssetName(U"creditFont");
			f->setText(U"プログラム・絵・曲　やっさん\n\n効果音　OtoLogic");
		},3));
		gameProgress->add(new prg::FuncAction([=] {
			f->setAssetName(U"creditFont");
			f->setText(U"感謝リスト:\n主人公のアンナちゃん　生みの親のりゅーの様\n\nお菓子提供者の宇宙人\n\nアドバイス・テストプレイなどしてくれた方々\n\nあなた　← new");
		},8));
		gameProgress->add(new prg::FuncAction([=] {
			f->setAssetName(U"creditFont");
			f->setText(U"ここまで遊んでくれてありがとう！");
			}, [=] {
		}, 5));
		//ズームアウト
		double outTime = 4;
		double firstBackGroundScale = 11;
		Vec2 outPos = Vec2{ Scene::Width() - 250,100};
		gameProgress->add(new prg::FuncAction([=] (double,prg::FuncAction* act){
			camera->scale = 1+(1/firstBackGroundScale-1)*act->time/outTime;
			camera->transform->setPos(Scene::Center() + (Scene::Center() - outPos) * firstBackGroundScale * act->time*act->time*act->time*act->time * act->time / (outTime*outTime*outTime * outTime * outTime));
		},[=] {
			for(auto obj:manager->getGameObjects(ObjectTag::ui))obj->relaseParent();
			auto backGround = new Object(manager);
			backGround->z = -200;
			auto tex=backGround->AddComponentImmediately<DrawTexture>();
			tex->setTexture(U"sumb", firstBackGroundScale);
			tex->gameCamera = camera;
			backGround->transform->setPos(Scene::Center() + (Scene::Center() - outPos) * firstBackGroundScale);//outPos - (firstBackGroundScale - 1) * Scene::Center());
		},outTime
		));

		//フェードアウト
		std::shared_ptr<ColorF> color(new ColorF{ Palette::Black,0 });
		gameProgress->add(new prg::FuncAction([=] {
			auto o = new Object(manager);
			auto drawing = o->AddComponentImmediately<DrawFunc>();
			drawing->drawingFunction = [=](const DrawFunc*)
			{
				color->a += Scene::DeltaTime() / 5;
				RectF{ Arg::center(Scene::Center() + (Scene::Center() - outPos) * firstBackGroundScale),Scene::Size() * firstBackGroundScale }.draw(*color);
			};
			o->z = 500;
			}, [=] {EndGame(true); }, 8)
		, 3);
	}	
	//開始
	gameProgress->start();

	//背景
	{
		auto backGround = new Object(manager);
		backGround->AddComponentImmediately<DrawFunc>()->drawingFunction = [=](const DrawFunc*)
		{
			RectF{ Arg::center(mainFieldPos + Vec2{mainFieldW,mainFieldH}/2),mainFieldW * 1.5,mainFieldH * 1.5 }.draw(Arg::top = Color{ 50,70,220 }, Arg::bottom = Palette::Skyblue);
		};
		backGround->z = -100;
		auto backGroundAction = new prg::ManageActions;
		double interval = 3;
		double randomTime = Random<double>(-1.5, 1.5);
		double accum = 0;
		Array<String> option{ U"c1",U"c2",U"c3",U"DoCloud",U"AmCloud",U"PeCloud" };
		DiscreteDistribution distribution{ 1,1,1,0.4,0.4,0.3 };
		Vec2 cloudVel{ -47,0 };
		//雲の追加
		std::shared_ptr<Array<Object*>> clouds(new Array<Object*>);
		backGroundAction->add(new prg::FuncAction(
			[=](double dt, prg::FuncAction*)mutable
			{//雲を生成
				double span = interval + randomTime;
				for (accum += dt; accum >= span; accum -= span)
				{
					auto cloud = (new Object(manager));
					auto tex = cloud->AddComponentImmediately<DrawTexture>();
					tex->setTexture(DiscreteSample(option, distribution), 0.4, 0);
					tex->gameCamera = camera;
					cloud->transform->setPos(mainFieldPos + Vec2{ mainFieldW * 1.2,Random<double>(mainFieldH * 0.55,mainFieldH * 5 / 6) });
					cloud->AddComponentImmediately<StandardMove>(new StandardMove(cloudVel))->start_action();
					*clouds << cloud;
					randomTime = Random<double>(-1.5, 3.5);
					cloud->z = -90;
				}
				//カメラに写ってないandウィンドウの外に行った雲は削除
				for (auto it = clouds->begin(); it != clouds->end();) {
					auto& cloud = (*it);
					if (not cloud->GetComponent<DrawTexture>()->inCamera and cloud->GetComponent<Transform>()->getPos().x < 0)
					{
						cloud->eraseThis();
						it = clouds->erase(it);
						continue;
					}
					++it;
				}
			}, Math::Inf
		));
		//初めから雲を配置する
		auto cloudGen = backGround->AddComponentImmediately<ObjectBehavior>()->progress->setAction(backGroundAction, U"createCloud");
		cloudGen->start();
		double st = 0.1;
		int32 c = int32(Scene::Width() / (cloudVel.length() * st));
		auto v = cloudVel.length();
		for (auto k : s3d::step(c)) {
			cloudGen->update(st);
			for (auto& cloud : *clouds)cloud->update_components(st);
		}
	}	
}

prg::ManageActions* Game::startUpBarrage(const String& barrageName)
{
	return Barrages(manager, mainEnemy,GameParameterManager::createManager(U"common", U"Barrages", barrageName).params<double>(U"difCoe") * difficalty).createBarrage(barrageName);
}

prg::ManageActions* Game::startUpBarrage(const String& barrageName,double dfficalty)
{
	return Barrages(manager, mainEnemy, difficalty).createBarrage(barrageName);
}

void Game::update()
{
	if (retry)
	{
		delete manager;
		delete pauseObjectsManager;
		delete gameProgress;
		GameDataModel::Destroy();
		Audio(U"mainBgm").stop();
		Init();
	}
	/*if (KeyY.pressed())camera->scale -= Scene::DeltaTime();

	if (KeyU.pressed())camera->transform->moveBy({ -100 * Scene::DeltaTime(),0 });*/

	if (end) return;

	//ゲームの処理

	if (not GameDataModel::getInstance().pause)
	{
		manager->updateGameObjects(Scene::DeltaTime() * GameDataModel::getInstance().timeScale);
		gameProgress->update(Scene::DeltaTime() * GameDataModel::getInstance().timeScale);
	}

	pauseObjectsManager->updateGameObjects(Scene::DeltaTime() * GameDataModel::getInstance().timeScale);
}

void Game::draw()const
{
	if (end) return;
	//カメラの影響を与える
	auto t = camera->getTransformer2D(true);
	manager->drawGameObjects();
	pauseObjectsManager->drawGameObjects();
}

void Game::EndGame(bool clear)
{
	end = true;
	getData().mini_clear = clear;//クリア状況保存
	AudioAsset(U"mainBgm").stop();
	changeScene(U"Title", 0);

	//if (getData().mini_mode == Stage_Mode) {
	//	changeScene(U"Stage",0);//ステージモードならステージに帰る
	//}
	//else changeScene(U"Mini_Game_Select",0);//ミニゲームセレクトモードならミニゲームセレクトに帰る
}

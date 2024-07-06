#include "stdafx.h"
#include "Scenes.h"
#include"GameObject/Object.h"
#include"Components/MakeObject.h"


using namespace snk;

void Title::registerGameAsset()
{
	r.type = RegisterResources::AssetType::texture;
	using namespace resource;

	texture::Register(r(U"sumb"), U"resource/title.png", TextureDesc::Mipped);
	texture::Register(r(U"slide"), U"resource/スライド1.PNG", TextureDesc::Mipped);
	texture::Register(r(U"cake"), U"resource/cakeBullet.png", TextureDesc::Mipped);
	r.type = RegisterResources::AssetType::audio;
	//audio::Register(r(U"mainBgm"), U"resource/SnackUFO.mid", 0.5);
	r.type = RegisterResources::AssetType::font;
	font::Register(r(U"titleFont"), 30, Typeface::Bold);
}

Title::Title(const InitData& init): IScene{ init }
{
	getData().KeyUp = KeyW;
	getData().KeyLeft = KeyA;
	getData().KeyDown = KeyS;
	getData().KeyRight = KeyD;

	GameDataModel::New(getData());

	//カメラの生成
	camera = (new Object(&manager))->AddComponentImmediately<Camera>();
	camera->scale = 1.0;
	camera->transform->setPosAndPrePos(Scene::Center());
	//カメラをセット
	manager.camera = camera;

	auto titleBehavior = (new Object(&manager))->AddComponentImmediately<ObjectBehavior>();

	//ローディング
	registerGameAsset();
	std::shared_ptr<bool> completeLoading(new bool(false));
	progress.add(new prg::FuncAction(
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

		},
			new prg::FuncCondition([=]() {
			return *completeLoading;
			}
		)));

	//タイトル画面を作成
	double time = 0.4;//fadeinのtime
	//フォントのベースカラー
	ColorF baseColor = Palette::Maroon;
	HashTable<String, Object*> buttons;
	Array<String> allButtonTexts{ U"ゲームを始める",U"オプション",U"ルール・攻略",U"ゲームを終了する",U"戻る",U"かんたん",U"ふつう",U"むずかしい"};
	std::shared_ptr<Array<String>> buttonTexts(new Array<String>());
	//ボタンの生成
	auto generateButton = [=](const String& text)mutable {
		auto button = new Object(&manager);
		auto font = button->AddComponentImmediately<DrawFont>();
		font->gameCamera = camera;
		font->center = false;
		font->setText(text);
		font->setAssetName(U"titleFont");
		font->color = baseColor; font->color.a = 0;
		auto behavior = button->AddComponentImmediately<ObjectBehavior>();
		auto fadeInOut = mitame::createFadeInAndOutAction(font, time);
		//ボタンのfadeInアクション
		{
			behavior->progress->setAction(fadeInOut.fadeIn, U"fadeIn");
			auto slideAction = new prg::ManageActions;
			slideAction->add(new prg::FuncAction([=] {
				Vec2 nowPos = button->transform->getPos();
				auto slide = new prg::ManageActions;
				if (text == U"ゲームを始める" or text == U"ゲームを終了する" or text==U"オプション" or text==U"ルール・攻略")slide->add(util::transformPosition(behavior, {nowPos - Vec2{-30,0}}, 0, 1, false));
				else slide->add(util::transformPosition(behavior, { nowPos - Vec2{-30,0} }, 0, time, false));
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
				slide->add(util::transformPosition(behavior, { nowPos - Vec2{30,0} }, 0, time, false));
				behavior->progress->setAction(slide, U"slideOfFadeOut")->start();
				}));
			behavior->progress->addAction(slideAction, U"fadeOut");
		}
		//当てられたときの動き 横に揺れて光る
		{
			auto shakeAction = mitame::shaking(font, 6, 0.06, 0_deg);
			shakeAction->loop = false;

			shakeAction->addParallel(new prg::FuncAction(
				[=] {
					font->color = Palette::Orangered;
				},
				[=] {
					font->color = baseColor;
				},
					Math::Inf
					));
			shakeAction->init = true;
			behavior->progress->setAction(shakeAction, U"attention");
		}
		return button;
	};

	for (const auto& text : allButtonTexts)
	{
		buttons.emplace(text, generateButton(text));
	}

	double interval = 55;
	Vec2 basePos = Vec2{ 170,450 };
	auto titleObj = (new Object(&manager))->AddComponentImmediately<ObjectBehavior>();
	progress.add(new prg::FuncAction(
		[=]()mutable {
			//タイトルの絵
			double interval = 55;
			Vec2 basePos = Vec2{ 120,500 };
			auto title = titleObj->AddComponentImmediately<DrawTexture>();
			title->transform->setPos(Scene::Center());
			title->gameCamera = camera;
			title->setTexture(U"sumb");
			auto snack = titleObj->AddComponentImmediately<DrawTexture>();
			Vec2 pos= Vec2{ 380,-300 };
			//お菓子
			snack->gameCamera = camera;
			snack->setTexture(U"cake");
			snack->pos = -pos;
			snack->rotateCenter = pos;
			snack->scale = 0.7;
			titleObj->progress->setAction(mitame::vibrateDirectin(snack, 15_deg, 3), U"yure")->start();
			titleObj->MainObject->z = -100;
			titleObj->progress->setAction(mitame::createFadeInAndOutAction(title, time).fadeIn, U"fadeIn")->start();
			//ボタン
			*buttonTexts = { U"ゲームを始める",U"オプション",U"ルール・攻略",U"ゲームを終了する"};
			int32 count = 0;
			for (String text : *buttonTexts) {
				buttons[text]->transform->setPosAndPrePos(basePos + Vec2{ 0,interval } *count);
				buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
				++count;
			}
			//表記
			auto te=titleObj->AddComponentImmediately<DrawFont>();
			te->setAssetName(U"titleFont");
			te->setText(U"Enterを押してください");
			te->color = Palette::Yellow;
			te->setPos(Vec2{0,350});
			te->gameCamera = camera;
		}, 1
	));
	//最初の選択画面
	std::shared_ptr<int32> selecting(new int32);
	progress.add(new prg::FuncAction(
		[=](double, prg::FuncAction* act)mutable
		{
			if (titleBehavior->progress->getAction(U"selectDifficulty")->isActive()
				or titleBehavior->progress->getAction(U"option")->isActive()
				or titleBehavior->progress->getAction(U"exp")->isActive())return;

			int32 preSelect = *selecting;
			if (GameDataModel::getData().KeyDown.down()) ++(*selecting);
			if (GameDataModel::getData().KeyUp.down()) --(*selecting);
			//ボタンを巡回する
			if (*selecting < 0)*selecting += buttonTexts->size();
			*selecting %= buttonTexts->size();
			
			const auto& text = (*buttonTexts)[*selecting];
			//EnterKeyを押したらボタンに対応した処理を行う
			if (KeyEnter.down()) {
				if (text == U"ゲームを始める") {
					titleBehavior->progress->restartAction(U"selectDifficulty");
				}
				else if (text == U"オプション"){
					titleBehavior->progress->restartAction(U"option");
				}
				else if (text == U"ルール・攻略") {
					titleBehavior->progress->restartAction(U"exp");
				}
				else if (text == U"ゲームを終了する"){
					System::Exit();
				}
			}
			//対応したボタンを揺らす
			if (*selecting != preSelect) {
				buttons[(*buttonTexts)[preSelect]]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
				buttons[text]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
			}
		},
		[=]()mutable {
			*selecting = 0;
			buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
		},
		[=]()mutable {
			buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
		},
		Math::Inf
	));
	//難易度選択
	{
		auto selectDifficulty = new prg::ManageActions;
		std::shared_ptr<Array<String>> firstButtonSets(new Array<String>());
		selectDifficulty->add(new prg::FuncAction(
			[=]()mutable {
				for (String text : *buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
				*firstButtonSets = *buttonTexts;
				*buttonTexts = { U"かんたん",U"ふつう",U"むずかしい",U"戻る" };
				double interval2 = 150;
				int32 count = 0;
				for (String text : *buttonTexts) {
					buttons[text]->transform->setPosAndPrePos(basePos + Vec2{150,0} + Vec2{ 0,interval } *count);
					buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
					++count;
				}
			}, time
		));
		std::shared_ptr<int32> selecting(new int32);
		//終了したらtrue
		selectDifficulty->add(new prg::FuncAction(
			[=](double, prg::FuncAction* act)mutable {
				int32 preSelecting = *selecting;
				if (GameDataModel::getData().KeyUp.down())--(*selecting);
				else if (GameDataModel::getData().KeyDown.down())++(*selecting);

				//ボタンを巡回する
				if (*selecting < 0)*selecting += buttonTexts->size();
				*selecting %= buttonTexts->size();

				if (KeyEnter.down())act->endCondition->forcedFire();//終了

				//対応したボタンを揺らす
				if (*selecting != preSelecting) {
					buttons[(*buttonTexts)[preSelecting]]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
					buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
				}
			},
			[=]()mutable {
				*selecting = 1;
				buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
			},
				Math::Inf
		));
		selectDifficulty->add(new prg::FuncAction(
			[=]()mutable {//フェードアウトさせる
				buttons[(*buttonTexts)[*selecting]]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
				for (String text : *buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
				if (*selecting == 3)return;
				getData().mini_mode = (mode)(*selecting+1);
				changeScene(U"SnackUFO", 0s);
			}, [=]()mutable {
				*buttonTexts = *firstButtonSets;
				//元に戻す
				for (String text : *buttonTexts) {
					buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
				}
			}, time
		));
		titleBehavior->progress->setAction(selectDifficulty, U"selectDifficulty");
	};
	//ルール・攻略の説明
	{
		auto image = (new Object(&manager))->AddComponentImmediately<DrawTexture>();
		image->transform->setPos(Scene::Center()+Vec2{0,-50});
		image->gameCamera = camera;
		image->setTexture(U"slide",0.9);
		image->visible = false;
		auto selectDifficulty = new prg::ManageActions;
		std::shared_ptr<Array<String>> firstButtonSets(new Array<String>());
		selectDifficulty->add(new prg::FuncAction(
			[=]()mutable {
				for (String text : *buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
				*firstButtonSets = *buttonTexts;
				*buttonTexts = { U"戻る" };
				int32 count = 0;
				for (String text : *buttonTexts) {
					buttons[text]->transform->setPosAndPrePos(basePos + Vec2{ -50,250 });
					buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
					++count;
				}
			}, time
		));
		std::shared_ptr<int32> selecting(new int32);

		selectDifficulty->add(new prg::FuncAction(
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
		selectDifficulty->add(new prg::FuncAction(
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
		titleBehavior->progress->setAction(selectDifficulty, U"exp");
	}
	//オプション
	{
		auto option = new prg::ManageActions;
		Array<Object*> optionObjects;
		Array<String> texts = { U"BGM音量",U"◀",U"", U"▶",U"効果音音量",U"◀",U"", U"▶",U"移動キー",U"◀",U"", U"▶",U"決定",U"戻る"};
		int32 count = 0;
		Vec2 p{ 0,0 };
		for (String text : texts) {
			auto obj = generateButton(text);
			obj->transform->setPosAndPrePos(basePos+p);
			obj->GetComponent<DrawFont>()->center = true;
			if(count>=texts.size()-2)
			{
				p = Vec2{ 0,p.y + 50 };				
			}
			else if (count % 4 == 0)p.x += 200;
			else if (count % 4 == 3)p = Vec2{ 0,p.y + 50 };
			else p.x += 100;

			optionObjects << obj;

			++count;
		}
		//フェードイン
		option->add(new prg::FuncAction(
			[=]()mutable {
				for (String text : *buttonTexts)buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
				for (auto& op : optionObjects) { op->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn"); }
			}, time
		));
		std::shared_ptr<int32> selecting(new int32);
		std::shared_ptr<bool> keyType(new bool);
		option->add(new prg::FuncAction(
			[=](double, prg::FuncAction* act)mutable {
				int32 preSelecting = *selecting;
				if (GameDataModel::getData().KeyUp.down())--(*selecting);
				else if (GameDataModel::getData().KeyDown.down())++(*selecting);

				if (getData().KeyRight.down() or getData().KeyLeft.down())
				{
					double d = getData().KeyRight.down() ? 0.05 : -0.05;
					switch (*selecting)
					{
					case 0:
						getData().BGM_volume += d;
						break;
					case 1:
						getData().Effect_volume += d;
						break;
					case 2:
						*keyType = not(*keyType);
						break;
					}
				}

				if (*selecting < 0)*selecting += 5;
				*selecting %= 5;

				getData().BGM_volume = Min(getData().BGM_volume, 1.0);
				getData().BGM_volume = Max(getData().BGM_volume, 0.0);
				getData().Effect_volume = Min(getData().Effect_volume, 1.0);
				getData().Effect_volume = Max(getData().Effect_volume, 0.0);

				optionObjects[2]->GetComponent<DrawFont>()->setText(Format(getData().BGM_volume * 100) + U"%");
				optionObjects[6]->GetComponent<DrawFont>()->setText(Format(getData().Effect_volume * 100) + U"%");
				optionObjects[10]->GetComponent<DrawFont>()->setText(*keyType?U"WASD":U"↑←↓→");

				if (KeyEnter.down() and (*selecting == 3 or *selecting == 4)) {
					act->endCondition->forcedFire();
				}

				//対応したボタンを揺らす
				if (*selecting != preSelecting) {
					int32 pre = preSelecting > 2 ? texts.size() + preSelecting - 5 : preSelecting * 4;
					int32 c = *selecting > 2 ? texts.size() + *selecting - 5 : (*selecting) * 4;					
					optionObjects[pre]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
					optionObjects[c]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
				}
			},
			[=]()mutable {
				*selecting = 0;
				optionObjects[*selecting]->GetComponent<ObjectBehavior>()->progress->startAction(U"attention");
				*keyType = getData().KeyUp == KeyW;
			},
				Math::Inf
			));
		option->add(new prg::FuncAction(
			[=]()mutable {
				if (*selecting == 3)
				{
					if (*keyType)
					{
						getData().KeyUp = KeyW;
						getData().KeyLeft = KeyA;
						getData().KeyDown = KeyS;
						getData().KeyRight = KeyD;
					}
					else {
						getData().KeyUp = KeyUp;
						getData().KeyLeft = KeyLeft;
						getData().KeyDown = KeyDown;
						getData().KeyRight = KeyRight;
					}
				}
				//フェードアウトさせる
				int32 c = *selecting > 2 ? texts.size() + *selecting - 5 : (*selecting) * 4;
				optionObjects[c]->GetComponent<ObjectBehavior>()->progress->getAction(U"attention")->endCondition->forcedFire();
				for (auto& o:optionObjects)o->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeOut");
			}, [=]()mutable {
				//元に戻す
				for (String text : *buttonTexts) {
					buttons[text]->GetComponent<ObjectBehavior>()->progress->restartAction(U"fadeIn");
				}
			}, time
				));
		titleBehavior->progress->setAction(option, U"option");
	};
	progress.start();
}

void Title::update()
{
	manager.updateGameObjects(Scene::DeltaTime());
	progress.update(Scene::DeltaTime());
}

void Title::draw()const
{
	//カメラの影響を与える
	auto t = camera->getTransformer2D(true);
	manager.drawGameObjects();
}

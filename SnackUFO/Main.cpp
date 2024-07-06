# include <Siv3D.hpp>
#include"Common.hpp"
# include "Game.h"//名前を変更してください
#include"Paraminit.h"
#include"Scenes.h"

class Mini_Game_Select : public App::Scene
{
public:
	Font font{ 50 };
	Mini_Game_Select(const InitData& init)
		: IScene{ init } {}
	void update() override
	{
		changeScene(U"Title");
	}
	void draw() const override
	{
		font(U"ミニゲーム選択画面").drawAt(600, 400);
	}
};

void Main()
{
	Window::Resize(1200, 800);

	ResourceManager::mode = resource::relase;
	////パラメータを記述したjsonを生成
	//GameParameterManager param(
	//	snk::ParamInit(JsonSetter::New(U"GameParameters.json", true))
	//);
	//パラメータを記述したjsonを読み込む
	GameParameterManager param(
		object_params::Init(U"GameParameters.json")
	);
	param.setCommonPath(U"GameParameters.json");

	// ウィンドウを閉じる操作のみを終了操作に設定する
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);

	App manager;
	manager.add<snk::Title>(U"Title");
	manager.add<Mini_Game_Select>(U"Mini_Game_Select");
	manager.add<snk::Game>(U"SnackUFO");//名前を変更してください

	while (System::Update())
	{
		ClearPrint();
		//System::Sleep(0.04s);
		if (not manager.update())
		{
			break;
		}
	}
}

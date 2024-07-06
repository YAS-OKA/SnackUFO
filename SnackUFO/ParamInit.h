#pragma once
#include"GameParameterManager/Init.h"
#include"GameObject/Actors.h"
#include"GameObject/Gun.h"
#include"Barrages.h"
#include"Game.h"

namespace snk
{
	inline JSON ParamInit(std::unique_ptr<JsonSetter> setter)
	{
		//ここでパラメータを設定
		// カメラで高さ変わるからいらないかも
		//set_param<int32>(40, U"画面の高さを100としたときの地面の高さ", U"GroundRatio");

		GameSceneParamInit();
		maker::ActorsParamInit();
		BarragesParamInit();

		return setter.get()->get_json();
	};
}

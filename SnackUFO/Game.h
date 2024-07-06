#pragma once
#include "Common.hpp"
#include"GameObject/Object.h"
#include"ProcedureManager/Actions.h"
#include"GameObject/Camera.h"
#include"ResourceManager/ResourceManager.h"


namespace snk
{
	void GameSceneParamInit();

	class Game : public App::Scene
	{
	public:
		void registerGameAsset();

		Game(const InitData& init);

		~Game();

		void Init();

		prg::ManageActions* startUpBarrage(const String& barrageName);

		prg::ManageActions* startUpBarrage(const String& barrageName, double difficalty);

		void update() override;

		void draw() const override;

	private:
		bool ableToPause;

		bool end;
		void EndGame(bool clear);

		bool retry;

		double difficalty;

		prg::ManageActions* gameProgress;

		Object* pauseObject;

		Object* player;

		Object* mainEnemy;

		Camera* camera;

		Object* field;

		Object* mainField;

		RegisterResources r;

		GameObjectManager* manager;

		GameObjectManager* pauseObjectsManager;
	};
}

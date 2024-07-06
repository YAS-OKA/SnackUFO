#include "stdafx.h"
#include"GameObject/Object.h"
#include "GameUtils.h"
#include"Components/HitCallback.h"
#include"GameObject/Camera.h"

using namespace snk;

GameDataModel* GameDataModel::Instance = nullptr;

void GameDataModel::New(GameData& gamedata)
{
	if (Instance == nullptr)
	{
		Instance = new GameDataModel(gamedata);
	}
}

void GameDataModel::Destroy()
{
	InstanceCheck();
	delete Instance;
	Instance = nullptr;
}

GameDataModel& GameDataModel::getInstance()
{
	InstanceCheck();
	return *Instance;
}

void GameDataModel::InstanceCheck()
{
	if (Instance == nullptr)
	{
		throw Error{ U"GameDataModelが生成されていません" };
	}
}

const GameData& GameDataModel::getData()
{
	InstanceCheck();
	return Instance->game_data;
}

void GameObjectManager::addGameObject(Object* obj)
{
	cash << obj;
}

void GameObjectManager::removeGameObject(Object* obj)
{
	for (auto& object : gameObjects)
	{
		if (object == obj)
		{
			object->eraseThis();
			break;
		}
	}
}

void GameObjectManager::removeAllGameObject()
{
	for (auto& object : gameObjects)
	{
		object->eraseThis();
	}
	for (auto& object : cash)
	{
		object->eraseThis();
	}
}

void GameObjectManager::updateGameObjects(double dt)
{
	for (auto& obj : garbages)
	{
		delete obj;
	}
	garbages.clear();

	gameObjects.append(cash);
	cash.clear();

	gameObjects.stable_sort_by([](const Object* obj1, const Object* obj2) {return obj1->z < obj2->z; });

	for (auto itr = gameObjects.begin(); itr != gameObjects.end();)
	{
		auto& object = *itr;
		//削除
		if (object->getEraseFlag())
		{
			garbages << object;
			dict[object->tag].erase(object);
			itr = gameObjects.erase(itr);
			continue;
		}
		++itr;
	}

	for (auto itr = gameObjects.begin();itr!=gameObjects.end();)
	{
		auto& object = *itr;
		//更新
		object->update_components(dt);

		dict[object->tag].emplace(object);

		++itr;
	}

	for (auto& object : gameObjects)
	{
		auto hitCallback=object->GetComponent<HitCallback>();
		if (hitCallback != nullptr)hitCallback->check();
	}

	for (auto& object : gameObjects)
	{
		auto transform = object->GetComponent<Transform>();
		if (transform != nullptr)transform->calUpdate(dt);
	}
}

void GameObjectManager::drawGameObjects()const
{
	if (camera == nullptr)return;

	for (const auto& object : gameObjects)
	{
		camera->getTransformer2D(object->cameraAffected);
		object->draw_components();
	}
}

Array<Object*> GameObjectManager::getGameObjects()
{
	return gameObjects;
}

Array<Object*> GameObjectManager::getGameObjects(const ObjectTag& tag)
{
	Array<Object*> objects;
	for (auto& o : dict[tag])objects << o;
	return objects;
}

GameObjectManager::~GameObjectManager()
{
	//removeAllGameObject();
	garbages.append(gameObjects);
	garbages.append(cash);

	for (auto& garbage : garbages)
	{
		delete garbage;
	}
}


prg::ManageActions* snk::createAnimation(DrawTexture* drawTextureComponent,double turnTime, Array<String> assetNames, std::function<double(int32)>timeScaleFunction)
{
	auto action = new prg::ManageActions;

	int32 count = 0;
	for (auto assetName : assetNames)
	{
		++count;
		auto names = assetName.split(U'/');
		String name;
		for (auto k : step(3))
		{
			k += 1;
			name += names[names.size() - 4 + k];
			if (k != 3)name += U"/";
		}
		action->add(new prg::FuncAction(
			[=]{
				drawTextureComponent->setAssetName(name);
			},
			timeScaleFunction(count)* turnTime
		));
	}

	action->loop = true;

	return action;
}

Array<String> snk::getAssetNames(const String& directory,int32 size)
{
	Array<String> result;
	for (auto k : step(size))
	{
		k += 1;
		result << directory + Format(k) + U".png";
	}
	return result;
}

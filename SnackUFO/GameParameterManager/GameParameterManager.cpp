#include "GameParameterManager.h"
#include"JsonSetter.h"

String GameParameterManager::common_path = U"";



GameParameterManager::GameParameterManager(const JSON& json)
	:json(json)
{

}

GameParameterManager::~GameParameterManager()
{

}

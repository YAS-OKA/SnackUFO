#pragma once
#include"../Components/Entity.hpp"
#include"../GameUtils.h"
#include"../Components/Transform.h"

namespace prg
{
	class ManageActions;
}

namespace snk
{
	//主に衝突処理を分けるためのタグ
	enum class ObjectTag
	{
		field,bulletField,player,enemy,playerBullet,enemyBullet,enemyBulletLaser,ui,none
	};

	class Object final :public component::Entity
	{
	public:
		//コピーコンストラクタ 無理　代わりにPrimitiveGunみたいな感じで解決して
		//Object(const Object& obj);

		Object(GameObjectManager* manager);

		void makeParent(Object* parent);

		void makeChiled(Object* chiled);

		void relaseParent();

		void relaseChild(Object* child);

		void relaseChildren();

		void eraseThis();

		bool getEraseFlag();

		bool hasParent();

		Transform* transform;

		GameObjectManager* objectManager;

		bool followParent = true;

		ObjectTag tag;

		bool cameraAffected=true;
	protected:
		friend Transform;
		//親オブジェクト　
		Object* parent;

		HashSet<Object*> children;

		bool erase;
	};
}

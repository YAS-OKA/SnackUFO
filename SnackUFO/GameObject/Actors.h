#pragma once
#include"../Components/MakeObject.h"
#include"Gun.h"
#include"Bullet.h"
#include"Object.h"

struct chargeGage {
	double value = 0;;
	bool colision = false;
	bool ready = false;
};

namespace snk
{
	//アクターが持つすてーたす
	class ActorState :public component::Component
	{
	public:
		double hp=0;
		//無敵状態
		bool unbeatable=false;
	};
	//ステータス 汎用的
	template<class T>
	class Status :public component::Component
	{
	public:
		T value;
	};
	//アクターが持つメインウエポン
	class PrimitiveGun;
	class MainWeapon :public component::Component
	{
	public:
		template<class ...Gun>
		void addGun(Gun... args)
		{
			gun->addGun(args...);
		}

		void start()override;
		IGun* gun = nullptr;
	};
}

namespace snk::maker
{
	void ActorsParamInit();
	//ファクトリーメソッドのようでファクトリーメソッドじゃないもの
	//プレイヤーとか敵とか1体1体に対してクラス作るのは面倒なので、関数で作ったオブジェクトを返すような仕組みにした
	//再利用性のないPlayerとかはGame.cppで生成したほうが良いけど、関数にまとめたほうが気持ちがいいので...
	class CreateActors
	{
		CreateActors() {};
		~CreateActors() {};
	public:
		static Object* Actor(GameObjectManager* objectManager);
		//このゲームのメインキャラクターに共通の処理を書く actorNameはパラメータを取得するときに必要
		static Object* MainActor(GameObjectManager* objectManager,const String& actorName);
	
		static Object* Player(GameObjectManager* objectManager);

		static Object* MainEnemy(GameObjectManager* objectManager);
	};

}


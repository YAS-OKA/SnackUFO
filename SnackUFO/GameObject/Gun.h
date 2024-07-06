#pragma once
#include"../Figure.h"
#include"../Components/MakeObject.h"
#include"../ProcedureManager/init.h"
#include"../GameParameterManager/Init.h"
#include"../Components/Moves.h"

/*------ IGunをBulletにしなかった理由 -------
Bulletはintersectsを持たないといけない＆BulletはShotを持たない。
IGunは単にBulletを放つクラスだからintersectsを持ってほしくない。
BulletにShotを持たせたらIGunを作る意味がない。
以上のことから、IGunとBulletを分離させた。*/

namespace snk
{
	class Collider;
	class Transform;
	class Bullet;

	class IGun :public ObjectBehavior
	{
	public:
		void eraseAllGuns();

		//const prg::ManageActions* getActions();
		virtual void shot() = 0;
		Array<Bullet*> getBullets()const;

		template<class ...Gun>
		void addGun(Gun... args)
		{
			for (auto& gun : std::initializer_list<IGun*>{ args... })
			{
				//自分の子にする
				gun->makeParent(this);
				otherGuns << gun;
			}
		}

		void shotOtherGun();

		Array<IGun*>otherGuns;
		Array<Bullet*> bullets;
	};

	using BulletFunction = std::function<Bullet* (IGun*, GameObjectManager*)>;

	using TouchUpBullet = std::function<Bullet* (IGun*, Bullet*)>;

	class PrimitiveGun;

	struct TouchUp
	{
		TouchUp();

		void reset();

		void operator =(const TouchUpBullet& f);
		TouchUpBullet operator +(const TouchUpBullet& f);
		void operator +=(const TouchUpBullet& f);
	private:
		friend PrimitiveGun;
		Optional<TouchUpBullet> shotFunction;
	};

	class PrimitiveGun :public IGun
	{
	public:
		~PrimitiveGun();

		void shot()override;

		BulletFunction shotFunction;

		TouchUp touch;
	};

	class AutoAimingGun final :public PrimitiveGun
	{
	public:
		Object* target = nullptr;
		void update(double dt)override;
	};

	//中心から外に向かって放射上にdirをセットする
	class RadiateGun final :public IGun
	{
	public:
		void addArm(IGun* gun, double dir, double distance = 0);
		void update(double dt)override;
		void shot()override;
		//k番目の銃だけ撃つ
		void shot(int32 k);
	};

	//等間隔、放射状に　RadiateGunのコンポジション
	class RegulerRadiateGun final :public IGun
	{
	public:
		void start()override;
		void addArm(IGun* gun, double distance = 0);
		void update(double dt)override;
		void shot()override;
		void shot(int32 k);
		// trueなら -range/2 <= x <= range/2 の間に銃を対象に配置
		// falseなら -range/2 <= x < range/2 の間に等間隔に配置
		bool symmetrical = true;
		double range = 360_deg;
	private:
		void regulered();
		RadiateGun* radiate_gun;
	};

}

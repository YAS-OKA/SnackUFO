#pragma once
#include"GameObject/Object.h"
#include"GameObject/Actors.h"
#include"GameUtils.h"
#include"ProcedureManager/Init.h"
#include"Components/Draw.h"
#include"Components/HitCallback.h"

namespace snk
{
	using Act = prg::Action*;

	class PrimitiveGun;
	struct BulletElements;

	class IGun;

	void BarragesParamInit();

	class Barrages;
	//ショートかっと
	namespace sc {
		BulletElements circleBullet(ObjectTag bulletTag, double r,double damage);

		//void setTexture(Barrages* b,ObjectTag tag,double scale,double rad)

		double aimDirection(GameObjectManager* manager,Object* obj,ObjectTag TargetTag);

		struct hitCallbackRegistring {
			void set(HitCallback* callback, ObjectTag target, const String& callbackID)const;
			HitMethod get(const String& callbackID)const;
			hitCallbackRegistring();
		private:
			HashTable<String, HitMethod>callbackDist;
		};
	}

	enum class BulletType {
		cake,
		cup,
		choco,
		ufo,
		bat,
		none
	};

	namespace util
	{
		using TransFunction = std::function<Vec2(double progress, Vec2 p0, Vec2 delta)>;

		void bulletSetting(Bullet* bullet, BulletType type, double scale = 1);
		
		//まぁ大抵これでいいでしょう
		PrimitiveGun* createPrimitiveGun(GameObjectManager* manager, double bulletSpeed, const BulletElements& elems,TouchUpBullet b);
		PrimitiveGun* createPrimitiveGun(GameObjectManager* manager, double bulletSpeed,const BulletElements& elems);
		PrimitiveGun* createPrimitiveGun(GameObjectManager* manager, TouchUpBullet b = [](IGun*,Bullet* b) {return b; });

		//PrimitiveGun* createWhipGun(PrimitiveGun* gun, int32 num, double timeSpan, double speed, double attenuation);

		PrimitiveGun* createLaserGun(GameObjectManager* manager, double speed, double thick, const BulletElements& elems);

		PrimitiveGun* createLaserGun(Object* gun, double speed, double thick, const BulletElements& elems);
		/// @brief この関数を呼び出した時点で登録しているアクションtime回数繰り返すアクションを返す 使い方によってはメモリーリークを起こす場合がありそう？基本的には大丈夫
		/// @param action 繰り返すアクション
		/// @param time 繰り返す回数
		prg::ManageActions* repeat(prg::ManageActions* action, int32 time);
		//なるべくこっちを使って buildFunctionは繰り返す生成手順
		prg::ManageActions* repeat(std::function<prg::ManageActions* ()>buildFunction, int32 time);

		/// @brief 長方形範囲をランダムに動き回る
		/// @param rectPos 長方形の左上座標
		/// @param width 幅
		/// @param height　高さ
		/// @param fit メインフィールド内に収める
		/// @param agility 頻繁に動く　5を基準
		/// @param speedly 動きの速さ　5を基準
		/// @param widely 動きの大きさ　5を基準
		/// @param ft 移動関数　デフォルトは減速運動　もし等速にしたければ p0+progress*delta でいけるはず
		Act rectRandomTransform(ObjectBehavior* actor, Vec2 rectPos, double width, double height, bool fit, double agility, double speedly, double widely,
			TransFunction ft = [](double progress, Vec2 p0, Vec2 delta) {return  delta * progress * (2 - progress) + p0; });

		Act rectRandomTransform(Object* actor, Object* rangeActor, double width, double height, double time, Vec2 offset = Vec2{ 0,0 }, TransFunction ft = [](double progress, Vec2 p0, Vec2 delta) {return  delta * progress * (2 - progress) + p0; });

		Act rectRandomTransform(Object* actor, Vec2 rectCenterPos, double width, double height, double transformTime, TransFunction ft = [](double progress, Vec2 p0, Vec2 delta) {return  delta * progress * (2 - progress) + p0; });
		/// @brief 指定した場所を順番に回る
		/// @param positions 指定した場所の配列
		/// @param timeSpan 間隔
		/// @param transTime 移る時間
		/// @param loop 繰り返すかどうか
		/// @param ft 移動関数 
		Act transformPosition(ObjectBehavior* actor,Array<Vec2> positions, double timeSpan, double transTime,bool loop, TransFunction ft = [](double progress, Vec2 p0, Vec2 delta) {return  delta * progress * (2 - progress) + p0; });

		Act aimingTarget(ObjectBehavior* actor, ObjectBehavior* target,Optional<double> time);
		/// @brief 銃を加工する　弾の種類を確率分布で変更
		void touchUpGun(PrimitiveGun* gun, HashTable<BulletType, double> distribution,double scale=0.15);
		/// @brief transformを回転させるアクションを返す
		/// @param time 周期
		/// @param numCycle 何回転するか
		prg::ManageActions* rotational(Object* actor, double time, Optional<int32> numCycle=none);
	}

	//弾幕を作る
	class Barrages
	{
	public:
		GameObjectManager* manager;
		//弾幕を放つオブジェクト
		ObjectBehavior* actor;

		double difficalty;

		const String prefix = U"startUp";

		HashTable<String, std::function <prg::ManageActions*()>> barrageDictionaly;

		Barrages(GameObjectManager* manager, Object* shooter,double difficalty);

		/// @brief ランダムな方向に打つアクションを返す　メインウェポンの向きを基準とする
		/// @param gun 対象のgun
		/// @param mainWeapon メインウェポン
		/// @param timeSpan 発射する間隔
		/// @param minRad 乱数の下限
		/// @param maxRad 乱数の上限
		Act randomDirectionShot(IGun* gun,IGun* mainWeapon, double timeSpan, double minRad, double maxRad);

		/// @brief ランダムな方向にgunを向けるアクションを返す メインウェポンの向きを基準とする
		Act randomDirection(IGun* gun, IGun* mainWeapon, double minRad, double maxRad);

		///// @brief 一定時間連射するアクションを返す
		///// @param time 連射する時間
		///// @param timeSpan 発射する間隔
		//Act constantFire(IGun* gun,double time, double timeSpan);
		
		/// @brief 指定した回数の連射をするアクションを返す
		Act constantFire(IGun* gun,int32 num, double timeSpan);
		/// @brief 回数の指定なし
		Act constantFire(IGun* gun, double timeSpan);		

		prg::ManageActions* createBarrage(const String& barrageName);
	};
}

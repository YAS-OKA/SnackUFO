#pragma once
#include"Components/Entity.hpp"
#include"Common.hpp"

namespace snk
{
	//ゲームのデータのUtil
	class GameDataModel final
	{
	public:
		double timeScale = 1.0;

		bool pause = false;
		//ゲームのデータを返す
		static const GameData& getData();// { return game_data; }

		static void New(GameData& game_data);

		static void Destroy();

		static GameDataModel& getInstance();

	private:
		static void InstanceCheck();

		static GameDataModel* Instance;

		GameDataModel(GameData& game_data): game_data(game_data) {};
		~GameDataModel() {};

		GameData& game_data;
	};

	class Object;

	class Camera;

	enum class ObjectTag;
	//ゲームオブジェクトの管理クラス
	class GameObjectManager final
	{
	public:
		GameObjectManager() { camera = nullptr; };

		~GameObjectManager();

		void addGameObject(Object* object);

		//一致するobjectをガーベージに
		void removeGameObject(Object* object);
		//すべてのオブジェクトをガーベージに
		void removeAllGameObject();

		void updateGameObjects(double dt);

		void drawGameObjects()const;

		Array<Object*> getGameObjects();

		Array<Object*> getGameObjects(const ObjectTag& tag);

		Camera* camera;
	private:
		HashTable<ObjectTag, HashSet<Object*>> dict;

		Array<Object*> gameObjects;

		Array<Object*> cash;

		Array<Object*>garbages;
	};
}
namespace prg {
	class ManageActions;
}
namespace snk
{
	class DrawTexture;
	/// @brief アニメーションのコマの切り替えを行うアクション
	/// @param drawTextureComponent 絵を切り替える対象のコンポーネント
	/// @param turnTime コマを切り替える時間の間隔
	/// @param assetNames アニメーションの各コマのアセット名
	/// @param timeScaleFunction 各コマに対する時間の早さを返す関数 何コマめかを引数に渡す
	prg::ManageActions* createAnimation(DrawTexture* drawTextureComponent, double turnTime, Array<String> assetNames, std::function<double(int32)>timeScaleFunction = [](int32) {return 1.0; });

	Array<String> getAssetNames(const String& directory, int32 size);

	namespace mitame
	{
		/// @brief 物体の角度を-rad～rad間で振動させるアクションを返す
		/// @param rad 揺れる範囲
		/// @param t 周期
		template<class DrawClass>
		prg::ManageActions* vibrateDirectin(DrawClass* obj, double rad, double t);
		/// @brief 指定した方向に揺らす
		/// @param amplitude 振幅
		/// @param t 周期
		/// @param direction 揺れる方向
		template<class DrawClass>
		prg::ManageActions* shaking(DrawClass* obj, double amplitude, double t, double direction = -90_deg);
		/// @brief 物体を回転させる
		/// @param time 一回転の周期
		template<class DrawClass>
		prg::ManageActions* rotational(DrawClass* obj, double time);

		/// @brief 点滅させるアクションを返す
		/// @param obj colorを持つクラス
		/// @param color 点滅させる色
		/// @param time 時間
		template<class DrawClass>
		prg::ManageActions* constantFlashing(DrawClass* obj, ColorF color, double time);

		//colorを持ったクラスに対してフェードインフェードアウトのアクションをセットする
		struct fadeInOutAction
		{
			prg::ManageActions* fadeIn;
			prg::ManageActions* fadeOut;
		};

		template<class DrawClass>
		fadeInOutAction createFadeInAndOutAction(DrawClass* drawing, double time, double from = 0, double to = 1);

		class SparkParticle {
		public:
			ParticleSystem2D particleSystem;
			ParticleSystem2DParameters parameters;
			ArcEmitter2D arcEmitter;

			SparkParticle() {
				parameters.rate = 50;
				parameters.startLifeTime = 0.5;
				arcEmitter.direction = 80;
				arcEmitter.angle = 20;
				parameters.startSpeed = 300;
				parameters.startColor = Palette::Orange;
				parameters.blendState = BlendState::Default2D;
				particleSystem.setEmitter(arcEmitter);
				particleSystem.setTexture(TextureAsset(U"particle"));
				particleSystem.setParameters(parameters);
				particleSystem.prewarm();
			}

			//位置を設定
			void setPosition(const Vec2& pos) {
				particleSystem.setPosition(pos);
			}

			//角度を設定
			void setDirection(double direction) {
				if (arcEmitter.direction != direction) {
					arcEmitter.direction = direction;
					particleSystem.setEmitter(arcEmitter);
				}
			}

			//量を設定
			void setRate(double rate) {
				parameters.rate = rate;
			}

			//速さを設定
			void setSpeed(double speed) {
				parameters.startSpeed = speed;
			}

			//色を設定
			void setColor(const ColorF& color) {
				parameters.startColor = color;
			}

			//更新(毎フレーム呼び出す)
			void update(double dt) {
				particleSystem.setParameters(parameters);
				particleSystem.update(dt);
			}

			//描画
			void draw()const {
				particleSystem.draw();
			}

		};
	}
}

#include"ProcedureManager/Init.h"
#include"Components/Draw.h"

using namespace snk;

template<class DrawClass>
prg::ManageActions* mitame::vibrateDirectin(DrawClass* obj, double rad, double time)
{
	auto action = new prg::ManageActions;

	std::shared_ptr<double> firstDirection(new double);
	action->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act) {//揺れる
			obj->rad = rad * Sin(2 * Math::Pi * act->time / time) + *firstDirection;
		},
		[=] {//最初の方向を取得
			*firstDirection = obj->rad;
		},
			[=] {//最初の方向を代入
			obj->rad = *firstDirection;
		}, time
			));

	action->loop = true;

	return action;
}

template<class DrawClass>
prg::ManageActions* mitame::shaking(DrawClass* obj, double ampl, double time, double direction)
{
	auto action = new prg::ManageActions;

	Vec2 dir = Vec2{ Cos(direction),Sin(direction) };
	std::shared_ptr<Vec2> firstPosition(new Vec2);
	action->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act) {
			obj->pos = ampl * Sin(2 * Math::Pi * act->time / time) * dir + *firstPosition;
		},
		[=] {
			*firstPosition = obj->pos;
		},
		[=] {
			obj->pos = *firstPosition;
		}, time
			));
	action->loop = true;
	return action;
}

template<class DrawClass>
prg::ManageActions* mitame::rotational(DrawClass* obj, double time)
{
	auto action = new prg::ManageActions;

	std::shared_ptr<double> firstDirection(new double);
	action->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act) {//回転
			obj->rad = 2 * Math::Pi * act->time / time + *firstDirection;
		},
		[=] {//最初の方向を取得
			*firstDirection = obj->rad;
		},
			[=] {//最初の方向を代入
			obj->rad = *firstDirection;
		}, time
			));

	action->loop = true;

	return action;
}

template<class DrawClass>
prg::ManageActions* mitame::constantFlashing(DrawClass* obj, ColorF color, double time)
{
	auto action = new prg::ManageActions;
	std::shared_ptr<ColorF> startColor(new ColorF{});
	action->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act) {
			obj->color = startColor->lerp(color, act->time / (time / 2));
		}, [=] {
			*startColor = obj->color;
		}, [=] {
			//本当はobj->color = colorとすべきだがここで無理やりアクションが打ち切れられた際元の色に戻したいからobj->color=*startColorとしている。
			obj->color = *startColor;
		}, time / 2
	));
	action->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act) {
			obj->color = color.lerp(*startColor, act->time / (time / 2));
		}, [=] {}, [=] {
			obj->color = *startColor;
		}, time / 2
	));
	action->loop = true;
	return action;
}

#include"ProcedureManager/Init.h"

template<class DrawClass>
mitame::fadeInOutAction mitame::createFadeInAndOutAction(DrawClass* drawing, double time, double from , double to)
{
	auto fadeIn = new prg::ManageActions;

	fadeIn->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act) {
			drawing->color.a = from + act->time * (to - from) / time;
		}
		, time)
	);

	auto fadeOut = new prg::ManageActions;

	fadeOut->add(new prg::FuncAction(
		[=](double, prg::FuncAction* act) {
			drawing->color.a = from + (time - act->time) * (to - from) / time;
		}
		, time)
	);
	auto result = fadeInOutAction();
	result.fadeIn = fadeIn; result.fadeOut = fadeOut;
	return result;
}


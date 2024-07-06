#pragma once
#include"Entity.hpp"
#include"Transform.h"

//namespace condition {
//	class ConditionChecker;
//}

namespace snk
{
	class BaseMove :
		public component::Component
	{
	public:
		BaseMove(const Vec2& vel = Vec2{ 0,0 }, const Vec2& acc = Vec2{ 0,0 })
			:vel(vel), acc(acc), init_vel(vel), init_acc(acc)
		{
		}

		//オーバーライドしたら必ずこのstart()を呼び出して。
		virtual void start();
		
		virtual void update(double dt);

		virtual void reset();

		Vec2 get_vel();
		Vec2 get_acc();

		void set_vel(const Vec2& vel);
		void set_acc(const Vec2& acc);
	protected:
		Transform* transform;
		Vec2 vel;
		Vec2 acc;

	private:
		Vec2 init_vel;
		Vec2 init_acc;
	};

	//start_action end_actionがある
	class StandardMove :public BaseMove
	{
	public:
		StandardMove(const Vec2& vel = Vec2{ 0,0 }, const Vec2& acc = Vec2{ 0,0 }) :
			active(false), BaseMove(vel, acc) {};

		//開始処理
		virtual void start_action();
		//終了条件
		virtual void end_action();
		//稼働中か
		virtual bool is_working();
		//実行
		virtual void update(double dt);
		//状態をリセット
		virtual void reset();
		//稼働時間を返す
		double get_time();

		Optional<double> maxSpeed = none;

	protected:
		bool active;
		//稼働時間
		double time = 0;
	};

	class FuncMove :public StandardMove
	{
	public:
		using StandardMove::StandardMove;

		void update(double dt)override;

		std::function<Vec2(double, FuncMove*)> Vt = [](double, FuncMove* f) {return f->get_vel(); };
		std::function<Vec2(double dt,FuncMove*)> At= [](double, FuncMove* f) {return f->get_acc(); };
	};

	//class MixedMove final :public StandardMove
	//{
	//public:
	//	template<class... Arg>
	//	MixedMove(Arg... pStandardMoves)
	//	{
	//		for (auto& move : std::initializer_list<StandardMove*>(pStandardMoves...))
	//		{
	//			moves << move;
	//		}
	//	}

	//	void start_action()override;

	//	void update(double dt)override;

	//	Array<StandardMove*> moves;
	//};

	class DecelerateMove final :public StandardMove
	{
	public:
		//静止摩擦
		double staticFriction;

		double threshold = 0.2;
		
		DecelerateMove(double staticFriction);

		void update(double dt)override;

	};
	//transformが向いてる方向へ進む
	class AdvanceMove final :public StandardMove
	{
	public:
		double speed;

		AdvanceMove(double speed = 0);

		void update(double dt)override;
	};

}


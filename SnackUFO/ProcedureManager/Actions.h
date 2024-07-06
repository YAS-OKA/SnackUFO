#pragma once
#include "Action.h"

namespace snk
{
	class Progress;
}

namespace prg
{
	class Condition;

	class ManageActions final:public Action
	{
	public:
		ManageActions(double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"");

		ManageActions(Condition* start, Condition* end, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"");

		~ManageActions();

		void setStartCondition(Condition* condition);

		void setEndCondition(Condition* condition);
		//開始条件/終了条件を指定してアクションを追加する
		Action* add(Action* action, Condition* start, Condition* end);
		
		Action* add(Action* action, double start_time, Condition* end);

		Action* add(Action* action, Condition* start, double time);

		Action* add(Action* action, double start_time, double time);

		//前に追加したアクションが終わってからoffsetTime秒後にスタート
		Action* add(Action* action, double offsetTime = 0);
		//preActionが終わってからoffsetTime秒後にスタート
		Action* add(Action* action, Action* preAction, double offsetTime = 0);
		//前に追加したアクションが始まってからoffsetTime秒後にスタート
		Action* addParallel(Action* action, double offsetTime = 0);
		//preActionが始まってからoffsetTime秒後にスタート
		Action* addParallel(Action* action, Action* preAction, double offsetTime = 0);

		Action* getAction(const String& id);

		bool isActive();

		int32 getLoopCount()const;
		/// @brief アクションをリセットしてから開始する
		void restart();
		/// @brief アクションを開始する
		void start()override;
		/// @brief アクションをリセット
		void reset()override;

		bool isAllFinished();

		void update(double dt)override;
		void draw()const override;
		//終了したときにリセットを行う
		bool init = false;

		bool loop = false;

		bool stopped = false;
		//ループカウントの上限　そんなに大きくなくても良いと思う。
		const int32 maxLoopCount = 10000;
	private:
		friend snk::Progress;

		void end()override;

		//void end()override;
		//終了していないActionがいくつあるか addしたときに++ updateでendのとき-- resetのとき=
		int32 activeNum = 0;
		//secondにはアクションが終わってからの経過時間を入れる
		HashTable<Action*,Optional<double>> endTimeList;
		//HashTableにしてendTimeListの役割も兼ねるようにしたかったけど,addは.backを使うのと,HashTableのソートがうまく機能するのかわからなかったのでやめた。
		Array<Action*> update_list;
		Array<Action*> draw_list;

		int32 loopCount = 0;
	};
}

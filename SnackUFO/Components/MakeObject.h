#pragma once
#include"Entity.hpp"
#include"../GameObject/Object.h"
#include"Progress.h"

namespace snk
{
	class Progress;
	class HitCallback;

	/// @brief コンポーネントをアタッチするなどしてオブジェクトを作るコンポーネント
	class ObjectBehavior :public component::Component
	{
	public:
		virtual void start();

		void makeParent(const ObjectBehavior* parent);
		void makeChiled(ObjectBehavior* chiled)const;

		Transform* transform;
		Object* MainObject;
		Progress* progress;
		HitCallback* hitCallback;
	};
}

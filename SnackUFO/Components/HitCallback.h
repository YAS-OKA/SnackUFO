#pragma once
#include"Entity.hpp"
#include"../GameObject/Object.h"
#include"../GameUtils.h"

namespace snk
{
	//当たった時のイベント
	using HitMethod = std::function<void(Object* self, Object* other)>;

	class HitCallback :public component::Component
	{
	private:
		Object* mainObject;
		GameObjectManager* manager;
		HashTable<ObjectTag, HitMethod>callbacks;
		//相手が衝突できるか
		HashTable<ObjectTag, bool>enable;
	public:

		void start()override;

		void registerCallback(const ObjectTag& tag, const HitMethod& method);

		void registerCallback(HashSet<ObjectTag> tags, const HitMethod& method);

		template <class... Arg>
		void stopHitCheck(Arg... stopTag)
		{
			for (const auto& tag : std::initializer_list<ObjectTag>{ stopTag... })
			{
				enable[tag] = false;
			}
		}
		template <class... Arg>
		void enableHitCheck(Arg... enableTag)
		{
			for (const auto& tag : std::initializer_list<ObjectTag>{ enableTag... })
			{
				enable[tag] = true;
			}
		}

		void allHitMethodEnabled();
		void allHitMethodStoped();

		void check();
	};
}

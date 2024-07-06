#include"Entity.hpp"
#include"../GameUtils.h"
#include"../ProcedureManager/Init.h"

namespace snk
{
	class Progress final:public component::Component
	{
	public:
		template <class... Name>
		void startAction(const Name&... names)
		{
			for (const String& name : std::initializer_list<String>{ names... })
			{
				if (actions.contains(name))
				{
					for (auto& act : actions[name]){
						act->stopped = false;
						act->start();
					}
				}
			}
		}

		template <class... Name>
		void restartAction(const Name&... names)
		{
			for (const String& name : std::initializer_list<String>{ names... })
			{
				if (actions.contains(name))
				{
					for (auto& act : actions[name]) {
						act->stopped = false;
						act->restart();
					}
				}
			}
		}

		template <class... Name>
		void stopAction(const Name&... names)
		{
			for (const String& name : std::initializer_list<String>{ names... })
			{
				if (actions.contains(name)) for (auto& act : actions[name])act->stopped=true;
			}
		}

		template<class... Name>
		void cancelStopped(const Name&... names)
		{
			for (const String& name : std::initializer_list<String>{ names... })
			{
				if (actions.contains(name))for (auto& act : actions[name])act->stoped=false;
			}
		}

		template <class... Name>
		void resetAction(const Name&... names)
		{
			for (const String& name : std::initializer_list<String>{ names... })
			{
				for(auto& act:actions[name])act->reset();
			}
		}

		void startAllAction(HashSet<String> except = {});

		void stopAllAction(HashSet<String> except = {});

		void cancelAllActionStopped();

		void resetAllAction(HashSet<String> except ={});

		virtual prg::ManageActions* setAction(prg::ManageActions* action, const String& actName = U"");

		virtual prg::ManageActions* addAction(prg::ManageActions* action, const String& actName = U"");

		prg::ManageActions* getAction(const String& actName = U"") const;

	private:
		HashTable<String,Array<std::unique_ptr<prg::ManageActions>>> actions;

		void update(double dt)override;

		friend void snk::GameObjectManager::updateGameObjects(double dt);
	};
}

#pragma once

namespace prg
{
	class Condition;

	class ManageActions;

	class Action
	{
	public:		
		//何秒間続けるか 基本的にこれをオーバーライドすればOK
		Action(double time = 0, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"");
		//終わり始まりのトリガーに柔軟性を持たせたい場合は以下のコンストラクタをオーバーロードする
		Action(Condition* end, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"");
		Action(Condition* start, Condition* end, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"");

		virtual ~Action();

		void setTime(double time);

		bool getStarted();

		bool getEnded();

		double updatePriority;
		double drawPriority;

		double time;

		double timeScale;

		Condition* startCondition = nullptr;
		Condition* endCondition = nullptr;
	protected:
		String id;
		bool started = false;
		bool ended = false;
		friend ManageActions;

		//draw以外のスーパーは呼び出す

		virtual void reset()
		{
			time = 0;
			started = false;
			ended = false;
		}
		virtual void start()
		{
			started = true;
		};
		virtual void end()
		{
			ended = true;
		};

		virtual void update(double dt);
		virtual void draw()const {};
	};

	class FuncAction;

	using Fty = std::function<void(double, FuncAction*)>;

	class FuncAction final:public Action
	{
	public:
		FuncAction(std::function<void()>startFunction, double time = 0, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_function([](double, FuncAction*) {}), m_start(startFunction), Action(time, timeScale, updatePriority, drawPriority, id) {}

		FuncAction(std::function<void()>startFunction,std::function<void()>endFunction, double time = 0, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_function([](double, FuncAction*) {}), m_start(startFunction),m_end(endFunction), Action(time, timeScale, updatePriority, drawPriority, id) {}

		FuncAction(std::function<void()>startFunction, std::function<void()>endFunction, Condition* end, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_function([](double, FuncAction*) {}), m_start(startFunction), m_end(endFunction), Action(end, timeScale, updatePriority, drawPriority, id) {}

		FuncAction(std::function<void()>startFunction, std::function<void()>endFunction, Condition* start, Condition* end, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_function([](double, FuncAction*) {}), m_start(startFunction), m_end(endFunction), Action(start, end, timeScale, updatePriority, drawPriority, id) {}

		FuncAction(Fty updateFunction, double time = 0, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_function(updateFunction), Action(time, timeScale, updatePriority, drawPriority, id) {};

		FuncAction(Fty updateFunction, Condition* end, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_function(updateFunction), Action(end, timeScale, updatePriority, drawPriority, id) {};

		FuncAction(Fty updateFunction, std::function<void()>startFunction, double time = 0, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_start(startFunction),m_function(updateFunction), Action(time, timeScale, updatePriority, drawPriority, id)
		{};

		FuncAction(Fty updateFunction, std::function<void()>startFunction,std::function<void()>endFunction, double time = 0, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_start(startFunction), m_function(updateFunction),m_end(endFunction), Action(time, timeScale, updatePriority, drawPriority, id)
		{};

		FuncAction(Fty updateFunction, std::function<void()>startFunction, std::function<void()>endFunction, Condition* end, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_start(startFunction), m_function(updateFunction), m_end(endFunction), Action(end, timeScale, updatePriority, drawPriority, id)
		{};

		FuncAction(Fty updateFunction, std::function<void()>startFunction, std::function<void()>endFunction, Condition* start, Condition* end, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"")
			:m_start(startFunction), m_function(updateFunction), m_end(endFunction), Action(start,end, timeScale, updatePriority, drawPriority, id)
		{};

			//開始処理の関数オブジェクトをセット
		void setStart(std::function<void()> startFunction)
		{
			m_start = startFunction;
		};
		//終了処理の関数オブジェクトをセット
		void setEnd(std::function<void()> endFunction)
		{
			m_end = endFunction;
		}

	private:
		void start()override
		{
			Action::start();
			m_start();
		}

		void update(double dt)override
		{
			Action::update(dt);
			m_function(dt, this);
		};

		void end()override
		{
			Action::end();
			m_end();
		}
		
		std::function<void()> m_start = [](){};
		Fty m_function;
		std::function<void()> m_end = [](){};
	};
}

#pragma once
#include "Action.h"

namespace snk
{
	class IGun;

	class Wait :public prg::Action
	{
	public:
		Wait(double time = 0, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"");
		/// @brief waitTargetアクションが終了するまで待つ
		Wait(Action* waitTarget);
		/*void update(double dt)override
		{
			Action::update(dt);
			Print << U"wating";
		}*/
	};

	class GunShot :
		public prg::Action
	{
	public:
		GunShot(IGun* gun, double time = 0, double timeScale = 1.0, double updatePriority = 1.0, double drawPriority = 1.0, const String& id = U"");

		void update(double dt)override;
	private:
		IGun* gun;
	};

	//class FuncDraw :public prg::Action
	//{
	//public:
	//	void update(double dt)override;

	//	void draw()const override;

	//	bool visible = true;

	//	double dt = 0;

	//	ColorF color = Palette::White;

	//	std::function<void(FuncDraw*)> updateFunction = [](FuncDraw*) {};
	//	std::function<void(const FuncDraw*)> drawFunction = [](const FuncDraw*) {};
	//};
}

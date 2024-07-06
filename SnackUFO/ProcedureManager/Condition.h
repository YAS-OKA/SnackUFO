#pragma once

namespace prg
{
	class Condition
	{
	public:
		virtual ~Condition() {}
		void forcedFire() { fire = true; }

		virtual bool check() {
			return checkFire();
		};
	protected:
		bool checkFire()
		{
			if (fire) { fire = false; return true; }
			return false;
		};
		bool fire=false;
	};

	////複数のCondictionCheckerをまとめたもの
	//class MixedCondition:public Condition
	//{
	//public:
	//	~MixedCondition() {};
	//	// 他の条件が成り立つか調べる
	//	// otherが空の時は無条件でtrue
	//	bool check()
	//	{
	//		bool ans = true;

	//		for (const auto& condition : other)
	//		{

	//			if (not condition.first)
	//			{
	//				//or
	//				if (condition.second)return true;
	//			}
	//			else
	//			{
	//				//and
	//				if (not condition.second)ans = false;
	//			}
	//		}
	//		return ans;
	//	};

	//	// condition_type がtrueのとき束縛する条件 falseのとき許容する条件 をそれぞれ付与する
	//	// check_otherの戻り値において
	//	// 束縛する条件はすべてがtrueのときtrueになる　許容する条件はその条件さえ成り立てばtrueになる
	//	// 二つの条件が混ざってる場合は許容する条件が優先される
	//	void addChecker(Condition* checker, bool condition_type = true);

	//private:
	//	//他の条件クラス　束縛する条件(and)と許容する条件(or)がある
	//	//first==true => and  first==false => or
	//	Array<std::pair<bool, Condition*>> other;
	//};

	//即席関数でチェック
	template <class Fty, std::enable_if_t<std::is_invocable_r_v<bool, Fty>>* = nullptr>
	class FuncCondition :public Condition
	{
	public:
		//ラムダ式などで関数を渡す　関数の戻り値はbool
		FuncCondition(Fty _function) :m_function{ _function } {}

		bool check()
		{
			return checkFire() or m_function();
		};
	private:
		const Fty m_function;
	};
}

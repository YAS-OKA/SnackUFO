#pragma once
#include"Entity.hpp"

namespace snk
{
	class Object;

	class Transform :public component::Component
	{
	private:
		//位置
		Vec2 pos;
		Vec2 prePos;
		Vec2 deltaPos;

		//方向
		double direction;
		double preDirection;
		double deltaDirection;

		//測定用
		Vec2 vel;
		Vec2 preFramePos;
		double angularVel;
		double preFrameDir;

		void calculate();

		void affectChildren();

		Object* objParent;
	public:
		
		//このコンポーネントをアタッチしたオブジェクトの描画優先度(実質z座標)
		double ParentDrawPriority;

		Transform();

		virtual ~Transform(){}

		void moveBy(const Vec2& delta);

		Vec2 getPos()const;

		Vec2 getVel()const;

		double getDirection()const;

		double getAngulerVel()const;

		void setDirection(double dir);

		void setDirAndPreDir(double dir);

		void setPos(const Vec2& pos);

		void setPosAndPrePos(const Vec2& pos);

		void rotateAt(const Vec2& center, double rad);

		void rotate(double rad);

		void start()override;

		void calUpdate(double dt);

		//void draw()const override;
	};
}


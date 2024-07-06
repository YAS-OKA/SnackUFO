#pragma once
#include"Entity.hpp"
#include"../Figure.h"
#include"Transform.h"

namespace snk
{
	class Collider :
		public component::Component
	{
	public:
		void set_hitbox(const Figure& hitbox,const Vec2& relative=Vec2{0,0});

		Figure getHitbox()const;

		void setScale(double scale);

		void addScale(double ds);

		double getScale()const;

		void start();

		void update(double dt);

		void draw()const;

		bool intersects(const Collider& other)const;

		//開発が進んだらこれをfalseにして、hitboxを描きたいやつだけtrueに変更させる
		bool draw_hitbox = false;
		//輪郭を描くか
		bool draw_hitbox_outline = false;
		//色
		ColorF color = ColorF(Palette::White, 0.7);
		ColorF outline_color = Palette::Black;
		//有効/無効
		bool active= true;

		//transform->posからずらしたい場合はこの値で調整
		Vec2 pos = { 0,0 };
		//回転位置　
		Vec2 rotateCenter{ 0,0 };

	private:
		Transform* transform;

		bool intersects(const Figure& other)const;

		Figure hitbox;

		double scale = 1.0;

	};

}

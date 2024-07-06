#pragma once
#include"Entity.hpp"
#include"../Figure.h"

namespace snk
{
	class Camera;

	class Transform;

	class Object;

	class DrawTexture :public component::Component
	{
	public:
		virtual void start()override;

		void update(double dt)override;

		void draw()const override;

		void setTexture(const String& assetName, double scale = 1.0, double rad = 0);

		void setAssetName(const String& assetName);

		void setPos(const Vec2& pos, bool center = true);

		bool visible = true;

		bool inCamera;

		ColorF color = Palette::White;

		double baseScale=1.0;

		double scale = 1.0;

		double rad = 0;

		bool center=true;
		//transform.posからの相対座標 center=trueならtransform.posに画像の中心をセットする falseなら画像の左上をセットする
		Vec2 pos = { 0,0 };
		//posからの相対座標
		Vec2 rotateCenter{ 0,0 };

		bool affectedByTransformsDirection = true;

		Camera* gameCamera;

		Transform* transform;
	protected:
		Optional<String> name = none;
	};

	class DrawFunc :public component::Component
	{
	public:
		void draw()const override;
		std::function<void(const DrawFunc*)> drawingFunction = [](const DrawFunc*) {};
	};

	class DrawFigure :public component::Component
	{
	public:
		void start()override;

		void draw()const override;

		Transform* transform;

		bool drawFrame = false;

		double frameThick = 1.0;

		ColorF frameColor = Palette::White;

		void setFigure(const Figure& fig, ColorF color, double scale = 1, double rad = 0, Vec2 pos = { 0,0 }, Vec2 rotateCenter = { 0,0 });

		Figure fig;

		ColorF color;

		double scale;

		double rad = 0;

		bool visible = true;

		Vec2 pos;//拡大位置

		Vec2 rotateCenter;
	};

	class DrawFigures :public component::Component
	{
	public:
		void start()override;

		void update(double dt)override;

		Transform* transform=nullptr;

		DrawFigure* addFigure(const String& name, const Figure& fig, const ColorF& color, double scale = 1, double rad = 0, Vec2 pos = {0,0}, Vec2 rotateCenter = {0,0});

		DrawFigure* setFigure(const String& name,const Figure& fig,const ColorF& color, double scale = 1, double rad = 0, Vec2 pos = { 0,0 }, Vec2 rotateCenter = { 0,0 });

		void clearFigures();

		HashTable<String,DrawFigure*> figures;

	private:
		Object* parent;
	};

	class DrawFont :public component::Component
	{
	public:
		virtual void start()override;

		void draw()const override;

		void setAssetName(const String& fontAssetName);

		void setPos(const Vec2& pos, bool center = true);

		bool visible = true;

		ColorF color = Palette::White;

		double baseScale = 1.0;

		double scale = 1.0;

		double rad = 0;

		bool center = true;
		//transform.posからの相対座標 center=trueならtransform.posに文章の中心をセットする falseなら文章の左上をセットする
		Vec2 pos = { 0,0 };
		//posからの相対座標
		Vec2 rotateCenter{ 0,0 };

		bool affectedByTransformsDirection = true;

		Camera* gameCamera;

		Transform* transform;

		void setText(const String& text);
	protected:
		Optional<String> name = none;
		Optional<String> text = none;
	};
}

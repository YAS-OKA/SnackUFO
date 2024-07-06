#pragma once
#include"../Components/MakeObject.h"

namespace snk
{
	class Camera :public ObjectBehavior
	{
	public:
		//Camera();
		/*void start()override;*/
		void start()override;

		const RectF getRegion()const;

		void update(double dt)override;

		void draw()const override { return; }

		//cameraAffected=trueのとき何もしないTransformer2Dを返す。
		const Transformer2D getTransformer2D(bool cameraAffected);

		double scale;

		Vec2 center;
	private:
		Camera2D camera;
	};


}

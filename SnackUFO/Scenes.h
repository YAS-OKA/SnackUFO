#include"Common.hpp"
#include"Paraminit.h"
#include"GameUtils.h"

namespace snk
{
	class Title : public App::Scene
	{
	public:
		size_t index = getData().mini_mode;
		size_t Key_index = 1;
		Font font{ 50 };

		void registerGameAsset();

		GameObjectManager manager;

		Title(const InitData& init); //: IScene{ init } {}

		void update()override;

		void draw()const override;

		prg::ManageActions progress;

		RegisterResources r;

		Camera* camera;
	};
}

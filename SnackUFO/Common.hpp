# pragma once
//# include <Siv3D.hpp>
//
enum mode { Stage_Mode, Easy_Mode, Normal_Mode, Hard_Mode };

struct GameData
{
	double BGM_volume = 1.0;
	double Effect_volume = 1.0;
	int stage = 1;
	mode mini_mode = Easy_Mode;
	bool mini_clear = false;
	bool boss_clear = false;
	Input KeyUp = KeyUp;
	Input KeyLeft = KeyLeft;
	Input KeyDown = KeyDown;
	Input KeyRight = KeyRight;
};

using App = SceneManager<String, GameData>;

#pragma once
#include"../Components/MakeObject.h"
#include"../Figure.h"
#include"../ProcedureManager/Actions.h"

/// @brief 以下のコンポーネントをアタッチしてオブジェクトを作る
namespace snk
{
	class Collider;
	class Transform;
	//class TextureDraw;
	class Object;
	class StandardMove;
	enum class ObjectTag;
	//弾はそんなに量産しないからCreateクラスを作らず、それぞれクラスとして定義する。

	class Bullet;
	struct BulletElements
	{
		BulletElements setting(const ObjectTag& tag, HashSet<ObjectTag> targetTag, double damage, const Figure& hitbox);

		double damage=0;

		ObjectTag tag;

		HashSet<ObjectTag> targetTags{};

		Bullet* bullet;// = nullptr;

		void setFigure(const Figure& hitbox,const Vec2& p={0,0});

		Figure getFigure() const;
	private:
		Figure figure = Rect{ 0,0,10,10 }.rotatedAt(0,0,45_deg);
	};

	class LenearLaser;
	//弾
	class Bullet final :public ObjectBehavior
	{
	public:
		~Bullet();

		IGun* gun;

		virtual void start()override;

		void update(double dt)override;

		void setHitbox(const Figure& hitbox, const Vec2& p = { 0,0 });

		void setElements(BulletElements elems);

		BulletElements* getPElems();

		BulletElements getElements()const;
	protected:
		friend LenearLaser;
		BulletElements elems;
	};

	class LenearLaser final :public component::Component
	{
	public:
		//bullet も親が同じだからbulletのデストラクタは呼び出される
		//~LenearLaser(); 

		void start()override;

		void update(double dt)override;

		void setThickness(double thickness);

		bool releaseFirstPos = false;

		IGun* gun;
		//ここに入っている値の分短くなる 0なら先端まで当たり判定を配置
		double disLength=0;

		Vec2 firstPos;
		//レーザーに当たった時のダメージはbaseDamage*dt
		double baseDamage = 0;

		double thickness=5;
		
	private:

		Bullet* bullet;
	};
	namespace util {
		prg::ManageActions* setDissapearAction(ObjectBehavior* behavior, double maxScale, double time);
	}
}

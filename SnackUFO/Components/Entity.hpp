#pragma once
//#include<Siv3D.hpp>

namespace snk
{
	namespace component
	{
		class Entity;

		class Priority
		{
			//値が大きい方が先に更新される
			double priority_update = 0;
			//値が大きい方が後に描画される(前に描画される)
			double priority_draw = 0;
		public:
			double getUpdate() const { return priority_update; };
			double getDraw() const { return -priority_draw; };
			void setUpdate(double priority) { priority_update = priority; };
			void setDraw(double priority) { priority_draw = -priority; };
		};

		//コンポーネントの基底クラス
		class Component
		{
		public:
			Component() {}
			virtual ~Component() {}
			virtual void start() {}
			virtual void update(double dt) {}
			virtual void draw()const {}

			//自身を排除する
			template <class T>
			void removeSelf();

			//デタッチする
			template<class T>
			void resign();

			// 親にコンポーネントを追加する 1f遅れる
			template<class T>
			T* AddComponent(const String& id, T* component = new T());
			template<class T>
			T* AddComponent(T* component = new T());
			// 親にコンポーネントを追加する　遅延なし
			template<class T>
			T* AddComponentImmediately(const String& id, T* component = new T());
			template<class T>
			T* AddComponentImmediately(T* component = new T());

			//親のコンポーネントを取得する。
			template<class T>
			T* GetComponent();
			template<class T>
			T* GetComponent(const String& id);
			template<class T>
			Array<T*> GetComponentArr();

			Priority priority;

			Entity* Parent = nullptr;
			//追加番号　priorityが等しい場合、これをもとにソート
			int32 index;
		};

		//コンポーネントを適用した基底クラス
		//HashTableを使っているので計算量は基本O(1)？
		//同一の型のコンポーネントを追加することは可能　その場合idを指定しないとGetComponentはできない（バグ防止）
		//型が重複してるならGetComponentArrを使うことを推奨
		class Entity
		{
		public:
			double z = 0;

			Entity() {}

			virtual ~Entity()
			{
				//削除されず残ったコンポーネントのポインタを削除
				deleteCash();
				deleteComponents();
				deleteGarbages();				
			};

			//コンポーネントをアップデート
			virtual void update_components(double dt)
			{
				deleteGarbages();
				Array<Component*> startComponents;
				//キャッシュからコンポーネントを追加
				for (auto& com_ls : Cash)
				{
					if (com_ls.second.empty())continue;
					for (auto& com : com_ls.second)
					{
						startComponents << com.second;
						components[com_ls.first][com.first] = com.second;
						componentsNum++;
					}
				}
				Cash.clear();
				cashNum = 0;
				for (auto& com : startComponents)com->start();

				//allComponentをセット
				allComponentForUpdate.clear();
				allComponentForDraw.clear();
				for (const auto& com_ls : components)
				{
					for (const auto& com : com_ls.second)
					{
						allComponentForUpdate << com.second;
						allComponentForDraw << com.second;
					}
				}
				std::sort(allComponentForUpdate.begin(), allComponentForUpdate.end(), [this](const Component* com1, const Component* com2) {return _replace_flag(true, com1, com2); });
				std::sort(allComponentForDraw.begin(), allComponentForDraw.end(), [this](const Component* com1, const Component* com2) {return _replace_flag(false, com1, com2); });

				for (auto& com : allComponentForUpdate)
					com->update(dt);
			};

			//コンポーネントをdraw
			virtual void draw_components() const
			{
				for (const auto& com : allComponentForDraw)
					com->draw();
			};

			const String& get_id() const { return id; }

			const String& get_name() const { return name; }

			//一致するコンポーネントを削除 下のオーバーロードの関数を呼び出す
			template<class T>
			void remove(Component* com)
			{
				Optional<String> id=_get_id<T>(com);
				if (not id.has_value())return;
				remove<T>(*id);
			}
			//コンポーネントの削除 コンポーネントの型が重複している場合下のif文でid指定で消す 削除したコンポーネントはgarbagesへ
			template<class T>
			void remove(const String& id = U"")
			{
				if (components[typeid(T).hash_code()].contains(id))
				{
					garbages << components[typeid(T).hash_code()][id];
					components[typeid(T).hash_code()].erase(id);
					if (ids.contains(typeid(T).hash_code()))ids[typeid(T).hash_code()] -= 1;
					//空だったらキーもクリア
					if(components[typeid(T).hash_code()].empty())components.erase(typeid(T).hash_code());
					componentsNum--;
				}
				else if (components[typeid(T).hash_code()].size() <= 1)
				{
					for (auto& com : components[typeid(T).hash_code()])garbages << com.second;
					components.erase(typeid(T).hash_code());
					if (ids.contains(typeid(T).hash_code()))ids[typeid(T).hash_code()] -= 1;
					componentsNum--;
				}
			}
			//コンポーネント解放
			template<class T>
			void release(Component* com)
			{
				Optional<String> id = _get_id<T>(com);
				if (not id.has_value())return;
				release<T>(*id);
			}
			//コンポーネント解放
			template<class T>
			void release(const String& id = U"")
			{
				if (components[typeid(T).hash_code()].contains(id))
				{
					components[typeid(T).hash_code()].erase(id);
					if (ids.contains(typeid(T).hash_code()))ids[typeid(T).hash_code()] -= 1;
					//空だったらキーもクリア
					if (components[typeid(T).hash_code()].empty())components.erase(typeid(T).hash_code());
					componentsNum--;
				}
				else if (components[typeid(T).hash_code()].size() <= 1)
				{
					components.erase(typeid(T).hash_code());
					if (ids.contains(typeid(T).hash_code()))ids[typeid(T).hash_code()] -= 1;
					componentsNum--;
				}
			}

			//コンポーネントの追加　idがかぶったら上書き
			template<class T>
			T* AddComponent(const String& id, T* component = new T())
			{
				if (component->Parent == nullptr)component->Parent = this;
				component->index = componentsNum + cashNum;

				Cash[typeid(T).hash_code()][id] = component;
				cashNum++;
				return component;
			}
			//第2引数はtypeid(..).hash_codeを渡す
			Component* AddComponent(Component* component,size_t objectHashCode)
			{
				if (component->Parent == nullptr)component->Parent = this;
				component->index = componentsNum + cashNum;

				if (not ids.contains(objectHashCode))
				{
					ids[objectHashCode] = 0;
				}
				else
				{
					ids[objectHashCode] += 1;
				}

				Cash[objectHashCode][Format(ids[objectHashCode])] = component;
			}
			//コンポーネントの追加　hash_codeなので異なるクラスならidはたぶんかぶらない
			template<class T>
			T* AddComponent(T* component = new T())
			{
				if (component->Parent == nullptr)component->Parent = this;
				component->index = componentsNum + cashNum;

				if (not ids.contains(typeid(T).hash_code()))
				{
					ids[typeid(T).hash_code()] = 0;
				}
				else
				{
					ids[typeid(T).hash_code()] += 1;
				}

				Cash[typeid(T).hash_code()][Format(ids[typeid(T).hash_code()])] = component;
				cashNum++;
				return component;
			}

			template<class T>
			T* AddComponentImmediately(const String& id, T* component = new T())
			{
				if (component->Parent == nullptr)component->Parent = this;
				component->start();
				component->index = componentsNum;

				components[typeid(T).hash_code()][id] = component;
				componentsNum++;
				return component;
			}

			//コンポーネントの追加　hash_codeなので異なるクラスならidはたぶんかぶらない
			template<class T>
			T* AddComponentImmediately(T* component = new T())
			{
				if (component->Parent == nullptr)component->Parent = this;
				component->start();
				component->index = componentsNum;

				if (not ids.contains(typeid(T).hash_code()))
				{
					ids[typeid(T).hash_code()] = 0;
				}
				else
				{
					ids[typeid(T).hash_code()] += 1;
				}
				components[typeid(T).hash_code()][Format(ids[typeid(T).hash_code()])] = component;
				componentsNum++;
				return component;
			}
			//第2引数はtypeid(..).hash_codeを渡す
			Component* AddComponentImmediately(Component* component,const size_t objectHashCode)
			{
				if (component->Parent == nullptr)component->Parent = this;
				component->start();
				component->index = componentsNum;

				if (not ids.contains(objectHashCode))
				{
					ids[objectHashCode] = 0;
				}
				else
				{
					ids[objectHashCode] += 1;
				}
				components[objectHashCode][Format(ids[objectHashCode])] = component;
				componentsNum++;
				return component;
			}

			//コンポーネントの取得　型の重複はなし
			template<class T>
			T* GetComponent()
			{
				if (not components.contains(typeid(T).hash_code()))
				{
					return nullptr;
				}
				//型の重複がある場合エラー
				if (components[typeid(T).hash_code()].size() > 1)
				{
					throw Error{ U"idを指定してください。\n重複:" + Format(typeid(T).hash_code()) };

					return nullptr;
				}
				return static_cast<T*>(components[typeid(T).hash_code()].begin()->second);
			}

			template<class T>
			const T* GetComponent() const
			{
				if (not components.contains(typeid(T).hash_code()))
				{
					return nullptr;
				}
				//型の重複がある場合エラー
				if (components.at(typeid(T).hash_code()).size() > 1)
				{
					throw Error{ U"idを指定してください。\n重複:" + Format(typeid(T).hash_code()) };

					return nullptr;
				}
				return static_cast<T*>(components.at(typeid(T).hash_code()).begin()->second);
			}

			//コンポーネントの取得 id指定。使いどころは複数のコンポーネントが同一の型で重複しているときとか。
			template<class T>
			T* GetComponent(const String& id)
			{
				if (not components.contains(typeid(T).hash_code()))
				{
					return nullptr;
				}

				if (not components[typeid(T).hash_code()].contains(id))
				{
					return nullptr;
				}

				return static_cast<T*>(components[typeid(T).hash_code()][id]);
			}

			template<class T>
			Array<T*> GetComponentArr(bool sort_by_priority_of_update = true)
			{
				Array<T*> arr;

				if (not components.contains(typeid(T).hash_code()))
					return arr;

				for (const auto& h : components[typeid(T).hash_code()])
				{
					arr << static_cast<T*>(h.second);
				}

				if (sort_by_priority_of_update) {
					std::sort(arr.begin(), arr.end(), [this](const T* com1, const T* com2) {return _replace_flag(true, com1, com2); });
				}
				else {
					std::sort(arr.begin(), arr.end(), [this](const T* com1, const T* com2) {return _replace_flag(false, com1, com2); });
					arr.reverse();
				}
				return arr;
			}

			HashTable<size_t, HashTable<String, Component*>> getAllComponents() {
				return components;
			};

			String id = U"entity";

			String name = U"entity";
		private:
			int32 componentsNum = 0;
			int32 cashNum = 0;
			HashTable<size_t, HashTable<String, Component*>> components;
			//すべてのコンポーネントをここにぶち込む priorityでソートするため
			//また、これらが持つポインタはcomponentsが持つポインタを指すのでdeleteComponents()でメモリが解放される。
			//わざわざdeleteする必要はない。
			Array<Component*> allComponentForUpdate;
			Array<Component*> allComponentForDraw;
			//コンポーネントのガーベージコレクション
			Array<Component*> garbages;
			//コンポーネントのキャッシュ　ここのコンポーネントは次のフレームでcomponentsに追加される
			HashTable<size_t, HashTable<String, Component*>> Cash;

			//メモリの解放
			void deleteComponents()
			{
				for (auto& multi_comp : components)
				{
					for (auto& comp : multi_comp.second)
					{
						delete comp.second;
					}
				}
				components.clear();
			};
			//メモリの解放
			void deleteCash()
			{
				for (auto& cash_ls : Cash)
				{
					for (auto& com : cash_ls.second)if (com.second != nullptr)delete com.second;
					cash_ls.second.clear();
				}
				Cash.clear();
			}
			//メモリの解放
			void deleteGarbages()
			{
				//ガーベージをクリア
				for (auto& garbage : garbages)
				{
					if (garbage != nullptr)delete garbage;
				}
				garbages.clear();
			};

			bool _replace_flag(bool update_priority, const Component* s, const Component* other)
			{
				if (update_priority) {
					if (s->priority.getUpdate() != other->priority.getUpdate())return s->priority.getUpdate() < other->priority.getUpdate();
				}
				else {
					if (s->priority.getDraw() != other->priority.getDraw())return s->priority.getDraw() < other->priority.getDraw();
				}
				return s->index < other->index;
			};

			template<class T>
			Optional<String> _get_id(Component* com)
			{
				if (not components.contains(typeid(T).hash_code()))return none;
				for (const auto& component : components[typeid(T).hash_code()])
				{
					if (component.second == com)
					{
						return component.first;
					}
				}
				return none;
			}

			HashTable<size_t, size_t>ids;
		};

		template<class T>
		void Component::resign()
		{
			Parent->release<T>(this);
		}

		template <class T>
		void Component::removeSelf()
		{
			Parent->remove<T>(this);
		};

		template<class T>
		T* Component::AddComponent(const String& id, T* component)
		{
			return Parent->AddComponent<T>(id, component);
		};

		template<class T>
		T* Component::AddComponent(T* component)
		{
			return Parent->AddComponent<T>(component);
		};

		template<class T>
		T* Component::AddComponentImmediately(const String& id, T* component)
		{
			return Parent->AddComponentImmediately<T>(id, component);
		}

		template<class T>
		T* Component::AddComponentImmediately(T* component)
		{
			return Parent->AddComponentImmediately<T>(component);
		}

		template<class T>
		T* Component::GetComponent()
		{
			return Parent->GetComponent<T>();
		}

		template<class T>
		T* Component::GetComponent(const String& id)
		{
			return Parent->GetComponent<T>(id);
		}

		template<class T>
		Array<T*> Component::GetComponentArr()
		{
			return Parent->GetComponentArr<T>();
		}
	}
}

#pragma once
#include"GameParameterManager.h"

class JsonSetter
{
public:
	//本当はコンストラクタをprivateに入れたかったけどprivateに入れるとmake_sharedが使えなくなる
	//おそらくmake_shared内でコンストラクタ等を呼び出すから？
	//このクラスはシングルトンにしたかったので実体化するときはNewメソッドを呼んで
	JsonSetter(const String& path, bool reset = true);
	~JsonSetter();
private:
	template<typename... Names>
	friend void params_setter::set_nest(Names...args);

	template<class T, typename... Names>
	friend void params_setter::set_param(T default_value,String explanation,Names ... args);

	friend GameParameterManager::GameParameterManager(const JSON& json);

	static void ClearNest();

	static void add_nest(const String& n);

	Array<String> nest;

	JSON json;

	static JsonSetter* instance;

public:
	JSON get_json();
	const String& path;
	static std::unique_ptr<JsonSetter> New(const String& path, bool reset = true);
};

namespace object_params
{
	/// @brief JSONファイルを作成せず既存のJSONからパラメータを得る
	/// @param path 既存のJSONファイルのパス
	/// @return JSONファイル
	inline JSON Init(const String& path)
	{
		return JSON::Load(Resource(path));
	}
}

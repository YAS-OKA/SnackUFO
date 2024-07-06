#pragma once
//#include<Siv3D.hpp>

class JsonSetter;

class GameParameterManager
{
public:
	GameParameterManager(const JSON& json);
	~GameParameterManager();

	/// @brief パラメータを取得します
	/// @tparam T パラメータの型
	/// @tparam ...Names パラメータ名　階層を表せる　基本的にjsonの値を取得するときと同じ
	/// @param ...args 各階層の名前
	/// @return パラメータを返す
	template<class T,typename... Names>
	T params(Names... args) const
	{
		if (std::initializer_list<String>{ args... }.size() == 0) { return json[U"value"].get<T>(); }

		JSON* j = nullptr;

		//NamesがStringじゃない場合、ここでエラー
		for (const String& str : std::initializer_list<String>{ args... }) {
			if (j == nullptr)j = new JSON(json[str]);
			else *j=(*j)[str];
		}

		T value = (*j)[U"value"].get<T>();
		delete j;
		return value;
	}

	/// @brief 指定した階層を最上部とするGameParameterManagerを新たに生成する
	/// @param ...args 各階層の名前
	/// @return 生成したGameParameterManager
	template<typename...Names>
	GameParameterManager getManager(Names... args)const
	{
		JSON* j = nullptr;

		//NamesがStringじゃない場合、ここでエラー
		for (const String& str : std::initializer_list<String>{ args... }) {
			if (j == nullptr)j = new JSON(json[str]);
			else *j = (*j)[str];
		}
		if (j == nullptr)return GameParameterManager(json);
		GameParameterManager m(*j);
		delete j;
		return m;
	}

	//新しく生成する pathがcommonの場合、common_pathのJSONが読み込まれる
	template<typename...Names>
	static GameParameterManager createManager(String path,Names... args)
	{
		if (path == U"common" and common_path != U"")path = common_path;
		JSON j = JSON::Load(Resource(path));
		//NamesがStringじゃない場合、ここでエラー
		for (const String& str : std::initializer_list<String>{ args... }) {
			j = j[str];
		}
		GameParameterManager m(j);
		return	m;
	}

	static void setCommonPath(const String& path)
	{
		common_path = path;
	}

	JSON json;
	//プロジェクト内で共有jsonパス
	static String common_path;
};

/// @brief 名前衝突を避ける
/// JsonSetterから分離させた理由はusing namespaceでparams_setter::を省略できるようにするため
namespace params_setter
{
	/// @brief オブジェクトの入れ子構造をここで指定 リセットされる
	/// @param ...args 各階層の名前
	template<typename... Names>
	void set_nest(Names...args)
	{
		JsonSetter::ClearNest();
		//NamesがStringじゃない場合、ここでエラー
		for (const String& str : std::initializer_list<String>{ args... })
		{
			JsonSetter::add_nest(str);
		}
	}
	/// @brief パラメータの初期設定を行う
	/// @tparam T パラメータの値の型
	/// @param default_value パラメータのデフォルト値
	/// @param explanation パラメータの説明　U""で省略
	/// @param ...args 各階層の名前
	template<class T, typename... Names>
	void set_param(T default_value, String explanation, Names ... args)
	{
		if (std::initializer_list<String>{ args... }.size() == 0)throw Error{ U"無名のオブジェクトは追加できません" };

		//入れ子情報を入れていく
		Array<JSON>nest_json{ JsonSetter::instance->json };
		//最下部までいく
		for (const auto& n : JsonSetter::instance->nest)
		{
			nest_json << nest_json.back()[n];
		}
		for (const String& n : std::initializer_list<String>{ args... })
		{
			if(not nest_json.back().contains(n))nest_json.back()[n] = JSON();
			nest_json << nest_json.back()[n];
		}
		//値を代入
		nest_json.back()[U"value"] = default_value;
		if (not explanation.isEmpty())nest_json.back()[U"explanation"] = explanation;
	}
}

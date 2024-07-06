#include "JsonSetter.h"

JsonSetter* JsonSetter::instance = NULL;

JsonSetter::JsonSetter(const String& path,bool reset)
	:path(path)
{
	if (reset or (not FileSystem::IsFile(path)))
	{
		json = JSON();
	}
	else
	{
		json = JSON::Load(Resource(path));
	}
}

JsonSetter::~JsonSetter()
{
	json.save(path);
};

JSON JsonSetter::get_json()
{
	return json;
}

void JsonSetter::ClearNest()
{
	instance->nest.clear();
}

void JsonSetter::add_nest(const String& n)
{
	instance->nest << n;
}

std::unique_ptr<JsonSetter> JsonSetter::New(const String& path, bool reset)
{
	if (instance == NULL) instance = new JsonSetter(path,reset);
	return std::make_unique<JsonSetter>(*std::move(instance));
}

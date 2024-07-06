#include "../stdafx.h"
#include "ResourceManager.h"

resource::Mode ResourceManager::mode = resource::debug;

FilePath ResourceManager::myResource(FilePathView path)
{
	if (mode == resource::relase)return Resource(path);
	else return FilePath{ path };
}

// FontAsset

resource::font::font(AssetNameView name)
	:FontAsset(name)
{

}

bool resource::font::Load(AssetNameView name, const String& preloadText)
{
	return FontAsset::Load(name, preloadText);

}

bool resource::font::Register(AssetNameView name, int32 fontSize, FilePathView path, FontStyle style)
{	
	
	return FontAsset::Register(name, fontSize,ResourceManager::myResource(path), style);
}

bool resource::font::Register(AssetNameView name, int32 fontSize, FilePathView path, size_t faceIndex, FontStyle style)
{
	
	
	return FontAsset::Register(name,fontSize,ResourceManager::myResource(path),faceIndex,style);
}

bool resource::font::Register(AssetNameView name, int32 fontSize, Typeface typeface, FontStyle style)
{
	
	return FontAsset::Register(name,fontSize,typeface,style);
}

bool resource::font::Register(AssetNameView name, FontMethod fontMethod, int32 fontSize, FilePathView path, FontStyle style)
{
	
	
	return FontAsset::Register(name,fontMethod,fontSize,ResourceManager::myResource(path),style);
}

bool resource::font::Register(AssetNameView name, FontMethod fontMethod, int32 fontSize, FilePathView path, size_t faceIndex, FontStyle style)
{
	
	
	return FontAsset::Register(name,fontMethod,fontSize,ResourceManager::myResource(path),faceIndex,style);
}

bool resource::font::Register(AssetNameView name, FontMethod fontMethod, int32 fontSize, Typeface typeface, FontStyle style)
{
	return FontAsset::Register(name,fontMethod,fontSize,typeface,style);
}

bool resource::font::Register(AssetNameView name, std::unique_ptr<FontAssetData>&& data)
{
	
	return FontAsset::Register(name,std::move(data));
}


bool resource::font::Register(AssetNameAndTags nameAndTags, int32 fontSize, FilePathView path, FontStyle style)
{
	
	return FontAsset::Register(nameAndTags,fontSize,ResourceManager::myResource(path),style);
}

bool resource::font::Register(AssetNameAndTags nameAndTags, int32 fontSize, FilePathView path, size_t faceIndex, FontStyle style)
{
	
	
	return FontAsset::Register(nameAndTags,fontSize,ResourceManager::myResource(path),faceIndex,style);
}

bool resource::font::Register(AssetNameAndTags nameAndTags, int32 fontSize, Typeface typeface, FontStyle style)
{
	
	return FontAsset::Register(nameAndTags,fontSize,typeface,style);
}

bool resource::font::Register(AssetNameAndTags nameAndTags, FontMethod fontMethod, int32 fontSize, FilePathView path, FontStyle style)
{
	
	
	return FontAsset::Register(nameAndTags,fontMethod,fontSize,ResourceManager::myResource(path),style);
}

bool resource::font::Register(AssetNameAndTags nameAndTags, FontMethod fontMethod, int32 fontSize, FilePathView path, size_t faceIndex, FontStyle style)
{
	
	
	return FontAsset::Register(nameAndTags,fontMethod,fontSize,ResourceManager::myResource(path),faceIndex,style);
}

bool resource::font::Register(AssetNameAndTags nameAndTags, FontMethod fontMethod, int32 fontSize, Typeface typeface, FontStyle style)
{
	
	return FontAsset::Register(nameAndTags,fontMethod,fontSize,typeface,style);
}

// TextureAsset

resource::texture::texture(AssetNameView name)
	:TextureAsset(name)
{

}

bool resource::texture::Load(AssetNameView name)
{
	return FontAsset::Load(name);
}

bool resource::texture::Register(AssetNameView name, FilePathView path, TextureDesc desc)
{
	
	return TextureAsset::Register(name,ResourceManager::myResource(path),desc);
}

bool resource::texture::Register(AssetNameView name, FilePathView rgb, FilePathView alpha, TextureDesc desc)
{
	
	return TextureAsset::Register(name,rgb,alpha,desc);
}

bool resource::texture::Register(AssetNameView name, const Color& rgb, FilePathView alpha, TextureDesc desc)
{
	
	return TextureAsset::Register(name,rgb,alpha);
}

bool resource::texture::Register(AssetNameView name, const Emoji& emoji, TextureDesc desc)
{
	
	return TextureAsset::Register(name,emoji,desc);
}

bool resource::texture::Register(AssetNameView name, const Icon& icon, int32 size, TextureDesc desc)
{
	return TextureAsset::Register(name,icon,size,desc);
}

bool resource::texture::Register(AssetNameView name, std::unique_ptr<TextureAssetData>&& data)
{
	return TextureAsset::Register(name,std::move(data));
}


bool resource::texture::Register(AssetNameAndTags nameAndTags, FilePathView path, TextureDesc desc)
{	
	return TextureAsset::Register(nameAndTags,ResourceManager::myResource(path),desc);
}

bool resource::texture::Register(AssetNameAndTags nameAndTags, FilePathView rgb, FilePathView alpha, TextureDesc desc)
{
	
	return TextureAsset::Register(nameAndTags,rgb,alpha,desc);
}

bool resource::texture::Register(AssetNameAndTags nameAndTags, const Color& rgb, FilePathView alpha, TextureDesc desc)
{
	
	return TextureAsset::Register(nameAndTags,rgb,alpha,desc);
}

bool resource::texture::Register(AssetNameAndTags nameAndTags, const Emoji& emoji, TextureDesc desc)
{
	
	return TextureAsset::Register(nameAndTags,emoji,desc);
}

bool resource::texture::Register(AssetNameAndTags nameAndTags, const Icon& icon, int32 size, TextureDesc desc)
{
	
	return TextureAsset::Register(nameAndTags,icon,size,desc);
}

// AudioAsset

resource::audio::audio(AssetNameView name)
	:AudioAsset(name)
{

}

bool resource::audio::Load(AssetNameView name)
{
	return AudioAsset::Load(name);
}

bool resource::audio::Register(AssetNameView name, FilePathView path)
{
	
	return AudioAsset::Register(name, ResourceManager::myResource(path));
}

bool resource::audio::Register(AssetNameView name, FilePathView path, const Loop loop)
{
	
	
	return AudioAsset::Register(name,ResourceManager::myResource(path),loop);
}

bool resource::audio::Register(AssetNameView name, FilePathView path, Arg::loopBegin_<uint64> loopBegin)
{
	
	
	return AudioAsset::Register(name,ResourceManager::myResource(path),loopBegin);
}

bool resource::audio::Register(AssetNameView name, FilePathView path, Arg::loopBegin_<uint64> loopBegin, Arg::loopEnd_<uint64> loopEnd)
{
	
	
	return AudioAsset::Register(name,ResourceManager::myResource(path),loopBegin,loopEnd);
}

bool resource::audio::Register(AssetNameView name, FilePathView path, const Optional<AudioLoopTiming>& loop)
{
	
	
	return AudioAsset::Register(name,ResourceManager::myResource(path),loop);
}

bool resource::audio::Register(AssetNameView name, Audio::FileStreaming st, FilePathView path)
{
	
	return AudioAsset::Register(name,ResourceManager::myResource(path));
}

bool resource::audio::Register(AssetNameView name, Audio::FileStreaming st, FilePathView path, Loop loop)
{
	return AudioAsset::Register(name,st,ResourceManager::myResource(path),loop);
}

bool resource::audio::Register(AssetNameView name, Audio::FileStreaming st, FilePathView path, Arg::loopBegin_<uint64> loopBegin)
{
	
	
	return AudioAsset::Register(name,st,ResourceManager::myResource(path),loopBegin);
}

bool resource::audio::Register(AssetNameView name, GMInstrument instrument, uint8 key, const Duration& duration, double velocity, Arg::sampleRate_<uint32> sampleRate)
{
	
	return AudioAsset::Register(name,instrument,key,duration,velocity,sampleRate);
}

bool resource::audio::Register(AssetNameView name, GMInstrument instrument, uint8 key, const Duration& noteOn, const Duration& noteOff, double velocity, Arg::sampleRate_<uint32> sampleRate)
{
	
	return AudioAsset::Register(name,instrument,key,noteOn,noteOff,velocity,sampleRate);
}

bool resource::audio::Register(AssetNameView name, std::unique_ptr<AudioAssetData>&& data)
{
	
	return AudioAsset::Register(name, std::move(data));
}


bool resource::audio::Register(AssetNameAndTags nameAndTag, FilePathView path)
{
	
	
	return AudioAsset::Register(nameAndTag, ResourceManager::myResource(path));
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, FilePathView path, const Loop loop)
{
	
	
	return AudioAsset::Register(nameAndTag,ResourceManager::myResource(path),loop);
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, FilePathView path, Arg::loopBegin_<uint64> loopBegin)
{
	return AudioAsset::Register(nameAndTag,ResourceManager::myResource(path),loopBegin);
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, FilePathView path, Arg::loopBegin_<uint64> loopBegin, Arg::loopEnd_<uint64> loopEnd)
{
	return AudioAsset::Register(nameAndTag,ResourceManager::myResource(path),loopBegin,loopEnd);
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, FilePathView path, const Optional<AudioLoopTiming>& loop)
{
	return AudioAsset::Register(nameAndTag,ResourceManager::myResource(path),loop);
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, Audio::FileStreaming st, FilePathView path)
{

	return AudioAsset::Register(nameAndTag,st, ResourceManager::myResource(path));
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, Audio::FileStreaming st, FilePathView path, Loop loop)
{
	return AudioAsset::Register(nameAndTag,st,ResourceManager::myResource(path),loop);
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, Audio::FileStreaming st, FilePathView path, Arg::loopBegin_<uint64> loopBegin)
{
	return AudioAsset::Register(nameAndTag,st,ResourceManager::myResource(path),loopBegin);
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, GMInstrument instrument, uint8 key, const Duration& duration, double velocity, Arg::sampleRate_<uint32> sampleRate)
{	
	return AudioAsset::Register(nameAndTag,instrument,key,duration,velocity,sampleRate);
}

bool resource::audio::Register(AssetNameAndTags nameAndTag, GMInstrument instrument, uint8 key, const Duration& noteOn, const Duration& noteOff, double velocity, Arg::sampleRate_<uint32> sampleRate)
{
	
	return AudioAsset::Register(nameAndTag,instrument,key,noteOn,noteOff,velocity,sampleRate);
}

String RegisterResources::operator()(const String& name)
{
	switch (type)
	{
	case AssetType::none:
		throw Error{ U"typeを設定してください" };
	case AssetType::texture:
		textures << name;
		break;
	case AssetType::font:
		fonts << name;
		break;
	case AssetType::audio:
		audios << name;
		break;
	}
	return name;
}

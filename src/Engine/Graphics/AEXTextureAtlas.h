#pragma once

#include <Core/AEXCore.h>
#include <aexmath/AEXMath.h>

namespace spine
{
	class Atlas;
}
namespace AEX
{
	// forward declarations
	class Texture;
	template <typename T> class TResource;
	
	
	class TextureAtlas :  public IBase
	{
		AEX_RTTI_DECL(TextureAtlas, IBase);

		// provide functions to get the region based on the texture name
		TResource<Texture>* GetTextureFromRegionName(const std::string& spriteName);
		void GetRegionMinMax(const std::string& regionName, AEVec2& uvMin, AEVec2& uvMax);
		AEVec2 GetRegionSize(const std::string& regionName);

		std::vector<std::string> GetAllRegionNames();

	//private: 
		spine::Atlas* mInternalAtlas = nullptr;
	};
}
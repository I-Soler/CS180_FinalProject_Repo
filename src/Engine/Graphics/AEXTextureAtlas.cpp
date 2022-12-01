#include<spine/spine.h>
#include <Graphics/AEXTextureAtlas.h>

namespace AEX
{
	TResource<Texture>* TextureAtlas::GetTextureFromRegionName(const std::string& regionName)
	{
		auto region = mInternalAtlas->findRegion(regionName.c_str());
		if (!region)return nullptr;
		return static_cast<TResource<Texture>*>(region->page->getRendererObject());
	}
	void TextureAtlas::GetRegionMinMax(const std::string& regionName, AEVec2& uvMin, AEVec2& uvMax)
	{
		auto region = mInternalAtlas->findRegion(regionName.c_str());
		if (nullptr == region)
			return;

		uvMin.x = region->u;  uvMin.y = 1.0f - region->v2; 
		uvMax.x = region->u2; uvMax.y = 1.0f - region->v;
	}
	AEVec2 TextureAtlas::GetRegionSize(const std::string& regionName)
	{

		auto region = mInternalAtlas->findRegion(regionName.c_str());
		if (nullptr == region)
			return AEVec2();

		return AEVec2(region->originalWidth, region->originalHeight);
	}
	std::vector<std::string> TextureAtlas::GetAllRegionNames()
	{
		std::vector<std::string> regionNames;
		if (mInternalAtlas)
		{
			for (u32 i = 0; i < mInternalAtlas->getRegions().size(); ++i) {
				regionNames.push_back(mInternalAtlas->getRegions()[i]->name.buffer());
			}
		}
		return regionNames;
	}
}
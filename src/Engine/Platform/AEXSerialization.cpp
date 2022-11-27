#include <aexmath/AEXMath.h>
#include <Graphics/AEXColor.h>
#include "AEXSerialization.h"
#include <iomanip> // std::setw()
#include <fstream>
#include <iostream>

using namespace AEX;
nlohmann::json& operator << (nlohmann::json& j, const AEVec2& vec) {
	j.push_back(vec.x);
	j.push_back(vec.y);

	return j;
}
AEVec2& operator >> (const nlohmann::json& j, AEVec2& vec) {
	// sanity check
	if (j.is_array() && j.size() == 2) {
		vec.x = j[0];
		vec.y = j[1];
	}
	return vec;
}

nlohmann::json& operator << (nlohmann::json& j, const AEX::AEVec3& vec)
{
	j.push_back(vec.x);
	j.push_back(vec.y);
	j.push_back(vec.z);

	return j;
}
AEX::AEVec3& operator >> (const nlohmann::json& j, AEX::AEVec3& vec)
{
	// sanity check
	if (j.is_array() && j.size() == 3) {
		vec.x = j[0];
		vec.y = j[1];
		vec.z = j[2];
	}
	return vec;
}

nlohmann::json& operator<<(nlohmann::json& j, const AEX::AEVec4& vec)
{
	j.push_back(vec.x);
	j.push_back(vec.y);
	j.push_back(vec.z);
	j.push_back(vec.w);

	return j;
}

AEX::AEVec4& operator>>(const nlohmann::json& j, AEX::AEVec4& vec)
{
	// sanity check
	if (j.is_array() && j.size() == 4) {
		vec.x = j[0];
		vec.y = j[1];
		vec.z = j[2];
		vec.w = j[3];
	}
	return vec;
}

nlohmann::json& operator<<(nlohmann::json& j, const AEX::Color& vec)
{
	j.push_back(vec.x);
	j.push_back(vec.y);
	j.push_back(vec.z);
	j.push_back(vec.w);

	return j;
}

AEX::Color& operator>>(const nlohmann::json& j, AEX::Color& vec)
{
	// sanity check
	if (j.is_array() && j.size() == 4) {
		vec.x = j[0];
		vec.y = j[1];
		vec.z = j[2];
		vec.w = j[3];
	}
	return vec;
}

nlohmann::json& operator << (nlohmann::json& j, const AEX::Transform& transform)
{
	j["position"] << transform.mTranslation;
	j["scale"] << transform.mScale;
	j["rotationZ"] = transform.mOrientation;

	return j;
}
AEX::Transform& operator >> (const nlohmann::json& j, AEX::Transform& transform)
{
	// read the position, if it exists
	auto it = j.find("position");
	if (it != j.end()) (*it) >> transform.mTranslation;

	it = j.find("scale");
	if (it != j.end()) (*it) >> transform.mScale;

	it = j.find("rotationZ");
	if (it != j.end()) transform.mOrientation = *it;

	return transform;
}

void SaveJsonToFile(const nlohmann::json& j, const char* filename)
{
	std::ofstream f(filename);
	if (f.is_open() && f.good())
	{
		f << std::setw(4) << j;
		f.close();
	}
}

void ReadJsonFromFile(nlohmann::json& j, const char* filename)
{
	std::ifstream f(filename);

	if (f.is_open() && f.good())
	{
		f >> j;
		f.close();
	}
}
#pragma once
#include <Extern/json/json.hpp> // ....MEH.... 
namespace AEX
{
	struct AEVec2; 
	struct AEVec3;
	struct AEVec4;
	struct Color;
	struct Transform;
}
nlohmann::json& operator << (nlohmann::json& j, const AEX::AEVec2& vec);
AEX::AEVec2& operator >> (const nlohmann::json& j, AEX::AEVec2& vec);

nlohmann::json& operator << (nlohmann::json& j, const AEX::AEVec3& vec);
AEX::AEVec3& operator >> (const nlohmann::json& j, AEX::AEVec3& vec);

nlohmann::json& operator << (nlohmann::json& j, const AEX::AEVec4& vec);
AEX::AEVec4& operator >> (const nlohmann::json& j, AEX::AEVec4& vec);

nlohmann::json& operator << (nlohmann::json& j, const AEX::Color& vec);
AEX::Color& operator >> (const nlohmann::json& j, AEX::Color& vec);

nlohmann::json& operator << (nlohmann::json& j, const AEX::Transform& vec);
AEX::Transform& operator >> (const nlohmann::json& j, AEX::Transform& vec);

void SaveJsonToFile(const nlohmann::json& j, const char* filename);

void ReadJsonFromFile(nlohmann::json& j, const char* filename);
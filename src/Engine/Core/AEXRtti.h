#pragma once
#include <typeinfo> // typeid
namespace AEX
{
    //! \brief  Rtti is a class that provides Run-Time Type information for the 
    // classes of AEX Engine. Make sure to use the maccros below
    class Rtti
    {
        // per-instance data
    private:
        std::string mTypeName;
        Rtti* mParentRtti = nullptr;
        std::map<std::string, Rtti*> mChildren;
    public:
        const std::string& GetName() const;
        const std::string& GetParentTypeName()const;
        bool IsDerivedFrom(const Rtti& otherType)const;

        // Static interface
    private:
        static std::map<std::string, Rtti> sAllTypes; // main storage for Rtti
        static std::vector<Rtti*> sRootTypes;
    public:
        static const std::map<std::string, Rtti>& GetAllTypes();
        static const Rtti& AddRtti(const char* thisTypeName, const char* parentTypeName = nullptr);
        static void PrintAllRtti();
        template<typename T> static std::string ParseTypeName(bool removeNamespaces = true)
        {
            std::string tName = typeid(T).name();
            std::size_t pos = tName.find_last_of(' ');
            tName = tName.substr(pos + 1);
            if (removeNamespaces) {
                pos = tName.find_last_of(':');
                if (pos != std::string::npos) {
                    tName = tName.substr(pos + 1);
                }
            }
            return tName;
        }
    };
}

// Maccros used to declare RTTI types within class declarations. 
#define AEX_RTTI_DECL_BASE(classType)\
public:\
virtual const AEX::Rtti& GetType() {\
    return AEX::Rtti::AddRtti(AEX::Rtti::ParseTypeName<classType>().c_str());\
}\
static const AEX::Rtti& TYPE() {\
    return AEX::Rtti::AddRtti(AEX::Rtti::ParseTypeName<classType>().c_str());\
}

#define AEX_RTTI_DECL(classType, parentType)\
public:\
virtual const AEX::Rtti& GetType() {\
return AEX::Rtti::AddRtti(AEX::Rtti::ParseTypeName<classType>().c_str(), AEX::Rtti::ParseTypeName<parentType>().c_str());\
}\
static const AEX::Rtti& TYPE() {\
    return AEX::Rtti::AddRtti(AEX::Rtti::ParseTypeName<classType>().c_str(), AEX::Rtti::ParseTypeName<parentType>().c_str());\
}

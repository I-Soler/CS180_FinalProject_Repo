#pragma once
namespace AEX {

    // forward declarations of necessary files
    class Rtti;
    class IBase;

    class Singletons 
    {
    private:
        Singletons();
        static std::map<const Rtti*, IBase*> sSingletons;
    public:

        template<typename T>    static T* Get();
        template<typename T>    static void Delete();
        static void ReleaseAll();
    };
}

/*  notice that the maccro doesn't end with a semi-colon. This is so that
    if you don't wan to implement the constructor, you can end the maccro differently:
    - AEX_SINGLETON(MyClass){} // default construtor is private and does nothing. 
    - AEX_SINGLETON(MyClass);  // default construtor is private but not defined. 
*/ 
#define AEX_SINGLETON(classType) private: friend class Singletons; classType() 

// template functions definition
#include "AEXSingleton.inl"


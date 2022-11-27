#include "AEXCore.h"

namespace AEX
{
    std::map<const Rtti*, IBase*> Singletons::sSingletons;
	void Singletons::ReleaseAll()
    {
        for (auto entry : sSingletons)
        {
            delete entry.second;
        }
        sSingletons.clear();
    }
}
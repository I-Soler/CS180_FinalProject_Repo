#include "AEXCore.h"
#include <iostream>
namespace AEX
{
    // STATIC INTERFACE
    std::map<std::string, Rtti> Rtti::sAllTypes; // main storage for Rtti
    std::vector<Rtti*> Rtti::sRootTypes;

    const std::map<std::string, Rtti>& Rtti::GetAllTypes()
    {
        return sAllTypes;
    }

    const Rtti& Rtti::AddRtti(const char* thisTypeName, const char* parentTypeName)
    {
        bool typeExists = sAllTypes.find(thisTypeName) != sAllTypes.end();

        Rtti& newRtti = sAllTypes[thisTypeName];
        newRtti.mTypeName = thisTypeName;
        // base class
        if (parentTypeName == nullptr)
        {
            if (!typeExists)
                sRootTypes.push_back(&newRtti);
        }
        // a derived class
        else
        {
            // find the parent
            auto it = sAllTypes.find(parentTypeName);

            // parent not yet created
            if (it == sAllTypes.end())
            {
                // add the parent to the list
                Rtti& parentRtti = sAllTypes[parentTypeName];
                parentRtti.mTypeName = parentTypeName;

                // update the iterator
                it = sAllTypes.find(parentTypeName);
            }

            // add to the parent the new rtti
            newRtti.mParentRtti = &it->second;
            it->second.mChildren[thisTypeName] = &newRtti;
        }

        return newRtti;
    }
    void Rtti::PrintAllRtti()
    {
        std::vector<std::pair<Rtti*, std::string>> stack;

        // add the root types to the stack
        for (auto rtti : sRootTypes)
            stack.push_back({ rtti, "" });

        // traverse all the Rtti tree
        // recursion in iterative form
        while (stack.size())
        {
            Rtti* rtti = stack.back().first;
            std::string indent = stack.back().second;
            stack.pop_back();

            std::cout << indent << rtti->mTypeName << "\n";
            indent += "->";
            for (auto c : rtti->mChildren)
                stack.push_back({ c.second, indent });
        }
    }

    // INSTANCE INTERFACE 
    const std::string& Rtti::GetName() const
    {
        return mTypeName;
    }
    const std::string& Rtti::GetParentTypeName()const
    {
        static std::string nullStr = "null";
        if (mParentRtti)return mParentRtti->GetName(); return nullStr;
    }
    bool Rtti::IsDerivedFrom(const Rtti& otherType)const
    {
        Rtti* parent = mParentRtti;
        while (parent) {
            if (parent == &otherType)
                return true;
            // go to next parent
            parent = parent->mParentRtti;
        }
        return false;
    }
}
#include <pch.h>

// Patches
#include <CompanionTakeAll.h>

namespace Main
{
    // Config Options
    static REX::INI::Bool iCompanionTakeAllPatch{ "Patches"sv, "EnableCompanionTakeAllPatch"sv, true };

    F4SE_PLUGIN_PRELOAD(const F4SE::LoadInterface* a_f4se)
    {
        // Init
        F4SE::Init(a_f4se);
        REX::INFO("Companion Take All NG Initializing...");

        // Get the Trampoline and Allocate
		auto& trampoline = REL::GetTrampoline();
		trampoline.create(256);

        // Load the Config
        const auto config = REX::INI::SettingStore::GetSingleton();
        config->Init("Data/F4SE/Plugins/CompanionTakeAllNG.ini", "Data/F4SE/Plugins/CompanionTakeAllNGCustom.ini");
        config->Load();

        // Install Companion Take All
        if (iCompanionTakeAllPatch.GetValue() == true)
        {
            if (CompanionTakeAll::Install())
            {
                REX::INFO("Companion Take All NG Initialized!");
            }
        }

        // Finished
        return true;
    }
}

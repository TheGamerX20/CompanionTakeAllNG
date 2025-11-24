#include "Messaging.h"

#include "Events.h"
#include "CompanionTakeAll.h"

namespace CompanionTakeAll::Messaging
{
    void Register()
    {
        // Listen for Messages
        auto MessagingInterface = F4SE::GetMessagingInterface();
        MessagingInterface->RegisterListener(F4SEMessageListener);
        REX::INFO("Started Listening for F4SE Message Callbacks.");
    }

    void F4SEMessageListener(F4SE::MessagingInterface::Message* a_msg)
    {
        switch (a_msg->type)
        {
        case F4SE::MessagingInterface::kGameDataReady:
        {
            REX::INFO("Received kGameDataReady");

            // Set our Pointers
            CompanionTakeAll::InventoryInterfacePointer = RE::BGSInventoryInterface::GetSingleton();
            CompanionTakeAll::PlayerCharacterPointer = RE::PlayerCharacter::GetSingleton();
            CompanionTakeAll::DataHandlerPointer = RE::TESDataHandler::GetSingleton();
            CompanionTakeAll::UIPointer = RE::UI::GetSingleton();

            // Lookup Keywords
            CompanionTakeAll::LockpickCommandKeyword = (RE::BGSKeyword*)CompanionTakeAll::DataHandlerPointer->LookupForm(0x0F4B91, "Fallout4.esm");
            CompanionTakeAll::UsesStimpakKeyword = (RE::BGSKeyword*)CompanionTakeAll::DataHandlerPointer->LookupForm(0x0AD52A, "Fallout4.esm");
            CompanionTakeAll::UsesRepairKitKeyword = (RE::BGSKeyword*)CompanionTakeAll::DataHandlerPointer->LookupForm(0x004F13, "DLCRobot.esm");

            // Lookup Forms
            CompanionTakeAll::BobbyPinItem = CompanionTakeAll::DataHandlerPointer->LookupForm(0x00000A, "Fallout4.esm");
            CompanionTakeAll::StimpakItem = CompanionTakeAll::DataHandlerPointer->LookupForm(0x023736, "Fallout4.esm");
            CompanionTakeAll::RepairKitItem = CompanionTakeAll::DataHandlerPointer->LookupForm(0x004F12, "DLCRobot.esm");

            // Listen for a Menu Open / Close Event
            Events::MenuOpenCloseEventSink* MenuEventSink = new Events::MenuOpenCloseEventSink();
            CompanionTakeAll::UIPointer->GetEventSource<RE::MenuOpenCloseEvent>()->RegisterSink(MenuEventSink);

            // Install Hook
            CompanionTakeAll::InstallHook();

            break;
        }
        default:
            break;
        }
    }
}

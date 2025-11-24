#include "Events.h"

#include "CompanionTakeAll.h"

namespace CompanionTakeAll::Events
{
    MenuOpenCloseEventSink MenuEventSink;

    RE::BSEventNotifyControl MenuOpenCloseEventSink::ProcessEvent(const RE::MenuOpenCloseEvent& a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_source)
    {
		if (a_event.menuName == "ContainerMenu")
		{
			if (a_event.opening == true)
			{
				CompanionTakeAll::CompanionActorPointer = nullptr;
				CompanionTakeAll::UsedAmmoPointer = nullptr;
				CompanionTakeAll::ExcludedItemsSet.clear();

				Scaleform::Ptr<RE::ContainerMenu> ContainerMenuPointer = CompanionTakeAll::UIPointer->GetMenu<RE::ContainerMenu>();
				if (ContainerMenuPointer)
				{
					RE::TESObjectREFR* ContainerReferencePointer = (ContainerMenuPointer->containerRef).get().get();
					if (ContainerReferencePointer && ContainerReferencePointer->formType == RE::ENUM_FORM_ID::kACHR)
					{
						RE::Actor* ActorReference = (RE::Actor*)ContainerReferencePointer;

						if (ActorReference && CompanionTakeAll::IsFollowing(ActorReference, CompanionTakeAll::PlayerCharacterPointer) && !ActorReference->IsDead(true))
						{
							CompanionTakeAll::CompanionActorPointer = ActorReference;

							// Get Keywords (for some reason, All Actors have the UsesRepairKitKeyword?)
							bool KeepBobbyPins = ActorReference->HasKeyword(CompanionTakeAll::LockpickCommandKeyword);
							bool UsesStimpak = ActorReference->HasKeyword(CompanionTakeAll::UsesStimpakKeyword);
							bool UsesRepairKit = ActorReference->HasKeyword(CompanionTakeAll::UsesRepairKitKeyword);

							// Populate Excluded Item Set
							if (KeepBobbyPins)
							{
								CompanionTakeAll::ExcludedItemsSet.insert(CompanionTakeAll::BobbyPinItem);
							}

							if (UsesStimpak)
							{
								CompanionTakeAll::ExcludedItemsSet.insert(CompanionTakeAll::StimpakItem);
							}
							else if (UsesRepairKit)
							{
								CompanionTakeAll::ExcludedItemsSet.insert(CompanionTakeAll::RepairKitItem);
							}
						}
						else
						{
							CompanionTakeAll::CompanionActorPointer = nullptr;
						}
					}
				}
			}
		}

		return RE::BSEventNotifyControl::kContinue;
    }
}

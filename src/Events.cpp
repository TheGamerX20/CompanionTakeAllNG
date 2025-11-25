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

						if (ActorReference && !ActorReference->IsDead(true) && (CompanionTakeAll::iTarget.GetValue() == CompanionTakeAll::TargetOptions::AllNPCs ||
							(CompanionTakeAll::iTarget.GetValue() == CompanionTakeAll::TargetOptions::CurrentCompanion && CompanionTakeAll::IsFollowing(ActorReference, CompanionTakeAll::PlayerCharacterPointer))))
						{
							CompanionTakeAll::CompanionActorPointer = ActorReference;

							// Keep Grenades
							if (CompanionTakeAll::iKeepGrenades.GetValue())
							{
								CompanionTakeAll::ExcludedItemsSet = CompanionTakeAll::GrenadeSet;
							}

							// Keep Bobby Pins
							if (CompanionTakeAll::iKeepBobbyPins.GetValue())
							{
								bool KeepBobbyPins = ActorReference->HasKeyword(CompanionTakeAll::LockpickCommandKeyword);

								if (KeepBobbyPins)
								{
									CompanionTakeAll::ExcludedItemsSet.insert(CompanionTakeAll::BobbyPinItem);
								}
							}

							// Keep Healing Items
							if (CompanionTakeAll::iKeepHealingItems.GetValue())
							{
								// For some reason, all Actors have the UsesRepairKitKeyword?
								bool UsesStimpak = ActorReference->HasKeyword(CompanionTakeAll::UsesStimpakKeyword);
								bool UsesRepairKit = ActorReference->HasKeyword(CompanionTakeAll::UsesRepairKitKeyword);

								if (UsesStimpak)
								{
									CompanionTakeAll::ExcludedItemsSet.insert(CompanionTakeAll::StimpakItem);
								}
								else if (UsesRepairKit)
								{
									CompanionTakeAll::ExcludedItemsSet.insert(CompanionTakeAll::RepairKitItem);
								}
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

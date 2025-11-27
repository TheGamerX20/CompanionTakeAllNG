#pragma once

#include "Messaging.h"

namespace CompanionTakeAll
{
	enum TargetOptions
	{
		CurrentCompanion = 0,
		AllNPCs = 1
	};

	// Config Options
	static REX::INI::I32 iTarget{ "Options"sv, "Target"sv, (int)TargetOptions::CurrentCompanion };
	static REX::INI::Bool iKeepGrenades{ "Options"sv, "KeepGrenades"sv, true };
	static REX::INI::Bool iKeepBobbyPins{ "Options"sv, "KeepBobbyPins"sv, true };
	static REX::INI::Bool iKeepHealingItems{ "Options"sv, "KeepHealingItems"sv, true };

	// Transfer Item Relocations
	inline REL::Relocation<uintptr_t> TransferItemCallRelocation(REL::ID(2248619), REL::Offset(0x22B));
	inline REL::ID TransferItemFunctionID(2248607);

	// IsFollowing(Actor) Relocation
	inline REL::ID IsFollowingActorFunctionID(2230141);

	// Keyword Forms
	inline RE::BGSKeyword* LockpickCommandKeyword = nullptr;
	inline RE::BGSKeyword* UsesStimpakKeyword = nullptr;
	inline RE::BGSKeyword* UsesRepairKitKeyword = nullptr;

	// Item Forms
	inline RE::TESForm* BobbyPinItem = nullptr;
	inline RE::TESForm* StimpakItem = nullptr;
	inline RE::TESForm* RepairKitItem = nullptr;

	// Slots
	inline RE::BGSEquipSlot* GrenadeSlot = nullptr;

	// Item Sets
	inline std::unordered_set<RE::TESForm*> GrenadeSet;
	inline std::unordered_set<RE::TESForm*> ExcludedItemsSet;

	// Pointers
	inline RE::BGSInventoryInterface* InventoryInterfacePointer = nullptr;
	inline RE::PlayerCharacter* PlayerCharacterPointer = nullptr;
	inline RE::TESDataHandler* DataHandlerPointer = nullptr;
	inline RE::Actor* CompanionActorPointer = nullptr;
	inline RE::TESAmmo* UsedAmmoPointer = nullptr;
	inline RE::UI* UIPointer = nullptr;
	inline int CurrentItemIndex = 0;

	// -------- Game Functions -------- //

	inline bool IsFollowing(RE::Actor* a_followerActor, RE::Actor* a_followingActor)
	{
		using func_t = decltype(&IsFollowing);
		static REL::Relocation<func_t> func{ IsFollowingActorFunctionID };
		return func(a_followerActor, a_followingActor);
	}

	inline void TransferItem(RE::ContainerMenu* a_container, const RE::InventoryUserUIInterfaceEntry& a_entry, uint32_t a_count, bool a_fromContainer)
	{
		using func_t = decltype(&TransferItem);
		static REL::Relocation<func_t> func{ TransferItemFunctionID };
		return func(a_container, a_entry, a_count, a_fromContainer);
	}

	// -------- Hooked Call -------- //

	inline void TransferItemHook(RE::ContainerMenu* a_container, const RE::InventoryUserUIInterfaceEntry& a_entry, uint32_t a_count, bool a_fromContainer)
	{
		// Increase Item Index Prematurely to avoid the Returns
		CurrentItemIndex++;

		if (CompanionActorPointer && a_container)
		{
			// Find Ammo Used
			RE::AIProcess* CompanionProcess = CompanionActorPointer->currentProcess;
			if (CompanionProcess && CompanionProcess->middleHigh)
			{
				RE::BSTArray<RE::EquippedItem>& CompanionEquippedItems = CompanionProcess->middleHigh->equippedItems;

				if (!CompanionEquippedItems.empty() && CompanionEquippedItems[0].item.instanceData)
				{
					RE::TESObjectWEAP::InstanceData* InstanceData = (RE::TESObjectWEAP::InstanceData*)CompanionEquippedItems[0].item.instanceData.get();
					if (InstanceData)
					{
						UsedAmmoPointer = InstanceData->ammo;
					}
				}
			}

			// Get the Item to Transfer
			const RE::BGSInventoryItem* ItemPointer = InventoryInterfacePointer->RequestInventoryItem(a_entry.invHandle.id);

			// Is it Valid
			if (ItemPointer)
			{
				RE::TESBoundObject* ItemObject = ItemPointer->object;
				if (ItemObject)
				{
					// Skip if it is the Used Ammo Type
					if (ItemObject->formType == RE::ENUM_FORM_ID::kAMMO && ItemObject == UsedAmmoPointer)
					{
						// Do not Transfer
						return;
					}
					// Skip Excluded Items (Healing, Bobby Pins, etc..)
					else if (ExcludedItemsSet.contains(ItemObject))
					{
						// Do not Transfer
						return;
					}
				}

				// Get the Stack & Stack Count
				RE::BGSInventoryItem::Stack* StackPointer = ItemPointer->stackData.get();
				int64_t StackCount = const_cast<RE::BGSInventoryItem*>(ItemPointer)->GetStackCount();

				// For Equipped Items (Singular vs Multiple Items)
				if (StackPointer && ItemPointer->stackData && StackPointer->IsEquipped())
				{
					if (StackCount == 1)
					{
						// Do not Transfer
						return;
					}
					else if (StackCount > 1)
					{
						// Un-Equip the Item
						a_container->ToggleItemEquipped(CurrentItemIndex - 1, true);

						// Transfer the Extra Items
						TransferItem(a_container, a_entry, ItemPointer->GetCount() - 1, a_fromContainer);

						// Re-Equip the Item if needed
						if (!StackPointer->IsEquipped())
						{
							a_container->ToggleItemEquipped(CurrentItemIndex - 1, true);
						}

						// Already Transferred
						return;
					}
				}
			}
		}

		// Transfer Unequipped Items
		return TransferItem(a_container, a_entry, a_count, a_fromContainer);
	}

	// -------- Installation -------- //

	inline void InstallHook()
	{
		// Get the Trampoline
		auto& trampoline = REL::GetTrampoline();

		// Write a Call for Transfering Items
		trampoline.write_call<5>(TransferItemCallRelocation.address(), TransferItemHook);

		REX::INFO("Installed Hook.");
	}

	inline bool Install()
	{
		// Register for the Messaging Interface
		Messaging::Register();

		return true;
	}
}

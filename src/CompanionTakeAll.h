#pragma once

#include "Messaging.h"

namespace CompanionTakeAll
{
	// Transfer Item Relocations
	inline REL::Relocation<uintptr_t> TransferItemCallRelocation(REL::ID(2248619), REL::Offset(0x22B));
	inline REL::ID TransferItemFunctionID(2248607);

	// IsFollowing(Actor) Relocation
	inline REL::ID IsFollowingActorFunctionID(2230141);

	// Pointers
	inline RE::BGSInventoryInterface* InventoryInterfacePointer = nullptr;
	inline RE::PlayerCharacter* PlayerCharacterPointer = nullptr;
	inline RE::TESDataHandler* DataHandlerPointer = nullptr;
	inline RE::Actor* CompanionActorPointer = nullptr;
	inline RE::TESAmmo* UsedAmmoPointer = nullptr;
	inline RE::UI* UIPointer = nullptr;

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

				// Skip if it is the Used Ammo Type
				if (ItemObject && ItemObject->formType == RE::ENUM_FORM_ID::kAMMO && ItemObject == UsedAmmoPointer)
				{
					return;
				}

				// Get the Stack & Stack Count
				RE::BGSInventoryItem::Stack* StackPointer = ItemPointer->stackData.get();
				int64_t StackCount = const_cast<RE::BGSInventoryItem*>(ItemPointer)->GetStackCount();

				// For Equipped Items (Singular vs Multiple Items)
				if (StackPointer && ItemPointer->stackData && StackPointer->IsEquipped())
				{
					if (StackCount == 1)
					{
						return;
					}
					else if (StackCount > 1)
					{
						// Remember & Remove Flags (to remove the Equip Lock)
						REX::EnumSet<RE::BGSInventoryItem::Stack::Flag, std::uint16_t> flags = StackPointer->flags;
						StackPointer->flags.reset();

						// Transfer the Extra Items
						TransferItem(a_container, a_entry, ItemPointer->GetCount() - 1, a_fromContainer);

						// Readd the Flags
						StackPointer->flags = flags;

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
		Messaging::Register();
		return true;
	}
}

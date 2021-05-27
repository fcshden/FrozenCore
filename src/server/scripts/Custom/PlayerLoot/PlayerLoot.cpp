#pragma execution_character_set("utf-8")
#include "PlayerLoot.h"

std::unordered_map<uint32/*itemid*/, PlayerLootTemplate> PlayerLootMap;

void PlayerLoot::Load()
{
	PlayerLootMap.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,掉落数量,掉落几率,是否禁止银行掉落,是否禁止背包掉落 FROM __玩家掉落" :
		"SELECT entry,lootCount,chance,banBank,banBag FROM _pvp_killed_loot");

	if (!result) 
		return;
	do
	{
		Field* fields		= result->Fetch();
		uint32 entry		= fields[0].GetUInt32();
		PlayerLootTemplate Temp;
		Temp.lootCount		= fields[1].GetUInt32();
		Temp.chance			= fields[2].GetFloat();
		Temp.banBank		= fields[3].GetBool();
		Temp.banBag			= fields[4].GetBool();
		PlayerLootMap.insert(std::make_pair(entry, Temp));
	} while (result->NextRow());
}

bool PlayerLoot::GetLootInfo(uint32 entry, uint32 &lootCount, bool &banBag, bool &banBank)
{
	auto iter = PlayerLootMap.find(entry);

	if (iter == PlayerLootMap.end())
		return false;

	if (!roll_chance_f(iter->second.chance))
		return false;

	lootCount = iter->second.lootCount;
	banBag = iter->second.banBag;
	banBank = iter->second.banBank;	
	return true;
}

uint32 PlayerLoot::GetItemCount(Player* player, uint32 entry, bool banBag, bool banBank)
{
	uint32 count = 0;

	for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			if (pItem->GetEntry() == entry)
				count += pItem->GetCount();

	if (!banBag)
	{
		for (uint8 i = KEYRING_SLOT_START; i < CURRENCYTOKEN_SLOT_END; ++i)
			if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
				if (pItem->GetEntry() == entry)
					count += pItem->GetCount();

		for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
			if (Bag* pBag = player->GetBagByPos(i))
				count += pBag->GetItemCount(entry);
	}
	
	if (!banBank)
	{
		for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
			if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
				if (pItem->GetEntry() == entry)
					count += pItem->GetCount();

		for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
			if (Bag* pBag = player->GetBagByPos(i))
				count += pBag->GetItemCount(entry);
	}

	return count;
}

void PlayerLoot::LootOnKilled(Player* killer, Player* victim)
{
	std::unordered_map<uint32, uint32> LootExtraItems;

	for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
		if (Item* item = victim->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			uint32 lootCount = 0; bool banBag = false; bool banBank = false;
			if (!GetLootInfo(entry, lootCount, banBag, banBank))
				continue;

			if (LootExtraItems.find(entry) == LootExtraItems.end())
				LootExtraItems.insert(std::make_pair(entry, 0));
		}

	for (uint8 i = KEYRING_SLOT_START; i < CURRENCYTOKEN_SLOT_END; ++i)
		if (Item* item = victim->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			uint32 lootCount = 0; bool banBag = false; bool banBank = false;
			if (!GetLootInfo(entry, lootCount, banBag, banBank) || banBag)
				continue;

			if (LootExtraItems.find(entry) == LootExtraItems.end())
				LootExtraItems.insert(std::make_pair(entry, 0));
		}

	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* item = victim->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			uint32 lootCount = 0; bool banBag = false; bool banBank = false;
			if (!GetLootInfo(entry, lootCount, banBag, banBank) || banBag)
				continue;

			if (LootExtraItems.find(entry) == LootExtraItems.end())
				LootExtraItems.insert(std::make_pair(entry, 0));
		}

	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = victim->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = victim->GetItemByPos(i, j))
				{
					uint32 entry = item->GetEntry();
					uint32 lootCount = 0; bool banBag = false; bool banBank = false;
					if (!GetLootInfo(entry, lootCount, banBag, banBank) || banBag)
						continue;

					if (LootExtraItems.find(entry) == LootExtraItems.end())
						LootExtraItems.insert(std::make_pair(entry, 0));
				}

	for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
		if (Item* item = victim->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			uint32 lootCount = 0; bool banBag = false; bool banBank = false;
			if (!GetLootInfo(entry, lootCount, banBag, banBank) || banBank)
				continue;

			if (LootExtraItems.find(entry) == LootExtraItems.end())
				LootExtraItems.insert(std::make_pair(entry, 0));
		}

	for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
		if (Bag* pBag = victim->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = victim->GetItemByPos(i, j))
				{
					uint32 entry = item->GetEntry();
					uint32 lootCount = 0; bool banBag = false; bool banBank = false;
					if (!GetLootInfo(entry, lootCount, banBag, banBank) || banBank)
						continue;

					if (LootExtraItems.find(entry) == LootExtraItems.end())
						LootExtraItems.insert(std::make_pair(entry, 0));
				}

	if (LootExtraItems.empty())
		return;

	if (GameObject* chest = killer->GetMap()->SummonGameObject(PLAYER_LOOT_GAMEOBJECT, victim->GetPositionX(), victim->GetPositionY(), victim->GetPositionZ(),
		0, 0, 0, 0, 0, PLAYER_LOOT_GAMEOBJECT_DURATION_TIME))
	{
		
		for (auto iter = LootExtraItems.begin(); iter != LootExtraItems.end(); iter++)
		{
			auto itr = PlayerLootMap.find(iter->first);
			if (itr != PlayerLootMap.end())
			{
				uint32 count = GetItemCount(victim, iter->first, itr->second.banBag, itr->second.banBank);
				iter->second = count > itr->second.lootCount ? itr->second.lootCount : count;
				victim->DestroyItemCount(iter->first, iter->second, true);
			}
		}

		chest->LootExtraItems = LootExtraItems;
	}
}

class PlayerLootScript : PlayerScript
{
public:
	PlayerLootScript() : PlayerScript("PlayerLootScript") {}

	void OnPVPKill(Player* killer, Player* killed)
	{
		sPlayerLoot->LootOnKilled(killer, killed);
	}
};

void AddSC_PlayerLoot()
{
	new PlayerLootScript();
}

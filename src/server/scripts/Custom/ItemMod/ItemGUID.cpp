#pragma execution_character_set("utf-8")
#include "ItemGUID.h"
#include "../GCAddon/GCAddon.h" //¿âº¯Êý

std::unordered_map<uint32, ItemGUIDTemplate> ItemGUIDMap;   
std::unordered_map<uint32, ItemPointsTemplate> ItemPointsMap;

void ItemGUID::Insert(uint32 guid)
{
	auto iter = ItemGUIDMap.find(guid);

	if (iter == ItemGUIDMap.end())
	{
		ItemGUIDTemplate Temp;
		Temp.Points = 0;
		for (size_t i = 0; i < MAX_ITEM_ENCHANTS; i++)
			Temp.Enchants[i] = 0;

		ItemGUIDMap.insert(std::make_pair(guid, Temp));
	}
}

void ItemGUID::Delete(uint32 guid)
{
	auto iter = ItemGUIDMap.find(guid);

	if (iter != ItemGUIDMap.end())
		ItemGUIDMap.erase(iter);
}

void ItemGUID::AddStatPoints(uint32 guid, uint32 points)
{
	auto iter = ItemGUIDMap.find(guid);

	if (iter != ItemGUIDMap.end())
		iter->second.Points += points;

	SendUpdateDataToAll(guid);
}

void ItemGUID::UpdateEnchants(uint32 guid, uint8 slot, uint32 enchantid)
{
	if (slot < PROP_ENCHANTMENT_SLOT_0 || slot > PROP_ENCHANTMENT_SLOT_4)
		return;

	slot = slot - PROP_ENCHANTMENT_SLOT_0;

	auto iter = ItemGUIDMap.find(guid);

	if (iter != ItemGUIDMap.end())
		iter->second.Enchants[slot] = enchantid;

	SendUpdateDataToAll(guid);	
}

void ItemGUID::SendAllData(Player* player)
{
	for (auto iter = ItemGUIDMap.begin(); iter != ItemGUIDMap.end(); iter++)
	{
		std::ostringstream oss;
		oss << iter->first << " ";
		oss << iter->second.Points << " ";
		for (size_t i = 0; i < MAX_ITEM_ENCHANTS; i++)
			oss << iter->second.Enchants[i] << " ";
		sGCAddon->SendPacketTo(player, "GC_S_ITEMGUID", oss.str());
	}
}

void ItemGUID::SendUpdateDataToAll(uint32 guid)
{
	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator itr = smap.begin(); itr != smap.end(); ++itr)
		if (Player* pl = itr->second->GetPlayer())
		{
			auto iter = ItemGUIDMap.find(guid);
			if (iter != ItemGUIDMap.end())
			{
				std::ostringstream oss;
				oss << guid << " ";
				oss << iter->second.Points << " ";
				for (size_t i = 0; i < MAX_ITEM_ENCHANTS; i++)
					oss << iter->second.Enchants[i] << " ";
				sGCAddon->SendPacketTo(pl, "GC_S_ITEMGUID", oss.str());
			}		
		}
}

class ItemGUIDPlayerScript : PlayerScript
{
public:
	ItemGUIDPlayerScript() : PlayerScript("ItemGUIDPlayerScript") {}

	void OnLogin(Player* player)
	{
		
	}

	void OnLogout(Player* player) 
	{ 
		
	}
};
void AddSC_ItemGUID()
{
	new ItemGUIDPlayerScript();
}

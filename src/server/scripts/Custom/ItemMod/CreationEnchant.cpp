#include "ItemMod.h"

uint32 ItemMod::GenerateEnchantId(uint32 entry, uint8 slot)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return 0;

	int32 groupId = -1;
	float chance = 0;

	uint32 len = CreateEnchantVec.size();
	
	for (size_t i = 0; i < len; i++)
		if (entry == CreateEnchantVec[i].entry && slot == CreateEnchantVec[i].slot)
		{
			groupId = CreateEnchantVec[i].groupId;
			chance = CreateEnchantVec[i].chance;
			break;
		}
		

	if (groupId < 0 || !roll_chance_f(chance))
		return 0;

	uint32 len_group = EnchantGroupVec.size();

	std::unordered_map<uint32, float> EnchantIdMap;

	for (size_t i = 0; i < len_group; i++)
		if (groupId == EnchantGroupVec[i].groupId)
			EnchantIdMap.insert(std::make_pair(EnchantGroupVec[i].enchantId, EnchantGroupVec[i].chance));

	std::vector<std::pair<int, float>> vtMap;
	for (auto it = EnchantIdMap.begin(); it != EnchantIdMap.end(); it++)
		vtMap.push_back(std::make_pair(it->first, it->second));

	sort(vtMap.begin(), vtMap.end(),
		[](const std::pair<int, float> &x, const std::pair<int, float> &y) -> int {
		return x.second < y.second;
	});


	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->second;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->second;
		if (rand <= sum)
			return it->first;
	}

	return 0;
}


uint32 ItemMod::GetIdentifyGroupId(uint32 entry, EnchantmentSlot slot)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return 0;

	uint32 groupId = 0;

	uint32 len = IdentifyVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (IdentifyVec[i].entry == entry && IdentifyVec[i].slot == slot)
		{
			groupId = IdentifyVec[i].groupId;
			break;
		}		
	}

	uint32 len_group = EnchantGroupVec.size();
	for (size_t i = 0; i < len_group; i++)
		if (groupId == EnchantGroupVec[i].groupId)
			return groupId;

	return 0;
}

uint32 ItemMod::GetIdentifyEnchantId(uint32 entry, EnchantmentSlot slot)
{
	int32 groupId = GetIdentifyGroupId(entry, slot);

	if (groupId <= 0)
		return 0;

	uint32 len_group = EnchantGroupVec.size();

	std::unordered_map<uint32, float> EnchantIdMap;

	for (size_t i = 0; i < len_group; i++)
		if (groupId == EnchantGroupVec[i].groupId)
			EnchantIdMap.insert(std::make_pair(EnchantGroupVec[i].enchantId, EnchantGroupVec[i].chance));

	std::vector<std::pair<int, float>> vtMap;
	for (auto it = EnchantIdMap.begin(); it != EnchantIdMap.end(); it++)
		vtMap.push_back(std::make_pair(it->first, it->second));

	sort(vtMap.begin(), vtMap.end(),
		[](const std::pair<int, float> &x, const std::pair<int, float> &y) -> int {
		return x.second < y.second;
	});


	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->second;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->second;
		if (rand <= sum)
			return it->first;
	}

	return 0;
}

void ItemMod::GetIdentifyInfo(uint32 entry, EnchantmentSlot slot, uint32 &reqId, std::string &gossipText, bool &slotHasEnchant)
{
	const ItemTemplate * temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return;

	uint32 len = IdentifyVec.size();
	for (size_t i = 0; i < len; i++)
		if (IdentifyVec[i].entry == entry && IdentifyVec[i].slot == slot)
		{
			reqId = IdentifyVec[i].reqId;
			gossipText = IdentifyVec[i].gossipText;
			slotHasEnchant = IdentifyVec[i].slotHasEnchant;
		}
}
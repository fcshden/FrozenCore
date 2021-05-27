#include "ItemSet.h"

std::vector<ItemSetTemplate> ItemSetVec;
std::vector<ItemSetItemsTemplate> ItemSetItemsVec;


bool ItemSetSort(ItemSetTemplate a, ItemSetTemplate b) { return (a.counts < b.counts); }

void ItemSet::Load()
{
	return;
	ItemSetVec.clear();
	QueryResult result1 = WorldDatabase.PQuery("SELECT ID,counts,spell1,spell2,spell3,description from _itemmod_set");
	if (result1)
	{
		do
		{
			Field* fields = result1->Fetch();
			ItemSetTemplate Temp;
			Temp.ID				= fields[0].GetUInt32();
			Temp.counts			= fields[1].GetUInt32();
			Temp.spell1			= fields[2].GetUInt32();
			Temp.spell2			= fields[3].GetUInt32();
			Temp.spell3			= fields[4].GetUInt32();
			Temp.description	= fields[5].GetString();
			ItemSetVec.push_back(Temp);
		} while (result1->NextRow());
	}

	ItemSetItemsVec.clear();
	QueryResult result2 = WorldDatabase.PQuery("SELECT ID,entry from _itemmod_set_items");
	if (result2)
	{
		do
		{
			Field* fields = result2->Fetch();
			ItemSetItemsTemplate Temp;
			Temp.ID		= fields[0].GetUInt32();
			Temp.entry	= fields[1].GetUInt32();
			ItemSetItemsVec.push_back(Temp);
		} while (result2->NextRow());
	}
}

std::string ItemSet::GetDes(uint32 entry)
{
	uint32 ID = 0;

	std::vector<ItemSetItemsTemplate>::iterator itr1;
	for (itr1 = ItemSetItemsVec.begin(); itr1 != ItemSetItemsVec.end(); itr1++)
		if (itr1->entry == entry)
		{
			ID = itr1->ID;
			break;
		}

	if (ID == 0)
		return "";
	
	std::ostringstream oss;

	std::sort(ItemSetVec.begin(), ItemSetVec.end(), ItemSetSort);
	std::vector<ItemSetTemplate>::iterator itr2;
	for (itr2 = ItemSetVec.begin(); itr2 != ItemSetVec.end(); itr2++)
		if (ID == itr2->ID)
		{
			oss << itr2->description << "\n";
		}

	return oss.str();
}

uint32 ItemSet::GetID(uint32 entry)
{
	uint32 ID = 0;

	std::vector<ItemSetItemsTemplate>::iterator itr;
	for (itr = ItemSetItemsVec.begin(); itr != ItemSetItemsVec.end(); itr++)
		if (itr->entry == entry)
		{
			ID = itr->ID;
			break;
		}

	return ID;
}

void ItemSet::AddAllSpell(Player* player)
{
	player->InvSetVec.clear();

	for (uint8 i = 0; i < INVENTORY_SLOT_BAG_END; ++i)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 ID = GetID(item->GetEntry());
			if (ID != 0)
				player->InvSetVec.push_back(ID);
		}

	//学习新的套装技能
	std::vector<uint32 /*spell*/> spellVec;

	for (std::vector<uint32>::iterator itr = player->InvSetVec.begin(); itr != player->InvSetVec.end(); itr++)
	{
		uint32 ID = *itr;
		uint32 counts = std::count(player->InvSetVec.begin(), player->InvSetVec.end(), ID);

		for (std::vector<ItemSetTemplate>::iterator i = ItemSetVec.begin(); i != ItemSetVec.end(); i++)
			if (ID == i->ID && counts >= i->counts)
			{
				if (SpellInfo const* spellEntry1 = sSpellMgr->GetSpellInfo(i->spell1))
					if (std::find(spellVec.begin(), spellVec.end(), i->spell1) == spellVec.end())
						spellVec.push_back(i->spell1);
				if (SpellInfo const* spellEntry2 = sSpellMgr->GetSpellInfo(i->spell2))
					if (std::find(spellVec.begin(), spellVec.end(), i->spell2) == spellVec.end())
						spellVec.push_back(i->spell2);
				if (SpellInfo const* spellEntry3 = sSpellMgr->GetSpellInfo(i->spell3))
					if (std::find(spellVec.begin(), spellVec.end(), i->spell3) == spellVec.end())
						spellVec.push_back(i->spell3);
			}
	}

	for (std::vector<uint32 /*spell*/>::iterator i = spellVec.begin(); i != spellVec.end(); i++)
		if (!player->HasSpell(*i))
			player->learnSpell(*i);
}

void ItemSet::UpdateSpell(Player* player, uint32 addEntry, uint32 remEntry)
{
	player->InvSetVec.clear();

	for (uint8 i = 0; i < INVENTORY_SLOT_BAG_END; ++i)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 ID = GetID(item->GetEntry());
			if (ID != 0)
				player->InvSetVec.push_back(ID);
		}

	uint32 remID = GetID(remEntry);
	uint32 addID = GetID(addEntry);

	if (remID == addID)
		return;

	//需要移除技能VEC

	std::vector<uint32 /*spell*/> remVec;

	for (std::vector<ItemSetTemplate>::iterator it = ItemSetVec.begin(); it != ItemSetVec.end(); it++)
	{
		if (remID == it->ID)
		{
			uint32 counts = std::count(player->InvSetVec.begin(), player->InvSetVec.end(), remID);
			if (counts >= it->counts)
				continue;

			if (SpellInfo const* spellEntry1 = sSpellMgr->GetSpellInfo(it->spell1))
				if (std::find(remVec.begin(), remVec.end(), it->spell1) == remVec.end())
					remVec.push_back(it->spell1);
			if (SpellInfo const* spellEntry2 = sSpellMgr->GetSpellInfo(it->spell2))
				if (std::find(remVec.begin(), remVec.end(), it->spell2) == remVec.end())
					remVec.push_back(it->spell2);
			if (SpellInfo const* spellEntry3 = sSpellMgr->GetSpellInfo(it->spell3))
				if (std::find(remVec.begin(), remVec.end(), it->spell3) == remVec.end())
					remVec.push_back(it->spell3);
		}		
	}

	//新套装技能VEC

	std::vector<uint32 /*spell*/> addVec;

	for (std::vector<uint32>::iterator itr = player->InvSetVec.begin(); itr != player->InvSetVec.end(); itr++)
	{
		uint32 ID = *itr;
		uint32 counts = std::count(player->InvSetVec.begin(), player->InvSetVec.end(), ID);

		for (std::vector<ItemSetTemplate>::iterator i = ItemSetVec.begin(); i != ItemSetVec.end(); i++)
			if (ID == i->ID && counts >= i->counts)
			{
				if (SpellInfo const* spellEntry1 = sSpellMgr->GetSpellInfo(i->spell1))
					if (std::find(addVec.begin(), addVec.end(), i->spell1) == addVec.end())
						addVec.push_back(i->spell1);
				if (SpellInfo const* spellEntry2 = sSpellMgr->GetSpellInfo(i->spell2))
					if (std::find(addVec.begin(), addVec.end(), i->spell2) == addVec.end())
						addVec.push_back(i->spell2);
				if (SpellInfo const* spellEntry3 = sSpellMgr->GetSpellInfo(i->spell3))
					if (std::find(addVec.begin(), addVec.end(), i->spell3) == addVec.end())
						addVec.push_back(i->spell3);
			}
	}

	//去重
	for (std::vector<uint32 /*spell*/>::iterator i = remVec.begin(); i != remVec.end();)
	{		
		if (std::find(addVec.begin(), addVec.end(), *i) != addVec.end())
			i = remVec.erase(i);
		else
			++i;
	}	

	//移除技能
	for (std::vector<uint32 /*spell*/>::iterator i = remVec.begin(); i != remVec.end(); i++)
		if (player->HasSpell(*i))
			player->removeSpell(*i, SPEC_MASK_ALL, false);

	//学习技能
	for (std::vector<uint32 /*spell*/>::iterator i = addVec.begin(); i != addVec.end(); i++)
		if (!player->HasSpell(*i))
			player->learnSpell(*i);
}


class ItemSetPlayerScript : PlayerScript
{
public:
	ItemSetPlayerScript() : PlayerScript("ItemSetPlayerScript") {}

	void OnLogin(Player* player)
	{
		sItemSet->AddAllSpell(player);
	}
};
void AddSC_ItemSet()
{
	new ItemSetPlayerScript();
}

#pragma execution_character_set("utf-8")
#include "EquipmentManager.h"
#include "Item.h"
#include "../Custom/ItemMod/NoPatchItem.h"
#include "..\..\server\scripts\Custom\CommonFunc\CommonFunc.h"
std::vector<EquipmentTemplate> EquipmentVec;

void Player::InitMapTempItems(uint32 Map)
{
    ApplyMapTempItems(false);

    for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
        if (_MapTempItems[i])
        {
            delete _MapTempItems[i];
            _MapTempItems[i] = NULL;
        }

    CanSawpOnMapTempItems = true;

    for (auto itr = EquipmentVec.begin(); itr != EquipmentVec.end(); itr++)
    {
        if (getClass() != itr->_class)
            continue;

        if (itr->MapOrArea < 0)
            continue;

        if (itr->MapOrArea != Map)
            continue;

        for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itr->items[i]))
            {
                _MapTempItems[i] = Item::CreateItem(itr->items[i], 1, this);
                ChatHandler(GetSession()).PSendSysMessage("加载地图装备%s..", sCF->GetItemLink(itr->items[i]).c_str());
            }

        CanSawpOnMapTempItems = false;
        break;
    }

    ApplyMapTempItems(true);
}

bool Player::HasMapTempItems()
{
    bool ok = false;
    for (auto itr = EquipmentVec.begin(); itr != EquipmentVec.end(); itr++)
    {
        if (GetMapId() == itr->MapOrArea)
        {
            ok = true;
            return ok;
            break;
        }
    }
    return ok;
}
void Player::InitAreaTempItems(uint32 Area)
{
    ApplyAreaTempItems(false);

    for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
        if (_AreaTempItems[i])
        {
            delete _AreaTempItems[i];
            _AreaTempItems[i] = NULL;
        }

    CanSawpOnAreaTempItems = true;

    for (auto itr = EquipmentVec.begin(); itr != EquipmentVec.end(); itr++)
    {
        if (getClass() != itr->_class)
            continue;

        if (itr->MapOrArea > 0)
            continue;

        if ((-itr->MapOrArea) != Area)
            continue;

        for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itr->items[i]))
            {
                _AreaTempItems[i] = Item::CreateItem(itr->items[i], 1, this);
                ChatHandler(GetSession()).PSendSysMessage("加载地域装备%s..", sCF->GetItemLink(itr->items[i]).c_str());
            }

        CanSawpOnAreaTempItems = false;
        break;
    }

    ApplyAreaTempItems(true);
}

void Player::ApplyMapTempItems(bool apply)
{
    if (apply)
    {
        for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
        {
            if (_MapTempItems[i])
            {
                SetVisibleItemSlot(i, _MapTempItems[i]);

                if (Item* dscItem = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    _ApplyItemMods(dscItem, i, false);

                _ApplyItemMods(_MapTempItems[i], i, true);
                ChatHandler(GetSession()).PSendSysMessage("加载区域装备%s..", sCF->GetItemLink(_MapTempItems[i]->GetEntry()).c_str());
            }
        }
    }
    else
    {
        for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
        {
            if (_MapTempItems[i])
            {
                SetVisibleItemSlot(i, NULL);
                _ApplyItemMods(_MapTempItems[i], i, false);
                ChatHandler(GetSession()).PSendSysMessage("卸载区域装备%s..", sCF->GetItemLink(_MapTempItems[i]->GetEntry()).c_str());

                if (Item* dscItem = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    _ApplyItemMods(dscItem, i, true);
                    SetVisibleItemSlot(i, dscItem);
                }
            }
        }
    }
}

void Player::ApplyAreaTempItems(bool apply)
{
    if (apply)
    {
        for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
        {
            if (_AreaTempItems[i])
            {
                SetVisibleItemSlot(i, _AreaTempItems[i]);

                if (Item* dscItem = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                    _ApplyItemMods(dscItem, i, false);

                _ApplyItemMods(_AreaTempItems[i], i, true);
                ChatHandler(GetSession()).PSendSysMessage("加载区域装备%s..", sCF->GetItemLink(_AreaTempItems[i]->GetEntry()).c_str());
            }
        }
    }
    else
    {
        for (uint32 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
        {
            if (_AreaTempItems[i])
            {
                SetVisibleItemSlot(i, NULL);
                _ApplyItemMods(_AreaTempItems[i], i, false);
                ChatHandler(GetSession()).PSendSysMessage("卸载区域装备%s..", sCF->GetItemLink(_AreaTempItems[i]->GetEntry()).c_str());

                if (Item* dscItem = GetItemByPos(INVENTORY_SLOT_BAG_0, i))
                {
                    _ApplyItemMods(dscItem, i, true);
                    SetVisibleItemSlot(i, dscItem);
                }
            }
        }
    }
}


void EquipmentManager::Load()
{
	EquipmentVec.clear();
	
	//QueryResult result = WorldDatabase.PQuery("SELECT class, map, zone, area,"
	//	"HEAD,NECK,SHOULDERS,BODY,CHEST,WAIST,LEGS,FEET,WRISTS,HANDS,FINGER1,FINGER2,TRINKET1,TRINKET2,BACK,MAINHAND,OFFHAND,RANGED,TABARD "
	//	"From _equipment");
	//if (result)
	//{
	//	do
	//	{
	//		Field* fields = result->Fetch();
	//		EquipmentTemplate Temp;
	//		Temp._class = fields[0].GetUInt8();
	//		Temp.map	= fields[1].GetUInt32();
	//		Temp.zone	= fields[2].GetUInt32();
	//		Temp.area	= fields[3].GetUInt32();
	//
	//		for (size_t i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
	//			Temp.slots[i] = fields[i + 4].GetInt32();
	//
	//		EquipmentVec.push_back(Temp);
	//	} while (result->NextRow());
	//}
}

bool EquipmentManager::SlotIsFreeze(Player* pl, uint8 slot)
{
    /*
	for (auto itr = EquipmentVec.begin(); itr != EquipmentVec.end(); itr++)
	{
		if (itr->_class != pl->getClass())
			continue;

		uint32 map = pl->GetMapId();
		uint32 zone = pl->GetZoneId();
		uint32 area = pl->GetAreaId();

		if (itr->map != map)
			continue;
		
		if (itr->zone == 0 && itr->area == 0 ||
			itr->zone == zone && itr->area == 0 ||
			itr->zone == zone && itr->area == area)
		{
			for (size_t i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
			{
				if (slot != i)
					continue;

				if (itr->slots[i] != 0)
					return true;
			}
		}
	}
    */
	return false;
}

void EquipmentManager::Update(Player* pl, uint8 update)
{
	if (update != 0)
		return;
    /**
	for (size_t i = KEYRING_SLOT_EM_START; i < KEYRING_SLOT_END; i++)
		if (Item* item = pl->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			pl->SwapItem(INVENTORY_SLOT_BAG_0 << 8 | i, INVENTORY_SLOT_BAG_0 << 8 | EQUIPMENT_SLOT_START + i - KEYRING_SLOT_EM_START);

	for (auto itr = EquipmentVec.begin(); itr != EquipmentVec.end(); itr++)
	{
		if (itr->_class != pl->getClass())
			continue;

		uint32 map = pl->GetMapId();
		uint32 zone = pl->GetZoneId();
		uint32 area = pl->GetAreaId();

		if (map != itr->map)
			continue;

		//if (update == 0 && itr->zone == 0 && itr->area == 0 ||
		//	update == 1 && itr->zone == zone && itr->area == 0 ||
		//	update == 2 && itr->zone == zone && itr->area == area)
		{
			for (size_t slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
			{
				int32 entry = itr->slots[slot];

				uint16 src = (INVENTORY_SLOT_BAG_0 << 8) | slot;
				uint16 dst = (INVENTORY_SLOT_BAG_0 << 8) | (KEYRING_SLOT_EM_START + slot);

				if (entry < 0)
					pl->SwapItem(src, dst);
				else if (const ItemTemplate* proto = sObjectMgr->GetItemTemplate(entry))
				{
					uint32 enchants[MAX_ENCHANTMENT_SLOT];
					for (uint8 j = 0; j < MAX_ENCHANTMENT_SLOT; ++j)
						enchants[j] = 0;

					if (Item* srcItem = pl->GetItemByPos(src))
						for (uint8 j = 0; j < MAX_ENCHANTMENT_SLOT; ++j)
						{
							if (j >= SOCK_ENCHANTMENT_SLOT && j <= BONUS_ENCHANTMENT_SLOT)
								continue;

							enchants[j] = srcItem->GetEnchantmentId(EnchantmentSlot(j));
						}
							
					
					pl->SwapItem(src, dst);

					if (!pl->GetItemByPos(src))
					{
						pl->EquipNewItem(src, entry, true);

						if (Item* srcItem = pl->GetItemByPos(src))
							for (uint8 j = 0; j < MAX_ENCHANTMENT_SLOT; ++j)
							{
								if (j >= SOCK_ENCHANTMENT_SLOT && j <= BONUS_ENCHANTMENT_SLOT)
									continue;

								pl->ApplyEnchantment(srcItem, EnchantmentSlot(j), false);
								srcItem->SetEnchantment(EnchantmentSlot(j), enchants[j], 0, 0);
								pl->ApplyEnchantment(srcItem, EnchantmentSlot(j), true);
							}
					}
						
				}
			}
		}
	}*/
}

bool EquipmentManager::InKeyings(Item* item)
{
	if (item)
		return item->GetBagSlot() == INVENTORY_SLOT_BAG_0 && item->GetSlot() >= KEYRING_SLOT_EM_START && item->GetSlot() < KEYRING_SLOT_END;
	return false;
}

bool EquipmentManager::InKeyings(uint8 bag, uint8 slot)
{
	return bag == INVENTORY_SLOT_BAG_0 && slot >= KEYRING_SLOT_EM_START && slot < KEYRING_SLOT_END;
}

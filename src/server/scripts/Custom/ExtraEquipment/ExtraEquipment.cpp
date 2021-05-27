#pragma execution_character_set("utf-8")
#include "ExtraEquipment.h"
#include "../Custom/CommonFunc/CommonFunc.h"
#include "../Custom/ItemMod/ItemMod.h"

float extra_stat_muil;
float extra_enchant_stat_muil;
bool extra_origin_item_back;

std::vector<uint32 /*entry*/> ExtraEuipMentEntryVec;

void ExtraEquipment::Load()
{
	extra_stat_muil = 1.0f;
	extra_enchant_stat_muil = 1.0f;
	extra_origin_item_back	= false;

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 属性倍率, 附魔属性倍率, 是否可以取回 from _物品_双甲" :
		"SELECT stat_muil, enchant_muil, originItemEnable from _itemmod_extra_equipments");
	if (result)
	{
		Field* fields = result->Fetch();

		extra_stat_muil = fields[0].GetFloat();
		extra_enchant_stat_muil = fields[1].GetFloat();
		extra_origin_item_back	= fields[2].GetBool();
	}

	ExtraEuipMentEntryVec.clear();
	QueryResult result1 = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID from _物品_双甲列表" :
		"SELECT entry from _itemmod_extra_equipments_enable");
	if (result1)
	{
		do
		{
			Field* fields = result1->Fetch();
			ExtraEuipMentEntryVec.push_back(fields[0].GetUInt32());
		} while (result1->NextRow());
	}

}

std::string ExtraEquipment::GetItemName(uint32 entry, uint32 width, uint32 height, int x, int y)
{
	std::ostringstream ss;
	ss << "|TInterface";
	const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
	const ItemDisplayInfoEntry* dispInfo = NULL;
	if (temp)
	{
		dispInfo = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);
		if (dispInfo)
			ss << "/ICONS/" << dispInfo->inventoryIcon;
	}
	if (!temp && !dispInfo)
		ss << "/InventoryItems/WoWUnknownItem01";
	ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";
	ss << temp->Name1;
	return ss.str();
}

std::string ExtraEquipment::GetSlotDefaultText(EquipmentSlots slot, uint32 width, uint32 height, int x, int y)
{
	std::ostringstream ss;
	ss << "|TInterface/PaperDoll/";
	switch (slot)
	{
	case EQUIPMENT_SLOT_HEAD: ss << "UI-PaperDoll-Slot-Head"; break;
	case EQUIPMENT_SLOT_SHOULDERS: ss << "UI-PaperDoll-Slot-Shoulder"; break;
	case EQUIPMENT_SLOT_NECK: ss << "UI-PaperDoll-Slot-Neck"; break;
	case EQUIPMENT_SLOT_BODY: ss << "UI-PaperDoll-Slot-Shirt"; break;
	case EQUIPMENT_SLOT_CHEST: ss << "UI-PaperDoll-Slot-Chest"; break;
	case EQUIPMENT_SLOT_WAIST: ss << "UI-PaperDoll-Slot-Waist"; break;
	case EQUIPMENT_SLOT_LEGS: ss << "UI-PaperDoll-Slot-Legs"; break;
	case EQUIPMENT_SLOT_FEET: ss << "UI-PaperDoll-Slot-Feet"; break;
	case EQUIPMENT_SLOT_WRISTS: ss << "UI-PaperDoll-Slot-Wrists"; break;
	case EQUIPMENT_SLOT_HANDS: ss << "UI-PaperDoll-Slot-Hands"; break;
	case EQUIPMENT_SLOT_FINGER1: ss << "UI-PaperDoll-Slot-Finger"; break;
	case EQUIPMENT_SLOT_FINGER2: ss << "UI-PaperDoll-Slot-Finger"; break;
	case EQUIPMENT_SLOT_TRINKET1: ss << "UI-PaperDoll-Slot-Trinket"; break;
	case EQUIPMENT_SLOT_TRINKET2: ss << "UI-PaperDoll-Slot-Trinket"; break;
	case EQUIPMENT_SLOT_BACK: ss << "UI-PaperDoll-Slot-Chest"; break;
	case EQUIPMENT_SLOT_MAINHAND: ss << "UI-PaperDoll-Slot-MainHand"; break;
	case EQUIPMENT_SLOT_OFFHAND: ss << "UI-PaperDoll-Slot-SecondaryHand"; break;
	case EQUIPMENT_SLOT_RANGED: ss << "UI-PaperDoll-Slot-Ranged"; break;
	case EQUIPMENT_SLOT_TABARD: ss << "UI-PaperDoll-Slot-Tabard"; break;
	default: ss << "UI-Backpack-EmptySlot";
	}
	ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";

	ss << "未装备";

	return ss.str();
}

std::string ExtraEquipment::GetSlotText(Player* player, EquipmentSlots slot)
{
	uint32 len = player->ExtraEquimentVec.size();
	for (size_t i = 0; i < len; i++)
		if (slot == player->ExtraEquimentVec[i].slot)
			return GetItemName(player->ExtraEquimentVec[i].itemEntry, 30, 30, 0, 0) + GetDes(player,slot);

	return GetSlotDefaultText(slot, 30, 30, 0, 0);
}

std::string ExtraEquipment::GetStatDes(uint32 entry)
{
	const ItemTemplate* proto = sObjectMgr->GetItemTemplate(entry);

	if (!proto)
		return "";

	std::ostringstream oss;

	//装备属性
	for (uint8 k = 0; k < MAX_ITEM_PROTO_STATS; ++k)
	{
		if (k >= proto->StatsCount)
			break;

		uint32 statType = proto->ItemStat[k].ItemStatType;
		int32  val = proto->ItemStat[k].ItemStatValue *extra_stat_muil;

		if (val == 0)
			continue;

		switch (statType)
		{
		case ITEM_MOD_MANA:
			oss << "\n              +" << val << "法力值";
			break;
		case ITEM_MOD_HEALTH:                           // modify HP
			oss << "\n              +" << val << "生命值";
			break;
		case ITEM_MOD_AGILITY:                          // modify agility
			oss << "\n              +" << val << "敏捷";
			break;
		case ITEM_MOD_STRENGTH:                         //modify strength
			oss << "\n              +" << val << "力量";
			break;
		case ITEM_MOD_INTELLECT:                        //modify intellect
			oss << "\n              +" << val << "智力";
			break;
		case ITEM_MOD_SPIRIT:                           //modify spirit
			oss << "\n              +" << val << "精神";
			break;
		case ITEM_MOD_STAMINA:                          //modify stamina
			oss << "\n              +" << val << "耐力";
			break;
		case ITEM_MOD_DEFENSE_SKILL_RATING:
			oss << "\n              +" << val << "防御等级";
			break;
		case ITEM_MOD_DODGE_RATING:
			oss << "\n              +" << val << "躲闪等级";
			break;
		case ITEM_MOD_PARRY_RATING:
			oss << "\n              +" << val << "招架等级";
			break;
		case ITEM_MOD_BLOCK_RATING:
			oss << "\n              +" << val << "格挡等级";
			break;
		case ITEM_MOD_HIT_MELEE_RATING:
			oss << "\n              +" << val << "命中等级";
			break;
		case ITEM_MOD_HIT_RANGED_RATING:
			oss << "\n              +" << val << "远程命中等级";
			break;
		case ITEM_MOD_HIT_SPELL_RATING:
			oss << "\n              +" << val << "法术命中等级";
			break;
		case ITEM_MOD_CRIT_MELEE_RATING:
			oss << "\n              +" << val << "暴击等级";
			break;
		case ITEM_MOD_CRIT_RANGED_RATING:
			oss << "\n              +" << val << "远程暴击等级";
			break;
		case ITEM_MOD_CRIT_SPELL_RATING:
			oss << "\n              +" << val << "法术暴击等级";
			break;
		case ITEM_MOD_HIT_TAKEN_MELEE_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_HIT_TAKEN_RANGED_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_HIT_TAKEN_SPELL_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_CRIT_TAKEN_MELEE_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_CRIT_TAKEN_RANGED_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_CRIT_TAKEN_SPELL_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_HASTE_MELEE_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_HASTE_RANGED_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_HASTE_SPELL_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_HIT_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_CRIT_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_HIT_TAKEN_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_CRIT_TAKEN_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_RESILIENCE_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_HASTE_RATING:
			oss << "\n              +" << val << "急速等级";
			break;
		case ITEM_MOD_EXPERTISE_RATING:
			oss << "\n              +" << val << "韧性等级";
			break;
		case ITEM_MOD_ATTACK_POWER:
			oss << "\n              +" << val << "攻击强度";
			break;
		case ITEM_MOD_RANGED_ATTACK_POWER:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_MANA_REGENERATION:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_ARMOR_PENETRATION_RATING:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_SPELL_POWER:
			oss << "\n              +" << val << "法术强度";
			break;
		case ITEM_MOD_HEALTH_REGEN:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_SPELL_PENETRATION:
			oss << "\n              +" << val << "";
			break;
		case ITEM_MOD_BLOCK_VALUE:
			oss << "\n              +" << val << "";
			break;
			// deprecated item mods
		case ITEM_MOD_SPELL_HEALING_DONE:
		case ITEM_MOD_SPELL_DAMAGE_DONE:
			break;
		}
	}

	return oss.str();
}

std::string ExtraEquipment::GetDes(Player* player, EquipmentSlots slot)
{
	std::ostringstream oss;
	
	oss << GetStatDes(GetItemEntry(player, slot));

	uint32 len = player->ExtraEquimentVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (slot == player->ExtraEquimentVec[i].slot)
		{
			uint32 entry = player->ExtraEquimentVec[i].itemEntry;

			for (uint8 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < PROP_ENCHANTMENT_SLOT_4; enchant_slot++)
			{
              uint32 enchantId = player->ExtraEquimentVec[i].enchant[enchant_slot];
              if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId))
              	oss << "\n              " << enchantEntry->description[4];              
			}
		}
	}

	return oss.str();
}

std::string ExtraEquipment::GetDes(Item* item)
{
	std::ostringstream oss;

	oss << GetStatDes(item->GetEntry());

	for (uint8 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; enchant_slot++)
	{
		uint32 enchantId = item->GetEnchantmentId(EnchantmentSlot(enchant_slot));
		if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId))
			oss << "\n              " << enchantEntry->description[4];
	}
	return oss.str();
}

uint32 ExtraEquipment::GetItemEntry(Player* player, EquipmentSlots slot)
{
	uint32 len = player->ExtraEquimentVec.size();
	for (size_t i = 0; i < len; i++)
		if (slot == player->ExtraEquimentVec[i].slot)
			return player->ExtraEquimentVec[i].itemEntry;

	return 0;
}

bool ExtraEquipment::IsForSlot(Item* item, EquipmentSlots slot)
{
	ItemTemplate const* proto = item->GetTemplate();
	if (proto->Class != ITEM_CLASS_ARMOR && proto->Class != ITEM_CLASS_WEAPON)
		return false;
	
	bool IsInEntryVec = std::find(ExtraEuipMentEntryVec.begin(), ExtraEuipMentEntryVec.end(), proto->ItemId) == ExtraEuipMentEntryVec.end() ? false : true;

	if (!IsInEntryVec && !ExtraEuipMentEntryVec.empty())
		return false;

	switch (slot)
	{
	case EQUIPMENT_SLOT_HEAD:
		return proto->InventoryType == INVTYPE_HEAD;
	case EQUIPMENT_SLOT_NECK:
		return proto->InventoryType == INVTYPE_NECK;
	case EQUIPMENT_SLOT_SHOULDERS:
		return proto->InventoryType == INVTYPE_SHOULDERS;
	case EQUIPMENT_SLOT_BODY:
		return proto->InventoryType == INVTYPE_BODY;
	case EQUIPMENT_SLOT_CHEST:
		return proto->InventoryType == INVTYPE_CHEST || proto->InventoryType == INVTYPE_ROBE;
	case EQUIPMENT_SLOT_WAIST:
		return proto->InventoryType == INVTYPE_WAIST;
	case EQUIPMENT_SLOT_LEGS:
		return proto->InventoryType == INVTYPE_LEGS;
	case EQUIPMENT_SLOT_FEET:
		return proto->InventoryType == INVTYPE_FEET;
	case EQUIPMENT_SLOT_WRISTS:
		return proto->InventoryType == INVTYPE_WRISTS;
	case EQUIPMENT_SLOT_HANDS:
		return proto->InventoryType == INVTYPE_HANDS;
	case EQUIPMENT_SLOT_FINGER1:
		return proto->InventoryType == INVTYPE_FINGER;
	case EQUIPMENT_SLOT_FINGER2:
		return proto->InventoryType == INVTYPE_FINGER;
	case EQUIPMENT_SLOT_TRINKET1:
		return proto->InventoryType == INVTYPE_TRINKET;
	case EQUIPMENT_SLOT_TRINKET2:
		return proto->InventoryType == INVTYPE_TRINKET;
	case EQUIPMENT_SLOT_BACK:
		return proto->InventoryType == INVTYPE_CLOAK;
	case EQUIPMENT_SLOT_MAINHAND:
		return proto->InventoryType == INVTYPE_2HWEAPON || proto->InventoryType == INVTYPE_WEAPONMAINHAND || proto->InventoryType == INVTYPE_WEAPON;
	case EQUIPMENT_SLOT_OFFHAND:
		return proto->InventoryType == INVTYPE_WEAPONOFFHAND || proto->InventoryType == INVTYPE_SHIELD || proto->InventoryType == INVTYPE_HOLDABLE;
		break;
	case EQUIPMENT_SLOT_RANGED:
		return proto->InventoryType == INVTYPE_THROWN || proto->InventoryType == INVTYPE_RANGED || proto->InventoryType == INVTYPE_RANGEDRIGHT || proto->InventoryType == INVTYPE_RELIC;
	case EQUIPMENT_SLOT_TABARD:
		return proto->InventoryType == INVTYPE_TABARD;
	default:
		return false;
	}
}

void ExtraEquipment::AddMenuList(Player* player, Object* obj)
{
	for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
	{
		//if (slot == EQUIPMENT_SLOT_NECK || slot == EQUIPMENT_SLOT_BODY || slot > EQUIPMENT_SLOT_HANDS)
		//	continue;

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GetSlotText(player, EquipmentSlots(slot)), slot, EXTRA_EQUIPMENT_NEXT_MENULIST);
	}
			
	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else 
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());

}

void ExtraEquipment::AddNextMenuList(Player* player, Object* obj, uint32 sender,bool firstOpen,bool fordisplay)
{
	if (firstOpen)
	{
		player->selectedEquipmentSlot = sender;

		player->GossipVec.clear();

		//主背包
		for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
			if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
              if (IsForSlot(item, EquipmentSlots(player->selectedEquipmentSlot)))
              {
              	ExtraEquimentGossipTemplate temp;
              	temp.smallIcon = GOSSIP_ICON_CHAT;
              	temp.text = GetItemName(item->GetEntry(), 30, 30, 0, 0) + GetDes(item);
              	temp.sender = item->GetGUIDLow();
              	temp.action = EXTRA_EQUIPMENT_UPDATE;
              	temp.item = item;
              	player->GossipVec.push_back(temp);
              }

		//额外三个背包
		for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
			if (Bag* pBag = player->GetBagByPos(i))
              for (uint32 j = 0; j < pBag->GetBagSize(); j++)
              	if (Item* item = player->GetItemByPos(i, j))
              		if (IsForSlot(item, EquipmentSlots(player->selectedEquipmentSlot)))
              		{
              			ExtraEquimentGossipTemplate temp;
              			temp.smallIcon = GOSSIP_ICON_CHAT;
              			temp.text = GetItemName(item->GetEntry(), 30, 30, 0, 0) + GetDes(item);
              			temp.sender = item->GetGUIDLow();
              			temp.action = EXTRA_EQUIPMENT_UPDATE;
              			player->GossipVec.push_back(temp);
              		}
	}
		
	for (size_t i = player->flag_i; i < player->GossipVec.size(); i++)
	{
		player->flag_i++;
		player->rowId++;

		player->ADD_GOSSIP_ITEM(player->GossipVec[i].smallIcon, player->GossipVec[i].text, player->GossipVec[i].sender, player->GossipVec[i].action);

		if (player->rowId >= MAX_EXTRA_EQUIPMENT_ROWS_COUNT)
		{
			player->rowId = 0;
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "下页", GOSSIP_SENDER_MAIN, EXTRA_EQUIPMENT_NEXT_PAGE);
			if (player->pageId >= 2)
              player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "上页", GOSSIP_SENDER_MAIN, EXTRA_EQUIPMENT_PREV_PAGE);
			break;
		}
	}


	if (player->rowId < MAX_EXTRA_EQUIPMENT_ROWS_COUNT && player->rowId != 0 && player->pageId !=1)
	{
		player->flag_i = player->flag_i + MAX_EXTRA_EQUIPMENT_ROWS_COUNT - player->rowId;
		player->rowId = 0;
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "上页", GOSSIP_SENDER_MAIN, EXTRA_EQUIPMENT_PREV_PAGE);
	}

	if (!fordisplay)
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "返回", GOSSIP_SENDER_MAIN, EXTRA_EQUIPMENT_BACK);

	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

void ExtraEquipment::Action(Player* player, Object* obj, uint32 sender, bool fordisplay)
{
	
	if (Item* item = player->GetItemByGuid(MAKE_NEW_GUID(sender, 0, HIGHGUID_ITEM)))
	{
		//这里处理display mainhand offhand ranged
		if (fordisplay)
		{
			player->CLOSE_GOSSIP_MENU();
			return;
		}

		if (item->GetTemplate()->Stackable > 1 && item->GetCount() > 1)
		{
			player->GetSession()->SendNotification("更新失败，该物品叠加数量应小于2，请分离该物品！");
			player->CLOSE_GOSSIP_MENU();
			return;
		}
	
		GiveItemBack(player, EquipmentSlots(player->selectedEquipmentSlot));
		Update(player, item, EquipmentSlots(player->selectedEquipmentSlot));
		player->DestroyItem(item->GetBagSlot(), item->GetSlot(),true);
		player->GetSession()->SendAreaTriggerMessage("更新完成！");
	}
	
	player->CLOSE_GOSSIP_MENU();
}

void ExtraEquipment::GiveItemBack(Player* player, EquipmentSlots slot)
{
	if (!extra_origin_item_back)
		return;

	uint32 len = player->ExtraEquimentVec.size();
	for (size_t i = 0; i < len; i++)
		if (slot == player->ExtraEquimentVec[i].slot)
		{
			ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(player->ExtraEquimentVec[i].itemEntry);

			if (!pProto)
              continue;

			if (Item* item = sItemMod->AddItem(player, player->ExtraEquimentVec[i].itemEntry, 1))
              for (uint8 enchant_slot = PERM_ENCHANTMENT_SLOT; enchant_slot < MAX_ENCHANTMENT_SLOT; enchant_slot++)
              	item->SetEnchantment(EnchantmentSlot(enchant_slot), player->ExtraEquimentVec[i].enchant[enchant_slot], 0, 0, 0, true);
		}
}

void ExtraEquipment::Update(Player* player, Item* item, EquipmentSlots slot)
{
	if (!player || !item)
		return;

	uint32 guid = player->GetGUIDLow();
	uint32 itemEntry = item->GetEntry();
	std::ostringstream ssEnchants;
	for (uint8 i = PERM_ENCHANTMENT_SLOT; i < MAX_ENCHANTMENT_SLOT; ++i)
		ssEnchants << item->GetEnchantmentId(EnchantmentSlot(i)) << ' ';

	//插入数据库
	QueryResult result = CharacterDatabase.PQuery("SELECT * FROM characters_extra_equipments WHERE guid = %d AND equip_slot = %d", guid, slot);

	if (result)
		CharacterDatabase.PExecute("UPDATE characters_extra_equipments SET enchantments = '%s',itemEntry = %d WHERE guid = %d AND equip_slot = %d", ssEnchants.str().c_str(), itemEntry, guid, slot);
	else
		CharacterDatabase.PExecute("INSERT INTO characters_extra_equipments(guid,itemEntry,equip_slot,enchantments) values(%d, %d, %d,'%s')", guid, itemEntry, slot, ssEnchants.str().c_str());

	//除去双甲效果
	Apply(player, false);

	//更新vec
	for (std::vector<ExtraEquipments>::iterator itr = player->ExtraEquimentVec.begin(); itr != player->ExtraEquimentVec.end();)
		if (itr->slot == slot)
			itr = player->ExtraEquimentVec.erase(itr);
		else
			++itr;

	ExtraEquipments temp;
	temp.itemEntry = itemEntry;
	temp.slot = slot;
	for (uint8 i = PERM_ENCHANTMENT_SLOT; i < MAX_ENCHANTMENT_SLOT; ++i)
		temp.enchant[i] = item->GetEnchantmentId(EnchantmentSlot(i));
	player->ExtraEquimentVec.push_back(temp);

	//重载双甲效果
	Apply(player, true);
}

void ExtraEquipment::Apply(Player* player, bool apply)
{
	if (!player)
		return;

	uint32 len = player->ExtraEquimentVec.size();

	for (size_t i = 0; i < player->ExtraEquimentVec.size(); i++)
	{	
		ItemTemplate const* proto = sObjectMgr->GetItemTemplate(player->ExtraEquimentVec[i].itemEntry);

		if (!proto)
			continue;

		//附魔部分
		for (size_t j = 0; j < MAX_ENCHANTMENT_SLOT; j++)
		{		
			uint32 enchant_id = player->ExtraEquimentVec[i].enchant[j];

			SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id);

			if (!pEnchant)
              continue;

			for (int s = 0; s < MAX_ITEM_ENCHANTMENT_EFFECTS; ++s)
			{
              uint32 enchant_display_type = pEnchant->type[s];
              uint32 enchant_amount = pEnchant->amount[s] * extra_enchant_stat_muil;
              uint32 enchant_spell_id = pEnchant->spellid[s];
              
#pragma region 装备技能
              if (enchant_display_type == ITEM_ENCHANTMENT_TYPE_EQUIP_SPELL)
              {
              	if (enchant_spell_id)
              	{
              		if (apply)
              			player->AddAura(enchant_spell_id, player);
              		else
              			player->RemoveAura(enchant_spell_id);
              	}			
              }
#pragma endregion

#pragma region 属性
              if (enchant_display_type == ITEM_ENCHANTMENT_TYPE_STAT && enchant_amount != 0)
              {
              	switch (enchant_spell_id)
              	{
              	case ITEM_MOD_MANA:
              		player->HandleStatModifier(UNIT_MOD_MANA, BASE_VALUE, float(enchant_amount), apply);
              		break;
              	case ITEM_MOD_HEALTH:
              		player->HandleStatModifier(UNIT_MOD_HEALTH, BASE_VALUE, float(enchant_amount), apply);
              		break;
              	case ITEM_MOD_AGILITY:
              		player->HandleStatModifier(UNIT_MOD_STAT_AGILITY, TOTAL_VALUE, float(enchant_amount), apply);
              		player->ApplyStatBuffMod(STAT_AGILITY, (float)enchant_amount, apply);
              		break;
              	case ITEM_MOD_STRENGTH:
              		player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, TOTAL_VALUE, float(enchant_amount), apply);
              		player->ApplyStatBuffMod(STAT_STRENGTH, (float)enchant_amount, apply);
              		break;
              	case ITEM_MOD_INTELLECT:
              		player->HandleStatModifier(UNIT_MOD_STAT_INTELLECT, TOTAL_VALUE, float(enchant_amount), apply);
              		player->ApplyStatBuffMod(STAT_INTELLECT, (float)enchant_amount, apply);
              		break;
              	case ITEM_MOD_SPIRIT:
              		player->HandleStatModifier(UNIT_MOD_STAT_SPIRIT, TOTAL_VALUE, float(enchant_amount), apply);
              		player->ApplyStatBuffMod(STAT_SPIRIT, (float)enchant_amount, apply);
              		break;
              	case ITEM_MOD_STAMINA:
              		player->HandleStatModifier(UNIT_MOD_STAT_STAMINA, TOTAL_VALUE, float(enchant_amount), apply);
              		player->ApplyStatBuffMod(STAT_STAMINA, (float)enchant_amount, apply);
              		break;
              	case ITEM_MOD_DEFENSE_SKILL_RATING:
              		player->ApplyRatingMod(CR_DEFENSE_SKILL, enchant_amount, apply);
              		break;
              	case  ITEM_MOD_DODGE_RATING:
              		player->ApplyRatingMod(CR_DODGE, enchant_amount, apply);
              		break;
              	case ITEM_MOD_PARRY_RATING:
              		player->ApplyRatingMod(CR_PARRY, enchant_amount, apply);
              		break;
              	case ITEM_MOD_BLOCK_RATING:
              		player->ApplyRatingMod(CR_BLOCK, enchant_amount, apply);
              		break;
              	case ITEM_MOD_HIT_MELEE_RATING:
              		player->ApplyRatingMod(CR_HIT_MELEE, enchant_amount, apply);
              		break;
              	case ITEM_MOD_HIT_RANGED_RATING:
              		player->ApplyRatingMod(CR_HIT_RANGED, enchant_amount, apply);
              		break;
              	case ITEM_MOD_HIT_SPELL_RATING:
              		player->ApplyRatingMod(CR_HIT_SPELL, enchant_amount, apply);
              		break;
              	case ITEM_MOD_CRIT_MELEE_RATING:
              		player->ApplyRatingMod(CR_CRIT_MELEE, enchant_amount, apply);
              		break;
              	case ITEM_MOD_CRIT_RANGED_RATING:
              		player->ApplyRatingMod(CR_CRIT_RANGED, enchant_amount, apply);
              		break;
              	case ITEM_MOD_CRIT_SPELL_RATING:
              		player->ApplyRatingMod(CR_CRIT_SPELL, enchant_amount, apply);
              		break;
              	case ITEM_MOD_HASTE_RANGED_RATING:
              		player->ApplyRatingMod(CR_HASTE_RANGED, enchant_amount, apply);
              		break;
              	case ITEM_MOD_HASTE_SPELL_RATING:
              		player->ApplyRatingMod(CR_HASTE_SPELL, enchant_amount, apply);
              		break;
              	case ITEM_MOD_HIT_RATING:
              		player->ApplyRatingMod(CR_HIT_MELEE, enchant_amount, apply);
              		player->ApplyRatingMod(CR_HIT_RANGED, enchant_amount, apply);
              		player->ApplyRatingMod(CR_HIT_SPELL, enchant_amount, apply);
              		break;
              	case ITEM_MOD_CRIT_RATING:
              		player->ApplyRatingMod(CR_CRIT_MELEE, enchant_amount, apply);
              		player->ApplyRatingMod(CR_CRIT_RANGED, enchant_amount, apply);
              		player->ApplyRatingMod(CR_CRIT_SPELL, enchant_amount, apply);
              		break;
              		//                        Values ITEM_MOD_HIT_TAKEN_RATING and ITEM_MOD_CRIT_TAKEN_RATING are never used in Enchantment
              		//                        case ITEM_MOD_HIT_TAKEN_RATING:
              		//                            ApplyRatingMod(CR_HIT_TAKEN_MELEE, enchant_amount, apply);
              		//                            ApplyRatingMod(CR_HIT_TAKEN_RANGED, enchant_amount, apply);
              		//                            ApplyRatingMod(CR_HIT_TAKEN_SPELL, enchant_amount, apply);
              		//                            break;
              		//                        case ITEM_MOD_CRIT_TAKEN_RATING:
              		//                            ApplyRatingMod(CR_CRIT_TAKEN_MELEE, enchant_amount, apply);
              		//                            ApplyRatingMod(CR_CRIT_TAKEN_RANGED, enchant_amount, apply);
              		//                            ApplyRatingMod(CR_CRIT_TAKEN_SPELL, enchant_amount, apply);
              		//                            break;
              	case ITEM_MOD_RESILIENCE_RATING:
              		player->ApplyRatingMod(CR_CRIT_TAKEN_MELEE, enchant_amount, apply);
              		player->ApplyRatingMod(CR_CRIT_TAKEN_RANGED, enchant_amount, apply);
              		player->ApplyRatingMod(CR_CRIT_TAKEN_SPELL, enchant_amount, apply);
              		break;
              	case ITEM_MOD_HASTE_RATING:
              		player->ApplyRatingMod(CR_HASTE_MELEE, enchant_amount, apply);
              		player->ApplyRatingMod(CR_HASTE_RANGED, enchant_amount, apply);
              		player->ApplyRatingMod(CR_HASTE_SPELL, enchant_amount, apply);
              		break;
              	case ITEM_MOD_EXPERTISE_RATING:
              		player->ApplyRatingMod(CR_EXPERTISE, enchant_amount, apply);
              		break;
              	case ITEM_MOD_ATTACK_POWER:
              		player->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, float(enchant_amount), apply);
              		player->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(enchant_amount), apply);
              		break;
              	case ITEM_MOD_RANGED_ATTACK_POWER:
              		player->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(enchant_amount), apply);
              		break;
              		//                        case ITEM_MOD_FERAL_ATTACK_POWER:
              		//                            ApplyFeralAPBonus(enchant_amount, apply);
              		//                            ;//sLog->outDebug(LOG_FILTER_PLAYER_ITEMS, "+ %u FERAL_ATTACK_POWER", enchant_amount);
              		//                            break;
              	case ITEM_MOD_MANA_REGENERATION:
              		player->ApplyManaRegenBonus(enchant_amount, apply);
              		break;
              	case ITEM_MOD_ARMOR_PENETRATION_RATING:
              		player->ApplyRatingMod(CR_ARMOR_PENETRATION, enchant_amount, apply);
              		break;
              	case ITEM_MOD_SPELL_POWER:
              		player->ApplySpellPowerBonus(enchant_amount, apply);
              		break;
              	case ITEM_MOD_HEALTH_REGEN:
              		player->ApplyHealthRegenBonus(enchant_amount, apply);
              		break;
              	case ITEM_MOD_SPELL_PENETRATION:
              		player->ApplySpellPenetrationBonus(enchant_amount, apply);
              		break;
              	case ITEM_MOD_BLOCK_VALUE:
              		player->HandleBaseModValue(SHIELD_BLOCK_VALUE, FLAT_MOD, float(enchant_amount), apply);
              		break;
              	case ITEM_MOD_SPELL_HEALING_DONE:   // deprecated
              	case ITEM_MOD_SPELL_DAMAGE_DONE:    // deprecated
              	default:
              		break;
              	}
              }
#pragma endregion
              
			}

		}
		
		//装备属性
		for (uint8 k = 0; k < MAX_ITEM_PROTO_STATS; ++k)
		{
			if (k >= proto->StatsCount)
              break;

			uint32 statType = proto->ItemStat[k].ItemStatType;
			int32  val = proto->ItemStat[k].ItemStatValue *extra_stat_muil;

			if (val == 0)
              continue;

			switch (statType)
			{
			case ITEM_MOD_MANA:
              player->HandleStatModifier(UNIT_MOD_MANA, BASE_VALUE, float(val), apply);
              break;
			case ITEM_MOD_HEALTH:                           // modify HP
              player->HandleStatModifier(UNIT_MOD_HEALTH, BASE_VALUE, float(val), apply);
              break;
			case ITEM_MOD_AGILITY:                          // modify agility
              player->HandleStatModifier(UNIT_MOD_STAT_AGILITY, BASE_VALUE, float(val), apply);
              player->ApplyStatBuffMod(STAT_AGILITY, float(val), apply);
              break;
			case ITEM_MOD_STRENGTH:                         //modify strength
              player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, BASE_VALUE, float(val), apply);
              player->ApplyStatBuffMod(STAT_STRENGTH, float(val), apply);
              break;
			case ITEM_MOD_INTELLECT:                        //modify intellect
              player->HandleStatModifier(UNIT_MOD_STAT_INTELLECT, BASE_VALUE, float(val), apply);
              player->ApplyStatBuffMod(STAT_INTELLECT, float(val), apply);
              break;
			case ITEM_MOD_SPIRIT:                           //modify spirit
              player->HandleStatModifier(UNIT_MOD_STAT_SPIRIT, BASE_VALUE, float(val), apply);
              player->ApplyStatBuffMod(STAT_SPIRIT, float(val), apply);
              break;
			case ITEM_MOD_STAMINA:                          //modify stamina
              player->HandleStatModifier(UNIT_MOD_STAT_STAMINA, BASE_VALUE, float(val), apply);
              player->ApplyStatBuffMod(STAT_STAMINA, float(val), apply);
              break;
			case ITEM_MOD_DEFENSE_SKILL_RATING:
              player->ApplyRatingMod(CR_DEFENSE_SKILL, int32(val), apply);
              break;
			case ITEM_MOD_DODGE_RATING:
              player->ApplyRatingMod(CR_DODGE, int32(val), apply);
              break;
			case ITEM_MOD_PARRY_RATING:
              player->ApplyRatingMod(CR_PARRY, int32(val), apply);
              break;
			case ITEM_MOD_BLOCK_RATING:
              player->ApplyRatingMod(CR_BLOCK, int32(val), apply);
              break;
			case ITEM_MOD_HIT_MELEE_RATING:
              player->ApplyRatingMod(CR_HIT_MELEE, int32(val), apply);
              break;
			case ITEM_MOD_HIT_RANGED_RATING:
              player->ApplyRatingMod(CR_HIT_RANGED, int32(val), apply);
              break;
			case ITEM_MOD_HIT_SPELL_RATING:
              player->ApplyRatingMod(CR_HIT_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_CRIT_MELEE_RATING:
              player->ApplyRatingMod(CR_CRIT_MELEE, int32(val), apply);
              break;
			case ITEM_MOD_CRIT_RANGED_RATING:
              player->ApplyRatingMod(CR_CRIT_RANGED, int32(val), apply);
              break;
			case ITEM_MOD_CRIT_SPELL_RATING:
              player->ApplyRatingMod(CR_CRIT_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_HIT_TAKEN_MELEE_RATING:
              player->ApplyRatingMod(CR_HIT_TAKEN_MELEE, int32(val), apply);
              break;
			case ITEM_MOD_HIT_TAKEN_RANGED_RATING:
              player->ApplyRatingMod(CR_HIT_TAKEN_RANGED, int32(val), apply);
              break;
			case ITEM_MOD_HIT_TAKEN_SPELL_RATING:
              player->ApplyRatingMod(CR_HIT_TAKEN_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_CRIT_TAKEN_MELEE_RATING:
              player->ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(val), apply);
              break;
			case ITEM_MOD_CRIT_TAKEN_RANGED_RATING:
              player->ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(val), apply);
              break;
			case ITEM_MOD_CRIT_TAKEN_SPELL_RATING:
              player->ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_HASTE_MELEE_RATING:
              player->ApplyRatingMod(CR_HASTE_MELEE, int32(val), apply);
              break;
			case ITEM_MOD_HASTE_RANGED_RATING:
              player->ApplyRatingMod(CR_HASTE_RANGED, int32(val), apply);
              break;
			case ITEM_MOD_HASTE_SPELL_RATING:
              player->ApplyRatingMod(CR_HASTE_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_HIT_RATING:
              player->ApplyRatingMod(CR_HIT_MELEE, int32(val), apply);
              player->ApplyRatingMod(CR_HIT_RANGED, int32(val), apply);
              player->ApplyRatingMod(CR_HIT_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_CRIT_RATING:
              player->ApplyRatingMod(CR_CRIT_MELEE, int32(val), apply);
              player->ApplyRatingMod(CR_CRIT_RANGED, int32(val), apply);
              player->ApplyRatingMod(CR_CRIT_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_HIT_TAKEN_RATING:
              player->ApplyRatingMod(CR_HIT_TAKEN_MELEE, int32(val), apply);
              player->ApplyRatingMod(CR_HIT_TAKEN_RANGED, int32(val), apply);
              player->ApplyRatingMod(CR_HIT_TAKEN_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_CRIT_TAKEN_RATING:
              player->ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(val), apply);
              player->ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(val), apply);
              player->ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_RESILIENCE_RATING:
              player->ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(val), apply);
              player->ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(val), apply);
              player->ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_HASTE_RATING:
              player->ApplyRatingMod(CR_HASTE_MELEE, int32(val), apply);
              player->ApplyRatingMod(CR_HASTE_RANGED, int32(val), apply);
              player->ApplyRatingMod(CR_HASTE_SPELL, int32(val), apply);
              break;
			case ITEM_MOD_EXPERTISE_RATING:
              player->ApplyRatingMod(CR_EXPERTISE, int32(val), apply);
              break;
			case ITEM_MOD_ATTACK_POWER:
              player->HandleStatModifier(UNIT_MOD_ATTACK_POWER, TOTAL_VALUE, float(val), apply);
              player->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(val), apply);
              break;
			case ITEM_MOD_RANGED_ATTACK_POWER:
              player->HandleStatModifier(UNIT_MOD_ATTACK_POWER_RANGED, TOTAL_VALUE, float(val), apply);
              break;
              //            case ITEM_MOD_FERAL_ATTACK_POWER:
              //                ApplyFeralAPBonus(int32(val), apply);
              //                break;
			case ITEM_MOD_MANA_REGENERATION:
              player->ApplyManaRegenBonus(int32(val), apply);
              break;
			case ITEM_MOD_ARMOR_PENETRATION_RATING:
              player->ApplyRatingMod(CR_ARMOR_PENETRATION, int32(val), apply);
              break;
			case ITEM_MOD_SPELL_POWER:
              player->ApplySpellPowerBonus(int32(val), apply);
              break;
			case ITEM_MOD_HEALTH_REGEN:
              player->ApplyHealthRegenBonus(int32(val), apply);
              break;
			case ITEM_MOD_SPELL_PENETRATION:
              player->ApplySpellPenetrationBonus(val, apply);
              break;
			case ITEM_MOD_BLOCK_VALUE:
              player->HandleBaseModValue(SHIELD_BLOCK_VALUE, FLAT_MOD, float(val), apply);
              break;
              // deprecated item mods
			case ITEM_MOD_SPELL_HEALING_DONE:
			case ITEM_MOD_SPELL_DAMAGE_DONE:
              break;
			}		
		}
	}
}

void ExtraEquipment::LoadPlayerData(Player* player)
{
	QueryResult result = CharacterDatabase.PQuery("SELECT itemEntry,equip_slot,enchantments FROM characters_extra_equipments where guid = %d",player->GetGUIDLow());
	if (!result) 
		return;
	do
	{
		Field* fields = result->Fetch();
		ExtraEquipments temp;
		temp.itemEntry = fields[0].GetUInt32();
		temp.slot = EquipmentSlots(fields[1].GetUInt8());
		std::vector<std::string> str_vec = sCF->SplitStr(fields[2].GetString(), " ");
		for (size_t i = 0; i < str_vec.size(); i++)
			temp.enchant[i] = (uint32)atoi(str_vec[i].c_str());		
			

		player->ExtraEquimentVec.push_back(temp);
	} while (result->NextRow());
}


class ExtraEquipment_PlayerScript : public PlayerScript
{
public:
	ExtraEquipment_PlayerScript() : PlayerScript("ExtraEquipment_PlayerScript") {}

	void OnLogin(Player* player)
	{
		sExtraEquipment->LoadPlayerData(player);
		sExtraEquipment->Apply(player, true);
	}
};

class NPCExtraEquipment : public CreatureScript
{
public:
	NPCExtraEquipment() : CreatureScript("NPCExtraEquipment") { }
	bool OnGossipHello(Player* player, Creature* creature)
	{
		player->PlayerTalkClass->ClearMenus();

		player->flag_i = 0;
		player->rowId = 0;
		player->pageId = 0;
		sExtraEquipment->AddMenuList(player, creature);
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		switch (action)
		{
		case EXTRA_EQUIPMENT_NEXT_MENULIST:
			player->pageId += 1;
			sExtraEquipment->AddNextMenuList(player, creature, sender);
			break;
		case EXTRA_EQUIPMENT_NEXT_PAGE:
			player->pageId += 1;
			sExtraEquipment->AddNextMenuList(player, creature, sender,false);
			break;
		case EXTRA_EQUIPMENT_PREV_PAGE:
			player->pageId -= 1;
			player->flag_i -= 2 * MAX_EXTRA_EQUIPMENT_ROWS_COUNT;
			sExtraEquipment->AddNextMenuList(player, creature, sender,false);
			break;
		case EXTRA_EQUIPMENT_BACK:
			player->flag_i = 0;
			player->rowId = 0;
			player->pageId = 0;
			sExtraEquipment->AddMenuList(player, creature);
			break;
		case EXTRA_EQUIPMENT_UPDATE:
			sExtraEquipment->Action(player, creature, sender);
			break;
		default:
			break;
		}
		
		return true;
	}
};

void AddSC_ExtraEquipment()
{
	new ExtraEquipment_PlayerScript();
	new NPCExtraEquipment();
}

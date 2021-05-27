#pragma execution_character_set("utf-8")
#include "Talisman.h"
#include "../GCAddon/GCAddon.h"
#include "../CommonFunc/CommonFunc.h"
#include "Group.h"
#include "../SpiritPower/SpiritPower.h"
#include "../Switch/Switch.h"

std::unordered_map<uint32, uint32> TalismanMap;

void Talisman::Load()
{
	TalismanMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 法宝物品ID,分组ID FROM __法宝" :
		"SELECT entry,groupid FROM _talisman");
	if (!result)
		return;
	do
	{
		Field* fields = result->Fetch();
		uint32 entry = fields[0].GetUInt32();
		if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(entry))
			TalismanMap.insert(std::make_pair(entry, fields[1].GetUInt32()));
	} while (result->NextRow());
}

bool Talisman::HasTalismanEquiped(Player* player, uint32 entry)
{
	for (std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.begin(); iter != player->TalismanMap.end(); iter++)
		if (entry == iter->second)
			return true;

	return false;
}

uint32 Talisman::GetEntryByID(Player* player, uint32 ID)
{
	std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.find(ID);

	if (iter != player->TalismanMap.end())
		return iter->second;

	return 0;
}

std::string Talisman::GetIcon(uint32 entry)
{
	const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp)
		return "TALISMAN_DEFAULT";

	ItemDisplayInfoEntry const* info = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);

	if (!info)
		return "TALISMAN_DEFAULT";

	return info->inventoryIcon;
}

std::string Talisman::GetName(uint32 entry)
{
	std::string name = sCF->GetItemLink(entry);

	if (name.empty())
		return "[未炼化]";

	return name;
}

bool Talisman::IsMidItem(uint32 entry)
{
	auto itr = TalismanMap.find(entry);
	if (itr == TalismanMap.end())
		return false;

	return itr->second == 0;
}

void Talisman::SendPacket(Player* player)
{
	std::ostringstream oss;

	for (std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.begin(); iter != player->TalismanMap.end(); iter++)
		oss << iter->first << "-" << GetIcon(iter->second) << "-" << iter->second << "#";

	sGCAddon->SendPacketTo(player, "GC_S_TALISMAM", oss.str());
}

void Talisman::ApplyOnEquip(Player* player, uint32 entry, bool apply)
{
	const ItemTemplate* proto = sObjectMgr->GetItemTemplate(entry);
	if (!proto)
		return;

#pragma region 装备属性
	for (uint8 k = 0; k < MAX_ITEM_PROTO_STATS; ++k)
	{
		if (k >= proto->StatsCount)
			break;

		uint32 statType = proto->ItemStat[k].ItemStatType;
		int32  val = proto->ItemStat[k].ItemStatValue;

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

	uint32 armor = proto->Armor;

	if (armor && proto->ArmorDamageModifier)
		armor -= uint32(proto->ArmorDamageModifier);

	if (armor)
	{
		UnitModifierType modType = TOTAL_VALUE;
		if (proto->Class == ITEM_CLASS_ARMOR)
		{
			switch (proto->SubClass)
			{
			case ITEM_SUBCLASS_ARMOR_CLOTH:
			case ITEM_SUBCLASS_ARMOR_LEATHER:
			case ITEM_SUBCLASS_ARMOR_MAIL:
			case ITEM_SUBCLASS_ARMOR_PLATE:
			case ITEM_SUBCLASS_ARMOR_SHIELD:
				modType = BASE_VALUE;
				break;
			}
		}
		player->HandleStatModifier(UNIT_MOD_ARMOR, modType, float(armor), apply);
	}

	// Add armor bonus from ArmorDamageModifier if > 0
	if (proto->ArmorDamageModifier > 0)
		player->HandleStatModifier(UNIT_MOD_ARMOR, TOTAL_VALUE, float(proto->ArmorDamageModifier), apply);

	if (proto->Block)
		player->HandleBaseModValue(SHIELD_BLOCK_VALUE, FLAT_MOD, float(proto->Block), apply);

	if (proto->HolyRes)
		player->HandleStatModifier(UNIT_MOD_RESISTANCE_HOLY, BASE_VALUE, float(proto->HolyRes), apply);

	if (proto->FireRes)
		player->HandleStatModifier(UNIT_MOD_RESISTANCE_FIRE, BASE_VALUE, float(proto->FireRes), apply);

	if (proto->NatureRes)
		player->HandleStatModifier(UNIT_MOD_RESISTANCE_NATURE, BASE_VALUE, float(proto->NatureRes), apply);

	if (proto->FrostRes)
		player->HandleStatModifier(UNIT_MOD_RESISTANCE_FROST, BASE_VALUE, float(proto->FrostRes), apply);

	if (proto->ShadowRes)
		player->HandleStatModifier(UNIT_MOD_RESISTANCE_SHADOW, BASE_VALUE, float(proto->ShadowRes), apply);

	if (proto->ArcaneRes)
		player->HandleStatModifier(UNIT_MOD_RESISTANCE_ARCANE, BASE_VALUE, float(proto->ArcaneRes), apply);
#pragma endregion
#pragma region 装备技能
	for (uint8 k = 0; k < MAX_ITEM_PROTO_SPELLS; ++k)
	{
		uint32 spellId = proto->Spells[k].SpellId;

		if (SpellEntry const* spellInfo = sSpellStore.LookupEntry(spellId))
		{
			if (apply)
				player->learnSpell(spellId);
			else
				player->removeSpell(spellId, SPEC_MASK_ALL, false);
		}
	}
#pragma endregion
}

void Talisman::ApplyOnLogin(Player* player)
{
	bool apply = true;

	for (std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.begin(); iter != player->TalismanMap.end(); iter++)
	{
		const ItemTemplate* proto = sObjectMgr->GetItemTemplate(iter->second);
		if (!proto)
			continue;

#pragma region 装备属性
		for (uint8 k = 0; k < MAX_ITEM_PROTO_STATS; ++k)
		{
			if (k >= proto->StatsCount)
				break;

			uint32 statType = proto->ItemStat[k].ItemStatType;
			int32  val = proto->ItemStat[k].ItemStatValue;

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
		uint32 armor = proto->Armor;

		if (armor && proto->ArmorDamageModifier)
			armor -= uint32(proto->ArmorDamageModifier);

		if (armor)
		{
			UnitModifierType modType = TOTAL_VALUE;
			if (proto->Class == ITEM_CLASS_ARMOR)
			{
				switch (proto->SubClass)
				{
				case ITEM_SUBCLASS_ARMOR_CLOTH:
				case ITEM_SUBCLASS_ARMOR_LEATHER:
				case ITEM_SUBCLASS_ARMOR_MAIL:
				case ITEM_SUBCLASS_ARMOR_PLATE:
				case ITEM_SUBCLASS_ARMOR_SHIELD:
					modType = BASE_VALUE;
					break;
				}
			}
			player->HandleStatModifier(UNIT_MOD_ARMOR, modType, float(armor), apply);
		}

		// Add armor bonus from ArmorDamageModifier if > 0
		if (proto->ArmorDamageModifier > 0)
			player->HandleStatModifier(UNIT_MOD_ARMOR, TOTAL_VALUE, float(proto->ArmorDamageModifier), apply);

		if (proto->Block)
			player->HandleBaseModValue(SHIELD_BLOCK_VALUE, FLAT_MOD, float(proto->Block), apply);

		if (proto->HolyRes)
			player->HandleStatModifier(UNIT_MOD_RESISTANCE_HOLY, BASE_VALUE, float(proto->HolyRes), apply);

		if (proto->FireRes)
			player->HandleStatModifier(UNIT_MOD_RESISTANCE_FIRE, BASE_VALUE, float(proto->FireRes), apply);

		if (proto->NatureRes)
			player->HandleStatModifier(UNIT_MOD_RESISTANCE_NATURE, BASE_VALUE, float(proto->NatureRes), apply);

		if (proto->FrostRes)
			player->HandleStatModifier(UNIT_MOD_RESISTANCE_FROST, BASE_VALUE, float(proto->FrostRes), apply);

		if (proto->ShadowRes)
			player->HandleStatModifier(UNIT_MOD_RESISTANCE_SHADOW, BASE_VALUE, float(proto->ShadowRes), apply);

		if (proto->ArcaneRes)
			player->HandleStatModifier(UNIT_MOD_RESISTANCE_ARCANE, BASE_VALUE, float(proto->ArcaneRes), apply);
#pragma endregion
	}
}

uint32 Talisman::GetGroupId(uint32 entry)
{
	auto itr = TalismanMap.find(entry);
	if (itr != TalismanMap.end())
		return itr->second;

	return 0;
}

void Talisman::EquipTalisman(Player* player, uint32 ID, uint32 entry)
{
	if (player->IsInCombat())
	{
		ChatHandler(player->GetSession()).PSendSysMessage("你正处在战斗中，更新法宝失败！", sCF->GetItemLink(entry).c_str());
		return;
	}

	if (const ItemTemplate* t = sObjectMgr->GetItemTemplate(entry))
	{
		InventoryResult msg = player->CanUseItem(t);

		if (msg != EQUIP_ERR_OK)
		{
			player->SendEquipError(msg, NULL, NULL);
			return;
		}
	}

	//取下法宝
	if (entry == 0)
	{
		std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.find(ID);

		if (iter != player->TalismanMap.end())
		{
			uint32 ori = iter->second;
			iter->second = 0;

			//获得原法宝
			if (const ItemTemplate* temp = sObjectMgr->GetItemTemplate(ori))
			{
				//去除装备属性及技能
				ApplyOnEquip(player, temp->ItemId, false);
				player->AddItem(temp->ItemId, 1);

				//移除本命法宝属性
				if (IsMidItem(ori))
					UpdateTalismanValueOnEquip(player, false);
			}
		}
	}
	else
	{
		//判断该位置是否已经装备法宝
		auto i = player->TalismanMap.find(ID);
		if (i != player->TalismanMap.end())
		{
			if (i->second != 0)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("请先右键取下该位置法宝%s！", sCF->GetItemLink(i->second).c_str());
				return;
			}
		}

		//判断是否是法宝

		auto itr = TalismanMap.find(entry);

		if (itr == TalismanMap.end())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("%s不是法宝！", sCF->GetItemLink(entry).c_str());
			return;
		}
		else
		{

			if (itr->second != 0)
			{
				for (std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.begin(); iter != player->TalismanMap.end(); iter++)
					if (itr->second == GetGroupId(iter->second))
					{
						ChatHandler(player->GetSession()).PSendSysMessage("你已经装备一个同一类型的法宝%s！", sCF->GetItemLink(iter->second).c_str());
						return;
					}
			}
		}

		//判断是否拥有该法宝
		if (!player->HasItemCount(entry, 1))
			return;

		//判断是否已装备该法宝
		for (std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.begin(); iter != player->TalismanMap.end(); iter++)
			if (entry == iter->second)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("你已经装备法宝%s！", sCF->GetItemLink(entry).c_str());
				return;
			}

		//判断是否本命法宝
		if (IsMidItem(entry) && ID != 7)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("本命法宝只能装备在中间位置！", sCF->GetItemLink(entry).c_str());
			return;
		}

		if (!IsMidItem(entry) && ID == 7)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("中间位置只能装备本命法宝！", sCF->GetItemLink(entry).c_str());
			return;
		}


		//摧毁该法宝
		player->DestroyItemCount(entry, 1, true, false);

		//装备该法宝
		std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.find(ID);

		if (iter != player->TalismanMap.end())
		{
			//完成法宝炼化任务 91146
			//if (IsMidItem(entry))
			//sCF->CompleteQuest(player, 30001);

			uint32 ori = iter->second;

			iter->second = entry;

			//获得原法宝
			if (const ItemTemplate* temp = sObjectMgr->GetItemTemplate(ori))
			{
				//去除装备属性及技能
				ApplyOnEquip(player, temp->ItemId, false);
				player->AddItem(temp->ItemId, 1);
			}
		}
		//添加装备属性及技能
		ApplyOnEquip(player, entry, true);

		//增加本命法宝属性
		if (IsMidItem(entry))
		{
			UpdateTalismanValueOnEquip(player, true);
			SendTalisManValue(player);
		}
	}

	//更新客户端显示
	SendPacket(player);

	//更新数据库
	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_REP_TALISMAN);
	stmt->setUInt32(0, player->GetGUIDLow());
	for (uint32 i = 1; i < 8; i++)
		stmt->setUInt32(i, GetEntryByID(player, i));
	CharacterDatabase.Execute(stmt);


	//掌天瓶
	//if (player->HasSpell(120003))
	//	player->SpiritPowerInterval = player->SpiritPowerInterval / 2;
	//else if (player->HasSpell(120004))
	//	player->SpiritPowerInterval = player->SpiritPowerInterval / 4;
	//else
	//	player->SpiritPowerInterval = 2000;
}

void Talisman::UpdateTalismanValueOnEquip(Player* player, bool equip)
{
	player->HandleStatModifier(UNIT_MOD_STAT_STAMINA, TOTAL_VALUE, player->TalismanValue, equip);
	player->ApplyStatBuffMod(STAT_STAMINA, player->TalismanValue, equip);
	player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, TOTAL_VALUE, player->TalismanValue, equip);
	player->ApplyStatBuffMod(STAT_STRENGTH, player->TalismanValue, equip);
	player->HandleStatModifier(UNIT_MOD_STAT_AGILITY, TOTAL_VALUE, player->TalismanValue, equip);
	player->ApplyStatBuffMod(STAT_AGILITY, player->TalismanValue, equip);
	player->HandleStatModifier(UNIT_MOD_STAT_INTELLECT, TOTAL_VALUE, player->TalismanValue, equip);
	player->ApplyStatBuffMod(STAT_INTELLECT, player->TalismanValue, equip);
	player->HandleStatModifier(UNIT_MOD_STAT_SPIRIT, TOTAL_VALUE, player->TalismanValue, equip);
	player->ApplyStatBuffMod(STAT_SPIRIT, player->TalismanValue, equip);
	player->UpdateAllStats();
}

void Talisman::AddTalismanValue(Player* player, uint32 value, bool kill)
{
	bool equiped = GetEntryByID(player, 7) != 0;//HasTalismanEquiped(player, TALISMAN_VALUE_ITEM);

	if (kill && !equiped)
		return;

	int32 gap = sSwitch->GetValue(ST_TALISMAN) - player->TalismanValue;

	if (gap <= 0)
		return;
	else if (value > gap)
		value = gap;

	player->TalismanValue += value;
	SendTalisManValue(player);

	if (equiped)
	{
		player->HandleStatModifier(UNIT_MOD_STAT_STAMINA, TOTAL_VALUE, value, true);
		player->ApplyStatBuffMod(STAT_STAMINA, value, true);
		player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, TOTAL_VALUE, value, true);
		player->ApplyStatBuffMod(STAT_STRENGTH, value, true);
		player->HandleStatModifier(UNIT_MOD_STAT_AGILITY, TOTAL_VALUE, value, true);
		player->ApplyStatBuffMod(STAT_AGILITY, value, true);
		player->HandleStatModifier(UNIT_MOD_STAT_INTELLECT, TOTAL_VALUE, value, true);
		player->ApplyStatBuffMod(STAT_INTELLECT, value, true);
		player->HandleStatModifier(UNIT_MOD_STAT_SPIRIT, TOTAL_VALUE, value, true);
		player->ApplyStatBuffMod(STAT_SPIRIT, value, true);
		player->UpdateAllStats();
	}
}

void Talisman::SendTalisManValue(Player* player)
{
	sGCAddon->SendPacketTo(player, "GC_S_TALISMAN_VALUE", std::to_string(player->TalismanValue));
}

void Talisman::SaveTalisManValue(Player* player, bool logout)
{
	if (logout)
		CharacterDatabase.DirectPExecute("UPDATE characters SET TalismanValue = '%u' WHERE guid = '%u'", player->TalismanValue, player->GetGUIDLow());
	else if (sSwitch->GetValue(ST_TALISMAN) > player->TalismanValue)
	{
		CharacterDatabase.DirectPExecute("UPDATE characters SET TalismanValue = '%u' WHERE guid = '%u'", player->TalismanValue, player->GetGUIDLow());
		ChatHandler(player->GetSession()).PSendSysMessage("本命法宝信息已保存");
	}
}

class TalismanPlayerScript : PlayerScript
{
public:
	TalismanPlayerScript() : PlayerScript("TalismanPlayerScript") {}

	void OnLogin(Player* player) override
	{
		for (size_t i = 1; i <= 7; i++)
			player->TalismanMap.insert(std::make_pair(i, 0));

		QueryResult result = CharacterDatabase.PQuery("SELECT ID_1,ID_2,ID_3,ID_4,ID_5,ID_6,ID_7 FROM character_talisman WHERE guid = %u", player->GetGUIDLow());

		if (result)
		{
			Field* fields = result->Fetch();

			for (std::unordered_map<uint32, uint32>::iterator iter = player->TalismanMap.begin(); iter != player->TalismanMap.end(); iter++)
				iter->second = fields[iter->first - 1].GetUInt32();

			sTalisman->ApplyOnLogin(player);
		}

		uint32 value = 0;

		result = CharacterDatabase.PQuery("SELECT TalismanValue FROM characters WHERE guid = '%u'", player->GetGUIDLow());

		if (result)
		{
			do
			{
				Field* fields = result->Fetch();
				value = fields[0].GetUInt32();
			} while (result->NextRow());

			sTalisman->AddTalismanValue(player, value, false);
		}
	}

	void OnLogout(Player* player) override
	{
		sTalisman->SaveTalisManValue(player, true);
	}

	void OnCreatureKill(Player* killer, Creature* killed) override
	{
		//if (killed->C_AddTalismanValue > 0)
		//	sTalisman->AddTalismanValue(killer, killed->C_AddTalismanValue, true);
		//
		//if (killed->IsDungeonBoss() || killed->IsDungeonBoss())
		//	if (killed->C_AddTalismanValue > 0)
		//		sTalisman->SaveTalisManValue(killer);
	}
};

void AddSC_Talisman()
{
	new TalismanPlayerScript();
}

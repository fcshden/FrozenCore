#pragma execution_character_set("utf-8")
#include "CharMod.h"
#include "../../FunctionCollection/FunctionCollection.h"
#include "../../Switch/Switch.h"
#include "../../Requirement/Requirement.h"

std::vector<CharStatTemplate> CharStatVec;
std::vector<StaticStatTemplate> StaticStatVec;
std::vector<uint32> AltClassSpellVec;

void CharMod::Load()
{
	CharStatVec.clear();
	QueryResult result = WorldDatabase.PQuery(
		//		0			1			2			3			4			5			6				7			8
		"SELECT 职业索引,物理伤害PVP倍率,法术伤害PVP倍率,治疗效果PVP倍率,护甲值上限,躲闪百分比上限,招架百分比上限,格挡百分比上限,爆击百分比上限,"
		//		9			10				11			12				13				14				15				16			17				18				19				20			21
		"额外精神转法伤, 额外精神转治疗, 额外智力转法伤, 额外智力转治疗, 额外力量转攻强, 额外敏捷转攻强, 额外精神转攻强, 额外智力转攻强, 额外力量转法伤, 额外敏捷转法伤, 额外力量转治疗, 额外敏捷转治疗,减伤百分比,"
		//		22			23				24				25		26			27		28			29				30
		"物理伤害PVE倍率,法术伤害PVE倍率,治疗效果PVE倍率,急速等级上限,生命值上限,法力值上限,物理伤害上限,法术伤害上限,治疗效果上限 FROM _属性调整_职业");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			CharStatTemplate CharStatTemp;
			CharStatTemp.classIndex		= fields[0].GetUInt8();
			CharStatTemp.meleePVPDmgMod	= fields[1].GetFloat();
			CharStatTemp.spellPVPDmgMod	= fields[2].GetFloat();
			CharStatTemp.healPVPMod		= fields[3].GetFloat();
			CharStatTemp.armorLimit		= fields[4].GetFloat();
			CharStatTemp.dodgeLimit		= fields[5].GetFloat();
			CharStatTemp.parryLimit		= fields[6].GetFloat();
			CharStatTemp.blockLimit		= fields[7].GetFloat();
			CharStatTemp.critLimit		= fields[8].GetFloat();
			CharStatTemp.spirit2SP		= fields[9].GetFloat();
			CharStatTemp.spirit2Heal	= fields[10].GetFloat();
			CharStatTemp.intellect2SP	= fields[11].GetFloat();
			CharStatTemp.intellect2Heal = fields[12].GetFloat();
			CharStatTemp.strength2AP	= fields[13].GetFloat();
			CharStatTemp.agility2AP		= fields[14].GetFloat();

			CharStatTemp.spirit2AP		= fields[15].GetFloat();
			CharStatTemp.intellect2AP	= fields[16].GetFloat();
			CharStatTemp.strength2SP	= fields[17].GetFloat();
			CharStatTemp.agility2SP		= fields[18].GetFloat();
			CharStatTemp.strength2Heal	= fields[19].GetFloat();
			CharStatTemp.agility2Heal	= fields[20].GetFloat();

			CharStatTemp.reduceDmgMod	= fields[21].GetFloat();

			CharStatTemp.meleePVEDmgMod = fields[22].GetFloat();
			CharStatTemp.spellPVEDmgMod = fields[23].GetFloat();
			CharStatTemp.healPVEMod		= fields[24].GetFloat();

			CharStatTemp.hasteLimit		= fields[25].GetUInt32();
			CharStatTemp.hpLimit		= fields[26].GetUInt32();
			CharStatTemp.manaLimit		= fields[27].GetUInt32();
			CharStatTemp.meleeDmgLimit	= fields[28].GetUInt32();
			CharStatTemp.spellDmgLimit	= fields[29].GetUInt32();
			CharStatTemp.healLimit		= fields[30].GetUInt32();

			CharStatVec.push_back(CharStatTemp);
		} while (result->NextRow());
	}

	StaticStatVec.clear();

	AltClassSpellVec.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 技能ID FROM __转职保留技能" :
		"SELECT spellid FROM _alt_class_spells"))
	{
		do
		{
			Field* fields = result->Fetch();
			AltClassSpellVec.push_back(fields[0].GetUInt32());
		} while (result->NextRow());
	}
	

	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator i = smap.begin(); i != smap.end(); ++i)
		if (Player* player = i->second->GetPlayer())
		{
			ModLimit(player);
			ModStats(player);
		}	
}

float CharMod::GetLimit(StatLimitTypes type, uint8 classIndex)
{
	uint32 len = CharStatVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (classIndex == CharStatVec[i].classIndex)
		{
			switch (type)
			{
			case LIMIT_ARMOR:
				return CharStatVec[i].armorLimit;
			case LIMIT_DODGE:
				return CharStatVec[i].dodgeLimit;
			case LIMIT_PARRY:
				return CharStatVec[i].parryLimit;
			case LIMIT_BLOCK:
				return CharStatVec[i].blockLimit;
			case LIMIT_CRIT:
				return CharStatVec[i].critLimit;
			case LIMIT_HASTE:
				return CharStatVec[i].hasteLimit;
			case LIMIT_HP:
				return CharStatVec[i].hpLimit;
			case LIMIT_MANA:
				return CharStatVec[i].manaLimit;
			case LIMIT_MELEEDMG:
				return CharStatVec[i].meleeDmgLimit;
			case LIMIT_SPELLDMG:
				return CharStatVec[i].spellDmgLimit;
			case LIMIT_HEAL:
				return CharStatVec[i].healLimit;
			}
		}
	}
	return 1000000.0f;
}

float CharMod::GetMod(StatModTypes type, uint8 classIndex)
{
	uint32 len = CharStatVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (classIndex == CharStatVec[i].classIndex)
		{
			switch (type)
			{
			case CHAR_MOD_PVP_MELEE_DMG:
				return CharStatVec[i].meleePVPDmgMod;
			case CHAR_MOD_PVP_SPELL_DMG:
				return CharStatVec[i].spellPVPDmgMod;
			case CHAR_MOD_PVP_HEAL:
				return CharStatVec[i].healPVPMod;
			case CHAR_MOD_PVE_MELEE_DMG:
				return CharStatVec[i].meleePVEDmgMod;
			case CHAR_MOD_PVE_SPELL_DMG:
				return CharStatVec[i].spellPVEDmgMod;
			case CHAR_MOD_PVE_HEAL:
				return CharStatVec[i].healPVEMod;
			case CHAR_MOD_REDUCE_DMG:
				return CharStatVec[i].reduceDmgMod;
			}
		}
	}
	return 1.0f;
}

float CharMod::GetValue(SwitchStatTypes type, uint8 classIndex)
{
	uint32 len = CharStatVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (classIndex == CharStatVec[i].classIndex)
		{
			switch (type)
			{
			case SWITCH_SPIRIT2SP:
				return CharStatVec[i].spirit2SP;
			case SWITCH_SPIRIT2HEAL:
				return CharStatVec[i].spirit2Heal;
			case SWITCH_INTELLECT2SP:
				return CharStatVec[i].intellect2SP;
			case SWITCH_INTELLECT2HEAL:
				return CharStatVec[i].intellect2Heal;
			case SWITCH_STRENGTH2AP:
				return CharStatVec[i].strength2AP;
			case SWITCH_AGILITY2AP:
				return CharStatVec[i].agility2AP;

			case SWITCH_SPIRIT2AP:
				return  CharStatVec[i].spirit2AP;
			case SWITCH_INTELLECT2AP:
				return  CharStatVec[i].intellect2AP;
			case SWITCH_STRENGTH2SP:
				return  CharStatVec[i].strength2SP;
			case SWITCH_AGILITY2SP:
				return  CharStatVec[i].agility2SP;
			case SWITCH_STRENGTH2HEAL:
				return  CharStatVec[i].strength2Heal;
			case SWITCH_AGILITY2HEAL:
				return  CharStatVec[i].agility2Heal;

			}
		}
	}
	return 0.0f;
}

int32 CharMod::GetExtraSP(Player* player)
{
	float intellectValue = player->GetTotalStatValue(Stats(STAT_INTELLECT));
	float spiritValue = player->GetTotalStatValue(Stats(STAT_SPIRIT));
	float strengthValue = player->GetTotalStatValue(Stats(STAT_STRENGTH));
	float agilityValue = player->GetTotalStatValue(Stats(STAT_AGILITY));

	return int32(player->p_sp + intellectValue * player->p_intellect2SP + spiritValue * player->p_spirit2SP + strengthValue * player->p_strength2SP + agilityValue * player->p_agility2SP);
}
int32 CharMod::GetExtraAP(Player* player)
{
	float strengthValue = player->GetTotalStatValue(Stats(STAT_STRENGTH));
	float agilityValue = player->GetTotalStatValue(Stats(STAT_AGILITY));
	float intellectValue = player->GetTotalStatValue(Stats(STAT_INTELLECT));
	float spiritValue = player->GetTotalStatValue(Stats(STAT_SPIRIT));

	return int32(player->p_ap + strengthValue * player->p_strength2AP + agilityValue * player->p_agility2AP + intellectValue * player->p_intellect2AP + spiritValue * player->p_spirit2AP);
}

int32 CharMod::GetExtraHeal(Player* player)
{
	float intellectValue = player->GetTotalStatValue(Stats(STAT_INTELLECT));
	float spiritValue = player->GetTotalStatValue(Stats(STAT_SPIRIT));
	float strengthValue = player->GetTotalStatValue(Stats(STAT_STRENGTH));
	float agilityValue = player->GetTotalStatValue(Stats(STAT_AGILITY));

	return int32(player->p_heal + intellectValue * player->p_intellect2Heal + spiritValue * player->p_spirit2Heal + strengthValue * player->p_strength2Heal + agilityValue * player->p_agility2Heal);
}

void CharMod::ModLimit(Player* player)
{
	player->p_intellect2SP = GetValue(SWITCH_INTELLECT2SP, player->getClass());
	player->p_spirit2SP = GetValue(SWITCH_SPIRIT2SP, player->getClass());
	player->p_intellect2Heal = GetValue(SWITCH_INTELLECT2HEAL, player->getClass());
	player->p_spirit2Heal = GetValue(SWITCH_SPIRIT2HEAL, player->getClass());
	player->p_strength2AP = GetValue(SWITCH_STRENGTH2AP, player->getClass());
	player->p_agility2AP = GetValue(SWITCH_AGILITY2AP, player->getClass());


	player->p_agility2SP = GetValue(SWITCH_AGILITY2SP, player->getClass());
	player->p_strength2SP = GetValue(SWITCH_STRENGTH2SP, player->getClass());
	player->p_intellect2AP = GetValue(SWITCH_INTELLECT2AP, player->getClass());
	player->p_spirit2AP = GetValue(SWITCH_SPIRIT2AP, player->getClass());
	player->p_agility2Heal = GetValue(SWITCH_AGILITY2HEAL, player->getClass());
	player->p_strength2Heal = GetValue(SWITCH_STRENGTH2HEAL, player->getClass());

	player->p_PVP_meleeDmgMod = GetMod(CHAR_MOD_PVP_MELEE_DMG, player->getClass());
	player->p_PVP_spellDmgMod = GetMod(CHAR_MOD_PVP_SPELL_DMG, player->getClass());
	player->p_PVP_healMod = GetMod(CHAR_MOD_PVP_HEAL, player->getClass());
	player->p_PVE_meleeDmgMod = GetMod(CHAR_MOD_PVE_MELEE_DMG, player->getClass());
	player->p_PVE_spellDmgMod = GetMod(CHAR_MOD_PVE_SPELL_DMG, player->getClass());
	player->p_PVE_healMod = GetMod(CHAR_MOD_PVE_HEAL, player->getClass());
	player->p_reduceDmgMod = GetMod(CHAR_MOD_REDUCE_DMG, player->getClass());

	player->p_armorLimit = GetLimit(LIMIT_ARMOR, player->getClass());
	player->p_dodgeLimit = GetLimit(LIMIT_DODGE, player->getClass());
	player->p_parryLimit = GetLimit(LIMIT_PARRY, player->getClass());
	player->p_blockLimit = GetLimit(LIMIT_BLOCK, player->getClass());
	player->p_critLimit = GetLimit(LIMIT_CRIT, player->getClass());

	player->p_hasteLimit = GetLimit(LIMIT_HASTE, player->getClass());
	player->p_hpLimit = GetLimit(LIMIT_HP, player->getClass());
	player->p_manaLimit = GetLimit(LIMIT_MANA, player->getClass());
	player->p_meleeDmgLimit = GetLimit(LIMIT_MELEEDMG, player->getClass());
	player->p_spellDmgLimit = GetLimit(LIMIT_SPELLDMG, player->getClass());
	player->p_healLimit = GetLimit(LIMIT_HEAL, player->getClass());


	player->UpdateAllStats();
}
void CharMod::ModStats(Player* player)
{
	bool exsit = false;

	for (auto itr = StaticStatVec.begin(); itr != StaticStatVec.end(); itr++)
	{
		if (itr->map == player->GetMapId() && itr->vip == player->vipLevel)// && player->getClass() == itr->_class)
		{
			player->StaticStatsMap = itr->StaticStatsMap;
			player->StaticCombatRatingMap = itr->StaticCombatRatingMap;
			player->StaticHealth = itr->StaticHealth;
			player->StaticSpellPower = itr->StaticSpellPower;
			player->StaticHealPower = itr->StaticHealPower;
			player->StaticRangeAttackPower = itr->StaticRangeAttackPower;
			player->StaticAttackPower = itr->StaticAttackPower;
			exsit = true;
		}
	}

	if (!exsit)
	{
		player->StaticStatsMap.clear();
		player->StaticCombatRatingMap.clear();
		player->StaticHealth = 0;
		player->StaticSpellPower = 0;
		player->StaticHealPower = 0;
		player->StaticRangeAttackPower = 0;
		player->StaticAttackPower = 0;
	}

	player->UpdateAllStats();
}


bool CharMod::CheckFamily(Player* player, uint32 SpellFamily)
{
	switch (player->getClass())
	{
	case CLASS_WARRIOR:
		return SpellFamily == SPELLFAMILY_WARRIOR;
	case CLASS_PALADIN:
		return SpellFamily == SPELLFAMILY_PALADIN;
	case CLASS_HUNTER:
		return SpellFamily == SPELLFAMILY_HUNTER;
	case CLASS_ROGUE:
		return SpellFamily == SPELLFAMILY_ROGUE;
	case CLASS_PRIEST:
		return SpellFamily == SPELLFAMILY_PRIEST;
	case CLASS_DEATH_KNIGHT:
		return SpellFamily == SPELLFAMILY_DEATHKNIGHT;
	case CLASS_SHAMAN:
		return SpellFamily == SPELLFAMILY_SHAMAN;
	case CLASS_MAGE:
		return SpellFamily == SPELLFAMILY_MAGE;
	case CLASS_WARLOCK:
		return SpellFamily == SPELLFAMILY_WARLOCK;
	case CLASS_DRUID:
		return SpellFamily == SPELLFAMILY_DRUID;
	}

	return false;
}

bool CharMod::CheckSkill(SpellInfo const* spellInfo)
{
	return
		spellInfo->IsAbilityOfSkillType(SKILL_ALCHEMY) ||			//炼金
		spellInfo->IsAbilityOfSkillType(SKILL_BLACKSMITHING) ||		//锻造
		spellInfo->IsAbilityOfSkillType(SKILL_COOKING) ||			//烹饪
		spellInfo->IsAbilityOfSkillType(SKILL_ENCHANTING) ||		//附魔
		spellInfo->IsAbilityOfSkillType(SKILL_ENGINEERING) ||		//工程
		spellInfo->IsAbilityOfSkillType(SKILL_FIRST_AID) ||			//急救
		spellInfo->IsAbilityOfSkillType(SKILL_HERBALISM) ||			//草药
		spellInfo->IsAbilityOfSkillType(SKILL_LEATHERWORKING) ||	//制皮
		spellInfo->IsAbilityOfSkillType(SKILL_INSCRIPTION) ||		//铭文
		spellInfo->IsAbilityOfSkillType(SKILL_TAILORING) ||			//裁缝
		spellInfo->IsAbilityOfSkillType(SKILL_MINING) ||			//挖矿
		spellInfo->IsAbilityOfSkillType(SKILL_FISHING) ||			//钓鱼
		spellInfo->IsAbilityOfSkillType(SKILL_SKINNING) ||			//剥皮
		spellInfo->IsAbilityOfSkillType(SKILL_JEWELCRAFTING) ||		//珠宝
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING_HORSE) ||		//骑术
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING_WOLF) ||
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING_TIGER) ||
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING_RAM) ||
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING_RAPTOR) ||
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING_MECHANOSTRIDER) ||
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING_UNDEAD_HORSE) ||
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING_KODO) ||
		spellInfo->IsAbilityOfSkillType(SKILL_RIDING) ||
		spellInfo->HasAura(SPELL_AURA_MOUNTED) ||					//坐骑
		spellInfo->IsAbilityOfSkillType(SKILL_COMPANIONS);			//小宠物
}

void CharMod::ModClass(Player* player, uint8 targetClass)
{
	player->CLOSE_GOSSIP_MENU();
	
	uint32 reqId = sSwitch->GetValue(ST_ALT_CLASS);

	if (!sReq->Check(player, reqId))
		return;

	sReq->Des(player, reqId);

	uint32 bytes0 = 0;
	bytes0 |= player->getRace(); 
	bytes0 |= targetClass << 8; 
	bytes0 |= player->getGender() << 16;

	ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(targetClass);
	if (cEntry && cEntry->powerType < MAX_POWERS)
		bytes0 |= cEntry->powerType << 24;

	player->SetUInt32Value(UNIT_FIELD_BYTES_0, bytes0);

	PlayerSpellMap spellMap = player->GetSpellMap();

	for (PlayerSpellMap::const_iterator iter = spellMap.begin(); iter != spellMap.end(); ++iter)
		if (SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(iter->first))
			if(!CheckSkill(spellInfo))
				if (AltClassSpellVec.empty() || std::find(AltClassSpellVec.begin(), AltClassSpellVec.end(), iter->first) == AltClassSpellVec.end())
					player->removeSpell(iter->first, SPEC_MASK_ALL, false);

	player->learnDefaultSpells();
	player->learnQuestRewardedSpells();

	player->resetTalents(true);
	player->SendTalentsInfoData(false);


	for (uint32 id = 0; id < sSkillLineStore.GetNumRows(); id++)
	{

		SkillLineEntry const* pSkill = sSkillLineStore.LookupEntry(id);
		if (!pSkill)
			continue;

		if (pSkill->categoryId != SKILL_CATEGORY_CLASS)
			continue;

		player->SetSkill(id, 0, 0, 0);
	}


	if (targetClass == CLASS_DEATH_KNIGHT)
		player->InitRunes();

	//CharacterDatabase.PExecute("update characters set class=%u where guid =%u", targetClass, player->GetGUIDLow()); //更新转职后的职业
	CharacterDatabase.PExecute("delete from character_glyphs where guid =%u", player->GetGUIDLow()); // 删除雕文
	CharacterDatabase.PExecute("delete from character_pet where owner =%u", player->GetGUIDLow());//删除宠物

	//Customize(GetGUID(), getGender(), 1, 1, 1, 1, 1);
	sWorld->UpdateGlobalPlayerData(player->GetGUIDLow(), PLAYER_UPDATE_DATA_CLASS, "", 0, 0, 0, targetClass);
	player->GetSession()->KickPlayer();
}

std::string CharMod::GetClassName1(uint32 _class)
{
	switch (_class)
	{
	case CLASS_WARRIOR:
		return "[战士]";
	case CLASS_PALADIN:
		return "[圣骑士]";
	case CLASS_HUNTER:
		return "[猎人]";
	case CLASS_ROGUE:
		return "[盗贼]";
	case CLASS_PRIEST:
		return "[牧师]";
	case CLASS_DEATH_KNIGHT:
		return "[死亡骑士]";
	case CLASS_SHAMAN:
		return "[萨满祭司]";
	case CLASS_MAGE:
		return "[法师]";
	case CLASS_WARLOCK:
		return "[术士]";
	case CLASS_DRUID:
		return "[德鲁伊]";
	default:
		return "";
	}
}

void CharMod::AddGossip(Player* player, Object* obj)
{
	uint8 race = player->getRace();
	uint8 _class = player->getClass();

	for (size_t i = CLASS_WARRIOR; i <= CLASS_DRUID; i++)
	{
		if (i == 10 || _class == i)
			continue;

		PlayerInfo const* info = sObjectMgr->GetPlayerInfo(race, i);

		if (!info)
			continue;

		std::string name = GetClassName1(i);
		player->ADD_GOSSIP_ITEM_EXTENDED(0, "转职 -> " + name, SENDER_ALT_CLASS, i, sReq->Notice(player, sSwitch->GetValue(ST_ALT_CLASS), "转职为", name), 0, false);
	}

	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

class CharModPlayerScript : public PlayerScript
{
public:
	CharModPlayerScript() : PlayerScript("CharModPlayerScript") {}

	void OnLogin(Player* player)
	{
		sCharMod->ModLimit(player);
	}

	void OnMapChanged(Player* player) 
	{
		sCharMod->ModStats(player);
	}
};


void AddSC_CharModPlayerScript()
{
	new CharModPlayerScript();
}

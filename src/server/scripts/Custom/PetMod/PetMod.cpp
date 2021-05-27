#pragma execution_character_set("utf-8")
#include "PetMod.h"
#include "../MapMod/MapMod.h"

std::unordered_map<uint8, PetModTemplate> PetModMap;

void PetMod::Load()
{
	PetModMap.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 职业索引, 根据玩家伤害增加物理伤害百分比, 根据玩家伤害增加法术伤害百分比, 根据玩家血量增加血量百分比, 护甲值上限 FROM _属性调整_宠物" :
		"SELECT classIndex, DmgAddPct, SpAddPct, HpAddPct, ArmorLimit FROM _pet");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint8 classIndex = fields[0].GetUInt8();
			PetModTemplate Temp;
			Temp.DmgAddPct	= fields[1].GetFloat();
			Temp.SpAddPct	= fields[2].GetFloat();
			Temp.HpAddPct	= fields[3].GetFloat();
			Temp.ArmorLimit = fields[4].GetFloat();
			PetModMap.insert(std::make_pair(classIndex, Temp));

		} while (result->NextRow());
	}
}

void PetMod::GetMinDmgMod(Guardian* guardian, float &mindamage, float &maxdamage)
{
	Unit* owner = guardian->GetOwner();

	if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
		return;

	float DmgAddPct = 0.0f;

	std::unordered_map<uint8, PetModTemplate>::iterator iter = PetModMap.find(owner->getClass());

	if (iter != PetModMap.end())
		DmgAddPct = iter->second.DmgAddPct;

	int32 base = 0;

	switch (owner->getClass())
	{
	case CLASS_HUNTER:
		base = owner->GetTotalAttackPowerValue(RANGED_ATTACK);
		break;
	case CLASS_PRIEST:
		base = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW);
		break;
	case CLASS_DEATH_KNIGHT:
		base = owner->GetTotalAttackPowerValue(BASE_ATTACK);
		break;
	case CLASS_SHAMAN:
		base = owner->GetTotalAttackPowerValue(BASE_ATTACK);
		break;
	case CLASS_DRUID:
		base = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_NATURE);
		break;
	case CLASS_WARLOCK:
	{
		int32 fire = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FIRE);
		int32 shadow = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW);
		base = (fire > shadow) ? fire : shadow;
	}
		break;
	default:
		break;
	}

	if (guardian->GetEntry() == 24207)//血虫
		DmgAddPct = 5;

	mindamage = mindamage + CalculatePct(base, DmgAddPct);
	maxdamage = maxdamage + CalculatePct(base, DmgAddPct);

	if (owner->getClass() == CLASS_HUNTER)
	{
		if (owner->HasAura(19551))
		{
			mindamage = mindamage + owner->GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE) * 0.3;
			maxdamage = maxdamage + owner->GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE) * 0.3;
		}
		else if (owner->HasAura(19550))
		{
			mindamage = mindamage + owner->GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE) * 0.2;
			maxdamage = maxdamage + owner->GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE) * 0.2;
		}
		if (owner->HasAura(19549))
		{
			mindamage = mindamage + owner->GetFloatValue(UNIT_FIELD_MINRANGEDDAMAGE) * 0.1;
			maxdamage = maxdamage + owner->GetFloatValue(UNIT_FIELD_MAXRANGEDDAMAGE) * 0.1;
		}
	}
}

float PetMod::GetSpAddPct(Unit* owner)
{
	std::unordered_map<uint8, PetModTemplate>::iterator iter = PetModMap.find(owner->getClass());

	if (iter != PetModMap.end())
		return iter->second.SpAddPct;

	return 0.0f;
}

void PetMod::GetHpMod(Guardian* guardian, float &value)
{
	Unit* owner = guardian->GetOwner();

	if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
		return;

	float HpAddPct = 0.0f;

	std::unordered_map<uint8, PetModTemplate>::iterator iter = PetModMap.find(owner->getClass());

	if (iter != PetModMap.end())
		HpAddPct = iter->second.HpAddPct;

	uint32 base = owner->GetMaxHealth();

	if (guardian->GetEntry() == 24207)//血虫
		HpAddPct = 20;

	value += CalculatePct(base, HpAddPct);

	if (owner->getClass() == CLASS_HUNTER)
	{
		if (owner->HasAura(19587))
			value += base * 0.5;
		else if (owner->HasAura(19586))
			value += base * 0.4;
		else if (owner->HasAura(19585))
			value += base * 0.3;
		else if (owner->HasAura(19584))
			value += base * 0.2;
		else if (owner->HasAura(19583))
			value += base * 0.1;
	}
}

void PetMod::GetArmorLimit(Guardian* guardian, float &value)
{
	Unit* owner = guardian->GetOwner();

	if (!owner || owner->GetTypeId() != TYPEID_PLAYER)
		return;

	std::unordered_map<uint8, PetModTemplate>::iterator iter = PetModMap.find(owner->getClass());

	if (iter != PetModMap.end())
		value = std::min(value, iter->second.ArmorLimit);
}

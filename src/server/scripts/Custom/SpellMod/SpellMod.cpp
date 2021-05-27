#pragma execution_character_set("utf-8")
#include "SpellMod.h"
#include "../Requirement/Requirement.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"

std::unordered_map<uint32, SpellModTemplate> SpellModMap;
std::unordered_map<uint32, SpellModBaseTemplate> SpellModBaseMap;
std::vector<AccountSpellTemplate> AccountSpellVec;
std::unordered_map<uint32, AuraTriggerSpellTemplate> AuraTriggerSpellMap;
std::vector<SpellLeechTemplate> SpellLeechVec;
std::unordered_map<uint32, AuraLeechTemplate> AuraLeechMap;
std::vector<AuraModSpellTemplate> AuraModSpellVec;
std::unordered_map<uint32, AuraModStatTemplate> AuraModStatMap;
std::unordered_map<uint32, AuraPctTemplate> AuraPctMap;
std::unordered_map<uint32, AuraModClassSpellTemplate> AuraModClassSpellMap;
std::unordered_map<uint32, std::string> OnRemoveSpellMap;
std::unordered_map<uint32, std::string> OnLearnSpellMap;
std::unordered_map<uint32, uint32> SpellCusTargetMaskMap;
std::unordered_map<uint32, MountSpellTemplate> MountSpellMap;
std::unordered_map<uint32, AuraStackTriggerTemplate> AuraStackTriggerMap;
std::unordered_map<uint32, AuraTriggerOnDeathTemplate> AuraTriggerOnDeathMap;

void SpellMod::Load()
{
	QueryResult result;

	SpellModMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 技能ID, 释放技能需求模板ID,伤害倍率,治疗倍率,是否账号绑定,是否禁用,施法时间,持续时间,冷却时间,触发机率,周期时间1,周期时间2,周期时间3 FROM _技能" :
		"SELECT spellId, reqId,dmgMod,healMod,accountBind,disable,castingtime,duration,cooldown,procChance,Periodic1,Periodic2,Periodic3 FROM _spellmod"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 spellId = fields[0].GetUInt32();
			SpellModTemplate Temp;
			Temp.reqId = fields[1].GetUInt32();
			Temp.dmgMod = fields[2].GetFloat();
			Temp.healMod = fields[3].GetFloat();
			Temp.accountBind = fields[4].GetBool();

			Temp.disable		= fields[5].GetBool();
			Temp.castingtime	= fields[6].GetUInt32();
			Temp.duration		= fields[7].GetUInt32();
			Temp.cooldown		= fields[8].GetUInt32();
			Temp.procChance		= fields[9].GetUInt32();

			for (size_t i = 0; i < MAX_SPELL_EFFECTS; i++)
				Temp.Periodic[i] = fields[10 + i].GetUInt32();

			SpellModMap.insert(std::make_pair(spellId, Temp));
		} while (result->NextRow());
	}

	AccountSpellVec.clear();
	if (result = LoginDatabase.PQuery("SELECT account, spell FROM account_spells"))
	{
		do
		{
			Field* fields = result->Fetch();
			AccountSpellTemplate Temp;
			Temp.accountId = fields[0].GetUInt32();
			Temp.spellId = fields[1].GetFloat();
			AccountSpellVec.push_back(Temp);
		} while (result->NextRow());
	}

	SpellModBaseMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT spellId,"
		"Effect1,Effect2,Effect3,"
		"AuraName1,AuraName2,AuraName3,MiscValue1,MiscValue2,MiscValue3,"
		"BasePoints1,BasePoints2,BasePoints3,"
		"SpellMaskA1,SpellMaskA2,SpellMaskA3,"
		"SpellMaskB1,SpellMaskB2,SpellMaskB3,"
		"SpellMaskC1,SpellMaskC2,SpellMaskC3,"
		"EffectTarget1,EffectTarget2,EffectTarget3,"
		"targetspell1,targetspell2,targetspell3,"
		"cooldown,targetflag,chance,charges,SpellFamilyName FROM _技能_数据库修改" :
		"SELECT spellId,"
		"Effect1,Effect2,Effect3,"
		"AuraName1,AuraName2,AuraName3,MiscValue1,MiscValue2,MiscValue3,"
		"BasePoints1,BasePoints2,BasePoints3,"
		"SpellMaskA1,SpellMaskA2,SpellMaskA3,"
		"SpellMaskB1,SpellMaskB2,SpellMaskB3,"
		"SpellMaskC1,SpellMaskC2,SpellMaskC3,"
		"EffectTarget1,EffectTarget2,EffectTarget3,"
		"targetspell1,targetspell2,targetspell3,"
		"cooldown,targetflag,chance,charges,SpellFamilyName FROM _spellmod_base"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 spellId = fields[0].GetUInt32();
			SpellModBaseTemplate Temp;
			Temp.Effect[0] = fields[1].GetUInt32();
			Temp.Effect[1] = fields[2].GetUInt32();
			Temp.Effect[2] = fields[3].GetUInt32();
			Temp.EffectApplyAuraName[0] = fields[4].GetUInt32();
			Temp.EffectApplyAuraName[1] = fields[5].GetUInt32();
			Temp.EffectApplyAuraName[2] = fields[6].GetUInt32();
			Temp.EffectMiscValue[0] = fields[7].GetInt32();
			Temp.EffectMiscValue[1] = fields[8].GetInt32();
			Temp.EffectMiscValue[2] = fields[9].GetInt32();
			Temp.EffectBasePoints[0] = fields[10].GetInt32();
			Temp.EffectBasePoints[1] = fields[11].GetInt32();
			Temp.EffectBasePoints[2] = fields[12].GetInt32();
			Temp.EffectSpellClassMask[0] = flag96(fields[13].GetUInt32(), fields[14].GetUInt32(), fields[15].GetUInt32());
			Temp.EffectSpellClassMask[1] = flag96(fields[16].GetUInt32(), fields[17].GetUInt32(), fields[18].GetUInt32());
			Temp.EffectSpellClassMask[2] = flag96(fields[19].GetUInt32(), fields[20].GetUInt32(), fields[21].GetUInt32());
			Temp.EffectImplicitTargetA[0] = fields[22].GetUInt32();
			Temp.EffectImplicitTargetA[1] = fields[23].GetUInt32();
			Temp.EffectImplicitTargetA[2] = fields[24].GetUInt32();
			Temp.EffectTriggerSpell[0] = fields[25].GetUInt32();
			Temp.EffectTriggerSpell[1] = fields[26].GetUInt32();
			Temp.EffectTriggerSpell[2] = fields[27].GetUInt32();
			Temp.RecoveryTime = fields[28].GetInt32();
			uint32 procFlags = fields[29].GetUInt32();

			switch (procFlags)
			{
			case 1:Temp.procFlags = 20;					break;//物理
			case 2:Temp.procFlags = 320;				break;//远程
			case 4:Temp.procFlags = 81920;				break;//法术
			case 7:Temp.procFlags = 20 + 320 + 81920;	break;//所有
			case 3:Temp.procFlags = 20 + 320;			break;
			case 5:Temp.procFlags = 20 + 81920;			break;
			case 6:Temp.procFlags = 320 + 81920;		break;
			default:Temp.procFlags = 0;					break;
			}

			Temp.procChance = fields[30].GetFloat();
			Temp.procCharges = fields[31].GetInt32();
			Temp.SpellFamilyName = fields[32].GetUInt32();

			SpellModBaseMap.insert(std::make_pair(spellId, Temp));
		} while (result->NextRow());
	}

	SpellCusTargetMaskMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 技能ID,目标掩码 FROM _技能_目标限制" :
		"SELECT spellId,targetMask FROM _spellmod_target"))
	{
		do
		{
			Field* fields = result->Fetch();
			SpellCusTargetMaskMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetUInt32()));
		} while (result->NextRow());
	}

	OnRemoveSpellMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 技能ID, GM命令组 FROM _技能_当遗忘时" :
		"SELECT spellId, command FROM _spellmod_on_remove"))
	{
		do
		{
			Field* fields = result->Fetch();
			OnRemoveSpellMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetString()));
		} while (result->NextRow());
	}

	OnLearnSpellMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 技能ID, GM命令组 FROM _技能_当学习时" :
		"SELECT spellId, command FROM _spellmod_on_learn"))
	{
		do
		{
			Field* fields = result->Fetch();
			OnLearnSpellMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetString()));
		} while (result->NextRow());
	}

	AuraTriggerSpellMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 光环ID,攻强百分比增加基础值1,攻强百分比增加基础值2,攻强百分比增加基础值3,法伤百分比增加基础值1,法伤百分比增加基础值2,法伤百分比增加基础值3,链接技能组,触发几率,触发技能组,冷却时间,忽略目标类型,目标最大数量,目标范围,触发类型  FROM _技能_光环触发" :
		"SELECT aura,bp0ApPct,bp1ApPct,bp2ApPct,bp0SpPct,bp1SpPct,bp2SpPct,linkSpellData,procChance,triggerSpell,cooldown,ignoreMask,MaxTargets,TargetsRange,procFlags  FROM _spellmod_aura_trigger"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 auraId = fields[0].GetUInt32();
			AuraTriggerSpellTemplate Temp;
			Temp.bp0ApPct = fields[1].GetFloat();
			Temp.bp1ApPct = fields[2].GetFloat();
			Temp.bp2ApPct = fields[3].GetFloat();
			Temp.bp0SpPct = fields[4].GetFloat();
			Temp.bp1SpPct = fields[5].GetFloat();
			Temp.bp2SpPct = fields[6].GetFloat();
			Tokenizer tokens(fields[7].GetString(), ' ');
			for (Tokenizer::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
				Temp.linkSpellData.push_back(uint32(atol(*itr)));
			Temp.procChance = fields[8].GetUInt32();
			//Temp.triggerSpell = fields[9].GetUInt32();
			Tokenizer tokens1(fields[9].GetString(), ' ');
			for (Tokenizer::const_iterator itr = tokens1.begin(); itr != tokens1.end(); ++itr)
				Temp.triggerSpellData.push_back(uint32(atol(*itr)));
			Temp.cooldown = fields[10].GetUInt32();
			Temp.ignoreMask = fields[11].GetUInt32();
			Temp.Targets = fields[12].GetUInt32();
			Temp.TargetRange = fields[13].GetFloat();
			Temp.procFlags = fields[14].GetUInt32();
			AuraTriggerSpellMap.insert(std::make_pair(auraId, Temp));
		} while (result->NextRow());
	}

	SpellLeechVec.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 吸血技能ID,吸血几率,吸血类型,基础值,吸血是否附带伤害,需要满足的光环ID FROM _技能_吸血技能" :
		"SELECT spell,chance,type,basepoints,addDamage,meetAura FROM _spellmod_leech_spell"))
	{
		do
		{
			Field* fields = result->Fetch();
			SpellLeechTemplate Temp;
			Temp.spellid = fields[0].GetUInt32();
			Temp.chance = fields[1].GetFloat();

			const char* str = fields[2].GetCString();

			if (strcmp(str, "自身当前生命值百分比") == 0)
				Temp.type = LEECH_TYPE_SELF_CUR_PCT;
			else if (strcmp(str, "自身最大生命值百分比") == 0)
				Temp.type = LEECH_TYPE_SELF_MAX_PCT;
			else if (strcmp(str, "目标当前生命值百分比") == 0)
				Temp.type = LEECH_TYPE_TARGET_CUR_PCT;
			else if (strcmp(str, "目标最大生命值百分比") == 0)
				Temp.type = LEECH_TYPE_TARGET_MAX_PCT;
			else if (strcmp(str, "固定数值") == 0)
				Temp.type = LEECH_TYPE_STATIC;
			else if (strcmp(str, "技能伤害百分比") == 0)
				Temp.type = LEECH_TYPE_DAMGE_PCT;
			else
				Temp.type = LEECH_TYPE_NONE;

			Temp.basepoints = fields[3].GetFloat();
			Temp.addDmg = fields[4].GetBool();
			Temp.meetAura = fields[5].GetUInt32();
			SpellLeechVec.push_back(Temp);
		} while (result->NextRow());
	}

	AuraLeechMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 吸血光环技能ID,吸血几率,吸血类型,基础值,吸血是否附带伤害 FROM _技能_吸血光环" :
		"SELECT aura,chance,type,basepoints,addDamage FROM _spellmod_leech_aura"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 aura = fields[0].GetUInt32();
			AuraLeechTemplate Temp;
			Temp.chance = fields[1].GetFloat();

			const char* str = fields[2].GetCString();

			if (strcmp(str, "自身当前生命值百分比") == 0)
				Temp.type = LEECH_TYPE_SELF_CUR_PCT;
			else if (strcmp(str, "自身最大生命值百分比") == 0)
				Temp.type = LEECH_TYPE_SELF_MAX_PCT;
			else if (strcmp(str, "目标当前生命值百分比") == 0)
				Temp.type = LEECH_TYPE_TARGET_CUR_PCT;
			else if (strcmp(str, "目标最大生命值百分比") == 0)
				Temp.type = LEECH_TYPE_TARGET_MAX_PCT;
			else if (strcmp(str, "固定数值") == 0)
				Temp.type = LEECH_TYPE_STATIC;
			else if (strcmp(str, "技能伤害百分比") == 0)
				Temp.type = LEECH_TYPE_DAMGE_PCT;
			else
				Temp.type = LEECH_TYPE_NONE;

			Temp.basepoints = fields[3].GetFloat();
			Temp.addDmg = fields[4].GetBool();
			AuraLeechMap.insert(std::make_pair(aura, Temp));
		} while (result->NextRow());
	}

	AuraModClassSpellMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT aura, spellfamily,type1,op1,value1,"
		"type2,op2,value2,type3,op3,value3 FROM _技能_职业全局光环" :
		"SELECT aura, spellfamily,type1,op1,value1,"
		"type2,op2,value2,type3,op3,value3 FROM _spellmod_aura_on_classmask"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 aura = fields[0].GetUInt32();
			AuraModClassSpellTemplate Temp;
			Temp.SpellFamilyName = SpellFamilyNames(fields[1].GetUInt32());

			for (size_t i = 0; i < MAX_SPELL_EFFECTS; i++)
			{
				Temp.Type[i] = SpellModType(fields[2 + i * 3].GetUInt32());
				Temp.Op[i] = SpellModOp(fields[3 + i * 3].GetUInt32());
				Temp.Value[i] = fields[4 + i * 3].GetInt32();
			}
			AuraModClassSpellMap.insert(std::make_pair(aura, Temp));
		} while (result->NextRow());
	}

	AuraModStatMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 光环技能ID,光环类型1,属性类型1,转换百分比1,光环类型2,属性类型2,转换百分比2,光环类型3,属性类型3,转换百分比3 FROM _技能_主属性转换" :
		"SELECT aura,auraType1,statType1,pct1,auraType2,statType2,pct2,auraType3,statType3,pct3 FROM _spellmod_aura_pct_on_stat"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 aura = fields[0].GetUInt32();
			AuraModStatTemplate Temp;

			AuraType auraType = SPELL_AURA_NONE;
			int32 misc = 0;
			int32 miscB = 0;

			GetParams_PctOnStat(fields[1].GetCString(), fields[2].GetCString(), auraType, misc, miscB);
			Temp.auraType[0] = auraType;
			Temp.misc[0] = misc;
			Temp.miscB[0] = miscB;
			Temp.basePoints[0] = fields[3].GetInt32();

			auraType = SPELL_AURA_NONE;
			misc = 0;
			miscB = 0;
			GetParams_PctOnStat(fields[4].GetCString(), fields[5].GetCString(), auraType, misc, miscB);
			Temp.auraType[1] = auraType;
			Temp.misc[1] = misc;
			Temp.miscB[1] = miscB;
			Temp.basePoints[1] = fields[6].GetInt32();

			auraType = SPELL_AURA_NONE;
			misc = 0;
			miscB = 0;
			GetParams_PctOnStat(fields[7].GetCString(), fields[8].GetCString(), auraType, misc, miscB);
			Temp.auraType[2] = auraType;
			Temp.misc[2] = misc;
			Temp.miscB[2] = miscB;
			Temp.basePoints[2] = fields[9].GetInt32();

			AuraModStatMap.insert(std::make_pair(aura, Temp));
		} while (result->NextRow());
	}

	AuraPctMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 光环技能ID,效果类型1,提升类型1,提升数值1,效果类型2,提升类型2,提升数值2,效果类型3,提升类型3,提升数值3 FROM _技能_光环百分比效果" :
		"SELECT aura,auraType1,modType1,value1,auraType2,modType2,value2,auraType3,modType3,value3 FROM _spellmod_aura_pct"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 aura = fields[0].GetUInt32();
			AuraPctTemplate Temp;

			AuraType auraType = SPELL_AURA_NONE;
			int32 misc = 0;
			int32 miscB = 0;
			GetParams_Pct(fields[1].GetCString(), fields[2].GetCString(), auraType, misc, miscB);
			Temp.auraType[0] = auraType;
			Temp.misc[0] = misc;
			Temp.miscB[0] = miscB;
			Temp.basePoints[0] = fields[3].GetInt32();

			auraType = SPELL_AURA_NONE;
			misc = 0;
			miscB = 0;
			GetParams_Pct(fields[4].GetCString(), fields[5].GetCString(), auraType, misc, miscB);
			Temp.auraType[1] = auraType;
			Temp.misc[1] = misc;
			Temp.miscB[1] = miscB;
			Temp.basePoints[1] = fields[6].GetInt32();

			auraType = SPELL_AURA_NONE;
			misc = 0;
			miscB = 0;
			GetParams_Pct(fields[7].GetCString(), fields[8].GetCString(), auraType, misc, miscB);
			Temp.auraType[2] = auraType;
			Temp.misc[2] = misc;
			Temp.miscB[2] = miscB;
			Temp.basePoints[2] = fields[9].GetInt32();

			AuraPctMap.insert(std::make_pair(aura, Temp));
		} while (result->NextRow());
	}

	MountSpellMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 主坐骑技能ID,60速度坐骑技能ID,100速度坐骑技能ID,150速度坐骑技能ID,280速度坐骑技能ID,310速度坐骑技能ID FROM _技能_骑术改变速度" :
		"SELECT spellId,spell_60,spell_100,spell_150,spell_280,spell_310 FROM _spellmod_mount"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 spellId = fields[0].GetUInt32();
			MountSpellTemplate Temp;
			Temp.mount60 = fields[1].GetUInt32();
			Temp.mount100 = fields[2].GetUInt32();
			Temp.mount150 = fields[3].GetUInt32();
			Temp.mount280 = fields[4].GetUInt32();
			Temp.mount310 = fields[5].GetUInt32();
			MountSpellMap.insert(std::make_pair(spellId, Temp));
		} while (result->NextRow());
	}

	AuraStackTriggerMap.clear();
	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?	
		"SELECT 光环技能ID, 叠加层数, 触发技能组, 触发技能后移除层数  FROM _技能_光环叠加" :
		"SELECT AuraId, Stacks, TriggerSpellData, RemoveStacks  FROM _spellmod_aura_stack");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 Aura = fields[0].GetUInt32();
			AuraStackTriggerTemplate Temp;
			Temp.Stacks = fields[1].GetUInt32();
			Tokenizer tokens(fields[2].GetString(), ' ');
			for (Tokenizer::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
				Temp.TriggerSpellVec.push_back(uint32(atol(*itr)));
			Temp.RemoveStacks = fields[3].GetUInt32();
			AuraStackTriggerMap.insert(std::make_pair(Aura, Temp));
		} while (result->NextRow());
	}

	AuraTriggerOnDeathMap.clear();
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 光环技能ID, 冷却时间, 对攻击者施放技能ID, 对攻击者施放技能几率, 对自身施放技能ID,对自身施放技能几率,是否阻止死亡  FROM _技能_光环死亡触发" :
		"SELECT AuraId,CoolDown,AttackerTriggerSpell,AttackerTriggerChance,SelfTriggerSpell,SelfTriggerChance,PreventLastDamage FROM _spellmod_aura_death_trigger"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 AuraId = fields[0].GetUInt32();
			AuraTriggerOnDeathTemplate Temp;
			Temp.CoolDown = fields[1].GetUInt32();
			Temp.AttackerTriggerSpell = fields[2].GetUInt32();
			Temp.AttackerTriggerChance = fields[3].GetFloat();
			Temp.SelfTriggerSpell = fields[4].GetUInt32();
			Temp.SelfTriggerChance = fields[5].GetFloat();
			Temp.PreventLastDamage = fields[6].GetBool();
			AuraTriggerOnDeathMap.insert(std::make_pair(AuraId, Temp));
		} while (result->NextRow());
	}
}

//void SpellMod::LoadAuraEffectSpell()
//{
//	
//
//	/*
//	AuraModSpellVec.clear();
//
//	result = WorldDatabase.PQuery("SELECT aura,spellfamily, spell,type1,op1,value1,"
//	"type2,op2,value2,type3,op3,value3 FROM _spellmod_aura_on_spell");
//
//	if (result)
//	{
//	do
//	{
//	Field* fields = result->Fetch();
//	AuraModSpellTemplate Temp;
//	Temp.AuraId = fields[0].GetUInt32();
//	Temp.SpellFamilyName = SpellFamilyNames(fields[1].GetUInt32());
//	std::string str = fields[2].GetString();
//	if (!str.empty())
//	{
//	Tokenizer tokens(str, ',');
//	for (Tokenizer::const_iterator iter = tokens.begin(); iter != tokens.end(); ++iter)
//	Temp.SpellVec.push_back(uint32(atol(*iter)));
//	}
//
//	for (size_t i = 0; i < MAX_SPELL_EFFECTS; i++)
//	{
//	Temp.Type[i] = SpellModType(fields[3 + i * 3].GetUInt32());
//	Temp.Op[i] = SpellModOp(fields[4 + i * 3].GetUInt32());
//	Temp.Value[i] = fields[5 + i * 3].GetInt32();
//	}
//	AuraModSpellVec.push_back(Temp);
//	} while (result->NextRow());
//	}
//	*/
//	
//
//}

void SpellMod::GetParams_PctOnStat(const char* strAuraType, const char* strStatType, AuraType &auraType, int32 &misc, int32 &miscB)
{
	int32 i = -1;

	if (strcmp(strStatType, "精神") == 0)
		i = STAT_SPIRIT;
	else if (strcmp(strStatType, "力量") == 0)
		i = STAT_STRENGTH;
	else if (strcmp(strStatType, "敏捷") == 0)
		i = STAT_AGILITY;
	else if (strcmp(strStatType, "耐力") == 0)
		i = STAT_STAMINA;
	else if (strcmp(strStatType, "智力") == 0)
		i = STAT_INTELLECT;

	if (strcmp(strAuraType, "法术强度") == 0)
	{
		auraType = SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT;
		misc = SPELL_SCHOOL_MASK_MAGIC;
		miscB = i;
	}

	if (strcmp(strAuraType, "近战攻击强度") == 0)
	{
		auraType = SPELL_AURA_MOD_ATTACK_POWER_OF_STAT_PERCENT;
		misc = i;
	}

	if (strcmp(strAuraType, "远程攻击强度") == 0)
	{
		auraType = SPELL_AURA_MOD_RANGED_ATTACK_POWER_OF_STAT_PERCENT;
		misc = i;
	}

	if (strcmp(strAuraType, "治疗效果") == 0)
	{
		auraType = SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT;
		misc = i;
	}
}

void SpellMod::GetParams_Pct(const char* strAuraType, const char* strModType, AuraType &auraType, int32 &misc, int32 &miscB)
{
	if (strcmp(strAuraType, "施法速度") == 0)
		auraType = SPELL_AURA_MOD_CASTING_SPEED_NOT_STACK;
	if (strcmp(strAuraType, "近战攻击速度") == 0)
		auraType = SPELL_AURA_MOD_MELEE_HASTE;
	if (strcmp(strAuraType, "远程攻击速度") == 0)
		auraType = SPELL_AURA_MOD_RANGED_HASTE;

	if (strcmp(strAuraType, "造成暴击伤害") == 0)
	{
		auraType = SPELL_AURA_MOD_CRIT_DAMAGE_BONUS;
		misc = SPELL_SCHOOL_MASK_ALL;
	}

	bool pct = strcmp(strModType, "百分比") == 0;

	if (strcmp(strAuraType, "生命值") == 0)
		pct ? auraType = SPELL_AURA_MOD_INCREASE_HEALTH_PERCENT : auraType = SPELL_AURA_MOD_INCREASE_HEALTH;

	if (strcmp(strAuraType, "全属性") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_PERCENT_STAT : auraType = SPELL_AURA_MOD_STAT;
		misc = -1;
	}

	if (strcmp(strAuraType, "耐力") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_PERCENT_STAT : auraType = SPELL_AURA_MOD_STAT;
		misc = STAT_STAMINA;
	}

	if (strcmp(strAuraType, "敏捷") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_PERCENT_STAT : auraType = SPELL_AURA_MOD_STAT;
		misc = STAT_AGILITY;
	}

	if (strcmp(strAuraType, "力量") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_PERCENT_STAT : auraType = SPELL_AURA_MOD_STAT;
		misc = STAT_STRENGTH;
	}

	if (strcmp(strAuraType, "智力") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_PERCENT_STAT : auraType = SPELL_AURA_MOD_STAT;
		misc = STAT_INTELLECT;
	}

	if (strcmp(strAuraType, "精神") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_PERCENT_STAT : auraType = SPELL_AURA_MOD_STAT;
		misc = STAT_STRENGTH;
	}

	if (strcmp(strAuraType, "近战攻击强度") == 0)
		pct ? auraType = SPELL_AURA_MOD_ATTACK_POWER_PCT : auraType = SPELL_AURA_MOD_ATTACK_POWER;

	if (strcmp(strAuraType, "远程攻击强度") == 0)
		pct ? auraType = SPELL_AURA_MOD_RANGED_ATTACK_POWER_PCT : auraType = SPELL_AURA_MOD_RANGED_ATTACK_POWER;

	if (strcmp(strAuraType, "法术强度") == 0)
	{
		if (pct)
		{
			auraType = SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT;
			miscB = -1000;
		}
		else
			auraType = SPELL_AURA_MOD_DAMAGE_DONE;

		misc = SPELL_SCHOOL_MASK_MAGIC;
	}

	if (strcmp(strAuraType, "奥术法术强度") == 0)
	{
		if (pct)
		{
			auraType = SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT;
			miscB = -1000;
		}
		else
			auraType = SPELL_AURA_MOD_DAMAGE_DONE;

		misc = SPELL_SCHOOL_MASK_ARCANE;
	}

	if (strcmp(strAuraType, "火焰法术强度") == 0)
	{
		if (pct)
		{
			auraType = SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT;
			miscB = -1000;
		}
		else
			auraType = SPELL_AURA_MOD_DAMAGE_DONE;

		misc = SPELL_SCHOOL_MASK_FIRE;
	}

	if (strcmp(strAuraType, "冰霜法术强度") == 0)
	{
		if (pct)
		{
			auraType = SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT;
			miscB = -1000;
		}
		else
			auraType = SPELL_AURA_MOD_DAMAGE_DONE;

		misc = SPELL_SCHOOL_MASK_FROST;
	}

	if (strcmp(strAuraType, "神圣法术强度") == 0)
	{
		if (pct)
		{
			auraType = SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT;
			miscB = -1000;
		}
		else
			auraType = SPELL_AURA_MOD_DAMAGE_DONE;

		misc = SPELL_SCHOOL_MASK_HOLY;
	}

	if (strcmp(strAuraType, "暗影法术强度") == 0)
	{
		if (pct)
		{
			auraType = SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT;
			miscB = -1000;
		}
		else
			auraType = SPELL_AURA_MOD_DAMAGE_DONE;

		misc = SPELL_SCHOOL_MASK_SHADOW;
	}

	if (strcmp(strAuraType, "自然法术强度") == 0)
	{
		if (pct)
		{
			auraType = SPELL_AURA_MOD_SPELL_DAMAGE_OF_STAT_PERCENT;
			miscB = -1000;
		}
		else
			auraType = SPELL_AURA_MOD_DAMAGE_DONE;

		misc = SPELL_SCHOOL_MASK_NATURE;
	}

	if (strcmp(strAuraType, "治疗效果") == 0)
	{
		if (pct)
		{
			auraType = SPELL_AURA_MOD_SPELL_HEALING_OF_STAT_PERCENT;
			miscB = -1000;
		}
		else
			auraType = SPELL_AURA_MOD_HEALING;

		misc = SPELL_SCHOOL_MASK_MAGIC;
	}

	if (strcmp(strAuraType, "造成所有伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_ALL;
	}

	if (strcmp(strAuraType, "造成物理伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_NORMAL;
	}

	if (strcmp(strAuraType, "造成法术伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_MAGIC;
	}

	if (strcmp(strAuraType, "造成奥术伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_ARCANE;
	}

	if (strcmp(strAuraType, "造成火焰伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_FIRE;
	}
	if (strcmp(strAuraType, "造成冰霜伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_FROST;
	}
	if (strcmp(strAuraType, "造成神圣伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_HOLY;
	}
	if (strcmp(strAuraType, "造成暗影伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_SHADOW;
	}
	if (strcmp(strAuraType, "造成自然伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_DONE : auraType = SPELL_AURA_MOD_DAMAGE_DONE;
		misc = SPELL_SCHOOL_MASK_NATURE;
	}

	if (strcmp(strAuraType, "受到所有伤害") == 0)
	{
		pct ? auraType = SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN : auraType = SPELL_AURA_MOD_DAMAGE_TAKEN;
		misc = SPELL_SCHOOL_MASK_ALL;
	}
}

uint32 SpellMod::GetReqId(uint32 spellId)
{
	std::unordered_map<uint32, SpellModTemplate>::iterator iter = SpellModMap.find(spellId);

	if (iter != SpellModMap.end())
		return iter->second.reqId;

	return 0;
}
float SpellMod::GetDmgMod(uint32 spellId)
{
	std::unordered_map<uint32, SpellModTemplate>::iterator iter = SpellModMap.find(spellId);

	if (iter != SpellModMap.end())
		return iter->second.dmgMod;

	return 1.0f;
}
float SpellMod::GetHealMod(uint32 spellId)
{
	std::unordered_map<uint32, SpellModTemplate>::iterator iter = SpellModMap.find(spellId);

	if (iter != SpellModMap.end())
		return iter->second.healMod == 0 ? 1.0f : iter->second.healMod;

	return 1.0f;
}
bool SpellMod::AccontBind(uint32 spellId)
{
	std::unordered_map<uint32, SpellModTemplate>::iterator iter = SpellModMap.find(spellId);

	if (iter != SpellModMap.end())
		return iter->second.accountBind;

	return false;
}

bool SpellMod::Enable(Player* player, uint32 spellId)
{
	std::unordered_map<uint32, SpellModTemplate>::iterator iter = SpellModMap.find(spellId);

	if (iter != SpellModMap.end())
	{
		if (iter->second.disable)
			return false;

		uint32 reqId = GetReqId(spellId);

		if (sReq->Check(player, reqId))
		{
			sReq->Des(player, reqId);
			return true;
		}
		else
			return false;
	}

	return true;
}

void SpellMod::LearnAccountSpell(Player* player)
{
	uint32 AccountId = player->GetSession()->GetAccountId();

	QueryResult result = LoginDatabase.PQuery("SELECT spell FROM account_spells WHERE account = %u", AccountId);

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 spellId = fields[0].GetUInt32();

			if (!player->HasSpell(spellId))
				player->learnSpell(spellId);

		} while (result->NextRow());
	}
}

void SpellMod::InsertAccountSpell(Player* player)
{
	uint32 accountId = player->GetSession()->GetAccountId();

	for (std::unordered_map<uint32, SpellModTemplate>::iterator iter = SpellModMap.begin(); iter != SpellModMap.end(); ++iter)
	{
		if (!iter->second.accountBind)
			continue;

		uint32 spellId = iter->first;

		if (!player->HasSpell(spellId))
			continue;

		bool exsist = false;

		for (std::vector<AccountSpellTemplate>::iterator ii = AccountSpellVec.begin(); ii != AccountSpellVec.end(); ii++)
		{
			if (accountId != ii->accountId)
				continue;

			if (spellId == ii->spellId)
				exsist = true;
		}

		if (!exsist)
		{
			AccountSpellTemplate Temp;
			Temp.accountId = accountId;
			Temp.spellId = spellId;
			AccountSpellVec.push_back(Temp);

			PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_ACCOUNT_SPELL);
			stmt->setUInt32(0, player->GetSession()->GetAccountId());
			stmt->setInt32(1, spellId);
			LoginDatabase.Execute(stmt);
		}
	}
}

bool SpellMod::Ignore(uint32 ignoreMask, Unit* target)
{
	if (!target)
		return true;

	bool ignore = false;

	if (target->GetMap()->IsDungeon())
		if ((ignoreMask & AURA_TRIGGER_IGNORE_DUNGEON) == AURA_TRIGGER_IGNORE_DUNGEON)
			ignore = true;

	if (!target->GetMap()->IsDungeon())
		if ((ignoreMask & AURA_TRIGGER_IGNORE_NO_DUNGEON) == AURA_TRIGGER_IGNORE_NO_DUNGEON)
			ignore = true;

	if (Creature* creature = target->ToCreature())
		if (creature->IsDungeonBoss() || creature->isWorldBoss())
			if ((ignoreMask & AURA_TRIGGER_IGNORE_BOSS) == AURA_TRIGGER_IGNORE_BOSS)
				ignore = true;

	if (target->GetTypeId() == TYPEID_PLAYER)
		if ((ignoreMask & AURA_TRIGGER_IGNORE_PLAYER) == AURA_TRIGGER_IGNORE_PLAYER)
			ignore = true;

	if (target->GetTypeId() == TYPEID_UNIT)
		if ((ignoreMask & AURA_TRIGGER_IGNORE_UNIT) == AURA_TRIGGER_IGNORE_UNIT)
			ignore = true;

	return ignore;
}

void GetEnemyList(std::list<Unit*> &EnemyList, Unit* caster, float range)
{
	acore::AnyAoETargetUnitInObjectRangeCheck u_check(caster, caster, range);
    acore::UnitListSearcher<acore::AnyAoETargetUnitInObjectRangeCheck> searcher(caster, EnemyList, u_check);
	caster->VisitNearbyObject(range, searcher);
}

void GetFriendList(std::list<Unit*> &FriendList, Unit* caster, float range)
{
    acore::AnyFriendlyUnitInObjectRangeCheck u_check(caster, caster, range, true);
    acore::UnitListSearcher<acore::AnyFriendlyUnitInObjectRangeCheck> searcher(caster, FriendList, u_check);
	caster->VisitNearbyObject(range, searcher);
}

struct UnitSortTemplate
{
	Unit* unit;
	float distance;
};

bool UnitSort(UnitSortTemplate a, UnitSortTemplate b)
{
	return a.distance < b.distance;
}

void GetNearestUnitVec(std::vector<Unit*> &UnitVec, std::list<Unit*> UnitList, Unit* caster, uint32 MaxTarget)
{
	std::vector<UnitSortTemplate> UnitSortVec;

	for (std::list<Unit*>::iterator itr = UnitList.begin(); itr != UnitList.end(); ++itr)
	{
		UnitSortTemplate Temp;
		Temp.unit = *itr;
		Temp.distance = (*itr)->GetDistance(caster);
		UnitSortVec.push_back(Temp);
	}

	std::sort(UnitSortVec.begin(), UnitSortVec.end(), UnitSort);

	for (size_t i = 0; i < UnitSortVec.size(); i++)
	{
		if (i < MaxTarget)
			UnitVec.push_back(UnitSortVec[i].unit);
	}
}

bool SpellMod::AuraTrigger(Unit* caster, Unit* victim, SpellInfo const* auraSpellInfo, SpellInfo const* procSpell, uint32 procFlags, Unit*  &target, AuraEffect* triggeredByAura)
{
	std::unordered_map<uint32, SpellModBaseTemplate>::iterator itr = SpellModBaseMap.find(auraSpellInfo->Id);

	if (itr != SpellModBaseMap.end())
	{
		for (size_t i = 0; i < MAX_SPELL_EFFECTS; i++)
		{
			if (const SpellInfo *triggerSpellInfo = sSpellMgr->GetSpellInfo(itr->second.EffectTriggerSpell[i]))
			{
				if (itr->second.procFlags != 0)
					continue;

				if (!roll_chance_f(itr->second.procChance))
					continue;

				if (procSpell->Effects[i].SpellClassMask != auraSpellInfo->Effects[i].SpellClassMask)
					continue;

				if (target == NULL)
					target = !(procFlags & (PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS)) && triggerSpellInfo && triggerSpellInfo->IsPositive() ? caster : victim;

				caster->CastSpell(target, triggerSpellInfo, true, NULL, triggeredByAura, NULL);
			}
		}
	}

	std::unordered_map<uint32, AuraTriggerSpellTemplate>::iterator iter = AuraTriggerSpellMap.find(auraSpellInfo->Id);

	if (iter != AuraTriggerSpellMap.end())
	{
		uint32 triggerSpell = 0;//iter->second.triggerSpell;

		if (!iter->second.triggerSpellData.empty())
			triggerSpell = iter->second.triggerSpellData[urand(0, iter->second.triggerSpellData.size() - 1)];

		if (triggerSpell == 0)
			triggerSpell = procSpell->Id;

		const SpellInfo *triggerSpellInfo = sSpellMgr->GetSpellInfo(triggerSpell);

		if (!triggerSpellInfo)
			return true;

		if (target == NULL)
			target = !(procFlags & (PROC_FLAG_DONE_SPELL_MAGIC_DMG_CLASS_POS | PROC_FLAG_DONE_SPELL_NONE_DMG_CLASS_POS)) && triggerSpellInfo && triggerSpellInfo->IsPositive() ? caster : victim;


		if (Ignore(iter->second.ignoreMask, target))
			return true;

		if (caster->HasSpellCooldown(triggerSpell))
		{
			if (Player* player = caster->ToPlayer())
			{
				if (player->GetSpellCooldownDelay(triggerSpell) <= iter->second.cooldown)
					return true;
			}
			else
				return true;
		}

		bool canTrigger = false;

		if (iter->second.linkSpellData.empty() || procSpell && std::find(iter->second.linkSpellData.begin(), iter->second.linkSpellData.end(), procSpell->Id) != iter->second.linkSpellData.end())
			canTrigger = true;

		if (!canTrigger || !roll_chance_f(iter->second.procChance))
			return true;

		float ap = std::max(caster->GetTotalAttackPowerValue(BASE_ATTACK), caster->GetTotalAttackPowerValue(RANGED_ATTACK));
		float sp = caster->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_MAGIC);

		int bp0 = triggerSpellInfo->Effects[EFFECT_0].BasePoints + int(ap * iter->second.bp0ApPct / 100 + sp * iter->second.bp0SpPct / 100);
		int bp1 = triggerSpellInfo->Effects[EFFECT_1].BasePoints + int(ap * iter->second.bp1ApPct / 100 + sp * iter->second.bp1SpPct / 100);
		int bp2 = triggerSpellInfo->Effects[EFFECT_2].BasePoints + int(ap * iter->second.bp2ApPct / 100 + sp * iter->second.bp2SpPct / 100);

		//最大目标个数为1时
		if (iter->second.Targets == 1)
		{
			caster->CastCustomSpell(target, triggerSpell, &bp0, &bp1, &bp2, true, NULL, triggeredByAura, NULL);
			caster->AddSpellCooldown(triggerSpell, 0, iter->second.cooldown);
			return true;
		}

		//最大目标个数大于1时 选择距离施法者最近的作为目标
		std::list<Unit*> UnitList;

		if (target == victim)
		{
			GetEnemyList(UnitList, caster, iter->second.TargetRange);
			if (UnitList.empty())
				return true;

			std::vector<Unit*> UnitVec;
			GetNearestUnitVec(UnitVec, UnitList, caster, iter->second.Targets);

			if (UnitVec.empty())
				return true;

			for (std::vector<Unit*>::iterator itr = UnitVec.begin(); itr != UnitVec.end(); ++itr)
				caster->CastCustomSpell((*itr), triggerSpell, &bp0, &bp1, &bp2, true, NULL, triggeredByAura, NULL);

			caster->AddSpellCooldown(triggerSpell, 0, iter->second.cooldown);
		}
		else if (target == caster)
		{
			GetFriendList(UnitList, caster, iter->second.TargetRange);

			if (UnitList.empty())
				return true;

			std::vector<Unit*> UnitVec;
			GetNearestUnitVec(UnitVec, UnitList, caster, iter->second.Targets);

			if (UnitVec.empty())
				return true;

			for (std::vector<Unit*>::iterator itr = UnitVec.begin(); itr != UnitVec.end(); ++itr)
				(*itr)->CastCustomSpell((*itr), triggerSpell, &bp0, &bp1, &bp2, true, NULL, triggeredByAura, NULL);

			caster->AddSpellCooldown(triggerSpell, 0, iter->second.cooldown);
		}
		return true;
	}

	return false;
}


void SpellMod::HealOnDamage(Unit* caster, Unit* target, SpellInfo const* spellInfo, uint32 &damage)
{
	//吸血技能
	for (auto iter = SpellLeechVec.begin(); iter != SpellLeechVec.end(); iter++)
	{
		if (iter->spellid == spellInfo->Id)
		{
			if (roll_chance_f(iter->chance))
			{
				if (iter->meetAura == 0 || caster->HasAura(iter->meetAura))
				{
					uint32 amount = 0;

					switch (iter->type)
					{
					case LEECH_TYPE_SELF_CUR_PCT:
						amount = caster->GetHealth() * iter->basepoints / 100;
						break;
					case LEECH_TYPE_SELF_MAX_PCT:
						amount = caster->GetMaxHealth() * iter->basepoints / 100;
						break;
					case LEECH_TYPE_TARGET_CUR_PCT:
						amount = target->GetMaxHealth() * iter->basepoints / 100;
						break;
					case LEECH_TYPE_TARGET_MAX_PCT:
						amount = target->GetMaxHealth() * iter->basepoints / 100;
						break;
					case LEECH_TYPE_STATIC:
						amount = iter->basepoints;
						break;
					case LEECH_TYPE_DAMGE_PCT:
						amount = damage * iter->basepoints / 100;
						break;
					}

					if (amount != 0)
					{
						if (iter->addDmg)
							damage += amount;
						if (caster->IsAlive())
							caster->HealBySpell(caster, spellInfo, amount);
					}
				}
			}
		}
	}

	//吸血光环
	for (std::unordered_map<uint32, AuraLeechTemplate>::iterator it = AuraLeechMap.begin(); it != AuraLeechMap.end(); it++)
	{
		if (roll_chance_f(it->second.chance))
		{
			if (caster->HasAura(it->first))
			{
				uint32 amount = 0;

				switch (it->second.type)
				{
				case LEECH_TYPE_SELF_CUR_PCT:
					amount = caster->GetHealth() * it->second.basepoints / 100;
					break;
				case LEECH_TYPE_SELF_MAX_PCT:
					amount = caster->GetHealth() * it->second.basepoints / 100;
					break;
				case LEECH_TYPE_TARGET_CUR_PCT:
					amount = target->GetMaxHealth() * it->second.basepoints / 100;
					break;
				case LEECH_TYPE_TARGET_MAX_PCT:
					amount = target->GetMaxHealth() * it->second.basepoints / 100;
					break;
				case LEECH_TYPE_STATIC:
					amount = it->second.basepoints;
					break;
				case LEECH_TYPE_DAMGE_PCT:
					amount = damage * it->second.basepoints / 100;
					break;
				}

				if (amount != 0)
				{
					if (it->second.addDmg)
						damage += amount;
					if (caster->IsAlive())
						caster->HealBySpell(caster, spellInfo, amount);
				}
			}
		}
	}
}

class SpellModPlayerScripts : PlayerScript
{
public:
	SpellModPlayerScripts() : PlayerScript("SpellModPlayerScripts") {}

	void OnUpdateZone(Player* player, uint32 newZone, uint32 /*newArea*/) override
	{
		for (std::unordered_map<uint32, SpellModTemplate>::iterator iter = SpellModMap.begin(); iter != SpellModMap.end(); ++iter)
			if (player->HasAura(iter->first))
				if (!sSpellMod->Enable(player, iter->first))
					player->RemoveAura(iter->first);
	}

	void OnLogin(Player* player) override
	{
		sSpellMod->LearnAccountSpell(player);
	}

	void OnLogout(Player* player) override
	{
		sSpellMod->InsertAccountSpell(player);
	}
};

class spell_gen_mount_custom : public SpellScriptLoader
{
public:
	spell_gen_mount_custom() : SpellScriptLoader("spell_gen_mount_custom") { }
	class spell_gen_mount_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_gen_mount_SpellScript);

	public:
		void HandleMount(SpellEffIndex effIndex)
		{
			PreventHitDefaultEffect(effIndex);

			if (Player* target = GetHitPlayer())
			{

				ChatHandler(target->GetSession()).PSendSysMessage("1");

				uint32 petNumber = target->GetTemporaryUnsummonedPetNumber();
				target->SetTemporaryUnsummonedPetNumber(0);

				// Prevent stacking of mounts and client crashes upon dismounting
				target->RemoveAurasByType(SPELL_AURA_MOUNTED, 0, GetHitAura());

				// Triggered spell id dependent on riding skill and zone
				bool canFly = false;
				uint32 map = GetVirtualMapForMapAndZone(target->GetMapId(), target->GetZoneId());
				if (map == 530 || (map == 571 && target->HasSpell(54197)))
					canFly = true;

				float x, y, z;
				target->GetPosition(x, y, z);
				uint32 areaFlag = target->GetBaseMap()->GetAreaId(x, y, z);
				AreaTableEntry const* area = sAreaTableStore.LookupEntry(areaFlag);
				// Xinef: add battlefield check
				Battlefield* Bf = sBattlefieldMgr->GetBattlefieldToZoneId(target->GetZoneId());
				if (!area || (canFly && ((area->flags & AREA_FLAG_NO_FLY_ZONE) || (Bf && !Bf->CanFlyIn()))))
					canFly = false;

				uint32 mount = 0;

				auto itr = MountSpellMap.find(GetSpellInfo()->Id);
				if (itr != MountSpellMap.end())
				{
					switch (target->GetBaseSkillValue(SKILL_RIDING))
					{
					case 75:
						mount = itr->second.mount60;
						break;
					case 150:
						mount = itr->second.mount100;
						break;
					case 225:
						if (canFly)
							mount = itr->second.mount150;
						else
							mount = itr->second.mount100;
						break;
					case 300:
						if (canFly)
						{
							if (itr->second.mount310 && target->Has310Flyer(false))
								mount = itr->second.mount60;
							else
								mount = itr->second.mount280;
						}
						else
							mount = itr->second.mount100;
						break;
					default:
						break;
					}
				}

				ChatHandler(target->GetSession()).PSendSysMessage("SKILL_RIDING %u", target->GetBaseSkillValue(SKILL_RIDING));
				ChatHandler(target->GetSession()).PSendSysMessage("mount %u", mount);

				if (mount)
				{
					PreventHitAura();
					target->CastSpell(target, mount, true);
				}

				if (petNumber)
					target->SetTemporaryUnsummonedPetNumber(petNumber);
			}
		}

		void Register()
		{
			OnEffectHitTarget += SpellEffectFn(spell_gen_mount_SpellScript::HandleMount, EFFECT_2, SPELL_EFFECT_SCRIPT_EFFECT);
		}

	private:
	};

	SpellScript* GetSpellScript() const
	{
		return new spell_gen_mount_SpellScript();
	}
};


void AddSC_SpellMod()
{
	new SpellModPlayerScripts();
	new spell_gen_mount_custom();
}

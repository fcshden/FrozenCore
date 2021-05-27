#pragma execution_character_set("utf-8")
#include "CreatureMod.h"
#include "../../Reward/Reward.h"
#include "../../CommonFunc/CommonFunc.h"
#include "../../String/myString.h"
#include "../../Talisman/Talisman.h"
#include "../../Rank/Rank.h"
#include "Group.h"

std::vector<CreautreModTemplate> CreautreModVec;

void CreatureMod::Load()
{
	CreautreModVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?

		//		0		1		2		3			4			5			6			7
		"SELECT 生物ID,等级,生命值,物理伤害值或倍率,法术伤害倍率,治疗效果倍率,减伤百分比,抗性值,"
		//8				9			10				11		12			13				14			15		16		17		18		19	
		"击杀奖励模板ID,击杀奖励几率,击杀是否全服提示,护甲值,攻击间隔,离开原位置重置距离,击杀奖励法宝值,掉落ID1,掉落ID2,掉落ID3,掉落ID4,掉落ID5,"
		//	20					21				22				23				24			25				26
		"队伍击杀奖励模板ID,队伍击杀奖励几率,击杀奖励自定义等级值,击杀召唤物体ID,副本挑战等级,是否加载原掉落,随机技能组模板ID,难度 FROM _属性调整_生物" :
		//		0		1		2	3			4			5		6			7
		"SELECT Entry,Level,Health,MeleeDmg,SpellDmgMod,HealMod,ReduceDmgPct,Resistance,"
		//8				9			10			11		12		13				14				15		16		17		18			19	
		"KillRewId,KillRewChance,KillAnnounce,Armor,AttackTime,ResetDistance,AddTalismanValue,LootId_1,LootId_2,LootId_3,LootId_4,LootId_5,"
		//	20					21				22			23					24		25			26
		"KillGroupRewId,KillGroupRewChance,AddRankValue,KillRewGameObject,ChallengeLv,SrcLoot,RandSpellGroupId FROM _attribute_creature");

	if (!result)
		return;

	do
	{
		Field* fields = result->Fetch();
		CreautreModTemplate Temp;
		Temp.Entry			= fields[0].GetUInt32();
		Temp.Level			= fields[1].GetUInt8();
		Temp.Health			= fields[2].GetUInt32();
		Temp.MeleeDmg		= fields[3].GetFloat();
		Temp.SpellDmgMod	= fields[4].GetFloat();
		Temp.HealMod		= fields[5].GetFloat();
		Temp.ReduceDmgPct	= fields[6].GetFloat();
		Temp.Resistance		= fields[7].GetInt32();
		Temp.KillRewId		= fields[8].GetUInt32();
		Temp.KillRewChance	= fields[9].GetFloat();
		Temp.KillAnnounce	= fields[10].GetBool();
		Temp.Armor			= fields[11].GetInt32();
		Temp.AttackTime		= fields[12].GetUInt32();
		Temp.ResetDistance	= fields[13].GetFloat();
		Temp.AddTalismanValue = fields[14].GetInt32();

		for (size_t i = 0; i < MAX_CUSTOM_LOOT_COUNT; i++)
			Temp.LootId[i] = fields[15 + i].GetUInt32();

		Temp.KillGroupRewId		= fields[20].GetUInt32();
		Temp.KillGroupRewChance = fields[21].GetFloat();
		Temp.AddRankValue		= fields[22].GetInt32();
		Temp.KillRewGameObject	= fields[23].GetInt32();
		Temp.ChallengeLv		= fields[24].GetUInt32();
		Temp.SrcLoot			= fields[25].GetBool();
		Temp.RandSpellGroupId	= fields[26].GetUInt32();
		Temp.Diff				= fields[27].GetUInt32();
		CreautreModVec.push_back(Temp);
	} while (result->NextRow());
}

void CreatureMod::SetMod(Creature* creature)
{
	if (creature->GetEntry() == 1964 || creature->IsGuardian() || creature->IsHunterPet() || creature->IsTotem())
		return;

	////小动物之类
	//if (creature->IsCivilian())
	//	return;

	uint32 Entry = creature->GetEntry();
	uint32 ChallengeLv = creature->GetMap()->challengeLv;
	uint32 Diff = creature->GetMap()->GetDifficulty();

	for (auto itr = CreautreModVec.begin(); itr != CreautreModVec.end(); itr++)
	{
		if (Entry == itr->Entry && ChallengeLv == itr->ChallengeLv && Diff == itr->Diff)
		{
			creature->C_Level = itr->Level;
			creature->C_Health = itr->Health;
			creature->C_HpMod = 1;
			creature->C_MeleeDmg = itr->MeleeDmg;
			creature->C_SpellDmgMod = itr->SpellDmgMod;
			creature->C_HealMod = itr->HealMod;
			creature->C_ReduceDmgPct = itr->ReduceDmgPct;
			creature->C_Resistance = itr->Resistance;
			creature->C_SrcLoot	= itr->SrcLoot;

			for (size_t i = 0; i < MAX_CUSTOM_LOOT_COUNT; i++)
				creature->C_LootId[i] = itr->LootId[i];

			creature->C_KillRewId = itr->KillRewId;
			creature->C_KillRewChance = itr->KillRewChance;
			creature->C_KillGroupRewId = itr->KillGroupRewId;
			creature->C_KillGroupRewChance = itr->KillGroupRewChance;
			creature->C_KillAnnounce = itr->KillAnnounce;
			creature->C_Armor = itr->Armor;
			creature->C_AttackTime = itr->AttackTime;
			creature->C_ResetDistance = itr->ResetDistance;
			creature->C_AddTalismanValue = itr->AddTalismanValue;
			creature->C_AddAddRankValue = itr->AddRankValue;
			creature->C_KillRewGameObject = itr->KillRewGameObject;
			creature->RandSpellGroupId = itr->RandSpellGroupId;
			break;
		}
	}
}

class CreatureKillRew : PlayerScript
{
public:
	CreatureKillRew() : PlayerScript("CreatureKillRew") {}

	void OnCreatureKill(Player* killer, Creature* killed) override
	{
		if (killed->C_KillRewGameObject != 0)
			killer->SummonGameObject(killed->C_KillRewGameObject, killed->GetPositionX(), killed->GetPositionY(), killed->GetPositionZ(), killed->GetOrientation(), 0, 0, 0, 0, 0);

		//击杀奖励
		if (killed->C_KillRewId != 0 && frand(0, 100) <= killed->C_KillRewChance)
			sRew->Rew(killer, killed->C_KillRewId);

		//队伍击杀奖励
		if (killed->C_KillGroupRewId != 0)
			if (Group* group = killer->GetGroup())
				for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
					if (Player* member = itr->GetSource())
						if (member->IsInWorld() && member->GetGUID() != killer->GetGUID() && frand(0, 100) <= killed->C_KillGroupRewChance && member->IsSelfOrInSameMap(killer) && member->GetDistance(killer) <= 200.0f)
							sRew->Rew(member, killed->C_KillGroupRewId);


        CreatureTemplate const* ccc = killed->GetCreatureTemplate();
        if (ccc->rank >= 2 && killed->GetMaxHealth() > 3000000)
        {
            const char*msg;

            if (killer->GetGroup())
                msg = sString->Format(sString->GetText(STR_GROUP_KILL_CREATURE), sCF->GetNameLink(killer).c_str(), killed->GetCreatureTemplate()->Name.c_str());
            else
                msg = sString->Format(sString->GetText(STR_KILL_CREATRE), sCF->GetNameLink(killer).c_str(), killed->GetCreatureTemplate()->Name.c_str());

            sWorld->SendScreenMessage(msg);
        }

		//击杀广播
		if (killed->C_KillAnnounce)
		{
			const char*msg;

			if (killer->GetGroup())
				msg = sString->Format(sString->GetText(CORE_STR_TYPES(STR_GROUP_KILL_CREATURE)), sCF->GetNameLink(killer).c_str(), killed->GetCreatureTemplate()->Name.c_str());
			else
				msg = sString->Format(sString->GetText(CORE_STR_TYPES(STR_KILL_CREATRE)), sCF->GetNameLink(killer).c_str(), killed->GetCreatureTemplate()->Name.c_str());

			sWorld->SendScreenMessage(msg);
		}

		//Rank值 正数-击杀者 负数-整个队伍
		if (killed->C_AddAddRankValue > 0)
			sRank->Update(killer, killed->C_AddAddRankValue, true);
		else if (killed->C_AddAddRankValue < 0)
		{
			if (Group* group = killer->GetGroup())
			{
				for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
					if (Player* member = itr->GetSource())
						if (member->IsInWorld() && member->GetDistance(killer) <= 200.0f)
							sRank->Update(member, abs(killed->C_AddAddRankValue), true);
			}
			else
				sRank->Update(killer, abs(killed->C_AddAddRankValue), true);
		}

		//法宝值 正数-击杀者 负数-整个队伍
		if (killed->C_AddTalismanValue > 0)
		{
			sTalisman->AddTalismanValue(killer, killed->C_AddTalismanValue, true);
			if (killed->IsDungeonBoss() || killed->IsDungeonBoss())
				sTalisman->SaveTalisManValue(killer);
		}		
		else if (killed->C_AddTalismanValue < 0)
		{
			if (Group* group = killer->GetGroup())
			{
				for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
					if (Player* member = itr->GetSource())
						if (member->IsInWorld() && member->GetDistance(killer) <= 200.0f)
						{
							sTalisman->AddTalismanValue(member, abs(killed->C_AddTalismanValue), true);

							if (killed->IsDungeonBoss() || killed->IsDungeonBoss())
								sTalisman->SaveTalisManValue(member);
						}
			}
			else
			{
				sTalisman->AddTalismanValue(killer, abs(killed->C_AddTalismanValue), true);

				if (killed->IsDungeonBoss() || killed->IsDungeonBoss())
					sTalisman->SaveTalisManValue(killer);
			}
		}
	}
};
void AddSC_CreatureKillRew()
{
	new CreatureKillRew();
}

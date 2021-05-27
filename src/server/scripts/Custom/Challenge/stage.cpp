#pragma execution_character_set("utf-8")
#include "../Switch/Switch.h"
#include "stage.h"

std::vector<StageTemplate> StageVec;

void Stage::Load()
{
	StageVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		//		0			1		2				3
		"SELECT 关卡ID,召唤生物组ID,通关召唤物体ID,菜单文本 FROM _副本_闯关模式" :
		//			0		1				2		3
		"SELECT stageRank,summonsGroupId,rewGobId,description FROM _stage");

	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		StageTemplate Temp;
		Temp.stageRank = fields[0].GetUInt32();
		Temp.summonsGroupId = fields[1].GetUInt32();
		Temp.rewardGameobjectId = fields[2].GetUInt32();
		Temp.description = fields[3].GetString();
		StageVec.push_back(Temp);
	} while (result->NextRow());
}

std::string Stage::GetDescription(uint32 stage)
{
	uint32 len = StageVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (stage == StageVec[i].stageRank)
			return StageVec[i].description;
	}

	return "";
}

uint32 Stage::GetSumId(uint32 stage)
{
	uint32 len = StageVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (stage == StageVec[i].stageRank)
			return StageVec[i].summonsGroupId;
	}

	return 1;
}

uint32 Stage::GetGobId(uint32 stage)
{
	uint32 len = StageVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (stage == StageVec[i].stageRank)
			return StageVec[i].rewardGameobjectId;
	}

	return 0;
}

class StageNPC : public CreatureScript
{
public:
	StageNPC() : CreatureScript("StageNPC") { }
 
	bool OnGossipHello(Player* player, Creature* creature) override
	{
		player->PlayerTalkClass->ClearMenus();

		uint32 eventid = atoi(sSwitch->GetFlagByIndex(ST_STAGE, 1).c_str());

		if (eventid != 0 && !sGameEventMgr->IsActiveEvent(eventid))
		{
			ChatHandler(player->GetSession()).PSendSysMessage("当前时间不允许挑战！");
			return false;
		}

		if (creature->summonsClear && creature->stage <= StageVec.size())
		{
			std::ostringstream oss;
			oss << "挑战等级:" << creature->stage;
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, oss.str(), creature->stage, GOSSIP_ACTION_INFO_DEF + 1);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, sStage->GetDescription(creature->stage), creature->stage, GOSSIP_ACTION_INFO_DEF);
		}

		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		if (action == GOSSIP_ACTION_INFO_DEF + 1)
		{
			OnGossipHello(player, creature);
			return true;
		}
		creature->SummonCreatureGroup(sStage->GetSumId(creature->stage));
		creature->summonsClear = false;
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	struct StageNPCAI : public ScriptedAI
	{
		StageNPCAI(Creature* creature) : ScriptedAI(creature), Summons(me){}

		SummonList Summons;
		void JustSummoned(Creature* summon) override
		{
			summon->GetMotionMaster()->MoveRandom(5.0f);
			Summons.Summon(summon);
		}

		void Reset() override
		{
			me->summonsClear = true;
			me->stage = 1;
			Summons.DespawnAll();
		}

		void MoveInLineOfSight(Unit* who)
		{
		}

		void SummonedCreatureDies(Creature* summon, Unit* killer)  override
		{
			std::ostringstream oss;
			oss << "挑战等级：" << me->stage;
			me->MonsterSay(oss.str().c_str(), LANG_UNIVERSAL, 0);

			Summons.Despawn(summon);

			if (Summons.empty())
			{
				uint32 gobEntry = sStage->GetGobId(me->stage);

				if (sObjectMgr->GetGameObjectTemplate(gobEntry))
					me->SummonGameObject(gobEntry, killer->GetPositionX(), killer->GetPositionY(), killer->GetPositionZ(), 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0);

				me->stage++;
				me->summonsClear = true;
			}	
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new StageNPCAI(creature);
	}
};

class StagePlayerScript : public PlayerScript
{
public:
	StagePlayerScript() : PlayerScript("StagePlayerScript") {}

	void OnMapChanged(Player* player) override
	{
		uint32 mapId = atoi(sSwitch->GetFlagByIndex(ST_STAGE, 2).c_str());

		if (player->GetMapId() != mapId)
			return;

		uint32 size = atoi(sSwitch->GetFlagByIndex(ST_STAGE, 4).c_str());

		if (Map* map = player->GetMap())
		{
			uint32 count = map->GetPlayersCountExceptGMs();

			if (count > size)
			{
				player->RepopAtGraveyard();
				return;
			}

			if (count == 1)
			{
				uint32 npcId = atoi(sSwitch->GetFlagByIndex(ST_STAGE, 3).c_str());

				if (Creature* creature = player->FindNearestCreature(npcId, 150))
					creature->AI()->Reset();
			}
		}
	}
};

void AddSC_Stage()
{
	new StageNPC();
	new StagePlayerScript();
}

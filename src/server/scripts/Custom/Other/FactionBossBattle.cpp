#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"
#include "../CommonFunc/CommonFunc.h"
#include "../CustomEvent/Event.h"

//LM三个BOSS的entry
#define A_A_ENTRY 200000
#define A_B_ENTRY 200001
#define A_C_ENTRY 200002

//BL三个BOSS的entry
#define H_A_ENTRY 200003
#define H_B_ENTRY 200004
#define H_C_ENTRY 200005

//事件ID
#define FACTION_BOSS_BATTLE_EVENTID 100

//获取奖励所需的伤害和治疗
#define DAMAGE_FOR_REWARD 500000
#define HEAL_FOR_REWARD 500000
#define KILLS_FOR_REWARD 100
#define KILLEDS_FOR_REWARD 100

//奖励的宝箱ID
#define FACTION_BOSS_BATTLE_CHESTID 200000

//请提供事件开始时LM和BL坐标
//请提供事件结束时传送坐标



class FactionBossTrigger : public CreatureScript
{
public:
	FactionBossTrigger() : CreatureScript("FactionBossTrigger") { }

	struct FactionBossTriggerAI : public ScriptedAI
	{
		FactionBossTriggerAI(Creature* creature) : ScriptedAI(creature), Summons(me){}
		SummonList Summons;

		void InitializeAI() override
		{
			me->SummonCreatureGroup(1);
		}

		void Reset() override
		{
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summoned) override
		{
			Summons.Summon(summoned);
			switch (summoned->GetEntry())
			{
			case A_A_ENTRY:
			case H_A_ENTRY:
				break;
			case A_B_ENTRY:
			case H_B_ENTRY:
			case A_C_ENTRY:
			case H_C_ENTRY:
				//summoned->SetReactState(REACT_PASSIVE);
				//summoned->SetVisible(false);
				//summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				break;
			default:
				break;
			}
		}

		void SummonedCreatureDespawn(Creature* summoned) override
		{
			Summons.Despawn(summoned);
		}

		void SummonedCreatureDies(Creature* summon, Unit* killer)
		{
			switch (summon->GetEntry())
			{
			case A_A_ENTRY:
				SetCreatureActive(A_B_ENTRY);
				break;
			case H_A_ENTRY:
				SetCreatureActive(H_B_ENTRY);
				break;
			case A_B_ENTRY:
				SetCreatureActive(A_C_ENTRY);
				break;
			case H_B_ENTRY:
				SetCreatureActive(H_C_ENTRY);
				break;
			case A_C_ENTRY:
			case H_C_ENTRY:
				RewardWinTeam(killer);
				if (sGameEventMgr->IsActiveEvent(FACTION_BOSS_BATTLE_EVENTID))
					sGameEventMgr->StopEvent(FACTION_BOSS_BATTLE_EVENTID,true);
			default:
				break;
			}
		}
		void UpdateAI(uint32 diff) override
		{
		
			
		}

		void JustDied(Unit* /*killer*/){}


		void SetCreatureActive(uint32 entry)
		{
			if (Creature* creature = GetClosestCreatureWithEntry(me, entry, 200))
			{
				creature->SetReactState(REACT_AGGRESSIVE);
				creature->SetVisible(true);
				creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
			}
		}

		void RewardWinTeam(Unit* killer)
		{
			Summons.DespawnAll();

			Player * player;

			if (killer->GetTypeId() == TYPEID_PLAYER)
				player = killer->ToPlayer();
			else if (killer->GetOwner()->GetTypeId() == TYPEID_PLAYER)
				player = killer->GetOwner()->ToPlayer();

			if (player)
			{
				SessionMap const& smap = sWorld->GetAllSessions();
				for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
					if (Player* pl = iter->second->GetPlayer())
						if (pl->GetSession())
						{
					
								if (pl->GetTeamId() != player->GetTeamId())
									continue;

								pl->AddItem(FACTION_BOSS_BATTLE_CHESTID, 1);
						}
			}
		}

	private:
		EventMap _events;
	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new FactionBossTriggerAI(creature);
	}
};



class FactionBossPlayerScript : public PlayerScript
{
public:
	FactionBossPlayerScript() : PlayerScript("FactionBossPlayerScript") {}
	void OnCreatureKill(Player* killer, Creature* killed) 
	{
		if (!sGameEventMgr->IsActiveEvent(FACTION_BOSS_BATTLE_EVENTID) && (killed->GetEntry() != 200000 || killed->GetEntry() != 200001))
			return;	

		Player * player = killed->GetLootRecipient();

		if (!player)
			return;	
		
		/*
		if (killer->GetTypeId() == TYPEID_PLAYER)
			player = killer->ToPlayer();
		else if (killer->GetOwner()->GetTypeId() == TYPEID_PLAYER)
			player = killer->GetOwner()->ToPlayer();
			*/
		
		SessionMap const& smap = sWorld->GetAllSessions();
		for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
			if (Player* pl = iter->second->GetPlayer())
				if (pl->GetSession())
				{
					
						if (pl->GetTeamId() != player->GetTeamId())
							continue;
						pl->AddItem(FACTION_BOSS_BATTLE_CHESTID, 1);							
				}

		sGameEventMgr->StopEvent(FACTION_BOSS_BATTLE_EVENTID, true);
		sWorld->SendServerMessage(SERVER_MSG_STRING, "南海镇战役结束！");
	}
};

void AddSC_FactionBOSS_BATTLE()
{
	//new FactionBossTrigger();
	//new FactionBossPlayerScript();
}

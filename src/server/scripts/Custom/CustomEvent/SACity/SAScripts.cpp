#pragma execution_character_set("utf-8")
#include "BattlegroundSA.h"
#include "SA.h"
#include "../Event.h"

#define GATE_1_R	450011
#define GATE_1_L	450013
#define GATE_2_R	450012
#define GATE_2_L	450014
#define GATE_3		450015
#define MAIN_GATE   192549

const Position SummonPos1 = { 1509.24f, -35.67f, 4.51f, 3.25f };
const Position SummonPos2 = { 1297.13f, -63.91f, 34.05f, 3.25f };
const Position SummonPos3 = { 1210.07f, -70.68f, 70.09f, 3.25f };

enum EventsSummon
{
	EVENT_SUMMON = 1,
	EVENT_NOTICE,
};

class SATrigger : public CreatureScript
{
public:
	SATrigger() : CreatureScript("SATrigger") { }
	struct SATriggerAI : public CreatureAI
	{
		SummonList Summons;
		EventMap events;

		SATriggerAI(Creature* creature) : CreatureAI(creature), Summons(me)
		{
		}

		void Reset() override
		{
			Summons.DespawnAll();
			events.Reset();

			for (uint8 i = 0; i < BG_SA_BOAT_ONE; i++)
			{
				if (BG_SA_ObjEntries[i] == 190722)
					me->SummonGameObject(450011, BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, 0);
				else if (BG_SA_ObjEntries[i] == 190723)
					me->SummonGameObject(450012, BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, 0);
				else if (BG_SA_ObjEntries[i] == 190724)
					me->SummonGameObject(450013, BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, 0);
				else if (BG_SA_ObjEntries[i] == 190726)
					me->SummonGameObject(450014, BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, 0);
				else if (BG_SA_ObjEntries[i] == 190727)
					me->SummonGameObject(450015, BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, 0);
				else
					me->SummonGameObject(BG_SA_ObjEntries[i], BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, 0);


			}
			for (uint8 i = BG_SA_PORTAL_DEFFENDER_BLUE; i < BG_SA_BOMB; i++)
				me->SummonGameObject(BG_SA_ObjEntries[i], BG_SA_ObjSpawnlocs[i][0], BG_SA_ObjSpawnlocs[i][1], BG_SA_ObjSpawnlocs[i][2], BG_SA_ObjSpawnlocs[i][3], 0, 0, 0, 0, 0);
			for (uint8 i = 0; i < BG_SA_DEMOLISHER_5; i++)
				me->SummonCreature(BG_SA_NpcEntries[i], BG_SA_NpcSpawnlocs[i][0], BG_SA_NpcSpawnlocs[i][1], BG_SA_NpcSpawnlocs[i][2], BG_SA_NpcSpawnlocs[i][3], TEMPSUMMON_TIMED_DESPAWN, 30 * MINUTE * IN_MILLISECONDS);

			events.ScheduleEvent(EVENT_SUMMON, 150 * IN_MILLISECONDS);
			events.ScheduleEvent(EVENT_NOTICE, 30 * IN_MILLISECONDS);
		}

		void SummonCorpseCreature()
		{
			GameObject* gate_1_r = me->FindNearestGameObject(GATE_1_R, 1000.0f);
			GameObject* gate_1_l = me->FindNearestGameObject(GATE_1_L, 1000.0f);
			GameObject* gate_2_r = me->FindNearestGameObject(GATE_2_R, 1000.0f);
			GameObject* gate_2_l = me->FindNearestGameObject(GATE_2_L, 1000.0f);
			GameObject* gate_3 = me->FindNearestGameObject(GATE_3, 1000.0f);

			if (!gate_1_r || !gate_1_l || !gate_2_r || !gate_2_l || !gate_3)
				return;
	

			if (gate_1_r->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED || gate_1_l->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED)
			{
				if (Creature* summon = me->SummonCreature(CorpseCreature_ENTRY, SummonPos1, TEMPSUMMON_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS))
					summon->AI()->DoAction(1);
			}
				
			else if (gate_2_r->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED || gate_2_l->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED)
			{
				if (Creature* summon = me->SummonCreature(CorpseCreature_ENTRY, SummonPos2, TEMPSUMMON_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS))
					summon->AI()->DoAction(2);
			}
			else if (gate_3->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED)
			{
				if (Creature* summon = me->SummonCreature(CorpseCreature_ENTRY, SummonPos3, TEMPSUMMON_TIMED_DESPAWN, 10 * MINUTE * IN_MILLISECONDS))
					summon->AI()->DoAction(3);
			}
		}

		void JustSummoned(Creature* summon) override
		{
			summon->SetReactState(REACT_PASSIVE);
			Summons.Summon(summon);
		}

		void SummonedCreatureDies(Creature* summon, Unit*) override
		{
			Summons.Despawn(summon);
		}

		void UpdateAI(uint32 diff) override
		{
			events.Update(diff);
			switch (events.ExecuteEvent())
			{
			case EVENT_SUMMON:
				if (Creature* corpse = me->FindNearestCreature(CorpseCreature_ENTRY, 5.0f))
				{
					Summons.DespawnAll();
					sGameEventMgr->StopEvent(EVENT_ID_SA,false,false);
					break;
				}
				if (sGameEventMgr->IsActiveEvent(EVENT_ID_SA))
					SummonCorpseCreature();
				events.ScheduleEvent(EVENT_SUMMON, 1000);
				break;
			case EVENT_NOTICE:
			{
				std::ostringstream oss;
				oss <<  "[僵尸围城]僵尸两分钟后出现，请做好准备！";
				sWorld->SendScreenMessage(oss.str().c_str());
			}
			break;
			default:
				break;
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new SATriggerAI(creature);
	}
};


enum EventsCorpseCreature
{
	EVENT_CHECK_DISTANCE = 1,
};

class CorpseCreature : public CreatureScript
{
public:
	CorpseCreature() : CreatureScript("CorpseCreature") { }
	struct CorpseCreatureAI : public CreatureAI
	{
		SummonList Summons;
		EventMap events;
		CorpseCreatureAI(Creature* creature) : CreatureAI(creature), Summons(me)
		{
		}

		void Reset() override
		{
			events.Reset();
			events.ScheduleEvent(EVENT_CHECK_DISTANCE, 0);
		}
		
		void LoadPath(uint32 path_id)
		{
			me->LoadPath(path_id);
			me->SetDefaultMovementType(WAYPOINT_MOTION_TYPE);
			me->GetMotionMaster()->Initialize();
		}

		void DoAction(int32 param) override
		{
			switch (param)
			{
			case 1:
				LoadPath(urand(1, 6));
				break;
			case 2:
				LoadPath(urand(7, 8));
				break;
			case 3:
				LoadPath(9);
				break;
			default:
				break;
			}
		}

		void UpdateAI(uint32 diff) override
		{
			events.Update(diff);
			switch (events.ExecuteEvent())
			{
			case EVENT_CHECK_DISTANCE:
				if (GameObject* gate = me->FindNearestGameObjectOfType(GAMEOBJECT_TYPE_DESTRUCTIBLE_BUILDING, 5.0f))
				{
					if (gate->GetDestructibleState() != GO_DESTRUCTIBLE_DESTROYED)
					{
						me->CastSpell(gate, 5555,true);
						DoCast(64228);
						me->setDeathState(JUST_DIED, true);
					}			
				}
				events.ScheduleEvent(EVENT_CHECK_DISTANCE, 1000);
				break;
			default:
				break;
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new CorpseCreatureAI(creature);
	}
};


void AddSC_SATrigger()
{
	new SATrigger();
	new CorpseCreature();
}

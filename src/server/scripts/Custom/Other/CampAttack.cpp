#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"
#include "../CommonFunc/CommonFunc.h"
enum Events
{
	CALL_PORT	= 1,
	FIRST_WAVE	= 2,
	SECOND_WAVE	= 3,
	THIRD_WAVE	= 4,
	FOURTH_WAVE = 5,
	CALL_GOB_PORT=6,
	GROUND_SHAKE =7,
	CALL_RAGNAROS =8,
	CALL_FIREKING =9,
};
enum NPCS
{
	NPC_PORT = 960031,
	NPC_FIRE_SON = 960032,
	NPC_BLACK_IRON = 960033,
	NPC_RAGNAROS = 960034,
	NPC_FIRE_KING = 960035,
};

enum GOBS
{
	GOB_PORT = 186880,
};
static Position PortLocation[] =
{
	// 传送门刷新位置
	{ -515.860f, 17.0702f, 49.6485f, 0 }, // 0
	{ -503.188f, 4.16218f, 51.8489f, 0 }, // 1
	{ -500.943f, 36.2931f, 50.6729f, 0 }, // 2
	{ -525.468f, 27.8368f, 48.6491f, 0 }, // 3
	{ -529.523f, -6.7841f, 53.2613f, 0 }, // 4
};



class camp_trigger : public CreatureScript
{
public:
	camp_trigger() : CreatureScript("camp_trigger") { }

	struct camp_triggerAI : public ScriptedAI
	{
		camp_triggerAI(Creature* creature) : ScriptedAI(creature), Summons(me){}
		SummonList Summons;

		void Reset() override
		{
			Summons.DespawnAll();
			bool FirstWaveStared = false;
			bool SecondWaveStared = false;
		}

		void JustSummoned(Creature* summoned) override
		{
			switch (summoned->GetEntry())
			{
			case NPC_PORT:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				break;
			case NPC_FIRE_SON:
				//summoned->SetInCombatWithZone();
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
					summoned->AI()->AttackStart(target);
				summoned->GetMotionMaster()->MoveRandom(20.0f);
				break;
			case NPC_BLACK_IRON:
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
					summoned->AI()->AttackStart(target);
				summoned->GetMotionMaster()->MoveRandom(20.0f);
				break;
			case NPC_RAGNAROS:
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
				summoned->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_DISABLE_MOVE);
				break;
			case NPC_FIRE_KING:
				if (Unit* target = SelectTarget(SELECT_TARGET_RANDOM, 0))
					summoned->AI()->AttackStart(target);
				summoned->GetMotionMaster()->MoveRandom(20.0f);
				break;
			default:
				break;
			}

			Summons.Summon(summoned);
		}

		void SummonedCreatureDespawn(Creature* summoned) override
		{
			Summons.Despawn(summoned);
		}

		void SummonedCreatureDies(Creature* summon, Unit*)
		{
			
		}
		void UpdateAI(uint32 diff) override
		{
			if (CampAttackStartFlag)
			{
				if (me->FindNearestCreature(NPC_FIRE_KING, 1000, true) || me->FindNearestCreature(NPC_FIRE_SON, 1000, true) || me->FindNearestCreature(NPC_BLACK_IRON, 1000, true))
				{
					CampAttackStartFlag = false;
					return;
				}
				CampAttackStartFlag = false;
				_events.ScheduleEvent(CALL_PORT, 1000);
				sWorld->SendGlobalText("新兵营地正在被攻击！", NULL);
			}
			_events.Update(diff);

			while (uint32 eventId = _events.ExecuteEvent())
			{
						
				switch (eventId)
				{
				case CALL_PORT:
					for (uint32 i = 0; i < 5; i++)
					{
						me->SummonCreature(NPC_PORT, PortLocation[i], TEMPSUMMON_MANUAL_DESPAWN);
					}
					_events.ScheduleEvent(FIRST_WAVE, 5000);
					_events.ScheduleEvent(FIRST_WAVE, 10000);

					_events.ScheduleEvent(GROUND_SHAKE, 20000);

					break;				
				case FIRST_WAVE:
					for (uint32 i = 0; i < 5; i++)
					{
						me->SummonCreature(NPC_FIRE_SON, PortLocation[i], TEMPSUMMON_TIMED_DESPAWN, 3 * 60000);
						me->SummonCreature(NPC_FIRE_SON, PortLocation[i], TEMPSUMMON_TIMED_DESPAWN, 3 * 60000);
					}
					break;
				case GROUND_SHAKE:
				{
					//删除传送门
					std::list<Creature*> NPCPorts;
					me->GetCreatureListWithEntryInGrid(NPCPorts, NPC_PORT, 300.0f);
					if (!NPCPorts.empty())
						for (std::list<Creature*>::iterator itr = NPCPorts.begin(); itr != NPCPorts.end(); ++itr)
						{
							if (*itr) (*itr)->RemoveFromWorld();
						}
					_events.ScheduleEvent(CALL_GOB_PORT, 4000);	
				}				
					break;
				case CALL_GOB_PORT:
				{

					for (uint32 i = 0; i < 5; i++)
					{
						float x, y, z;
						PortLocation[i].GetPosition(x, y, z);
						if (GameObject* gob = me->SummonGameObject(GOB_PORT, x, y, z, 0, 0, 0, 0, 0, 0))
							gob->Delete();

					}
					_events.ScheduleEvent(SECOND_WAVE, 5000);
					_events.ScheduleEvent(CALL_RAGNAROS, 20000);
				}
				break;
				case SECOND_WAVE:
				{
					for (uint32 i = 0; i < 5; i++)
					{
						me->SummonCreature(NPC_BLACK_IRON, PortLocation[i], TEMPSUMMON_TIMED_DESPAWN, 3*60000);
						me->SummonCreature(NPC_BLACK_IRON, PortLocation[i], TEMPSUMMON_TIMED_DESPAWN, 3*60000);
						me->SummonCreature(NPC_BLACK_IRON, PortLocation[i], TEMPSUMMON_TIMED_DESPAWN, 3*60000);
					}
				}
					break;
				case CALL_RAGNAROS:
				{
					Creature* ragnaros = me->SummonCreature(NPC_RAGNAROS, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(),0, TEMPSUMMON_TIMED_DESPAWN, 15000);
					if (ragnaros) ragnaros->MonsterYell("这里将烧成灰烬，森卢肯，执行我的意志！", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(CALL_FIREKING, 5000);
				}		
					break;
				case CALL_FIREKING:
				{
					Creature* fireking = me->SummonCreature(NPC_FIRE_KING, me->m_positionX + 30, me->m_positionY, me->m_positionZ, 0, TEMPSUMMON_TIMED_DESPAWN, 5 * 60000);
					if (fireking) fireking->MonsterYell("遵命，伟大的主人！", LANG_UNIVERSAL, NULL);

				}
					break;
				default:
					break;
				}
			}
		}
	private:
		EventMap _events;
	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new camp_triggerAI(creature);
	}
};


void AddSC_Camp_Attack()
{
	new camp_trigger();
}

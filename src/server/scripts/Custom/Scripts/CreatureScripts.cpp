/*

ALTER TABLE _ai_creature CHANGE actionType actionType ENUM('激活或使失活','设定阶段','释放技能','移动范围','召唤','说话','移除') CHARSET utf8 COLLATE utf8_general_ci NOT NULL;
ALTER TABLE _ai_creature ADD COLUMN actionParam3 INT(11) DEFAULT 0 NOT NULL AFTER actionParam2, DROP PRIMARY KEY, ADD PRIMARY KEY (entry, eventType, eventPhase, delayTime, repeatMinTime, repeatMaxTime, actionType, actionParam1, actionParam2, actionParam3);
*/

#pragma execution_character_set("utf-8")
#include "CustomScripts.h"
#include "ObjectMgr.h"
#include "../CustomEvent/Event.h"
#include <random>
#include <algorithm>
#include <vector>
std::vector<CreatureScriptTemplate> CreatureScriptVec;
std::vector<AIRandSpellTemplate> AIRandSpellVec;

void CustomScript::LoadCreautreScripts()
{
	QueryResult result;

	CreatureScriptVec.clear();
	
	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT 生物ID,事件类型,事件阶段ID,最小重复时间,最大重复时间,动作类型,动作参数1,动作参数2,延迟时间,动作参数3 FROM _自定义AI_生物" :
		"SELECT entry,eventType,eventPhase,repeatMinTime,repeatMaxTime,actionType,actionParam1,actionParam2,delayTime,actionParam3 FROM _ai_creature"))
	{
		do
		{
			Field* fields = result->Fetch();
			CreatureScriptTemplate Temp;

			Temp.entry = fields[0].GetUInt32();

			const char*  str1 = fields[1].GetCString();

			if (strcmp("进入战斗", str1) == 0)
				Temp.eventType = EVENT_TYPE_COMBAT;
			else if (strcmp("杀死玩家", str1) == 0)
				Temp.eventType = EVENT_TYPE_KILL;
			else if (strcmp("怪物死亡", str1) == 0)
				Temp.eventType = EVENT_TYPE_DIED;
			else if (strcmp("血量低于90%", str1) == 0)
				Temp.eventType = EVENT_TYPE_HP_90;
			else if (strcmp("血量低于75%", str1) == 0)
				Temp.eventType = EVENT_TYPE_HP_75;
			else if (strcmp("血量低于50%", str1) == 0)
				Temp.eventType = EVENT_TYPE_HP_50;
			else if (strcmp("血量低于25%", str1) == 0)
				Temp.eventType = EVENT_TYPE_HP_25;
			else if (strcmp("血量低于10%", str1) == 0)
				Temp.eventType = EVENT_TYPE_HP_10;
			else if (strcmp("重置AI", str1) == 0)
				Temp.eventType = EVENT_TYPE_RESET;
			else
				Temp.eventType = EVENT_TYPE_NONE;

			//Temp.eventType		= CreatureEventTypes(fields[1].GetUInt32());
			Temp.eventPhase = fields[2].GetUInt32();
			Temp.repeatMinTime = fields[3].GetUInt32() * 1000;
			Temp.repeatMaxTime = fields[4].GetUInt32() * 1000;

			const char* str2 = fields[5].GetCString();

			if (strcmp("释放技能", str2) == 0)
				Temp.actionType = ACTION_TYPE_CAST_SPELL;
			else if (strcmp("激活或使失活", str2) == 0)
				Temp.actionType = ACTION_TYPE_ACTIVE_OR_DEACTIVE_OBJECT;
			else if (strcmp("设定阶段", str2) == 0)
				Temp.actionType = ACTION_TYPE_ALT_PHASE;
			else if (strcmp("说话", str2) == 0)
				Temp.actionType = ACTION_TYPE_TALK;
			else if (strcmp("召唤", str2) == 0)
				Temp.actionType = ACTION_TYPE_SUMMON;
			else if (strcmp("转换", str2) == 0)
				Temp.actionType = ACTION_TYPE_ALT_GOB;
			else if (strcmp("移动范围", str2) == 0)
				Temp.actionType = ACTION_TYPE_MOVE_RANDOM;
			else
				Temp.actionType = ACTION_TYPE_NONE;

			//Temp.actionType		= CreatureActionTypes(fields[5].GetUInt32());
			Temp.actionParam1 = fields[6].GetString();
			Temp.actionParam2 = fields[7].GetInt32();
			Temp.actionFlags = 0;//fields[8].GetUInt32();

			Temp.delayTime = fields[8].GetUInt32() * 1000;
			Temp.actionParam3 = fields[9].GetInt32();
			CreatureScriptVec.push_back(Temp);
		} while (result->NextRow());
	}
	

	for (uint32 i = 0; i < CreatureScriptVec.size(); i++)
	{
		uint32 entry = CreatureScriptVec[i].entry;

		CreatureTemplateContainer  *creatureTemplateStore = sObjectMgr->GetCreatureTemplatesStore();

		CreatureTemplateContainer::iterator itr = creatureTemplateStore->find(entry);

		if (itr != creatureTemplateStore->end())
		{
			itr->second.AIName = "";
			itr->second.ScriptID = sObjectMgr->GetScriptId("CustomCreatureScript");
		}
	}

	AIRandSpellVec.clear();

	if (result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 随机技能组ID,技能ID,释放下一技能最小延迟,释放下一技能最大延迟 FROM _模板_随机技能组" :
		"SELECT GroupId,SpellId,MinDelayTime,MaxDelayTime FROM _ai_random_spell_group"))
	{
		do
		{
			Field* fields = result->Fetch();
			AIRandSpellTemplate Temp;
			Temp.GroupId		= fields[0].GetUInt32();
			Temp.SpellId		= fields[1].GetUInt32();
			Temp.MinDelayTime	= fields[2].GetUInt32() * IN_MILLISECONDS;
			Temp.MaxDelayTime	= fields[3].GetUInt32() * IN_MILLISECONDS;

			if (Temp.MaxDelayTime < Temp.MinDelayTime)
				Temp.MaxDelayTime = Temp.MinDelayTime;

			AIRandSpellVec.push_back(Temp);
		} while (result->NextRow());
	}
}

void CustomScript::CastRandSpell(Creature* creautre, uint32 diff)
{
	if (creautre->RandSpellGroupId == 0)
		return;

	if (AIRandSpellVec.empty())
		return;

	if (creautre->IsInCombat())
	{
		creautre->RandSpellTimer -= diff;

		if (creautre->RandSpellTimer < 0)
		{
			std::vector<AIRandSpellTemplate> v;

			for (auto itr = AIRandSpellVec.begin(); itr != AIRandSpellVec.end(); itr++)
				if (itr->GroupId == creautre->RandSpellGroupId)
					v.push_back(*itr);

			if (!v.empty())
			{
				//std::random_shuffle(v.begin(), v.end());
                std::default_random_engine generator{ std::random_device{}() };
                std::shuffle(std::begin(v), std::end(v), generator);

				creautre->RandSpellTimer = urand(v[0].MinDelayTime, v[0].MaxDelayTime);
				if (creautre->GetVictim())
					creautre->CastSpell(creautre->GetVictim(), v[0].SpellId);
			}
			else
				creautre->RandSpellTimer = 0;		
		}		
	}
	else
		creautre->RandSpellTimer = 0;
}

enum EVENT_IDS
{
	EVENT_CHECK_HEALTH = 1,
};

class CustomCreatureScript : public CreatureScript
{
public:
	CustomCreatureScript() : CreatureScript("CustomCreatureScript") { }

	struct CustomCreatureScriptAI : public ScriptedAI
	{
		CustomCreatureScriptAI(Creature* creature) : ScriptedAI(creature), Summons(me){}

		SummonList Summons;

		std::vector<ActionTemplate> ActionVec;

		bool hp_90_active;
		bool hp_75_active;
		bool hp_50_active;
		bool hp_25_active;
		bool hp_10_active;

		// EVENT_TYPE_RESET
		void Reset() override
		{
			events.Reset();
			Summons.DespawnAll();

			hp_90_active	= false;
			hp_75_active	= false;
			hp_50_active	= false;
			hp_25_active	= false;
			hp_10_active	= false;

			ActionVec.clear();

			uint32 count = 2;

			for (std::vector<CreatureScriptTemplate>::iterator itr = CreatureScriptVec.begin(); itr != CreatureScriptVec.end(); itr++)
			{
				if (itr->entry == me->GetEntry())
				{
					ActionTemplate Temp;
					Temp.eventID = count;
					Temp.eventType = itr->eventType;
					Temp.eventPhase = itr->eventPhase;
					Temp.delayTime = itr->delayTime;
					Temp.repeatMinTime = itr->repeatMinTime;
					Temp.repeatMaxTime = itr->repeatMaxTime;
					Temp.actionType = itr->actionType;
					Temp.actionParam1 = itr->actionParam1;
					Temp.actionParam2 = itr->actionParam2;
					Temp.actionFlags = itr->actionFlags;
					Temp.actionParam3 = itr->actionParam3;
					ActionVec.push_back(Temp);

					count++;
				}
			}

			SetPhase(0);

			TriggerEvent(EVENT_TYPE_RESET, me);
		}


		void AddEvent(CreatureEventTypes eventType)
		{
			for (std::vector<ActionTemplate>::iterator itr = ActionVec.begin(); itr != ActionVec.end(); itr++)
			{
				if (itr->eventType == eventType)
					events.ScheduleEvent(itr->eventID, itr->delayTime);		
			}
		}

		void TriggerEvent(uint32 eventID)
		{
			for (std::vector<ActionTemplate>::iterator itr = ActionVec.begin(); itr != ActionVec.end(); itr++)
			{
				if (itr->eventID != eventID)
					continue;

				if (itr->repeatMinTime > 0 && itr->repeatMaxTime == itr->repeatMinTime)
					events.ScheduleEvent(itr->eventID, itr->repeatMinTime);

				if (itr->repeatMinTime > 0 && itr->repeatMaxTime > itr->repeatMinTime)
					events.ScheduleEvent(itr->eventID, urand(itr->repeatMinTime, itr->repeatMaxTime));

				if (itr->eventPhase == 0 || itr->eventPhase == GetPhase())
					Action(itr->actionType, itr->actionParam1, itr->actionParam2, itr->actionFlags,itr->actionParam3, me);
			}
		}

		void TriggerEvent(CreatureEventTypes eventType, Unit* unit)
		{
			for (std::vector<ActionTemplate>::iterator itr = ActionVec.begin(); itr != ActionVec.end(); itr++)
				if (itr->eventType == eventType && itr->eventPhase == GetPhase())
					Action(itr->actionType, itr->actionParam1, itr->actionParam2, itr->actionFlags, itr->actionParam3, unit);
		}


		void UpdateAI(uint32 diff)
		{
			if (!UpdateVictim())
				return;

			events.Update(diff);

			if (me->HasUnitState(UNIT_STATE_CASTING))
				return;


			while (uint32 eventId = events.ExecuteEvent())
			{

				if (eventId == EVENT_CHECK_HEALTH)
				{
					if (!hp_90_active && me->HealthBelowPct(90))
					{
						hp_90_active = true;
						TriggerEvent(EVENT_TYPE_HP_90, me);
					}
					
					if (!hp_75_active && me->HealthBelowPct(75))
					{
						hp_75_active = true;
						TriggerEvent(EVENT_TYPE_HP_75, me);
					}

					if (!hp_50_active && me->HealthBelowPct(50))
					{
						hp_50_active = true;
						TriggerEvent(EVENT_TYPE_HP_50, me);
					}

					if (!hp_25_active && me->HealthBelowPct(25))
					{
						hp_25_active = true;
						TriggerEvent(EVENT_TYPE_HP_25, me);
					}

					if (!hp_10_active && me->HealthBelowPct(10))
					{
						hp_10_active = true;
						TriggerEvent(EVENT_TYPE_HP_10, me);
					}

					events.ScheduleEvent(EVENT_CHECK_HEALTH, 1000);
				}

				TriggerEvent(eventId);
			}

			DoMeleeAttackIfReady();
		}


		// EVENT_TYPE_COMBAT
		void EnterCombat(Unit* /*victim*/) override
		{
			AddEvent(EVENT_TYPE_COMBAT);
			events.ScheduleEvent(EVENT_CHECK_HEALTH, 1000);
		}

		//EVENT_TYPE_KILL
		void KilledUnit(Unit* /*victim*/) override
		{
			AddEvent(EVENT_TYPE_KILL);
		}

		//EVENT_TYPE_DIED
		void JustDied(Unit* killer) override
		{
			TriggerEvent(EVENT_TYPE_DIED, killer);
			Summons.DespawnAll();
		}

		void JustSummoned(Creature* summoned) override
		{
			summoned->GetMotionMaster()->MoveRandom(5.0f);
			Summons.Summon(summoned);
		}

		void SummonedCreatureDies(Creature* summoned, Unit* /*killer*/) override
		{

			Summons.Despawn(summoned);
		}


		void SummonedCreatureDespawn(Creature* summoned) override
		{
			Summons.Despawn(summoned);
		}

		void Action(CreatureActionTypes actionType, std::string actionParam1, int32 actionParam2, uint32 actionFlags, int32 actionParam3, Unit* unit)
		{
			switch (actionType)
			{
			case ACTION_TYPE_CAST_SPELL:
				{
					if (!me->IsAlive())
						break;

					uint32 spellid1 = atoi(actionParam1.c_str());
					uint32 spellid2 = actionParam2;

					if (spellid1 != 0 && spellid2 != 0)
						urand(0, 1) == 0 ? DoCast(spellid1) : DoCast(spellid2);
					else if (spellid1 != 0)
						DoCast(spellid1);
					else if (spellid2 != 0)
						DoCast(spellid2);
				}
				break;
			case ACTION_TYPE_TALK:
				switch (actionParam2)
				{
				case 1:
					me->MonsterSay(actionParam1.c_str(), LANG_UNIVERSAL, NULL);
					break;
				case 2:
					me->MonsterYell(actionParam1.c_str(), LANG_UNIVERSAL, NULL);
					break;
				case 3:
					if (Unit* victim = me->GetVictim())
						if (Player* v = victim->ToPlayer())
							me->MonsterWhisper(actionParam1.c_str(), v);
					break;
				case 4:
					sWorld->SendZoneText(me->GetZoneId(), actionParam1.c_str());
					break;
				case 5:
				{
					SessionMap m_sessions = sWorld->GetAllSessions();
					for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
					{
						if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld() || itr->second->GetPlayer()->GetZoneId() != me->GetZoneId())
							continue;
						itr->second->SendAreaTriggerMessage(actionParam1.c_str());
					}
				}
					break;
				case 6:
				{
					SessionMap m_sessions = sWorld->GetAllSessions();
					for (SessionMap::iterator itr = m_sessions.begin(); itr != m_sessions.end(); ++itr)
					{
						if (!itr->second || !itr->second->GetPlayer() || !itr->second->GetPlayer()->IsInWorld() || itr->second->GetPlayer()->GetZoneId() != me->GetZoneId())
							continue;
						itr->second->SendAreaTriggerMessage(actionParam1.c_str());
					}
					sWorld->SendZoneText(me->GetZoneId(), actionParam1.c_str());
				}
					break;
				case 7:
					sWorld->SendScreenMessage(actionParam1.c_str());
					break;
				}
				break;
			case ACTION_TYPE_ACTIVE_OR_DEACTIVE_OBJECT:
			{
				int32 param1 = atoi(actionParam1.c_str());
				bool isCreature = actionParam2 == 0 ? true : false;
				bool active = param1 > 0 ? true : false;

				if (actionParam3 < 0)
				{
					uint32 entry = abs(param1);
					if (isCreature)
					{
						if (Creature* creature = unit->FindNearestCreature(entry, 500.0f))
							creature->RemoveFromWorld();
					}
					else
					{
						if (GameObject* gob = unit->FindNearestGameObject(entry, 500.0f))
							gob->RemoveFromWorld();
					}		
				}
				else
				{
					
					uint32 guid = abs(param1);		
					
					if (isCreature)
					{
						if (CreatureData const* data = sObjectMgr->GetCreatureData(guid))
							if (Creature* target = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(guid, data->id, HIGHGUID_UNIT), (Creature*)NULL))
								active ? target->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE) : target->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
					}
					else
					{
						if (GameObjectData const* data = sObjectMgr->GetGOData(guid))
							if (GameObject* target = ObjectAccessor::GetObjectInWorld(MAKE_NEW_GUID(guid, data->id, HIGHGUID_GAMEOBJECT), (GameObject*)NULL))
								active ? target->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE) : target->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
					}
				}
			}
			
				break;
			case ACTION_TYPE_SUMMON:
			{
				int32 entry = atoi(actionParam1.c_str());

				uint32 map = me->GetMapId();
				float x = me->GetPositionX();
				float y = me->GetPositionY();
				float z = me->GetPositionZ();
				float o = me->GetOrientation(); 

				sEvent->GetTelePos(actionParam3, map, x, y, z, o);

				if (unit)
				{
					if (entry > 0)
						unit->SummonCreature(entry, x, y, z, o, TEMPSUMMON_TIMED_DESPAWN, actionParam2 * IN_MILLISECONDS);
					else
						unit->SummonGameObject(abs(entry), x, y, z, o, 0, 0, 0, 0, actionParam2);
				}	
			}
				break;
			case ACTION_TYPE_ALT_GOB:
				if (GameObject* gob = me->FindNearestGameObject(abs(atoi(actionParam1.c_str())), actionParam2))
					gob->SetGoState(GO_STATE_ACTIVE_ALTERNATIVE);
			break;
			case ACTION_TYPE_ALT_PHASE:
				SetPhase(atoi(actionParam1.c_str()));
				break;
			case ACTION_TYPE_MOVE_RANDOM:
				me->GetMotionMaster()->MoveRandom(atoi(actionParam1.c_str()));
				break;
			default:
				break;
			}
		}

		void SetPhase(uint32 phase){ m_phase = phase; }
		uint32 GetPhase(){ return m_phase; }
	private:
		EventMap events;
		uint32 m_phase;	
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new CustomCreatureScriptAI(creature);
	}
};

void AddSC_CustomCreatureScript()
{
	new CustomCreatureScript();
}

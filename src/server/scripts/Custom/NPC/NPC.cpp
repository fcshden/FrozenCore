#pragma execution_character_set("utf-8")
#include "NPC.h"
#include "../CustomEvent/Event.h"

std::unordered_map<uint32/*entry*/, std::unordered_map<uint32/*index*/, StoryTemplate> > StoryMap;

void Story::Load()
{
	StoryMap.clear();

	//QueryResult result = WorldDatabase.PQuery("SELECT entry,id,action,param1,param2 FROM _npc");
	//
	//if (!result) 
	//	return;
	//do
	//{
	//	Field* fields = result->Fetch();
	//	uint32 entry = fields[0].GetUInt32();
	//	uint32 index = fields[1].GetUInt32();
	//	const char*  s = fields[2].GetCString();
	//
	//	StoryAction action = STORY_ACTION_NONE;
	//
	//	if (strcmp("开始", s) == 0)
	//		action = STORY_ACTION_START;
	//	else if (strcmp("移动", s) == 0)
	//		action = STORY_ACTION_MOVE;
	//	else if (strcmp("说话", s) == 0)
	//		action = STORY_ACTION_SAY;
	//	else if (strcmp("大喊", s) == 0)
	//		action = STORY_ACTION_YELL;
	//	else if (strcmp("表情", s) == 0)
	//		action = STORY_ACTION_EMOTE;
	//	else if (strcmp("技能", s) == 0)
	//		action = STORY_ACTION_SPELL;
	//	else if (strcmp("结束", s) == 0)
	//		action = STORY_ACTION_END;
	//
	//	
	//	std::string param1 = fields[3].GetString();
	//	std::string param2 = fields[4].GetString();
	//
	//	//if (StoryMap.empty())
	//	//{
	//	//	std::unordered_map<uint32/*index*/, StoryTemplate> map;
	//	//	
	//	//	StoryTemplate Temp;
	//	//	Temp.action = action;
	//	//	Temp.param1 = param1;
	//	//	Temp.param2 = param2;
	//	//	map.insert(std::make_pair(index, Temp));
	//	//
	//	//	StoryMap.insert(std::make_pair(entry, map));
	//	//}
	//	//else
	//	//{
	//		std::unordered_map<uint32/*entry*/, std::unordered_map<uint32/*index*/, StoryTemplate> >::iterator itr = StoryMap.find(entry);
	//
	//		if (itr == StoryMap.end())
	//		{
	//			std::unordered_map<uint32/*index*/, StoryTemplate> map;
	//
	//			StoryTemplate Temp;
	//			Temp.action = action;
	//			Temp.param1 = param1;
	//			Temp.param2 = param2;
	//			map.insert(std::make_pair(index, Temp));
	//
	//			StoryMap.insert(std::make_pair(entry, map));
	//		}
	//		else
	//		{
	//
	//			StoryTemplate Temp;
	//			Temp.action = action;
	//			Temp.param1 = param1;
	//			Temp.param2 = param2;
	//			itr->second.insert(std::make_pair(index, Temp));
	//		}
	//	//}
	//
	//
	//} while (result->NextRow());
}

void Story::GetParams(uint32 entry, uint32 index, StoryAction &action, std::string &param1, std::string &param2)
{
	std::unordered_map<uint32/*entry*/, std::unordered_map<uint32/*index*/, StoryTemplate> >::iterator itr = StoryMap.find(entry);

	if (itr != StoryMap.end())
	{
		std::unordered_map<uint32/*index*/, StoryTemplate>::iterator i = itr->second.find(index);

		if (i != itr->second.end())
		{
			action = i->second.action;
			param1 = i->second.param1;
			param2 = i->second.param2;
		}
	}
}

class StoryNPC : public CreatureScript
{
public:
	StoryNPC() : CreatureScript("StoryNPC") { }

	bool OnGossipHello(Player* player, Creature* creature) override
	{
		player->PlayerTalkClass->ClearMenus();

		StoryAction action = STORY_ACTION_NONE;
		std::string param1 = "";
		std::string param2 = "";
		sStory->GetParams(creature->GetEntry(), 1, action, param1, param2);

		std::string text = param1;
		uint32 questId = atoi(param2.c_str());

		Quest const* quest = sObjectMgr->GetQuestTemplate(questId);

		if (!quest || quest && player->IsActiveQuest(questId))
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, text.c_str(), GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
			return true;
		}
			
		player->CLOSE_GOSSIP_MENU();
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		player->CLOSE_GOSSIP_MENU();
		creature->GetAI()->DoAction(1);
		return true;
	}

	struct StoryNPCAI : public ScriptedAI
	{
		StoryNPCAI(Creature* creature) : ScriptedAI(creature), Summons(me){}
		SummonList Summons;
		EventMap events;
		uint32 index = 1;
		bool _start = false;
		StoryAction action = STORY_ACTION_NONE;
		std::string param1 = "";
		std::string param2 = "";


		void DoAction(int32 i)
		{
			if (i == 1)
			{
				_start = true;
				me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
				index++;
				sStory->GetParams(me->GetEntry(), index, action, param1, param2);
				events.ScheduleEvent(index, IN_MILLISECONDS);
			}
		}

		void JustSummoned(Creature* summon) override
		{
			Summons.Summon(summon);
		}

		void Reset() override
		{
			_start = false;
			index = 1;
			action = STORY_ACTION_NONE;
			param1 = "";
			param2 = "";
			Summons.DespawnAll();
			me->GetMotionMaster()->MoveTargetedHome();
			me->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_NOT_SELECTABLE);
		}

		void MoveInLineOfSight(Unit* who)
		{
		}

		void SummonedCreatureDies(Creature* summon, Unit* killer)  override
		{
			
		}

		void UpdateAI(uint32 diff) override
		{
			if (!_start)
				return;

			events.Update(diff);
			
			if (uint32 eventid = events.ExecuteEvent())
			{
				switch (action)
				{
				case STORY_ACTION_MOVE:
				{
					uint32 posId = atoi(param1.c_str());

					std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId);

					if (it != PosMap.end())
						me->GetMotionMaster()->MoveCharge(it->second.x, it->second.y, it->second.z, 2.0);

					index++;
					sStory->GetParams(me->GetEntry(), index, action, param1, param2);
					events.ScheduleEvent(index, IN_MILLISECONDS * atoi(param2.c_str()));
				}
					break;
				case STORY_ACTION_SAY:
					me->MonsterSay(param1.c_str(), LANG_UNIVERSAL, NULL);
					index++;
					sStory->GetParams(me->GetEntry(), index, action, param1, param2);
					events.ScheduleEvent(index, IN_MILLISECONDS * atoi(param2.c_str()));
					break;
				case STORY_ACTION_YELL:
					me->MonsterYell(param1.c_str(), LANG_UNIVERSAL, NULL);
					index++;
					sStory->GetParams(me->GetEntry(), index, action, param1, param2);
					events.ScheduleEvent(index, IN_MILLISECONDS * atoi(param2.c_str()));
					break;
				case STORY_ACTION_EMOTE:
					me->HandleEmoteCommand(atoi(param1.c_str()));
					index++;
					sStory->GetParams(me->GetEntry(), index, action, param1, param2);
					events.ScheduleEvent(index, IN_MILLISECONDS * atoi(param2.c_str()));
					break;
				case STORY_ACTION_SPELL:
				{
					uint32 spellid = atoi(param1.c_str());
					DoCast(spellid);
					index++;
					sStory->GetParams(me->GetEntry(), index, action, param1, param2);
					events.ScheduleEvent(index, IN_MILLISECONDS * atoi(param2.c_str()));
				}
					break;
				case STORY_ACTION_END:
				{
					uint32 questId = atoi(param1.c_str());

					Quest const* quest = sObjectMgr->GetQuestTemplate(questId);

					if (quest)
					{
						std::list<Player*> playersNearby;
						me->GetPlayerListInGrid(playersNearby, 100.0f, true);

						if (!playersNearby.empty())
							for (std::list<Player*>::iterator iter = playersNearby.begin(); iter != playersNearby.end(); ++iter)
							{
								Player* player = *iter;

								if (!player || !player->hasQuest(questId))
									continue;
								player->CompleteQuest(questId);
							}
					}

					Reset();
				}
					return;
				}
			}
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new StoryNPCAI(creature);
	}
};

void AddSC_StoryNPC()
{
	new StoryNPC();
}

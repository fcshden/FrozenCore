#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"
#include "../CommonFunc/CommonFunc.h"


class NPC_Quest : public CreatureScript
{
public:
	NPC_Quest() : CreatureScript("NPC_Quest") { }
	struct NPC_QuestAI : public ScriptedAI
	{
		NPC_QuestAI(Creature* creature) : ScriptedAI(creature) {}

		void Reset() override {}
		void UpdateAI(uint32 diff) override
		{

		}
	};
	CreatureAI* GetAI(Creature* creature) const
	{
		return new NPC_QuestAI(creature);
	}
	//接受任务
	bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
	{
		switch (quest->GetQuestId())
		{
		break;
		//翡翠四龙
		case 100001:
			player->SetPhaseMask(1, true);//chatHandler.cpp /sleep
			player->RemoveAura(35838);
			break;
		default:
			break;
		}
		return false;
	}
	//完成任务
	bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32)
	{
		switch (quest->GetQuestId())
		{
			//翡翠四龙
		case 100001:
			player->SetPhaseMask(1, true);
			player->RemoveAura(35838);
			break;
		default:
			break;
		}
		return true;
	}
};

//抽取梦魇任务

enum Events
{
	EVENT_CALL_Y = 1,

	EVENT_Y_SAY1 = 2,//伊兰尼库斯:狡猾的精灵，你们设下可恶的圈套封印了我，现在又要来取笑我的么？梦魇暴君绝不会对你们卑躬屈膝！
	EVENT_REMULUS_SAY1 = 3,//守护者雷姆洛斯：伊兰尼库斯，你曾用梦魇之力折磨了无数无辜的受害者，这是对你应有的惩罚，现在，你有一个自我救赎的机会
	EVENT_REMULUS_SAY2 = 4, //守护者雷姆洛斯：你必须从这把剑中完全抽取出残存的梦魇印记，并发誓不再残害艾泽拉斯的无辜生命，否则我将让你陷入永久的沉睡。
	EVENT_Y_SAY2 = 5,//伊兰尼库斯:我…答应你的要求，但这个誓言中不包括对你的仇恨，我一定会让你付出代价的！
	EVENT_REMULUS_SAY3 = 6, //守护者雷姆洛斯：我同意，我会在这一直等着你。那么，开始吧

	EVENT_HOLD_SWORD = 7,
	EVENT_CLEAN_SOWRD = 8,
	EVENT_CALL_4_DK = 9,

	//库尔塔兹公爵960020 女伯爵布劳缪克斯960021 瑟里耶克爵士960022 瑞文戴尔男爵960023 伊兰尼库斯960016
	EVENT_K_SAY = 10,
	EVENT_N_SAY = 11,
	EVENT_S_SAY = 12,
	EVENT_R_SAY1 = 13,
	EVENT_Y_SAY3 = 14,
	EVENT_R_SAY2 = 15,
	EVENT_R_SAY3 = 16,
	EVENT_Y_SAY4 = 17,
	EVENT_KILL_4_DK = 18,
	EVENT_Y_LEAVE = 19,
	EVENT_DONE = 20

};

bool PureSwordStartFlag = false;

class npc_remulus : public CreatureScript
{
public:
	npc_remulus() : CreatureScript("npc_remulus") { }


	bool OnGossipHello(Player *player, Creature *creature)
	{
		if (PureSwordStartFlag) return true;
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "让我们开始", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
		player->SEND_GOSSIP_MENU(player->GetGossipTextId(creature), creature->GetGUID());
		return true;
	}
	bool OnGossipSelect(Player *player, Creature *creature, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();
		PureSwordStartFlag = true;
		player->CLOSE_GOSSIP_MENU();
		return true;
	}
	struct npc_remulusAI : public ScriptedAI
	{
		npc_remulusAI(Creature* creature) : ScriptedAI(creature) {}

		Creature *k, *n, *s, *r, *y;//库尔塔兹公爵960020 女伯爵布劳缪克斯960021 瑟里耶克爵士960022 瑞文戴尔男爵960023 伊兰尼库斯960016


		void Reset() override
		{

		}
		void UpdateAI(uint32 diff) override
		{
			if (PureSwordStartFlag)
			{
				PureSwordStartFlag = false;
				_events.ScheduleEvent(EVENT_CALL_Y, 2000);
				sWorld->SendGlobalText("事件开始", NULL);
			}
			_events.Update(diff);

			while (uint32 eventId = _events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_CALL_Y:
					y = me->SummonCreature(960016, me->GetPositionX(), me->GetPositionY() + 100, me->GetPositionZ() + 70, 0, TEMPSUMMON_TIMED_DESPAWN, 180000);
					if (y)
					{
						y->GetMotionMaster()->MovePoint(0, me->GetPositionX(), me->GetPositionY() + 40, me->GetPositionZ() + 15, false);
					}
					_events.ScheduleEvent(EVENT_Y_SAY1, 16000);
					break;
				case EVENT_Y_SAY1:
					if (y = me->FindNearestCreature(960016, 200.0f))
					{
						y->MonsterYell("是谁在召唤我？", LANG_UNIVERSAL, NULL);
						y->MonsterSay("狡猾的精灵，你们设下可恶的圈套封印了我，现在又要来取笑我的么？梦魇暴君绝不会对你们卑躬屈膝！", LANG_UNIVERSAL, NULL);

					}
					me->AddAura(63771, me);
					_events.ScheduleEvent(EVENT_REMULUS_SAY1, 5000);
					break;
				case EVENT_REMULUS_SAY1:
					me->MonsterSay("伊兰尼库斯，你曾用梦魇之力折磨了无数无辜的受害者，这是对你应有的惩罚，现在，你有一个自我救赎的机会。", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_REMULUS_SAY2, 5000);
					break;
				case EVENT_REMULUS_SAY2:
					me->MonsterSay("你必须从这把剑中完全抽取出残存的梦魇印记，并发誓不再残害艾泽拉斯的无辜生命，否则我将让你陷入永久的沉睡。", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_Y_SAY2, 5000);
					break;
				case EVENT_Y_SAY2:
					if (y = me->FindNearestCreature(960016, 200.0f))
						y->MonsterSay("我…答应你的要求，但这个誓言中不包括对你的仇恨，我一定会让你付出代价的！", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_REMULUS_SAY3, 5000);
					break;
				case EVENT_REMULUS_SAY3:
					me->MonsterSay("我同意，我会在这一直等着你。那么，开始吧。", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_CLEAN_SOWRD, 5000);
					break;
				case EVENT_CLEAN_SOWRD:
				{
					me->SummonCreature(960019, me->GetPositionX() + 1.5, me->GetPositionY() + 1, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);
					me->SummonCreature(960019, me->GetPositionX() + 1.5, me->GetPositionY() + 1, me->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 15000);

					std::list<Creature*> triggerlist;
					me->GetCreatureListWithEntryInGrid(triggerlist, 960019, 200.0f);

					for (std::list<Creature*>::iterator itr = triggerlist.begin(); itr != triggerlist.end(); ++itr)
					{
						if (Creature* target = (*itr)->FindNearestCreature(960016, 200.0f))
							(*itr)->CastSpell(target, 43151);
					}
				}
				_events.ScheduleEvent(EVENT_CALL_4_DK, 18000);
				break;
				case EVENT_CALL_4_DK:

					//库尔塔兹公爵960020 女伯爵布劳缪克斯960021 瑟里耶克爵士960022 瑞文戴尔男爵960023 伊兰尼库斯960016

					me->SummonCreature(960020, me->GetPositionX() + 3, me->GetPositionY() + 15, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
					me->SummonCreature(960021, me->GetPositionX() + 1, me->GetPositionY() + 15, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
					me->SummonCreature(960022, me->GetPositionX() - 1, me->GetPositionY() + 15, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
					me->SummonCreature(960023, me->GetPositionX() - 3, me->GetPositionY() + 16, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
					_events.ScheduleEvent(EVENT_K_SAY, 5000);
					break;
				case EVENT_K_SAY:
					if (k = me->FindNearestCreature(960020, 200.0f))
						k->MonsterSay("终于出来了，我都被圣光之力压迫的受不了了。", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_N_SAY, 5000);
					break;
				case EVENT_N_SAY:
					if (n = me->FindNearestCreature(960021, 200.0f))
						n->MonsterSay("该死的，是谁净化了灰烬使者！", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_S_SAY, 5000);
					break;
				case EVENT_S_SAY:
					if (s = me->FindNearestCreature(960022, 200.0f))
						s->MonsterSay("我要扒了那家伙的皮用来装饰我的马鞍！", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_R_SAY1, 5000);
					break;
				case EVENT_R_SAY1:
					if (r = me->FindNearestCreature(960023, 200.0f))
						r->MonsterSay("都别说了，看看眼前的是谁，梦魇暴君—伊兰尼库斯，为什么我们会在这里，你又是来干什么的。", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_Y_SAY3, 5000);
					break;
				case EVENT_Y_SAY3:
					if (y = me->FindNearestCreature(960016, 200.0f))
						y->MonsterSay("真是愚蠢透顶，你们只不过是那四个废物残存的意识罢了，是时候了结这件事了。", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_R_SAY2, 5000);
					break;
				case EVENT_R_SAY2:
					if (r = me->FindNearestCreature(960023, 200.0f))
						r->MonsterSay("你已经背叛了天灾军团么？巫妖王不会放过你的。", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_Y_SAY4, 5000);
					break;
				case EVENT_Y_SAY4:
					if (y = me->FindNearestCreature(960016, 200.0f))
						y->MonsterSay("你们只不过是当初我赐予你们主子的一丝梦魇之力罢了，这根本算不上背叛，我只不过是把力量收回而已。天启四骑士的力量会进一步被削弱。", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_KILL_4_DK, 5000);
					break;
				case EVENT_KILL_4_DK:
					if (y = me->FindNearestCreature(960016, 200.0f))
					{
						if (k = me->FindNearestCreature(960020, 200.0f)) k->setDeathState(JUST_DIED);
						if (n = me->FindNearestCreature(960021, 200.0f))
						{
							y->CastSpell(n, 74768);
							n->CastSpell(n, 67043);
							n->setDeathState(JUST_DIED);
						}
						if (s = me->FindNearestCreature(960022, 200.0f)) s->setDeathState(JUST_DIED);
						if (r = me->FindNearestCreature(960023, 200.0f)) r->setDeathState(JUST_DIED);
					}
					_events.ScheduleEvent(EVENT_Y_LEAVE, 2000);
					break;
				case EVENT_Y_LEAVE:
					if (y = me->FindNearestCreature(960016, 200.0f))
					{
						y->MonsterYell("噩梦永不终结...", LANG_UNIVERSAL, NULL);
						y->RemoveFromWorld();
					}
					_events.ScheduleEvent(EVENT_DONE, 5000);
					break;
				case EVENT_DONE:
					me->MonsterYell("带着你的自负永远离开这个世界！", LANG_UNIVERSAL, NULL);
					me->RemoveAura(63771);
					sWorld->SendGlobalText("事件结束", NULL);
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
		return new npc_remulusAI(creature);
	}
};


void AddSC_Quest_Scripts()
{
	new NPC_Quest();
	new npc_remulus();
}

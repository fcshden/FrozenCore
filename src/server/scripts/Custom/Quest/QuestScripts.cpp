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
	//��������
	bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest)
	{
		switch (quest->GetQuestId())
		{
		break;
		//�������
		case 100001:
			player->SetPhaseMask(1, true);//chatHandler.cpp /sleep
			player->RemoveAura(35838);
			break;
		default:
			break;
		}
		return false;
	}
	//�������
	bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32)
	{
		switch (quest->GetQuestId())
		{
			//�������
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

//��ȡ��������

enum Events
{
	EVENT_CALL_Y = 1,

	EVENT_Y_SAY1 = 2,//�������˹:�ƻ��ľ��飬�������¿ɶ��Ȧ�׷�ӡ���ң�������Ҫ��ȡЦ�ҵ�ô�����ʱ�������������Ǳ�����ϥ��
	EVENT_REMULUS_SAY1 = 3,//�ػ�����ķ��˹���������˹������������֮����ĥ�������޹����ܺ��ߣ����Ƕ���Ӧ�еĳͷ������ڣ�����һ�����Ҿ���Ļ���
	EVENT_REMULUS_SAY2 = 4, //�ػ�����ķ��˹����������ѽ�����ȫ��ȡ���д������ӡ�ǣ������Ĳ��ٲк�������˹���޹������������ҽ������������õĳ�˯��
	EVENT_Y_SAY2 = 5,//�������˹:�ҡ���Ӧ���Ҫ�󣬵���������в���������ĳ�ޣ���һ�������㸶�����۵ģ�
	EVENT_REMULUS_SAY3 = 6, //�ػ�����ķ��˹����ͬ�⣬�һ�����һֱ�����㡣��ô����ʼ��

	EVENT_HOLD_SWORD = 7,
	EVENT_CLEAN_SOWRD = 8,
	EVENT_CALL_4_DK = 9,

	//������ȹ���960020 Ů���������ѿ�˹960021 ɪ��Ү�˾�ʿ960022 ���Ĵ����о�960023 �������˹960016
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
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "�����ǿ�ʼ", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
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

		Creature *k, *n, *s, *r, *y;//������ȹ���960020 Ů���������ѿ�˹960021 ɪ��Ү�˾�ʿ960022 ���Ĵ����о�960023 �������˹960016


		void Reset() override
		{

		}
		void UpdateAI(uint32 diff) override
		{
			if (PureSwordStartFlag)
			{
				PureSwordStartFlag = false;
				_events.ScheduleEvent(EVENT_CALL_Y, 2000);
				sWorld->SendGlobalText("�¼���ʼ", NULL);
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
						y->MonsterYell("��˭���ٻ��ң�", LANG_UNIVERSAL, NULL);
						y->MonsterSay("�ƻ��ľ��飬�������¿ɶ��Ȧ�׷�ӡ���ң�������Ҫ��ȡЦ�ҵ�ô�����ʱ�������������Ǳ�����ϥ��", LANG_UNIVERSAL, NULL);

					}
					me->AddAura(63771, me);
					_events.ScheduleEvent(EVENT_REMULUS_SAY1, 5000);
					break;
				case EVENT_REMULUS_SAY1:
					me->MonsterSay("�������˹������������֮����ĥ�������޹����ܺ��ߣ����Ƕ���Ӧ�еĳͷ������ڣ�����һ�����Ҿ���Ļ��ᡣ", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_REMULUS_SAY2, 5000);
					break;
				case EVENT_REMULUS_SAY2:
					me->MonsterSay("��������ѽ�����ȫ��ȡ���д������ӡ�ǣ������Ĳ��ٲк�������˹���޹������������ҽ������������õĳ�˯��", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_Y_SAY2, 5000);
					break;
				case EVENT_Y_SAY2:
					if (y = me->FindNearestCreature(960016, 200.0f))
						y->MonsterSay("�ҡ���Ӧ���Ҫ�󣬵���������в���������ĳ�ޣ���һ�������㸶�����۵ģ�", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_REMULUS_SAY3, 5000);
					break;
				case EVENT_REMULUS_SAY3:
					me->MonsterSay("��ͬ�⣬�һ�����һֱ�����㡣��ô����ʼ�ɡ�", LANG_UNIVERSAL, NULL);
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

					//������ȹ���960020 Ů���������ѿ�˹960021 ɪ��Ү�˾�ʿ960022 ���Ĵ����о�960023 �������˹960016

					me->SummonCreature(960020, me->GetPositionX() + 3, me->GetPositionY() + 15, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
					me->SummonCreature(960021, me->GetPositionX() + 1, me->GetPositionY() + 15, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
					me->SummonCreature(960022, me->GetPositionX() - 1, me->GetPositionY() + 15, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
					me->SummonCreature(960023, me->GetPositionX() - 3, me->GetPositionY() + 16, me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 120000);
					_events.ScheduleEvent(EVENT_K_SAY, 5000);
					break;
				case EVENT_K_SAY:
					if (k = me->FindNearestCreature(960020, 200.0f))
						k->MonsterSay("���ڳ����ˣ��Ҷ���ʥ��֮��ѹ�ȵ��ܲ����ˡ�", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_N_SAY, 5000);
					break;
				case EVENT_N_SAY:
					if (n = me->FindNearestCreature(960021, 200.0f))
						n->MonsterSay("�����ģ���˭�����˻ҽ�ʹ�ߣ�", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_S_SAY, 5000);
					break;
				case EVENT_S_SAY:
					if (s = me->FindNearestCreature(960022, 200.0f))
						s->MonsterSay("��Ҫ�����Ǽһ��Ƥ����װ���ҵ�����", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_R_SAY1, 5000);
					break;
				case EVENT_R_SAY1:
					if (r = me->FindNearestCreature(960023, 200.0f))
						r->MonsterSay("����˵�ˣ�������ǰ����˭�����ʱ������������˹��Ϊʲô���ǻ����������������ʲô�ġ�", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_Y_SAY3, 5000);
					break;
				case EVENT_Y_SAY3:
					if (y = me->FindNearestCreature(960016, 200.0f))
						y->MonsterSay("�����޴�͸��������ֻ���������ĸ�����д����ʶ���ˣ���ʱ���˽�������ˡ�", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_R_SAY2, 5000);
					break;
				case EVENT_R_SAY2:
					if (r = me->FindNearestCreature(960023, 200.0f))
						r->MonsterSay("���Ѿ����������־���ô������������Ź���ġ�", LANG_UNIVERSAL, NULL);
					_events.ScheduleEvent(EVENT_Y_SAY4, 5000);
					break;
				case EVENT_Y_SAY4:
					if (y = me->FindNearestCreature(960016, 200.0f))
						y->MonsterSay("����ֻ�����ǵ����Ҵ����������ӵ�һ˿����֮�����ˣ�������㲻�ϱ��ѣ���ֻ�����ǰ������ջض��ѡ���������ʿ���������һ����������", LANG_UNIVERSAL, NULL);
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
						y->MonsterYell("ج�������ս�...", LANG_UNIVERSAL, NULL);
						y->RemoveFromWorld();
					}
					_events.ScheduleEvent(EVENT_DONE, 5000);
					break;
				case EVENT_DONE:
					me->MonsterYell("��������Ը���Զ�뿪������磡", LANG_UNIVERSAL, NULL);
					me->RemoveAura(63771);
					sWorld->SendGlobalText("�¼�����", NULL);
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

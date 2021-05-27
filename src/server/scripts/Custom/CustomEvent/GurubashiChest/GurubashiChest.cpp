//eventid 112
//creature guid 213732


#define CHEST_ID 1000000

enum GurubashiEvents
{
	EVENT_Gurubashi_NPC_MOVE = 1,
	EVENT_Gurubashi_NPC_SUMMON_GAMEOBJECT = 2,
	EVENT_Gurubashi_NPC_MOVE_BACK = 3,
	EVENT_Gurubashi_DONE = 4,
};

Position const POS_Gurubashi_NPC[2] =
{
	{ -13219.6f, 221.663f, 33.243f, 1.54758f },//初始位置
	{ -13207.1f, 280.8f, 21.9f, 4.2f },//宝箱刷新位置
};

class npc_gurubashi_chest : public CreatureScript
{
public:
	npc_gurubashi_chest() : CreatureScript("npc_gurubashi_chest") { }

	struct npc_gurubashi_chestAI : public ScriptedAI
	{
		npc_gurubashi_chestAI(Creature* creature) : ScriptedAI(creature){}

		void Reset() override{}
		
		void InitializeAI()
		{
			events.ScheduleEvent(EVENT_Gurubashi_NPC_MOVE, 10);
			events.ScheduleEvent(EVENT_Gurubashi_NPC_SUMMON_GAMEOBJECT, 40000);
			events.ScheduleEvent(EVENT_Gurubashi_NPC_MOVE_BACK, 45000);
		}

		void UpdateAI(uint32 diff) override
		{
			events.Update(diff);
	
			while (uint32 eventId = events.ExecuteEvent())
			{
				switch (eventId)
				{
				case EVENT_Gurubashi_NPC_MOVE:
					me->SetWalk(true);
					me->GetMotionMaster()->MovePoint(1, POS_Gurubashi_NPC[1]);
					break;
				case EVENT_Gurubashi_NPC_SUMMON_GAMEOBJECT:
				{
					me->CastSpell(me, 37665);
					uint32 gobCount = urand(50, 50);
					for (uint32 i = 0; i < gobCount; i++)
					{
						if (GameObject* chest = me->SummonGameObject(CHEST_ID, me->GetPositionX() + frand(-20, 20), me->GetPositionY() + frand(-20, 20), me->GetPositionZ(), 0, 0, 0, 0, 0, 10 * 60 * 1000))
						{
							chest->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_NOT_SELECTABLE);
						}
					}

				}
				break;
				case EVENT_Gurubashi_NPC_MOVE_BACK:
					me->GetMotionMaster()->MovePoint(2, POS_Gurubashi_NPC[0]);
					break;
				}
			}
		}
	private:
		EventMap events;
	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new npc_gurubashi_chestAI(creature);
	}
};

void AddSC_Gurubashi()
{
	new npc_gurubashi_chest();
}

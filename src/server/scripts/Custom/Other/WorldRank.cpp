#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"

class WorldRank_NPC : public CreatureScript
{
public:
	WorldRank_NPC() : CreatureScript("WorldRank_NPC") { }
	struct WorldRank_NPCAI : public ScriptedAI
	{
		WorldRank_NPCAI(Creature* creature) : ScriptedAI(creature), Summons(me){}

		SummonList Summons;
		uint32 Timer = 0;

		void UpdateAI(uint32 diff) override
		{
			Timer += diff;
			if (Timer > 1000)
			{
				Timer = 0;
				SetPlayerGossipFlag();
			}
		}

		void SetPlayerGossipFlag()
		{
			
		}
	};

	CreatureAI* GetAI(Creature* creature) const override
	{
		return new WorldRank_NPCAI(creature);
	}
};

void AddSC_WorldRank_NPC()
{
	new WorldRank_NPC();
}

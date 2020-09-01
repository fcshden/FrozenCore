#include "Common.h"
#include "DatabaseEnv.h"
#include "DisableMgr.h"
#include "GameEventMgr.h"
#include "GossipDef.h"
#include "BotSystem.h"

void SYSSunBotTemplate::LoadData()
{
	m_sunBot.clear();
	uint32 count = 0;
	QueryResult result = WorldDatabase.Query("SELECT entry, trainer_class, trainer_race, ScriptName FROM creature_template WHERE entry > 70001 and entry < 70248");
	if (result)
	{
		do
		{
			Field *fields = result->Fetch();

			SunBotTemplate itr;
			itr.entry = fields[0].GetUInt32();
			itr.trainer_class = fields[1].GetUInt8();
			itr.trainer_race = fields[2].GetUInt8();
			itr.scriptname = fields[3].GetString();
			++count;
			m_sunBot[itr.entry] = itr;
		} while (result->NextRow());
	}
	sLog->outString(">> Loaded `creature_template` Data. %u", count);
}

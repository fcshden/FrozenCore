#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "ArenaTeam.h"
#include "ArenaTeamMgr.h"
#include "Chat.h"
#include "Common.h"
#include "DatabaseEnv.h"
#include "DisableMgr.h"
#include "GameEventMgr.h"
#include "GossipDef.h"
#include "GroupMgr.h"
#include "GuildMgr.h"
#include "InstanceSaveMgr.h"
#include "Language.h"
#include "LFGMgr.h"
#include "Log.h"
#include "MapManager.h"
#include "ObjectMgr.h"
#include "Pet.h"
#include "PoolMgr.h"
#include "ReputationMgr.h"
#include "SpellAuras.h"
#include "Spell.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "Transport.h"
#include "UpdateMask.h"
#include "Util.h"
#include "Vehicle.h"
#include "WaypointManager.h"
#include "World.h"
#include "BotSystem.h"
#include "Player.h"
#pragma execution_character_set("utf-8")

NPCBotTemplate::NPCBotTemplate()
{

}

NPCBotTemplate::~NPCBotTemplate()
{

}

void NPCBotTemplate::LoadData()
{
    m_sunBot.clear();
    uint32 count = 0;
    QueryResult result = WorldDatabase.Query("SELECT entry, trainer_class, trainer_race, ScriptName FROM creature_template WHERE entry > 80000 and entry < 80248");
    if (result)
    {
        do
        {
            Field *fields = result->Fetch();

            BotTemplate itr;
            itr.entry = fields[0].GetUInt32();
            itr.trainer_class = fields[1].GetUInt8();
            itr.trainer_race = fields[2].GetUInt8();
            itr.scriptname = fields[3].GetString();
            ++count;
            m_sunBot[itr.entry] = itr;
        } while (result->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_NPCBOT_],共%u条数据读取加载...", count);
}

NpcBotsDataTemplate  NPCBotTemplate::GetNPCBotTemplate(uint32 bclass, std::string scriptname, uint32 brace1, uint32 brace2, uint32 brace3, uint32 brace4, uint32 brace5)
{
	NpcBotsDataTemplate npcBotsData;
	for (NPCBotTemplateMap::const_iterator itr = m_sunBot.begin(); itr != m_sunBot.end(); ++itr)
	{
        BotTemplate bb = itr->second;
		if (bb.trainer_class == bclass && bb.scriptname == scriptname && (bb.trainer_race == brace1 || bb.trainer_race == brace2 || bb.trainer_race == brace3 ||	bb.trainer_race == brace4 || bb.trainer_race == brace5))
			npcBotsData.push_back(std::pair<uint32, uint8>(bb.entry, bb.trainer_race));
	}
	return npcBotsData;
}

uint32 NPCBotTemplate::GetNPCBotTemplateCount(uint32 bclass, std::string scriptname, uint32 brace1, uint32 brace2, uint32 brace3, uint32 brace4, uint32 brace5)
{
	uint32 count = 0;
	for (NPCBotTemplateMap::const_iterator itr = m_sunBot.begin(); itr != m_sunBot.end(); ++itr)
	{
        BotTemplate bb = itr->second;
		if (bb.trainer_class == bclass && bb.scriptname == scriptname && (bb.trainer_race == brace1 || bb.trainer_race == brace2 || bb.trainer_race == brace3 || bb.trainer_race == brace4 || bb.trainer_race == brace5))
			count = count + 1;
	}
	return count;
}

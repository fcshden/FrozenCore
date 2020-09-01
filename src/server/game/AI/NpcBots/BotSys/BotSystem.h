#ifndef _SUNMGR_H
#define _SUNMGR_H

#include "Log.h"
#include "Object.h"
#include "Bag.h"
#include "Creature.h"
#include "DynamicObject.h"
#include "GameObject.h"
#include "TemporarySummon.h"
#include "Corpse.h"
#include "QuestDef.h"
#include "NPCHandler.h"
#include "DatabaseEnv.h"
#include "Mail.h"
#include "Map.h"
#include "ObjectAccessor.h"
#include "ObjectDefines.h"
#include <ace/Singleton.h>
#include "VehicleDefines.h"
#include <string>
#include <map>
#include <limits>
#include "ConditionMgr.h"
#include <functional>

struct BotTemplate
{
	uint32 entry;
	uint32 trainer_race;
	uint32 trainer_class;
	std::string scriptname;
};

typedef std::unordered_map<uint32, BotTemplate>NPCBotTemplateMap;
typedef std::list< std::pair<uint32, uint8> > NpcBotsDataTemplate;

class NPCBotTemplate
{
	friend class PlayerDumpReader;
	friend class ACE_Singleton<NPCBotTemplate, ACE_Null_Mutex>;
private:
    NPCBotTemplate();
	~NPCBotTemplate();
public:

	void LoadData();
	NpcBotsDataTemplate  GetNPCBotTemplate(uint32 bclass, std::string scriptname, uint32 brace1, uint32 brace2, uint32 brace3, uint32 brace4, uint32 brace5);
	uint32 GetNPCBotTemplateCount(uint32 bclass, std::string scriptname, uint32 brace1, uint32 brace2, uint32 brace3, uint32 brace4, uint32 brace5);

private:

    NPCBotTemplateMap		m_sunBot;
	NpcBotsDataTemplate npcBotsData;
};

#define sBotTemplate		ACE_Singleton<NPCBotTemplate,ACE_Null_Mutex>::instance()
#endif

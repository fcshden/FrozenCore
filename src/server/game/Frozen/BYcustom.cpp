#include "BYcustom.h"
#include "Player.h"
#include "Chat.h"
#include "Group.h"
#include "CreatureTextMgr.h"
#include "GameEventMgr.h"
#include "Transport.h"
#include "DisableMgr.h"
#include "Language.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "BattlefieldWG.h"
#include "Battleground.h"
#include "BattlegroundAV.h"
#include "BattlegroundMgr.h"
#include "AuctionHouseMgr.h"
#include "ReputationMgr.h"
#include "SpellAuras.h"
#include "SpellAuraEffects.h"
#include "Config.h"
#include "GuildMgr.h"
#include "ScriptedAI/ScriptedGossip.h"
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <string>
#include "Wininet.h"
#include <time.h>
#pragma comment(lib,"Wininet.lib")
#pragma execution_character_set("utf-8")
#include "BotSystem.h"

CustomMgr::CustomMgr()
{

}

CustomMgr::~CustomMgr()
{

}

void CustomMgr::LoadAllCustomData()
{
    sBotTemplate->LoadData();
}

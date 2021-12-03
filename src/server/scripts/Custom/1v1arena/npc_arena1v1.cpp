/*
 *
 * Copyright (C) 2013 Emu-Devstore <http://emu-devstore.com/>
 * Written by Teiby <http://www.teiby.de/>
 *
 */
#pragma execution_character_set("utf-8")
#include "ScriptMgr.h"
#include "ArenaTeamMgr.h"
#include "Common.h"
#include "DisableMgr.h"
#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "ArenaTeam.h"
#include "Language.h"
#include "Config.h"
#include "../Custom/String/myString.h"
 //Config
std::vector<uint32> forbiddenTalents;

class configloader_1v1arena : public WorldScript
{
public:
    configloader_1v1arena() : WorldScript("configloader_1v1arena") {}


    virtual void OnAfterConfigLoad(bool Reload) override
    {
        std::string blockedTalentsStr = sConfigMgr->GetStringDefault("Arena.1v1.ForbiddenTalentsIDs", "");
        Tokenizer toks(blockedTalentsStr, ',');
        for (auto&& token : toks)
        {
            forbiddenTalents.push_back(std::stoi(token));
        }
    }

};

class npc_1v1arena : public CreatureScript
{
public:
    npc_1v1arena() : CreatureScript("npc_1v1arena")
    {
    }


    bool JoinQueueArena(Player* player, Creature* me, bool isRated)
    {
        if(!player || !me)
            return false;

        if(sWorld->getIntConfig(CONFIG_ARENA_1V1_MIN_LEVEL) > player->getLevel())
            return false;

		uint64 guid = player->GetGUID();
        uint8 arenaslot = ArenaTeam::GetSlotByType(ARENA_TEAM_1v1);
        uint8 arenatype = ARENA_TYPE_1v1;
        uint32 arenaRating = 0;
        uint32 matchmakerRating = 0;

        // ignore if we already in BG or BG queue
        if (player->InBattleground())
            return false;

        //check existance
        Battleground* bg = sBattlegroundMgr->GetBattlegroundTemplate(BATTLEGROUND_AA);
        if (!bg)
        {
            sLog->outError("Arena", "Battleground: template bg (all arenas) not found");
            return false;
        }

        if (DisableMgr::IsDisabledFor(DISABLE_TYPE_BATTLEGROUND, BATTLEGROUND_AA, NULL))
        {
            ChatHandler(player->GetSession()).PSendSysMessage(LANG_ARENA_DISABLED);
            return false;
        }

		BattlegroundTypeId bgTypeId = bg->GetBgTypeID();
        BattlegroundQueueTypeId bgQueueTypeId = BattlegroundMgr::BGQueueTypeId(bgTypeId, arenatype);
        PvPDifficultyEntry const* bracketEntry = GetBattlegroundBracketByLevel(bg->GetMapId(), player->getLevel());
        if (!bracketEntry)
            return false;

        GroupJoinBattlegroundResult err = ERR_GROUP_JOIN_BATTLEGROUND_FAIL;

        // check if already in queue
        if (player->GetBattlegroundQueueIndex(bgQueueTypeId) < PLAYER_MAX_BATTLEGROUND_QUEUES)
            //player is already in this queue
            return false;
        // check if has free queue slots
        if (!player->HasFreeBattlegroundQueueId())
            return false;

        uint32 ateamId = 0;

        if(isRated)
        {
            ateamId = player->GetArenaTeamId(arenaslot);
            ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(ateamId);
            if (!at)
            {
                player->GetSession()->SendNotInArenaTeamPacket(arenatype);
                return false;
            }

            // get the team rating for queueing
            arenaRating = at->GetRating();
            matchmakerRating = arenaRating;
            // the arenateam id must match for everyone in the group

            if (arenaRating <= 0)
                arenaRating = 1;
        }

        BattlegroundQueue &bgQueue = sBattlegroundMgr->GetBattlegroundQueue(bgQueueTypeId);
        bg->SetRated(isRated);

		GroupQueueInfo* ginfo = bgQueue.AddGroup(player, NULL, bracketEntry, isRated, false, arenaRating, matchmakerRating, ateamId);
        uint32 avgTime = bgQueue.GetAverageQueueWaitTime(ginfo);
        uint32 queueSlot = player->AddBattlegroundQueueId(bgQueueTypeId);

        WorldPacket data;
        // send status packet (in queue)
		sBattlegroundMgr->BuildBattlegroundStatusPacket(&data, bg, queueSlot, STATUS_WAIT_QUEUE, avgTime, 0, arenatype, TEAM_NEUTRAL, isRated);
        player->GetSession()->SendPacket(&data);
		if (ateamId)
			sBattlegroundMgr->ScheduleArenaQueueUpdate(ateamId, bgQueueTypeId, bracketEntry->GetBracketId());

        return true;
    }


    bool CreateArenateam(Player* player, Creature* me, std::string teamName)
    {
        if(!player || !me)
            return false;

        uint8 slot = ArenaTeam::GetSlotByType(ARENA_TEAM_1v1);
        if (slot == 0)
            return false;

        // Check if player is already in an arena team
        if (player->GetArenaTeamId(slot))
        {
            player->GetSession()->SendArenaTeamCommandResult(ERR_ARENA_TEAM_CREATE_S, player->GetName(), "", ERR_ALREADY_IN_ARENA_TEAM);
            return false;
        }

        // Create arena team
        ArenaTeam* arenaTeam = new ArenaTeam();

        if (!arenaTeam->Create(player->GetGUID(), ARENA_TEAM_1v1, teamName, 4283124816, 45, 4294242303, 5, 4294705149))
        {
            delete arenaTeam;
            return false;
        }

        // Register arena team
        sArenaTeamMgr->AddArenaTeam(arenaTeam);
        arenaTeam->AddMember(player->GetGUID());

        ChatHandler(player->GetSession()).SendSysMessage("竞技场队伍创建完成!");

        return true;
    }

    bool Arena1v1CheckTalents(Player* player)
    {
        if (!player)
            return false;

        if (sWorld->getBoolConfig(CONFIG_ARENA_1V1_BLOCK_FORBIDDEN_TALENTS) == false)
            return true;

        uint32 count = 0;

        for (uint32 talentId = 0; talentId < sTalentStore.GetNumRows(); ++talentId)
        {
            TalentEntry const* talentInfo = sTalentStore.LookupEntry(talentId);

            if (!talentInfo)
                continue;

            if (std::find(forbiddenTalents.begin(), forbiddenTalents.end(), talentInfo->TalentID) != forbiddenTalents.end())
            {
                ChatHandler(player->GetSession()).SendSysMessage("你有1v1竞技场被禁止的天赋,不能加入1V1竞技场.");
                return false;
            }

            for (int8 rank = MAX_TALENT_RANK - 1; rank >= 0; --rank)
                if (talentInfo->RankID[rank] == 0)
                    continue;
        }

        uint32 talentsss = sConfigMgr->GetIntDefault("Arena.1v1.ForbiddenTalentpoints", 35);
        if (count >= talentsss)
        {
            ChatHandler(player->GetSession()).PSendSysMessage("你在治疗或者坦克天赋中投入超过%u点天赋,被禁止进入1V1竞技场", talentsss);
            return false;
        }

        return true;
    }

    bool OnGossipHello(Player* player, Creature* me)
    {
        if(!player || !me)
            return true;

		if (sWorld->getBoolConfig(CONFIG_ARENA_1V1_ENABLE) == false)//如果读取不了conf值,那就暂时改为true
        {
            ChatHandler(player->GetSession()).SendSysMessage("1VS1已被禁止");
            return true;
        }

        uint32 eventid = sConfigMgr->GetIntDefault("Arena.1v1.Event", 130);
        if (!sGameEventMgr->IsActiveEvent(eventid))
        {
            if (!eventid)
            {
                ChatHandler(player->GetSession()).SendSysMessage("错误的事件ID");
                return true;
            }
            else
            {
                GameEventMgr::GameEventDataMap const& events = sGameEventMgr->GetEventMap();

                if (eventid >= events.size())
                {
                    ChatHandler(player->GetSession()).SendSysMessage("错误的事件ID");
                    return true;
                }

                GameEventData const& eventData = events[eventid];
                if (!eventData.isValid())
                {
                    ChatHandler(player->GetSession()).SendSysMessage("错误的事件ID");
                    return true;
                }

                uint32 diff = sGameEventMgr->NextCheck(eventid);  //80
                std::string timestd = secsToTimeString(diff, true);
                ChatHandler(player->GetSession()).PSendSysMessage("1V1活动将在%s后开始", timestd.c_str());
                return true;
            }
        }

		if (sWorld->getIntConfig(CONFIG_ARENA_1V1_MIN_LEVEL) > player->getLevel())//增加等级过滤
		{
			ChatHandler(player->GetSession()).PSendSysMessage("你的等级需要达到%u，快去练级！", sWorld->getIntConfig(CONFIG_ARENA_1V1_MIN_LEVEL));
			player->CLOSE_GOSSIP_MENU();
			return true;
		}
		
        if(player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_1v1))
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "离开1V1竞技场队列", GOSSIP_SENDER_MAIN, 3, "你确定吗?", 0, false);
        else
            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "加入1V1竞技场(练习赛)", GOSSIP_SENDER_MAIN, 20);

        if(player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_1v1)) == 0)
            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "创建1V1竞技场队伍", GOSSIP_SENDER_MAIN, 1, "请在兑换验证码中输入你想创建的战队名\n\n是否花费以下货币创建战队\n\n你确定吗?", sWorld->getIntConfig(CONFIG_ARENA_1V1_COSTS), true);
        else
        {
            if(player->InBattlegroundQueueForBattlegroundQueueType(BATTLEGROUND_QUEUE_1v1) == false)
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "加入1V1竞技场(竞技赛)", GOSSIP_SENDER_MAIN, 2);
                player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "删除你的竞技场队伍", GOSSIP_SENDER_MAIN, 5, "你确定吗?\n\n删除后所有1v1数据全部清理!!!", 0, false);
            }

            player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "查阅1V1竞技场统计数据", GOSSIP_SENDER_MAIN, 4);
        }

        //player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Script Info", GOSSIP_SENDER_MAIN, 8);
        player->SEND_GOSSIP_MENU(68, me->GetGUID());
        return true;
    }

    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
    {
        if (!player || !creature)
            return true;

        ClearGossipMenuFor(player);

        ChatHandler handler(player->GetSession());

        if (action == 1)
        {
            if (sWorld->getIntConfig(CONFIG_ARENA_1V1_MIN_LEVEL) <= player->getLevel())
            {
                if (player->GetMoney() >= sWorld->getIntConfig(CONFIG_ARENA_1V1_COSTS) && CreateArenateam(player, creature, code))
                {
                    player->ModifyMoney(sWorld->getIntConfig(CONFIG_ARENA_1V1_COSTS) * -1);
                    OnGossipHello(player, creature);
                    return true;
                } 
            }
            else
            {
                ChatHandler(player->GetSession()).PSendSysMessage("你必须达到%u+级别才能创建1v1竞技场团队 .", sWorld->getIntConfig(CONFIG_ARENA_1V1_MIN_LEVEL));
            }
        }
        CloseGossipMenuFor(player);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* me, uint32 /*uiSender*/, uint32 uiAction)
    {
        if(!player || !me)
            return true;

        player->PlayerTalkClass->ClearMenus();

        switch (uiAction)
        {
        case 2: // Join Queue Arena (rated)
            {
                if(Arena1v1CheckTalents(player) && JoinQueueArena(player, me, true) == false)
                    ChatHandler(player->GetSession()).SendSysMessage("加入队列发生了部分错误.");

                player->CLOSE_GOSSIP_MENU();
                return true;
            }
            break;

        case 20: // Join Queue Arena (unrated)
            {
                if(Arena1v1CheckTalents(player) && JoinQueueArena(player, me, false) == false)
                    ChatHandler(player->GetSession()).SendSysMessage("加入队列发生了部分错误.");

                player->CLOSE_GOSSIP_MENU();
                return true;
            }
            break;

        case 3: // Leave Queue
            {
                WorldPacket Data;
                Data << (uint8)0x1 << (uint8)0x0 << (uint32)BATTLEGROUND_AA << (uint16)0x0 << (uint8)0x0;
                player->GetSession()->HandleBattleFieldPortOpcode(Data);
                player->CLOSE_GOSSIP_MENU();
                return true;
            }
            break;

        case 4: // get statistics
            {
                ArenaTeam* at = sArenaTeamMgr->GetArenaTeamById(player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_1v1)));
                if(at)
                {
                    std::string sendmess = sString->Format(sString->GetText(CORE_STR_TYPES(98)), at->GetName().c_str(), at->GetStats().Rating, at->GetStats().Rank, at->GetStats().SeasonGames, at->GetStats().SeasonWins, at->GetStats().WeekGames, at->GetStats().WeekWins);
                    sString->Replace(sendmess, "@", "\n");
                    ChatHandler(player->GetSession()).PSendSysMessage(sendmess.c_str());
                }
				player->CLOSE_GOSSIP_MENU();
				return true;
            }
            break;


        case 5: // Disband arenateam
            {
                WorldPacket Data;
                Data << (uint32)player->GetArenaTeamId(ArenaTeam::GetSlotByType(ARENA_TEAM_1v1));
                player->GetSession()->HandleArenaTeamLeaveOpcode(Data);
                ChatHandler(player->GetSession()).SendSysMessage("1V1竞技场队伍被删除!");
                player->CLOSE_GOSSIP_MENU();
                return true;
            }
            break;

        case 8: // Script Info
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Developer: sunwellcore", GOSSIP_SENDER_MAIN, uiAction);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Website: www.iwowo.top", GOSSIP_SENDER_MAIN, uiAction);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Version: 1.-", GOSSIP_SENDER_MAIN, uiAction);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "<-- Back", GOSSIP_SENDER_MAIN, 7);
                player->SEND_GOSSIP_MENU(68, me->GetGUID());
                return true;
            }
            break;

        }

        OnGossipHello(player, me);
        return true;
    }
};

class eventstop : public GameEventScript
{
public:
    eventstop() : GameEventScript("1v1arena_eventstop") {}

    virtual void OnStop(uint16 event_id) override
    {
        if (event_id == sConfigMgr->GetIntDefault("Arena.1v1.Event", 130))
        {
            sArenaTeamMgr->DistributeArenaPoints();
        }
    }
};

void AddSC_npc_1v1arena()
{
    new npc_1v1arena();
    new configloader_1v1arena();
    new eventstop();
}

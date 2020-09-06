#ifndef __CUSTOM_H
#define __CUSTOM_H
#include "Common.h"
#include "Timer.h"
#include <ace/Singleton.h>
#include <ace/Atomic_Op.h>
#include "SharedDefines.h"
#include "QueryResult.h"
#include "Callback.h"
#include "Util.h"
#include <atomic>
#include <map>
#include <set>
#include <list>
#pragma execution_character_set("utf-8")
#include "LootMgr.h"
#include "Item.h"
#include "ObjectMgr.h"
#include "AchievementMgr.h"
#include "Player.h"
#include "need.h"
#include "rew.h"

class CustomMgr
{
public: //完全访问权限
	CustomMgr();
	~CustomMgr();

    std::string Format(char const *format, ...)
    {
        if (format)
        {
            va_list ap;
            char str[2048];
            va_start(ap, format);
            vsnprintf(str, 2048, format, ap);
            va_end(ap);

            std::string msg(str);
            return str;
        }
        return NULL;
    }

    //获取成就点
    uint32 GetAchievementPoints(Player* player)
    {
        CompletedAchievementMap cam = player->getAchievementMgr()->get_m_completedAchievements();
        uint32 points = 0;
        for (CompletedAchievementMap::const_iterator itr = cam.begin(); itr != cam.end(); ++itr)
            if (AchievementEntry const* completedAchievements = sAchievementStore.LookupEntry(itr->first))
                points += completedAchievements->points;
        return points;
    }

    std::string GetAchievementName(uint32 achievementid)
    {
        AchievementEntry const* achievement = sAchievementStore.LookupEntry(achievementid);
        if (achievement)
            return achievement->name[LOCALE_zhCN];

        return "成就ID设置错误";
    }

    bool HasAchievement(uint32 achievementid)
    {
        AchievementEntry const* achievement = sAchievementStore.LookupEntry(achievementid);
        if (achievement)
            return true;
        return false;
    }

    bool HasQuestInCom(Player* player, uint32 inquestid)
    {
        for (uint8 i = 0; i < MAX_QUEST_LOG_SIZE; ++i)
        {
            if (inquestid == player->GetQuestSlotQuestId(i))
            {
                Quest const* qInfo = sObjectMgr->GetQuestTemplate(inquestid);
                if (qInfo)
                {
                    if (player->GetQuestStatus(inquestid) == QUEST_STATUS_INCOMPLETE)
                        return true;
                }
            }
        }
        return false;
    }

    std::string GetQuestName(uint32 questid) const
    {
        Quest const* qInfo = sObjectMgr->GetQuestTemplate(questid);
        if (qInfo)
        {
            std::ostringstream str;
            str << "|cFFEFFD00|Hquest:" << qInfo->GetQuestId() << ':' << qInfo->GetQuestLevel() << "|h[" << qInfo->GetTitle() << "]|h|r";
            return str.str();
        }
        return "未知任务";
    }

    std::string GetItemName(uint32 itemid) const
    {
        if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid))
            return proto->GetINameLink();

        return "错误的物品ID";
    }

    void LoadPlayStatFrom(Player* pl, uint32 playguid);
    void SavePlayStatTo(Player* pl, uint32 playguid);
    void RewardItem(Player *plr, uint32 item_id, uint32 count);
    void SendRewardMarkByMail(Player *plr, uint32 mark, uint32 count);
    void DoCommand(Player* player, std::string command);
    void LoadAllCustomData();

    //购买需求检测与消耗
    bool BuyFromNeedSys(Player* pl, uint32 needid, uint8 count) { return sNeedMgr->Candothis(pl, needid, count); }
    void BuyDeleteFromNeedSys(Player * pl, uint32 needid, uint8 count) { sNeedMgr->Needcost(pl, needid, count); }
    void SendToPlayerByRewID(Player * pl, uint32 rewid, uint8 count) { sRewMgr->SendToPlayer(pl, rewid, count); }

protected:
private:
};

#define sCustomMgr ACE_Singleton<CustomMgr, ACE_Null_Mutex>::instance()

#endif


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
#include "SpellInfo.h"
#include "customconf.h"
#include "SpellMod.h"
#include "dqsys.h"

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


            std::string aaaaaaa = std::string(str);
            std::string strsrc = "@";
            std::string strdst = "\n";
            std::string::size_type pos = 0;
            std::string::size_type srclen = strsrc.size();
            std::string::size_type dstlen = strdst.size();

            while ((pos = aaaaaaa.find(strsrc, pos)) != std::string::npos)
            {
                aaaaaaa.replace(pos, srclen, strdst);
                pos += dstlen;
            }
            return aaaaaaa;
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
    bool IsInNeedSys(uint32 needid) { return sNeedMgr->GetNeedParameters(needid); }
    void SendBuyItemUI(Player * pl, uint32 itemid, uint32 itemcount, uint32 needid);
    void SendWorldAnnText(char const *format, ...);
    void SendWorldNotifyText(char const * format, ...);
    void SendPVPText(char const * format, ...);
    bool CanSendBuyItemUI(Player * pl, uint64 vendorguid, uint32 itemid, uint32 itemcount, uint32 slot, uint8 bag, uint8 bagslot);

    // 游戏内信息链接
    uint32 getClassMask(uint32 classid) const { return 1 << (classid - 1); }
    uint32 getRaceMask(uint32 raceid) const { return 1 << (raceid - 1); }

    std::string GetClassNameInMask(uint32 classmask, uint32 classid)
    {
        std::string player_class ="";
        if (classmask & getClassMask(classid))
        {
            switch (classid)
            {
            case CLASS_WARRIOR: player_class = "战士"; break;
            case CLASS_PALADIN:    player_class = "圣骑士"; break;
            case CLASS_HUNTER:    player_class = "猎人"; break;
            case CLASS_ROGUE:    player_class = "潜行者"; break;
            case CLASS_PRIEST:    player_class = "牧师"; break;
            case CLASS_DEATH_KNIGHT:    player_class = "死亡骑士"; break;
            case CLASS_SHAMAN:    player_class = "萨满"; break;
            case CLASS_MAGE:    player_class = "法师"; break;
            case CLASS_WARLOCK:    player_class = "术士"; break;
            case CLASS_DRUID:    player_class = "德鲁伊"; break;
            default:
                break;
            }
        }
        return player_class;
    }

    std::string GetRaceNameInMask(uint32 racemask, uint32 raceid)
    {
        std::string player_race = "";
        if (racemask & getRaceMask(raceid))
        {
            switch (raceid)
            {
            case RACE_HUMAN: player_race = "人类"; break;
            case RACE_ORC: player_race = "兽人"; break;
            case RACE_DWARF: player_race = "侏儒"; break;
            case RACE_NIGHTELF:    player_race = "暗夜精灵"; break;
            case RACE_UNDEAD_PLAYER:    player_race = "亡灵"; break;
            case RACE_TAUREN:    player_race = "牛头人"; break;
            case RACE_GNOME:    player_race = "矮人"; break;
            case RACE_TROLL:    player_race = "巨魔"; break;
            case RACE_BLOODELF:    player_race = "血精灵"; break;
            case RACE_DRAENEI:    player_race = "德莱尼"; break;
            default:
                break;
            }
        }
        return player_race;
    }

    std::string GetSpellLink(uint32 spellid)
    {
        SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellid);
        if (!spellInfo)
            return "错误的技能ID";

        std::ostringstream ss_name;
        ss_name << "|cffffffff|Hspell:" << spellid << "|h[" << spellInfo->SpellName[LOCALE_zhCN] << "]|h|r";
        return ss_name.str();
    }

    std::string GetAchLink(Player* pl, uint32 achid)
    {
        AchievementEntry const* achievement = sAchievementStore.LookupEntry(achid);
        if (!achievement)
            return "错误的成就ID";

        std::ostringstream achname;
        achname << "|cffffff00|Hachievement:" << achid << ":" << pl->GetGUID() << ":0:0:0:-1:0:0:0:0|h[" << achievement->name[LOCALE_zhCN] << "]|h|r";
        return achname.str();
    }

    std::string GetQuestLink(uint32 questid) const
    {
        Quest const* qInfo = sObjectMgr->GetQuestTemplate(questid);
        if (!qInfo)
            return "错误的任务ID";

        std::ostringstream str;
        str << "|cFFEFFD00|Hquest:" << qInfo->GetQuestId() << ':' << qInfo->GetQuestLevel() << "|h[" << qInfo->GetTitle() << "]|h|r";
        return str.str();
    }

    std::string GetItemLink(uint32 itemid) const
    {
        if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemid))
            return proto->GetINameLink();

        return "错误的物品ID";
    }
    // 游戏内信息链接结束
    //配置文件读取
    std::string GetSendText(uint32 entry, ...) const
    {
        if (sConfMgr->Get2Conftxt(entry).c_str())
        {
            va_list ap;
            char szStr[1024];
            szStr[0] = '\0';
            va_start(ap, entry);
            vsnprintf(szStr, 1024, sConfMgr->Get2Conftxt(entry).c_str(), ap);
            va_end(ap);

            std::string aaaaaaa = std::string(szStr);
            std::string strsrc = "@";
            std::string strdst = "\n";
            std::string::size_type pos = 0;
            std::string::size_type srclen = strsrc.size();
            std::string::size_type dstlen = strdst.size();

            while ((pos = aaaaaaa.find(strsrc, pos)) != std::string::npos)
            {
                aaaaaaa.replace(pos, srclen, strdst);
                pos += dstlen;
            }
            return aaaaaaa;
        }
        return "";
    }

    bool GetConfBool(uint32 entry, uint32 index = 1) const { return  sConfMgr->GetConfBool(entry, index); }
    int32 GetConfInt(uint32 entry, uint32 index = 1) const { return  sConfMgr->GetConfInt(entry, index); }
    float GetConfFloat(uint32 entry, uint32 index = 1) const { return  sConfMgr->GetConfFloat(entry, index); }
    std::string GetConfText(uint32 entry, uint32 index = 1) const { return  sConfMgr->GetConfText(entry, index); }
    //配置文件读取结束

    //技能修改
    SpellCustomMod const * FindSpellCustomMod(uint32 spellid) const { return sSpellModMgr->FindSpellCustomMod(spellid); }
    SpellCustomLearnMod const * FindSpellLearnMod(uint32 spellid) const { return sSpellModMgr->FindSpellLearnMod(spellid); }
    SpellCustomUnLearnMod const * FindSpellUnLearnMod(uint32 spellid) const { return sSpellModMgr->FindSpellUnLearnMod(spellid); }

    int32 GetSpellModDuration(uint32 spellid)
    {
        if (FindSpellCustomMod(spellid))
            if (FindSpellCustomMod(spellid)->duration != 0)
                return FindSpellCustomMod(spellid)->duration;

        return 0;
    }

    int32 GetSpellModCastTime(uint32 spellid)
    {
        if (FindSpellCustomMod(spellid))
            if (FindSpellCustomMod(spellid)->castingtime >= 0)
                return FindSpellCustomMod(spellid)->castingtime;

        return -1;
    }

    int32 GetSpellModCooldown(uint32 spellid)
    {
        if (FindSpellCustomMod(spellid))
            if (FindSpellCustomMod(spellid)->cooldown >= 0)
                return FindSpellCustomMod(spellid)->cooldown;

        return -1;
    }

    float GetSpellModDmg(uint32 spellid)
    {
        if (FindSpellCustomMod(spellid))
            if (FindSpellCustomMod(spellid)->dmgMod != 0)
                return FindSpellCustomMod(spellid)->dmgMod;

        return 1.0f;
    }

    float GetSpellModHeal(uint32 spellid)
    {
        if (FindSpellCustomMod(spellid))
            if (FindSpellCustomMod(spellid)->healMod != 0)
                return FindSpellCustomMod(spellid)->healMod;

        return 1.0f;
    }

    bool CheckSpellCast(Player* pl, uint32 spellid)
    {
        if (FindSpellCustomMod(spellid))
            if (FindSpellCustomMod(spellid)->reqId != 0)
                return sNeedMgr->Candothis(pl, FindSpellCustomMod(spellid)->reqId, 1);

        return true;
    }

    void DeleteAndRewSpellCast(Player* pl, uint32 spellid)
    {
        if (FindSpellCustomMod(spellid))
        {
            if (FindSpellCustomMod(spellid)->reqId != 0)
                sNeedMgr->Needcost(pl, FindSpellCustomMod(spellid)->reqId, 1);
            if (FindSpellCustomMod(spellid)->rewId != 0)
                sRewMgr->SendToPlayer(pl, FindSpellCustomMod(spellid)->rewId, 1);
        } 
    }

    bool CheckSpellDisable(Player* pl, uint32 spellid)
    {
        if (FindSpellCustomMod(spellid))
            return FindSpellCustomMod(spellid)->disable;

        return false;
    }

    bool CheckSpellLearn(Player* pl, uint32 spellid)
    {
        if (FindSpellLearnMod(spellid))
            if (FindSpellLearnMod(spellid)->reqId != 0)
                return sNeedMgr->Candothis(pl, FindSpellLearnMod(spellid)->reqId, 1);

        return true;
    }

    void DeleteAndRewSpellLearn(Player* pl, uint32 spellid)
    {
        if (FindSpellLearnMod(spellid))
        {
            if (FindSpellLearnMod(spellid)->reqId != 0)
                sNeedMgr->Needcost(pl, FindSpellLearnMod(spellid)->reqId, 1);

            if (FindSpellLearnMod(spellid)->rewId != 0)
                sRewMgr->SendToPlayer(pl, FindSpellLearnMod(spellid)->rewId, 1);
        }
            
    }

    bool CheckSpellUnLearn(Player* pl, uint32 spellid)
    {
        if (FindSpellUnLearnMod(spellid))
            if (FindSpellUnLearnMod(spellid)->reqId != 0)
                return sNeedMgr->Candothis(pl, FindSpellUnLearnMod(spellid)->reqId, 1);

        return true;
    }

    void DeleteAndRewSpellUnLearn(Player* pl, uint32 spellid)
    {
        if (FindSpellUnLearnMod(spellid))
        {
            if (FindSpellUnLearnMod(spellid)->reqId != 0)
                sNeedMgr->Needcost(pl, FindSpellUnLearnMod(spellid)->reqId, 1);

            if (FindSpellUnLearnMod(spellid)->rewId != 0)
                sRewMgr->SendToPlayer(pl, FindSpellUnLearnMod(spellid)->rewId, 1);
        } 
    }

    //技能吸血
    void HealOnDamage(Unit* caster, Unit* target, SpellInfo const* spellInfo, uint32 &damage) { sSpellModMgr->HealOnDamage(caster, target, spellInfo, damage); }
    void ShieldOnDamage(Unit* caster, Unit* target, SpellInfo const* spellInfo, uint32 &damage) { sSpellModMgr->ShieldOnDamage(caster, target, spellInfo, damage); }

    AuraStackTriggerMod const * FindAuraStackMod(uint32 spellid) { return sSpellModMgr->FindAuraStackMod(spellid); }
    AuraOnDeathMod const * FindAuraOnDeathMod(uint32 spellid) { return sSpellModMgr->FindAuraOnDeathMod(spellid); }

    bool CheckAuraOnDeath(Unit* victim, Unit* attacker)
    {
        bool PreventLastDamage = false;

        for (auto itr = sSpellModMgr->m_AuraOnDeath.begin(); itr != sSpellModMgr->m_AuraOnDeath.end(); itr++)
        {
            if (victim->HasAura((*itr)->auraid) && !victim->HasSpellCooldown((*itr)->auraid))
            {
                bool trigger = false;

                if (roll_chance_f((*itr)->SelfTriggerChance))
                {
                    trigger = true;
                    victim->CastSpell(victim, (*itr)->SelfTriggerSpell, true);
                }

                if (attacker)
                {
                    if (roll_chance_f((*itr)->AttackerTriggerChance) && victim != attacker)
                    {
                        trigger = true;
                        victim->CastSpell(attacker, (*itr)->AttackerTriggerSpell, true);
                    }
                }

                if (trigger)
                {
                    victim->AddSpellCooldown((*itr)->auraid, 0, (*itr)->CoolDown);
                    if ((*itr)->PreventLastDamage)
                    {
                        victim->SetHealth(1);
                        PreventLastDamage = true;
                    }
                }
            }
        }

        if (PreventLastDamage)
            return true;

        return false;
    }

    //加载斗气

    void AppDQlevel(Player * pl, bool applystat) { sDqSysMgr->AppDQlevel(pl, applystat); }
    void OnPlayerSelectDQ(Player * player, uint32 sender, uint32 action, std::string code) { sDqSysMgr->OnPlayerSelectDQ(player, sender, action, code); }
    void OnPlayerUpdateDQXP(Player * player) { sDqSysMgr->OnPlayerUpdateDQXP(player); }
protected:
private:
};

#define sCustomMgr ACE_Singleton<CustomMgr, ACE_Null_Mutex>::instance()

#endif


#include "need.h"
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
#include "BYcustom.h"

NeedMgr::NeedMgr()
{
    m_needParameters.clear();
}

NeedMgr::~NeedMgr()
{
    
}
 
void NeedMgr::Load()
{
    
    m_needParameters.clear();

    int count = 0;
    QueryResult needsys = WorldDatabase.PQuery("SELECT 需求模板ID,职业需求,种族需求,人物等级,会员等级,斗气等级,成就点需求,成就需求组,技能需求组,BUFF需求组,任务接取需求组,任务完成需求组,物品需求组,物品需求个数,积分消耗,金币消耗,荣誉点消耗,竞技点消耗,物品消耗组,物品消耗个数 FROM _模板_需求");
    if (needsys)
    {
        do
        {
            CNeedParameters &tmpNeed = m_needParameters[needsys->Fetch()[0].GetUInt32()];

            tmpNeed.needid = needsys->Fetch()[0].GetUInt32();
            tmpNeed.classmask = needsys->Fetch()[1].GetUInt32();
            tmpNeed.racemask = needsys->Fetch()[2].GetUInt32();
            tmpNeed.level = needsys->Fetch()[3].GetUInt32();
            tmpNeed.viplevel = needsys->Fetch()[4].GetUInt32();
            tmpNeed.dqlevel = needsys->Fetch()[5].GetUInt32();
            tmpNeed.achpoint = needsys->Fetch()[6].GetUInt32();

            Tokenizer achidneeds(needsys->Fetch()[7].GetString(), '#');
            if (achidneeds.size())
            {
                for (uint32 index = 0; index < achidneeds.size(); ++index)
                    tmpNeed.m_achneeds.push_back(atoi(achidneeds[index]));
            }

            Tokenizer spellneeds(needsys->Fetch()[8].GetString(), '#');
            if (spellneeds.size())
            {
                for (uint32 index = 0; index < spellneeds.size(); ++index)
                    tmpNeed.m_spellneeds.push_back(atoi(spellneeds[index]));
            }

            Tokenizer auraneeds(needsys->Fetch()[9].GetString(), '#');
            if (auraneeds.size())
            {
                for (uint32 index = 0; index < auraneeds.size(); ++index)
                    tmpNeed.m_auraneeds.push_back(atoi(auraneeds[index]));
            }

            Tokenizer questhasneeds(needsys->Fetch()[10].GetString(), '#');
            if (questhasneeds.size())
            {
                for (uint32 index = 0; index < questhasneeds.size(); ++index)
                    tmpNeed.m_questhasneeds.push_back((atoi(questhasneeds[index])));
            }

            Tokenizer questcomneeds(needsys->Fetch()[11].GetString(), '#');
            if (questcomneeds.size())
            {
                for (uint32 index = 0; index < questcomneeds.size(); ++index)
                    tmpNeed.m_questcomneeds.push_back(atoi(questcomneeds[index]));
            }

            Tokenizer itemneeds(needsys->Fetch()[12].GetString(), '#');
            if (itemneeds.size())
            {
                for (uint32 index = 0; index < itemneeds.size(); ++index)
                    tmpNeed.m_haveitems.push_back(atoi(itemneeds[index]));
            }

            Tokenizer itemneedcounts(needsys->Fetch()[13].GetString(), '#');
            if (itemneedcounts.size())
            {
                for (uint32 index = 0; index < itemneedcounts.size(); ++index)
                    tmpNeed.m_haveitemcounts.push_back(atoi(itemneedcounts[index]));
            }

            if (itemneeds.size() != itemneedcounts.size())
                sLog->outString(">> _模板_需求表%u段配置错误 ...", tmpNeed.needid);

            tmpNeed.jfcost = needsys->Fetch()[14].GetUInt32();
            tmpNeed.gbcost = needsys->Fetch()[15].GetUInt32();
            tmpNeed.honorcost = needsys->Fetch()[16].GetUInt32();
            tmpNeed.arenacost = needsys->Fetch()[17].GetUInt32();

            Tokenizer itemcosts(needsys->Fetch()[18].GetString(), '#');
            if (itemcosts.size())
            {
                for (uint32 index = 0; index < itemcosts.size(); ++index)
                    tmpNeed.m_itemcosts.push_back(atoi(itemcosts[index]));
            }

            Tokenizer itemcostcounts(needsys->Fetch()[19].GetString(), '#');
            if (itemcostcounts.size())
            {
                for (uint32 index = 0; index < itemcostcounts.size(); ++index)
                    tmpNeed.m_itemcostcounts.push_back(atoi(itemcostcounts[index]));
            }

            if (itemcosts.size() != itemcostcounts.size())
                sLog->outString(">> _模板_需求表%u段配置错误 ...", tmpNeed.needid);

            count++;
        } while (needsys->NextRow());
    }

    sLog->outString(">> 读取功能数据表[_模板_需求],共%u条数据读取加载...", count);
}

bool NeedMgr::Candothis(Player* pl, uint32 needid, uint8 count)
{
    const CNeedParameters * needsys = sNeedMgr->GetNeedParameters(needid);
    if (!needsys)
        return true;

    bool cando = true;

    if (needsys->classmask)
    {
        if (!(needsys->classmask & pl->getClassMask()))
        {
            std::ostringstream classnotin;
            for (int32 i = 1; i < 12; i++)
            {
                if (!sCustomMgr->GetClassNameInMask(needsys->classmask, i).empty())
                    classnotin << sCustomMgr->GetClassNameInMask(needsys->classmask, i) << "-";
            }
            cando = false;
            ChatHandler(pl->GetSession()).PSendSysMessage("    ·需要职业：%s。", classnotin.str().c_str());;
        }
    }

    if (needsys->racemask)
    {
        if (!(needsys->racemask & pl->getRaceMask()))
        {
            std::ostringstream racenotin;

            for (int32 i = 1; i < 12; i++)
            {
                if (!sCustomMgr->GetRaceNameInMask(needsys->racemask, i).empty())
                    racenotin << sCustomMgr->GetRaceNameInMask(needsys->racemask, i) << "-";
            }

            cando = false;
            ChatHandler(pl->GetSession()).PSendSysMessage("    ·需要种族：%s。", racenotin.str().c_str());
        }
    }

    if (needsys->level > pl->getLevel())
    {
        cando = false;
        ChatHandler(pl->GetSession()).PSendSysMessage("    ·需要等级%u。", needsys->level);
    }

    if (needsys->viplevel > pl->viplevel)
    {
        cando = false;
        ChatHandler(pl->GetSession()).PSendSysMessage("    ·会员等级%u。", needsys->viplevel);
    }

    if (needsys->dqlevel > pl->dq_level)
    {
        cando = false;
        ChatHandler(pl->GetSession()).PSendSysMessage("    ·斗气等级%u。", needsys->dqlevel);
    }

    if (needsys->achpoint > sCustomMgr->GetAchievementPoints(pl))
    {
        cando = false;
        ChatHandler(pl->GetSession()).PSendSysMessage("    ·成就点数%u。", needsys->achpoint);
    }

    if (needsys->m_achneeds.size())
    {
        for (int i = 0; i < needsys->m_achneeds.size(); i++)
        {
            if (sCustomMgr->HasAchievement(needsys->m_achneeds[i]))
            {
                if (!pl->HasAchieved(needsys->m_achneeds[i]))
                {
                    cando = false;
                    ChatHandler(pl->GetSession()).PSendSysMessage("    ·未完成成就%s。", sCustomMgr->GetAchLink(pl, needsys->m_achneeds[i]).c_str());
                }
            }
        }
    }

    if (needsys->m_spellneeds.size())
    {
        for (int i = 0; i < needsys->m_spellneeds.size(); i++)
        {
            if (!pl->HasSpell(needsys->m_spellneeds[i]))
            {
                cando = false;
                ChatHandler(pl->GetSession()).PSendSysMessage("    ·未学会技能%s。", sCustomMgr->GetSpellLink(needsys->m_spellneeds[i]).c_str());
            }
        }
    }

    if (needsys->m_auraneeds.size())
    {
        for (int i = 0; i < needsys->m_auraneeds.size(); i++)
        {
            if (!pl->HasSpell(needsys->m_auraneeds[i]))
            {
                cando = false;
                ChatHandler(pl->GetSession()).PSendSysMessage("    ·无BUFF%s。", sCustomMgr->GetSpellLink(needsys->m_auraneeds[i]).c_str());
            }
        }
    }

    if (needsys->m_questhasneeds.size())
    {
        for (int i = 0; i < needsys->m_questhasneeds.size(); i++)
        {
            if (!sCustomMgr->HasQuestInCom(pl, needsys->m_questhasneeds[i]))
            {
                cando = false;
                ChatHandler(pl->GetSession()).PSendSysMessage("    ·未接取任务%s。", sCustomMgr->GetQuestLink(needsys->m_questhasneeds[i]).c_str());
            }
        }
    }

    if (needsys->m_questcomneeds.size())
    {
        for (int i = 0; i < needsys->m_questcomneeds.size(); i++)
        {
            if (!pl->GetQuestRewardStatus(needsys->m_questcomneeds[i]))
            {
                cando = false;
                ChatHandler(pl->GetSession()).PSendSysMessage("    ·未完成任务%s。", sCustomMgr->GetQuestLink(needsys->m_questcomneeds[i]).c_str());
            }
        }
    }

    if (needsys->m_haveitems.size() && needsys->m_haveitems.size() == needsys->m_haveitemcounts.size())
    {
        for (int i = 0; i < needsys->m_haveitems.size(); i++)
        {
            if (!pl->HasItemCount(needsys->m_haveitems[i], needsys->m_haveitemcounts[i], true))
            {
                cando = false;
                ChatHandler(pl->GetSession()).PSendSysMessage("    ·未持有物品%s%u个。", sCustomMgr->GetItemLink(needsys->m_haveitems[i]).c_str(), needsys->m_haveitemcounts[i]);
            }
        }
    }

    if (needsys->jfcost * count > pl->jftoken)
    {
        cando = false;
        ChatHandler(pl->GetSession()).PSendSysMessage("    ·消耗积分%u。", needsys->jfcost * count);
    }

    if (needsys->gbcost * GOLD  * count > pl->GetMoney())
    {
        cando = false;
        ChatHandler(pl->GetSession()).PSendSysMessage("    ·消耗%u金。", needsys->gbcost * count);
    }

    if (needsys->honorcost * count > pl->GetHonorPoints())
    {
        cando = false;
        ChatHandler(pl->GetSession()).PSendSysMessage("    ·消耗荣誉点%u。", needsys->honorcost * count);
    }

    if (needsys->arenacost * count > pl->GetArenaPoints())
    {
        cando = false;
        ChatHandler(pl->GetSession()).PSendSysMessage("    ·消耗竞技点%u。", needsys->arenacost * count);
    }

    if (needsys->m_itemcosts.size() && needsys->m_itemcosts.size() == needsys->m_itemcostcounts.size())
    {
        for (int i = 0; i < needsys->m_itemcosts.size(); i++)
        {
            if (!pl->HasItemCount(needsys->m_itemcosts[i], needsys->m_itemcostcounts[i] * count))
            {
                cando = false;
                ChatHandler(pl->GetSession()).PSendSysMessage("    ·消耗物品%s%u个。", sCustomMgr->GetItemLink(needsys->m_itemcosts[i]).c_str(), needsys->m_itemcostcounts[i] * count);
            }
        }
    }

    return cando;
}

void NeedMgr::Needcost(Player * pl, uint32 needid, uint8 count)
{
    const CNeedParameters * needsys = sNeedMgr->GetNeedParameters(needid);
    if (!needsys)
        return;

    if (needsys->jfcost)
        pl->ModifyJf(-1 * needsys->jfcost * count);

    if (needsys->gbcost)
        pl->ModifyMoney(-1 * needsys->gbcost * GOLD * count);

    if (needsys->honorcost)
        pl->ModifyHonorPoints(-1 * needsys->honorcost * count);

    if (needsys->arenacost)
        pl->ModifyArenaPoints(-1 * needsys->arenacost * count);

    if (needsys->m_itemcosts.size() && needsys->m_itemcosts.size() == needsys->m_itemcostcounts.size())
    { 
        for (int i = 0; i < needsys->m_itemcosts.size(); i++)
            pl->DestroyItemCount(needsys->m_itemcosts[i], needsys->m_itemcostcounts[i] * count, true);
    }
}

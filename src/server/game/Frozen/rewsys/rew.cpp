#include "rew.h"
#include "Player.h"
#include "Chat.h"
#pragma execution_character_set("utf-8")
#include "BYcustom.h"

RewMgr::RewMgr()
{
    m_rewParameters.clear();
}

RewMgr::~RewMgr()
{
    
}
 
void RewMgr::Load()
{
    
    m_rewParameters.clear();

    int count = 0;
    QueryResult rewsys = WorldDatabase.PQuery("SELECT 奖励模板ID,积分,金币,荣誉点,竞技点,经验值,斗气经验值,物品组,物品个数组,技能组,光环组,GM命令组 FROM _模板_奖励");
    if (rewsys)
    {
        do
        {
            CRewParameters *tmpRew = new CRewParameters;
            
            tmpRew->rewid = rewsys->Fetch()[0].GetUInt32();
            tmpRew->jfrew = rewsys->Fetch()[1].GetUInt32();
            tmpRew->gbrew = rewsys->Fetch()[2].GetUInt32();
            tmpRew->honorrew = rewsys->Fetch()[3].GetUInt32();
            tmpRew->arenarew = rewsys->Fetch()[4].GetUInt32();
            tmpRew->xprew = rewsys->Fetch()[5].GetUInt32();
            tmpRew->dqxprew = rewsys->Fetch()[6].GetUInt32();

            Tokenizer itemrews(rewsys->Fetch()[7].GetString(), '#');
            if (itemrews.size())
            {
                for (uint32 index = 0; index < itemrews.size(); ++index)
                    tmpRew->m_items.push_back(atoi(itemrews[index]));
            }

            Tokenizer itemcountrews(rewsys->Fetch()[8].GetString(), '#');
            if (itemcountrews.size())
            {
                for (uint32 index = 0; index < itemcountrews.size(); ++index)
                    tmpRew->m_itemcounts.push_back(atoi(itemcountrews[index]));
            }

            if (itemrews.size() != itemcountrews.size())
                sLog->outString(">> _模板_奖励表%u段配置错误 ...", tmpRew->rewid);

            Tokenizer spellrews(rewsys->Fetch()[9].GetString(), '#');
            if (spellrews.size())
            {
                for (uint32 index = 0; index < spellrews.size(); ++index)
                    tmpRew->m_spellrews.push_back(atoi(spellrews[index]));
            }

            Tokenizer aurarews(rewsys->Fetch()[10].GetString(), '#');
            if (aurarews.size())
            {
                for (uint32 index = 0; index < aurarews.size(); ++index)
                    tmpRew->m_aurarews.push_back(atoi(aurarews[index]));
            }

            Tokenizer commands(rewsys->Fetch()[11].GetString(), '#');
            if (commands.size())
            {
                for (uint32 index = 0; index < commands.size(); ++index)
                    tmpRew->m_commands.push_back(commands[index]);
            }

            m_rewParameters[tmpRew->rewid] = tmpRew;

            count++;
        } while (rewsys->NextRow());
    }

    sLog->outString(">> 读取功能数据表[_模板_奖励],共%u条数据读取加载...", count);
}


void RewMgr::SendToPlayer(Player * pl, uint32 needid, uint8 count)
{
    const CRewParameters * rewsys = GetRewParameters(needid);
    if (!rewsys)
        return;

    if (rewsys->jfrew)
        pl->ModifyJf(rewsys->jfrew * count);

    if (rewsys->gbrew)
        pl->ModifyMoney(rewsys->gbrew * GOLD * count);

    if (rewsys->honorrew)
        pl->ModifyHonorPoints(rewsys->honorrew * count);

    if (rewsys->arenarew)
        pl->ModifyArenaPoints(rewsys->arenarew * count);

    if (rewsys->xprew)
        pl->GiveXP(rewsys->xprew, nullptr);

    if (rewsys->m_items.size() && rewsys->m_items.size() == rewsys->m_itemcounts.size())
        for (int i = 0; i < rewsys->m_items.size(); i++)
            sCustomMgr->RewardItem(pl, rewsys->m_items[i], rewsys->m_itemcounts[i] * count);

    if (rewsys->m_spellrews.size())
        for (int i = 0; i < rewsys->m_spellrews.size(); i++)
            if (!pl->HasSpell(rewsys->m_spellrews[i]))
                pl->learnSpell(rewsys->m_spellrews[i]);
            
    if (rewsys->m_aurarews.size())
        for (int i = 0; i < rewsys->m_aurarews.size(); i++)
            pl->CastSpell(pl, rewsys->m_aurarews[i], false);

    if (rewsys->m_commands.size())
        for (int i = 0; i < rewsys->m_commands.size(); i++)
            sCustomMgr->DoCommand(pl, rewsys->m_commands[i]);
}

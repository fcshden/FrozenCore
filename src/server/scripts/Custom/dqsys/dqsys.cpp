#pragma execution_character_set("utf-8")
#include "dqsys.h"
#include "BYcustom.h"

DqSysMgr::DqSysMgr():m_dqParameters(NULL)
{
    dqjs = 0;
    maxdqjs = 0;
    dqbj = 0;
    maxdqbj = 0;
    dqrx = 0;
    maxdqrx = 0;
    dqjz = 0;
    maxdqjz = 0;
    dqds = 0;
    maxdqds = 0;
    dqzj = 0;
    maxdqzj = 0;
    dqhj = 0;
    maxdqhj = 0;
    dqll = 0;
    maxdqll = 0;
    dqmj = 0;
    maxdqmj = 0;
    dqzl = 0;
    maxdqzl = 0;
    dqnl = 0;
    maxdqnl = 0;
    dqjs6 = 0;
    maxdqjs6 = 0;
    dqwlct = 0;
    maxdqwlct = 0;
    dqfsct = 0;
    maxdqfsct = 0;
    Custom_confs.clear();
}

DqSysMgr::~DqSysMgr()
{
    delete[]m_dqParameters;
}

void DqSysMgr::Load()
{
    Custom_confs.clear();
    QueryResult conf = WorldDatabase.Query("SELECT 配置ID,数据 FROM _斗气_配置");
    int nCount = 0;
    if (conf)
    {
        do
        {
            Field *fields = conf->Fetch();
            AddCustomConf(fields[0].GetUInt32(), fields[1].GetString());
            nCount++;

        } while (conf->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_斗气_配置],共%u条数据读取加载...", nCount);

    SetDqData();

    if (m_dqParameters)
    {
        delete[] m_dqParameters;
        m_dqParameters = NULL;
    }

    QueryResult doulevel = WorldDatabase.PQuery("SELECT 等级,斗气头衔,需求经验,奖励斗气点,奖励天赋点,升级需要物品 FROM _斗气_数据 group by 等级");
    int ncount = 0;

    if (doulevel)
    {
        m_dqlevel = doulevel->GetRowCount();
        m_dqParameters = new CDQParameters[m_dqlevel];
        do
        {
            m_dqParameters[ncount].dqlevel = doulevel->Fetch()[0].GetUInt32();
            m_dqParameters[ncount].dqtitlestring = doulevel->Fetch()[1].GetString();
            m_dqParameters[ncount].dqxp = doulevel->Fetch()[2].GetUInt32();
            m_dqParameters[ncount].dqpoint = doulevel->Fetch()[3].GetUInt32();
            m_dqParameters[ncount].addtalent = doulevel->Fetch()[4].GetUInt32();
            m_dqParameters[ncount].itemcost = doulevel->Fetch()[5].GetUInt32();

            if (m_dqParameters[ncount].itemcost)
                m_dqParameters[ncount].upcast = true;
            else
                m_dqParameters[ncount].upcast = false;

            ncount++;
        } while (doulevel->NextRow());
    }
    else
    {
        m_dqlevel = 1;
        m_dqParameters = new CDQParameters[1];
        m_dqParameters[0].dqlevel = 0;
        m_dqParameters[0].dqtitlestring = "";
        m_dqParameters[0].dqxp = 0;
        m_dqParameters[0].dqpoint = 0;
        m_dqParameters[0].itemcost = 0;
        m_dqParameters[0].upcast = false;
        m_dqParameters[0].addtalent = 0;
    }
    sLog->outString(">> 读取功能数据表[_斗气_数据],共%u条数据读取加载...", ncount);
}

void DqSysMgr::OnPlayerHelloDQ(Player *player)
{
    player->PlayerTalkClass->ClearMenus();
    player->PlayerTalkClass->GetGossipMenu().SetMenuId(10000);
    uint32 shu36 = dqjs * Getdqjs(player); //急速
    uint32 shu32 = dqbj * Getdqbj(player); //暴击
    uint32 shu35 = dqrx * Getdqrx(player); //韧性
    uint32 shu37 = dqjz * Getdqjz(player); //精准
    uint32 shu13 = dqds * Getdqds(player); //躲闪
    uint32 shu14 = dqzj * Getdqzj(player); //招架
    uint32 shu50 = dqhj * Getdqhj(player); //护甲
    uint32 shu4 = dqll * Getdqll(player);  // 力量
    uint32 shu3 = dqmj * Getdqmj(player);  //敏捷
    uint32 shu5 = dqzl * Getdqzl(player);  //智力
    uint32 shu7 = dqnl * Getdqnl(player);  //耐力
    uint32 shu6 = dqjs6 * Getdqjs6(player);  //精神
    uint32 shu44 = dqwlct * Getdqwlct(player);  //精神
    uint32 shu47 = dqfsct * Getdqfsct(player);  //精神

    char * tmp = new char[500];
    std::string m_blp, touxian;
    if (player->GetTeamId() == 0)
        m_blp = "|TInterface\\GROUPFRAME\\UI-Group-PVP-Alliance.blp:15|t";
    else
        m_blp = "|TInterface\\GROUPFRAME\\UI-Group-PVP-Horde.blp:15|t";


    touxian = GetDQParameters(Getdqlevel(player))->dqtitlestring;
    sprintf(tmp, "%s%s", m_blp.c_str(), "|cFFFF0000加点说明|r");
    player->ADD_GOSSIP_ITEM_EXTENDED(1, tmp, 4, 2999, GetConfText(1).c_str(), 0, false);


    if (Getdqlevel(player) < GetMaxdqLevel())
    {
        sprintf(tmp, "%s%s|cFF990000%u/%u|r", m_blp.c_str(), "|cFF0000FF斗气经验:|r", Getdqxp(player), GetDQParameters(Getdqlevel(player) + 1)->dqxp);
        player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
    }

    sprintf(tmp, "%s%s|cFF990000%s%s%s|r", m_blp.c_str(), "|cFF0000FF当前等级:|r", "【", touxian.c_str(), "】");
    player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);


    sprintf(tmp, "%s|cFF0000FF已加属性点:|r|cFF990000%u  |cFF0000FF剩余属性点:|r|cFF990000%u", m_blp.c_str(), Getdqshuzong(player), Getdqshuxing(player));
    player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);

    if (Getdqshuxing(player) > 0)
    {
        if (dqjs > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励急速:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu36, Getdqjs(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3001, "", 0, true);
        }
        if (dqbj > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励暴击:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu32, Getdqbj(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3002, "", 0, true);
        }
        if (dqrx > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励韧性:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu35, Getdqrx(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3003, "", 0, true);
        }
        if (dqjz > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励精准:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu37, Getdqjz(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3004, "", 0, true);
        }
        if (dqds > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励躲闪:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu13, Getdqds(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3005, "", 0, true);
        }
        if (dqzj > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励招架:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu14, Getdqzj(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3006, "", 0, true);
        }
        if (dqhj > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励护甲:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu50, Getdqhj(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3007, "", 0, true);
        }
        if (dqll > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励力量:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu4, Getdqll(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3008, "", 0, true);
        }
        if (dqmj > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励敏捷:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu3, Getdqmj(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3009, "", 0, true);
        }
        if (dqzl > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励智力:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu5, Getdqzl(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3010, "", 0, true);
        }
        if (dqnl > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励耐力:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu7, Getdqnl(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3011, "", 0, true);
        }
        if (dqjs6 > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励精神:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu6, Getdqjs6(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3012, "", 0, true);
        }
        if (dqwlct > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励护甲穿透:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu44, Getdqwlct(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3013, "", 0, true);
        }
        if (dqfsct > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励法术穿透:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u |TInterface\\BUTTONS\\UI-AttributeButton-Encourage-Up.blp:15|t", m_blp.c_str(), shu47, Getdqfsct(player));
            player->ADD_GOSSIP_ITEM_EXTENDED(10, tmp, 4, 3014, "", 0, true);
        }
    }
    else
    {
        if (dqjs > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励急速:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu36, Getdqjs(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqbj > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励暴击:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu32, Getdqbj(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqrx > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励韧性:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu35, Getdqrx(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqjz > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励精准:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu37, Getdqjz(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqds > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励躲闪:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu13, Getdqds(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqzj > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励招架:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu14, Getdqzj(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqhj > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励护甲:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu50, Getdqhj(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqll > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励力量:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu4, Getdqll(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqmj > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励敏捷:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu3, Getdqmj(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqzl > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励智力:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu5, Getdqzl(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqnl > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励耐力:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu7, Getdqnl(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqjs6 > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励精神:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu6, Getdqjs6(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }

        if (dqwlct > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励护甲穿透:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu44, Getdqwlct(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
        if (dqfsct > 0)
        {
            sprintf(tmp, "%s|cFF0000FF奖励法术穿透:|r|cFF990000%u |cFF0000FF点数:|r|cFF990000%u", m_blp.c_str(), shu47, Getdqfsct(player));
            player->ADD_GOSSIP_ITEM(10, tmp, 4, 3000);
        }
    }
    delete[]tmp;
    player->SEND_GOSSIP_MENU(20001, player->GetGUID());
}

void DqSysMgr::OnPlayerSelectDQ(Player *player, uint32 sender, uint32 action, std::string code)
{
    if (sender == 4)
    {
        if (action == 2999 || action == 3000)
        {
            OnPlayerHelloDQ(player);
            return;
        }
    }

    if (player->IsInCombat())
    {
        ChatHandler(player->GetSession()).PSendSysMessage("战斗状态,无法加点");
        OnPlayerHelloDQ(player);
        return;
    }

    uint32 xzcount = 0;
    if (!code.empty()) //如果不是空的
    {
        static const char* allowedcharacters = "1234567890";
        if (!code.length() || code.find_first_not_of(allowedcharacters) != std::string::npos)
        {
            player->GetSession()->SendNotification("请输入合法数字");
            OnPlayerHelloDQ(player);
            return;
        }

        xzcount = atol(code.c_str());
    }

    if (sender == 4)
    {
        if (xzcount == 0)
        {
            player->GetSession()->SendNotification("输入大于0的数字");
            OnPlayerHelloDQ(player);
            return;
        }

        if (xzcount > player->dq_shuxing)
        {
            player->GetSession()->SendNotification("你没有这么多点数可加.");
            OnPlayerHelloDQ(player);
            return;
        }

        if (!CheckPldqcount(player, xzcount, action))
        {
            OnPlayerHelloDQ(player);
            return;
        }

        player->Dismount();
        player->RemoveAurasByType(SPELL_AURA_MOUNTED);

        if (action == 3001) //急速加点
        {
            int32 aaaa = Getdqjs(player) * dqjs;
            player->ApplyRatingMod(CR_HASTE_MELEE, int32(aaaa), false);
            player->ApplyRatingMod(CR_HASTE_RANGED, int32(aaaa), false);
            player->ApplyRatingMod(CR_HASTE_SPELL, int32(aaaa), false);

            Modifydqjs(player, xzcount);

            int32 bbbb = Getdqjs(player) * dqjs;
            player->ApplyRatingMod(CR_HASTE_MELEE, int32(bbbb), true);
            player->ApplyRatingMod(CR_HASTE_RANGED, int32(bbbb), true);
            player->ApplyRatingMod(CR_HASTE_SPELL, int32(bbbb), true);
        }
        else if (action == 3002) //暴击加点
        {
            int32 aaaa = Getdqbj(player) * dqbj;
            player->ApplyRatingMod(CR_CRIT_MELEE, int32(aaaa), false);
            player->ApplyRatingMod(CR_CRIT_RANGED, int32(aaaa), false);
            player->ApplyRatingMod(CR_CRIT_SPELL, int32(aaaa), false);

            Modifydqbj(player, xzcount);

            int32 bbbb = Getdqbj(player) * dqbj;
            player->ApplyRatingMod(CR_CRIT_MELEE, int32(bbbb), true);
            player->ApplyRatingMod(CR_CRIT_RANGED, int32(bbbb), true);
            player->ApplyRatingMod(CR_CRIT_SPELL, int32(bbbb), true);
        }
        else if (action == 3003) //韧性加点
        {
            int32 aaaa = Getdqrx(player) * dqrx;
            player->ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(aaaa), false);
            player->ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(aaaa), false);
            player->ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(aaaa), false);


            Modifydqrx(player, xzcount);

            int32 bbbb = Getdqrx(player) * dqrx;
            player->ApplyRatingMod(CR_CRIT_TAKEN_MELEE, int32(bbbb), true);
            player->ApplyRatingMod(CR_CRIT_TAKEN_RANGED, int32(bbbb), true);
            player->ApplyRatingMod(CR_CRIT_TAKEN_SPELL, int32(bbbb), true);
        }
        else if (action == 3004) //精准加点
        {
            int32 aaaa = Getdqjz(player) * dqjz;
            player->ApplyRatingMod(CR_EXPERTISE, int32(aaaa), false);

            Modifydqjz(player, xzcount);

            int32 bbbb = Getdqjz(player) * dqjz;
            player->ApplyRatingMod(CR_EXPERTISE, int32(bbbb), true);
        }
        else if (action == 3005) //躲闪加点
        {
            int32 aaaa = Getdqds(player) * dqds;
            player->ApplyRatingMod(CR_DODGE, int32(aaaa), false);

            Modifydqds(player, xzcount);

            int32 bbbb = Getdqds(player) * dqds;
            player->ApplyRatingMod(CR_DODGE, int32(bbbb), true);
        }
        else if (action == 3006) //招架加点
        {
            int32 aaaa = Getdqzj(player) * dqzj;
            player->ApplyRatingMod(CR_PARRY, int32(aaaa), false);
            Modifydqzj(player, xzcount);

            int32 bbbb = Getdqzj(player) * dqzj;
            player->ApplyRatingMod(CR_PARRY, int32(bbbb), true);
        }
        else if (action == 3007) //护甲加点
        {
            int32 aaaa = Getdqhj(player) * dqhj;
            player->HandleStatModifier(UNIT_MOD_ARMOR, BASE_VALUE, float(aaaa), false);

            Modifydqhj(player, xzcount);

            int32 bbbb = Getdqhj(player) * dqhj;
            player->HandleStatModifier(UNIT_MOD_ARMOR, BASE_VALUE, float(bbbb), true);
        }
        else if (action == 3008) //力量加点
        {
            int32 aaaa = Getdqll(player) * dqll;
            player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, BASE_VALUE, float(aaaa), false);
            player->ApplyStatBuffMod(STAT_STRENGTH, float(aaaa), false);

            Modifydqll(player, xzcount);

            int32 bbbb = Getdqll(player) * dqll;
            player->HandleStatModifier(UNIT_MOD_STAT_STRENGTH, BASE_VALUE, float(bbbb), true);
            player->ApplyStatBuffMod(STAT_STRENGTH, float(bbbb), true);
        }
        else if (action == 3009) //敏捷加点
        {
            int32 aaaa = Getdqmj(player) * dqmj;
            player->HandleStatModifier(UNIT_MOD_STAT_AGILITY, BASE_VALUE, float(aaaa), false);
            player->ApplyStatBuffMod(STAT_AGILITY, float(aaaa), false);

            Modifydqmj(player, xzcount);

            int32 bbbb = Getdqmj(player) * dqmj;
            player->HandleStatModifier(UNIT_MOD_STAT_AGILITY, BASE_VALUE, float(bbbb), true);
            player->ApplyStatBuffMod(STAT_AGILITY, float(bbbb), true);
        }
        else if (action == 3010) //智力加点
        {
            int32 aaaa = Getdqzl(player) * dqzl;

            player->HandleStatModifier(UNIT_MOD_STAT_INTELLECT, BASE_VALUE, float(aaaa), false);
            player->ApplyStatBuffMod(STAT_INTELLECT, float(aaaa), false);
            Modifydqzl(player, xzcount);

            int32 bbbb = Getdqzl(player) * dqzl;
            player->HandleStatModifier(UNIT_MOD_STAT_INTELLECT, BASE_VALUE, float(bbbb), true);
            player->ApplyStatBuffMod(STAT_INTELLECT, float(bbbb), true);
        }
        else if (action == 3011) //耐力加点
        {
            int32 aaaa = Getdqnl(player) * dqnl;
            player->HandleStatModifier(UNIT_MOD_STAT_STAMINA, BASE_VALUE, float(aaaa), false);
            player->ApplyStatBuffMod(STAT_STAMINA, float(aaaa), false);

            Modifydqnl(player, xzcount);

            int32 bbbb = Getdqnl(player) * dqnl;
            player->HandleStatModifier(UNIT_MOD_STAT_STAMINA, BASE_VALUE, float(bbbb), true);
            player->ApplyStatBuffMod(STAT_STAMINA, float(bbbb), true);
        }
        else if (action == 3012) //精神加点
        {
            int32 aaaa = Getdqjs6(player) * dqjs6;
            player->HandleStatModifier(UNIT_MOD_STAT_SPIRIT, BASE_VALUE, float(aaaa), false);
            player->ApplyStatBuffMod(STAT_SPIRIT, float(aaaa), false);

            Modifydqjs6(player, xzcount);

            int32 bbbb = Getdqjs6(player) * dqjs6;
            player->HandleStatModifier(UNIT_MOD_STAT_SPIRIT, BASE_VALUE, float(bbbb), true);
            player->ApplyStatBuffMod(STAT_SPIRIT, float(bbbb), true);
        }
        else if (action == 3013) //精神加点
        {
            int32 aaaa = Getdqwlct(player) * dqwlct;
            player->ApplyRatingMod(CR_ARMOR_PENETRATION, int32(aaaa), false);

            Modifydqwlct(player, xzcount);

            int32 bbbb = Getdqwlct(player) * dqwlct;
            player->ApplyRatingMod(CR_ARMOR_PENETRATION, int32(bbbb), true);
        }
        else if (action == 3014) //精神加点
        {
            int32 aaaa = Getdqfsct(player) * dqfsct;
            player->ApplySpellPenetrationBonus(aaaa, false);

            Modifydqfsct(player, xzcount);

            int32 bbbb = Getdqfsct(player) * dqfsct;
            player->ApplySpellPenetrationBonus(bbbb, true);
        }
        Modifydqshuxing(player, -1 * xzcount);
        OnPlayerHelloDQ(player);
    }
}

bool DqSysMgr::CheckPldqcount(Player * player, uint32 count, uint32 type)
{
    bool setreset = false;
    if (maxdqjs && maxdqjs < Getdqjs(player))
        setreset = true;
    if (maxdqbj && maxdqbj < Getdqbj(player))
        setreset = true;
    if (maxdqrx && maxdqrx < Getdqrx(player))
        setreset = true;
    if (maxdqjz && maxdqjz < Getdqjz(player))
        setreset = true;
    if (maxdqds && maxdqds < Getdqds(player))
        setreset = true;
    if (maxdqzj && maxdqzj < Getdqzj(player))
        setreset = true;
    if (maxdqhj && maxdqhj < Getdqhj(player))
        setreset = true;
    if (maxdqll && maxdqll < Getdqll(player))
        setreset = true;
    if (maxdqmj && maxdqmj < Getdqmj(player))
        setreset = true;
    if (maxdqzl && maxdqzl < Getdqzl(player))
        setreset = true;
    if (maxdqnl && maxdqnl < Getdqnl(player))
        setreset = true;
    if (maxdqjs6 && maxdqjs6 < Getdqjs6(player))
        setreset = true;
    if (maxdqwlct && maxdqwlct < Getdqwlct(player))
        setreset = true;
    if (maxdqfsct && maxdqfsct < Getdqfsct(player))
        setreset = true;

    if (setreset) //重置属性
    {
        resetdq(player);
        player->GetSession()->SendNotification("检测到玩家斗气属性加点有BUG,重置玩家斗气属性....");
        return false;
    }

    bool canincount = true;
    if (type == 3001)
    {
        if (maxdqjs && maxdqjs < Getdqjs(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3002)
    {
        if (maxdqbj && maxdqbj < Getdqbj(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3003)
    {
        if (maxdqrx && maxdqrx < Getdqrx(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3004)
    {
        if (maxdqjz && maxdqjz < Getdqjz(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3005)
    {
        if (maxdqds && maxdqds < Getdqds(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3006)
    {
        if (maxdqzj && maxdqzj < Getdqzj(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3007)
    {
        if (maxdqhj && maxdqhj < Getdqhj(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3008)
    {
        if (maxdqll && maxdqll < Getdqll(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3009)
    {
        if (maxdqmj && maxdqmj < Getdqmj(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3010)
    {
        if (maxdqzl && maxdqzl < Getdqzl(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3011)
    {
        if (maxdqnl && maxdqnl < Getdqnl(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3012)
    {
        if (maxdqjs6 && maxdqjs6 < Getdqjs6(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3013)
    {
        if (maxdqwlct && maxdqwlct < Getdqwlct(player) + count) //斗气加点高于总点数
            canincount = false;
    }
    else if (type == 3014)
    {
        if (maxdqfsct && maxdqfsct < Getdqfsct(player) + count) //斗气加点高于总点数
            canincount = false;
    }

    if (!canincount)
        player->GetSession()->SendNotification("所加属性点大于系统配置最高可加点数,请看服务器说明...");

    return canincount;
}

void DqSysMgr::OnPlayerUpdateDQXP(Player *player)
{
    if (player->getLevel() < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        return;

    if (Getdqlevel(player) >= GetMaxdqLevel())
    {
        player->SetUInt32Value(PLAYER_NEXT_LEVEL_XP, 0);
        return;
    }

    uint32 xp = player->dq_xp;
    uint32 maxxp = GetDQParameters(Getdqlevel(player) + 1)->dqxp;
    player->SetUInt32Value(PLAYER_XP, xp);
    player->SetUInt32Value(PLAYER_NEXT_LEVEL_XP, maxxp);
}

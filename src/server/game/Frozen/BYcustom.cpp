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
#include "need.h"
#include "rew.h"
#include "customconf.h"
#include "SpellMod.h"

CustomMgr::CustomMgr()
{

}

CustomMgr::~CustomMgr()
{

}

void CustomMgr::LoadAllCustomData()
{
    sBotTemplate->LoadData();
    sNeedMgr->Load();
    sRewMgr->Load();
    sConfMgr->Load();
    sSpellModMgr->Load();
    sDqSysMgr->Load();
}

void CustomMgr::SavePlayStatTo(Player* pl, uint32 playguid) //玩家下线保存自定义属性
{
    CharacterDatabase.PExecute("UPDATE _玩家_斗气 SET 等级 = %u,剩余点数=%u,急速=%u,暴击=%u,韧性=%u,精准=%u,躲闪=%u,招架=%u,护甲=%u,力量=%u,敏捷=%u,智力=%u,耐力=%u,精神=%u,经验值 = %u,护甲穿透=%u,法术穿透=%u WHERE 玩家 = %u", pl->dq_level, pl->dq_shuxing, pl->dq_js, pl->dq_bj, pl->dq_rx, pl->dq_jz, pl->dq_ds, pl->dq_zj, pl->dq_hj, pl->dq_ll, pl->dq_mj, pl->dq_zl, pl->dq_nl, pl->dq_js6, pl->dq_xp, pl->dq_wlct, pl->dq_fsct, playguid);

}

void CustomMgr::LoadPlayStatFrom(Player* pl, uint32 playguid) // 玩家登陆查询自定义属性
{
    pl->viplevel = 0;
    pl->jftoken = 0;


    //斗气属性加载
    QueryResult lnwow_dq = CharacterDatabase.PQuery("SELECT 等级,剩余点数,急速,暴击,韧性,精准,躲闪,招架,护甲,力量,敏捷,智力,耐力,精神,经验值,护甲穿透,法术穿透 FROM _玩家_斗气 WHERE 玩家=%u", playguid);
    if (lnwow_dq)
    {
        pl->dq_level = lnwow_dq->Fetch()[0].GetUInt32();
        pl->dq_shuxing = lnwow_dq->Fetch()[1].GetUInt32();
        pl->dq_js = lnwow_dq->Fetch()[2].GetUInt32();
        pl->dq_bj = lnwow_dq->Fetch()[3].GetUInt32();
        pl->dq_rx = lnwow_dq->Fetch()[4].GetUInt32();
        pl->dq_jz = lnwow_dq->Fetch()[5].GetUInt32();
        pl->dq_ds = lnwow_dq->Fetch()[6].GetUInt32();
        pl->dq_zj = lnwow_dq->Fetch()[7].GetUInt32();
        pl->dq_hj = lnwow_dq->Fetch()[8].GetUInt32();
        pl->dq_ll = lnwow_dq->Fetch()[9].GetUInt32();
        pl->dq_mj = lnwow_dq->Fetch()[10].GetUInt32();
        pl->dq_zl = lnwow_dq->Fetch()[11].GetUInt32();
        pl->dq_nl = lnwow_dq->Fetch()[12].GetUInt32();
        pl->dq_js6 = lnwow_dq->Fetch()[13].GetUInt32();
        pl->dq_xp = lnwow_dq->Fetch()[14].GetUInt32();
        pl->dq_wlct = lnwow_dq->Fetch()[15].GetUInt32();
        pl->dq_fsct = lnwow_dq->Fetch()[16].GetUInt32();
    }
    else
    {
        CharacterDatabase.PExecute("INSERT INTO _玩家_斗气 (玩家) VALUES ('%u')", playguid);
    }
}

void CustomMgr::RewardItem(Player *plr, uint32 item_id, uint32 count)
{
    ItemPosCountVec dest;
    uint32 no_space_count = 0;
    uint8 msg = plr->CanStoreNewItem(NULL_BAG, NULL_SLOT, dest, item_id, count, &no_space_count);

    if (msg == EQUIP_ERR_ITEM_NOT_FOUND)
        return;

    if (msg != EQUIP_ERR_OK)                               // convert to possible store amount
        count -= no_space_count;

    if (count != 0 && !dest.empty())                        // can add some
    {
        if (Item* item = plr->StoreNewItem(dest, item_id, true, Item::GenerateItemRandomPropertyId(item_id)))
            plr->SendNewItem(item, count, true, false);
    }
    if (no_space_count > 0)
        SendRewardMarkByMail(plr, item_id, no_space_count);
}

void CustomMgr::SendRewardMarkByMail(Player *plr, uint32 mark, uint32 count)
{
    ItemTemplate const* markProto = sObjectMgr->GetItemTemplate(mark);
    if (!markProto)
        return;

    // extract items
    typedef std::pair<uint32, uint32> ItemPair;
    typedef std::list< ItemPair > ItemPairs;
    ItemPairs items;

    std::string itemText = "邮件获取物品";

    std::string subject = "这些道具在装备/存放中出现一个问题.";

    while (count > markProto->GetMaxStackSize())
    {
        items.push_back(ItemPair(mark, markProto->GetMaxStackSize()));
        count -= markProto->GetMaxStackSize();
    }

    items.push_back(ItemPair(mark, count));

    MailDraft draft(subject, itemText);

    SQLTransaction trans = CharacterDatabase.BeginTransaction();

    for (ItemPairs::const_iterator itr = items.begin(); itr != items.end(); ++itr)
    {
        if (Item* item = Item::CreateItem(itr->first, itr->second, plr))
        {
            item->SaveToDB(trans);
            draft.AddItem(item);
        }
    }

    draft.SendMailTo(trans, plr, MailSender(plr, MAIL_STATIONERY_GM));
    CharacterDatabase.CommitTransaction(trans);
}

void CustomMgr::DoCommand(Player* player, std::string command)
{
    if (command.empty())
        return;

    player->CastStop();
    uint64 oriTarget = player->GetTarget();


    AccountTypes security = player->GetSession()->GetSecurity();
    player->GetSession()->SetSecurity(SEC_CONSOLE);

    if (!ChatHandler(player->GetSession()).ParseCommands(command.c_str(), false))
    {
        std::ostringstream oss;
        oss << "命令配置出错:" << command.c_str();
        player->GetSession()->SendNotification(oss.str().c_str());
    }

    player->SetSelection(oriTarget);
    player->GetSession()->SetSecurity(security);
}

bool CustomMgr::CanSendBuyItemUI(Player * pl, uint64 vendorguid, uint32 itemid, uint32 itemcount, uint32 slot, uint8 bag, uint8 bagslot)
{
    //购买弹窗
    uint32 currentVendor = pl->GetSession()->GetCurrentVendor();
    Creature* creature = pl->GetNPCIfCanInteractWith(vendorguid, UNIT_NPC_FLAG_VENDOR);
    if (currentVendor || creature)
    {
        VendorItemData const* vItems = currentVendor ? sObjectMgr->GetNpcVendorItemList(currentVendor) : creature->GetVendorItems();
        if (vItems)
        {
            if (sCustomMgr->IsInNeedSys(vItems->GetItem(slot)->Needid))
            {
                pl->buy_vendor = vendorguid;
                pl->buy_item = itemid;
                pl->buy_count = itemcount;
                pl->buy_slot = slot;
                pl->buy_bag = bag;
                pl->buy_bagslot = bagslot;
                SendBuyItemUI(pl, itemid, itemcount, vItems->GetItem(slot)->Needid);
                return true;
            }
        }
    }
    return false;
}

void CustomMgr::SendBuyItemUI(Player * pl, uint32 itemid, uint32 itemcount, uint32 needid)
{
    ItemTemplate const* ipt = sObjectMgr->GetItemTemplate(itemid);
    if (!ipt)
        return;

    std::ostringstream ss1, ss2;
    std::string strTitle;

    ss1 << itemid << "#" << itemcount;

    const CNeedParameters * needsys = sNeedMgr->GetNeedParameters(needid);
    if (!needsys)
        return;

    

    if (needsys->classmask)
    {
        std::ostringstream classnotin;

        for (int32 i = 1; i < 12; i++)
        {
            if (!GetClassNameInMask(needsys->classmask, i).empty())
                classnotin << GetClassNameInMask(needsys->classmask, i) << "-";
        }
        if (!(needsys->classmask & pl->getClassMask()))
            ss2 << "可购买职业：" << classnotin.str() << "\n";
    }

    if (needsys->racemask)
    {
        std::ostringstream racenotin;

        for (int32 i = 1; i < 12; i++)
        {
            if (!GetRaceNameInMask(needsys->racemask, i).empty())
                racenotin << GetRaceNameInMask(needsys->racemask, i) << "-";
        }
        if (!(needsys->racemask & pl->getRaceMask()))
            ss2 << "可购买种族：" << racenotin.str() << "\n";
    }

    if (needsys->level)
    {
        ss2 << "等级：" << needsys->level << "\n";
    }
    if (needsys->viplevel)
    {
        ss2 << "会员等级：" << needsys->viplevel << "\n";
    }
    if (needsys->dqlevel)
    {
        ss2 << "斗气等级：" << needsys->dqlevel << "\n";
    }
    if (needsys->achpoint)
    {
        ss2 << "成就点数达到：" << needsys->achpoint << "\n";
    }

    if (needsys->m_achneeds.size())
    {
        for (int i = 0; i < needsys->m_achneeds.size(); i++)
        {
            ss2 <<"成就："<< GetAchLink(pl, needsys->m_achneeds[i]) << " 完成\n";
        }
    }

    if (needsys->m_spellneeds.size())
    {
        for (int i = 0; i < needsys->m_spellneeds.size(); i++)
        {
            ss2<<"技能：" << GetSpellLink(needsys->m_spellneeds[i]) << " 学会\n";
        }
    }

    if (needsys->m_auraneeds.size())
    {
        for (int i = 0; i < needsys->m_auraneeds.size(); i++)
        {
            ss2<<"BUFF：" << GetSpellLink(needsys->m_auraneeds[i]) << " 持有BUFF\n";
        }
    }

    if (needsys->m_questhasneeds.size())
    {
        for (int i = 0; i < needsys->m_questhasneeds.size(); i++)
        {
            ss2<< "任务："<< GetQuestLink(needsys->m_questhasneeds[i]) << " 接取\n";
        }
    }

    if (needsys->m_questcomneeds.size())
    {
        for (int i = 0; i < needsys->m_questcomneeds.size(); i++)
        {
            ss2<<"任务：" << GetQuestLink(needsys->m_questcomneeds[i]) << " 完成\n";
        }
    }

    if (needsys->m_haveitems.size() && needsys->m_haveitems.size() == needsys->m_haveitemcounts.size())
    {
        for (int i = 0; i < needsys->m_haveitems.size(); i++)
        {
            ss2 <<"物品："<< GetItemLink(needsys->m_haveitems[i]) << " 持有" << needsys->m_haveitemcounts[i] << "个\n";
        }
    }

    if (needsys->jfcost)
        ss2 << "|TInterface/icons/INV_Misc_Coin_01:11:11:0:-11|t积分：" << needsys->jfcost*itemcount << " 消耗\n";

    if (needsys->gbcost)
        ss2 << "|TInterface/icons/INV_Misc_Coin_01:11:11:0:-11|t金币：" << needsys->gbcost*itemcount << " 消耗\n";

    if (needsys->honorcost)
        ss2 << "|TInterface/icons/INV_Misc_Coin_01:11:11:0:-11|t荣誉点：" << needsys->honorcost*itemcount << " 消耗\n";

    if (needsys->arenacost)
        ss2 << "|TInterface/icons/INV_Misc_Coin_01:11:11:0:-11|t竞技点：" << needsys->arenacost*itemcount << " 消耗\n";

    if (needsys->m_itemcosts.size() && needsys->m_itemcosts.size() == needsys->m_itemcostcounts.size())
    {
        for (int i = 0; i < needsys->m_itemcosts.size(); i++)
        {
            ss2 <<"物品："<< GetItemLink(needsys->m_itemcosts[i]) << " 消耗" << needsys->m_itemcostcounts[i] * itemcount << "个\n";
        }
    }

    strTitle = "FrozenBuy^" + ss1.str() + "^" + ss2.str();
    WorldPacket data(SMSG_QUEST_CONFIRM_ACCEPT, (4 + strTitle.size() + 8));
    data << uint32(3);
    data << strTitle;
    data << uint64(pl->GetGUID());
    pl->GetSession()->SendPacket(&data);
}

void CustomMgr::SendWorldAnnText(char const *format, ...)   //全世界下方提示公告
{
    if (format)
    {
        va_list ap;
        char str[2048];
        va_start(ap, format);
        vsnprintf(str, 2048, format, ap);
        va_end(ap);
        std::string msg = (std::string)str;

        WorldPacket data(SMSG_SERVER_MESSAGE, 50);
        data << uint32(SERVER_MSG_STRING);
        data << msg.c_str();
        sWorld->SendGlobalMessage(&data);
    }
}

void CustomMgr::SendWorldNotifyText(char const *format, ...) //全世界上方提示
{
    if (format)
    {
        va_list ap;
        char str[2048];
        va_start(ap, format);
        vsnprintf(str, 2048, format, ap);
        va_end(ap);

        WorldPacket data(SMSG_NOTIFICATION, (strlen(str) + 1));
        data << str;
        sWorld->SendGlobalMessage(&data);
    }
}

void CustomMgr::SendPVPText(char const *format, ...)  //世界弹出式公告
{
    if (format)
    {
        va_list ap;
        char str[2048];
        va_start(ap, format);
        vsnprintf(str, 2048, format, ap);
        va_end(ap);

        WorldPacket data(SMSG_MESSAGECHAT, 200);
        data << (uint8)CHAT_MSG_RAID_BOSS_EMOTE;
        data << (uint32)LANG_UNIVERSAL;
        data << (uint64)0;
        data << (uint32)0;                                     // 2.1.0
        data << (uint32)1;
        data << "";
        data << (uint64)0;
        data << (uint32)(strlen(str) + 1);
        data << str;
        data << (uint8)0;
        sWorld->SendGlobalMessage(&data);
    }
}

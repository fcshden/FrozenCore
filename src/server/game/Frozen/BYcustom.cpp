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
}

void CustomMgr::SavePlayStatTo(Player* pl, uint32 playguid) //玩家下线保存自定义属性
{

}

void CustomMgr::LoadPlayStatFrom(Player* pl, uint32 playguid) // 玩家登陆查询自定义属性
{
    pl->viplevel = 0;
    pl->dqlevel = 0;
    pl->jftoken = 0;
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

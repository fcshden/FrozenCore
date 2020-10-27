#pragma execution_character_set("utf-8")
#include "dqsys.h"
#include "BYcustom.h"

class itemdqxpup : public ItemScript
{
public:itemdqxpup() : ItemScript("item_dqxp") {}
       bool OnUse(Player * pPlayer, Item * pItem, SpellCastTargets const& targets)
       {
           if (sDqSysMgr->Getdqlevel(pPlayer) >= sDqSysMgr->GetMaxdqLevel())
           {
               ChatHandler(pPlayer->GetSession()).PSendSysMessage("斗气等级已经到最高.");
               return true;
           }

           float xpbeibuff = 1.0f;

           int32 maxAmount = 0;


           xpbeibuff += float(maxAmount);

           uint32 getdqxp = pItem->GetTemplate()->ItemStat[0].ItemStatValue * xpbeibuff;
           WorldPacket data(SMSG_LOG_XPGAIN, 21);
           data << uint64(0);
           data << uint32(getdqxp);
           data << uint8(0);
           data << uint8(0);
           pPlayer->GetSession()->SendPacket(&data);

           sDqSysMgr->Modifydqxp(pPlayer, getdqxp);
           uint32 count = 1;
           pPlayer->DestroyItemCount(pItem, count, true);

           if (!sDqSysMgr->GetDQParameters(sDqSysMgr->Getdqlevel(pPlayer) + 1)->upcast)
           {
               if (sDqSysMgr->Getdqxp(pPlayer) > sDqSysMgr->GetDQParameters(sDqSysMgr->Getdqlevel(pPlayer) + 1)->dqxp)
               {

                   sDqSysMgr->Setdqxp(pPlayer, sDqSysMgr->Getdqxp(pPlayer) - sDqSysMgr->GetDQParameters(sDqSysMgr->Getdqlevel(pPlayer) + 1)->dqxp);
                   sDqSysMgr->Modifydqlevel(pPlayer, 1);
                   sDqSysMgr->Modifydqshuxing(pPlayer, sDqSysMgr->GetDQParameters(sDqSysMgr->Getdqlevel(pPlayer))->dqpoint);

                   pPlayer->CastSpell(pPlayer, 47292, false);
                   if (sDqSysMgr->GetDQParameters(sDqSysMgr->Getdqlevel(pPlayer))->dqpoint)
                       sDqSysMgr->OnPlayerHelloDQ(pPlayer);

                   switch (sDqSysMgr->GetConfInt(100, 1))
                   {
                   case 0:
                       break;
                   case 1:
                       sCustomMgr->SendWorldNotifyText(sDqSysMgr->GetConftxt(1000).c_str(), pPlayer->GetSNameLink().c_str(), sDqSysMgr->GetdqTitleStd(pPlayer).c_str()); //屏幕上方
                       break;
                   case 2:
                       sCustomMgr->SendWorldAnnText(sDqSysMgr->GetConftxt(1000).c_str(), pPlayer->GetSNameLink().c_str(), sDqSysMgr->GetdqTitleStd(pPlayer).c_str()); //屏幕下面
                       break;
                   case 3:
                       sCustomMgr->SendPVPText(sDqSysMgr->GetConftxt(1000).c_str(), pPlayer->GetSNameLink().c_str(), sDqSysMgr->GetdqTitleStd(pPlayer).c_str()); // 弹出式提示
                       break;
                   }
               }
           }
           return true;
       }
};

class custom_player_login : public PlayerScript
{
public:
    custom_player_login() : PlayerScript("nidaye") {}
    void OnLogin(Player* player)
    {
        sCustomMgr->OnPlayerUpdateDQXP(player);
    }
};

void AddSC_custom_Player()
{
    new itemdqxpup();
    new custom_player_login();
}

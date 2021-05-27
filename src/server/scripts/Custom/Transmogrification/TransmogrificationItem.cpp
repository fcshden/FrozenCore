#pragma execution_character_set("utf-8")
#include "../CommonFunc/CommonFunc.h"
#include "../DataLoader/DataLoader.h"
#include "../Requirement/Requirement.h"
#include "../GCAddon/GCAddon.h"

class TRANS_ITEM_NPC : public CreatureScript
{
public:
	TRANS_ITEM_NPC() : CreatureScript("TRANS_ITEM_NPC") { }

	bool OnGossipHello(Player* player, Creature* creature) override
	{
		player->PlayerTalkClass->ClearMenus();
		
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "������Ʒ���", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF, "", 0, true);
		player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
		return true;
	}

	bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
	{
		player->PlayerTalkClass->ClearMenus();

		if (!*code)
			return false;
		
		uint32 entry = (uint32)atoi(code);
		//uint32 entry = inputEntry + 5000000;

		ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(entry);
		if (!pProto || entry >56806)
		{
			player->GetSession()->SendNotification("����Ʒ������,��������ȷ��ƷID��");
			return false;

		}

		if (pProto->Class != 4 && pProto->Class != 2)
		{
			player->GetSession()->SendNotification("����Ʒ�������ڻû�,��������ȷ��ƷID��");
			return false;
		}

		player->trans_item = entry;
		player->trans_reqId = sGCAddon->GetTransReqId(entry);

		std::string notice = sReq->Notice(player, player->trans_reqId, "����û���Ʒ\n", sCF->GetItemLink(entry));
		sCF->SendAcceptOrCancel(player, 999, notice);
		return true;
	}
};

void AddSC_TRANS_ITEM_NPC()
{
	new TRANS_ITEM_NPC();
}

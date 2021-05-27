#pragma execution_character_set("utf-8")
#include "SellReward.h"
#include "../Custom/Reward/Reward.h"
#include "../ItemMod/ItemMod.h"
#include "../Requirement/Requirement.h"
#include "../Command/CustomCommand.h"
#include "../ItemMod/NoPatchItem.h"

//������Ʒʱ�����������
void SellReward::Reward(Player* player, Item* pItem)
{
	uint32 rewId = sNoPatchItem->GetSellRewId(pItem);

	if (rewId != 0)
	{
		sRew->Rew(player, rewId, pItem->GetCount());
		return;
	}
	
	uint32 chance = 0;
	std::string command = "";

	sItemMod->GetSaleInfo(pItem->GetEntry(), rewId, chance, command);

	if (urand(1, 100) <= chance)
		sRew->Rew(player, rewId, pItem->GetCount());

	sCustomCommand->DoCommand(player, command);
}
//�ɻ�����������������Ʒ�����˻�
bool SellReward::CanRefund(Player* player, Item* pItem)
{
	if (sNoPatchItem->GetSellRewId(pItem) != 0)
	{
		player->GetSession()->SendNotification("�ɻ�����������������Ʒ�����˻���");
		return false;
	}

	uint32 len = ItemSaleVec.size();

	for (uint32 i = 0; i < len; i++)
	{
		if (ItemSaleVec[i].entry == pItem->GetEntry())
		{
			player->GetSession()->SendNotification("�ɻ�����������������Ʒ�����˻���");
			return false;
		}
	}
	return true;
}

//��Ʒ��������
std::string SellReward::GetSellDes(uint32 entry)
{
	uint32 rewId = 0;
	uint32 chance = 0;
	std::string command = "";

	sItemMod->GetSaleInfo(entry, rewId, chance, command);

	if (rewId == 0)
		return "";

	std::ostringstream oss;
	oss << "|cFF00FF00������ |cFFFFCC00";
	oss << chance;
	oss << "%|r |cFF00FF00���ʻ�á�|r\n";
	oss << sRew->GetDescription(rewId);
	return oss.str();
}

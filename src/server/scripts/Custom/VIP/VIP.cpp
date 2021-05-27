#pragma execution_character_set("utf-8")
#include "VIP.h"
#include "../Custom/Requirement/Requirement.h"
#include "../Custom/Reward/Reward.h"
#include "../String/myString.h"
#include "../CommonFunc/CommonFunc.h"
#include "../FunctionCollection/FunctionCollection.h"

std::vector<VIPTemplate> VIPVec;

void VIP::Load()
{
	VIPVec.clear();
	QueryResult result1 = WorldDatabase.PQuery(
		sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		//			0		1			2			3						4							5						6					7					8				9	10	11				12
		"SELECT ��Ա�ȼ�,��������ģ��ID,���䱶��,��Ʒ����ģʽ0���ӳɹ��ٷֱ�,��Ʒ����ģʽ1���ӳɹ��ٷֱ�,��Ʒǿ�����ӳɹ��ٷֱ�,��ӡǿ�����ӳɹ��ٷֱ�,�Ƴ���ʯ���ӳɹ��ٷֱ�,��������ģ��ID,ͼ��,����,�ݵ㽱��ģ��ID,�˵��ı� from __��Ա" :
		//		0		1		2		3				4			5				6			7			8	9	10		11			12
		"SELECT vipLv,reqId,lootRate,exchangeRate_0,exchangeRate_1,strengthenRate,sigilRate,removeGemRate,rewId,icon,name,timeRewId,gossipText from _vip");
	if (result1)
	{
		do
		{
			Field* fields = result1->Fetch();
			VIPTemplate VIPTemp;
			VIPTemp.vipLv				= fields[0].GetUInt32();
			VIPTemp.reqId				= fields[1].GetUInt32();
			VIPTemp.lootRate			= fields[2].GetFloat();
			VIPTemp.exchangeRate_0		= fields[3].GetUInt32();
			VIPTemp.exchangeRate_1		= fields[4].GetUInt32();
			VIPTemp.strengthenRate		= fields[5].GetUInt32();
			VIPTemp.sigilRate			= fields[6].GetUInt32();
			VIPTemp.removeGemRate		= fields[7].GetUInt32();
			VIPTemp.rewId				= fields[8].GetUInt32();
			VIPTemp.icon				= fields[9].GetString();
			VIPTemp.name				= fields[10].GetString();
			VIPTemp.timeRewId			= fields[11].GetUInt32();
			VIPTemp.gossipText			= fields[12].GetString();
			VIPVec.push_back(VIPTemp);
		} while (result1->NextRow());
	}
}


void VIP::GetVIPTitle(Player* player, std::string &vipTitle, std::string &vipIcon, bool fakeplayer, uint32 vip)
{
	if (fakeplayer)
	{

		for (std::vector<VIPTemplate>::iterator itr = VIPVec.begin(); itr != VIPVec.end(); ++itr)
			if (itr->vipLv == vip)
			{
				vipTitle = itr->name;
				vipIcon = "|TInterface/ICONS/" + itr->icon + ":14:14:0:-2|t";
				break;
			}

		return;
	}

	for (std::vector<VIPTemplate>::iterator itr = VIPVec.begin(); itr != VIPVec.end(); ++itr)
		if (itr->vipLv == player->vipLevel)
		{
			vipTitle = itr->name;
			vipIcon = "|TInterface/ICONS/" + itr->icon + ":14:14:0:-2|t";
			break;
		}
}
std::string VIP::GetVIPName(uint32 viplv)
{
	for (std::vector<VIPTemplate>::iterator itr = VIPVec.begin(); itr != VIPVec.end(); ++itr)
		if (itr->vipLv == viplv)
			return itr->name;
			
	return "";
}

void VIP::GetVIP(Player* player, std::string &vipTitle, std::string &vipIcon)
{
	for (std::vector<VIPTemplate>::iterator itr = VIPVec.begin(); itr != VIPVec.end(); ++itr)
		if (itr->vipLv == player->vipLevel)
		{
			vipTitle = itr->name;
			vipIcon = "|TInterface/ICONS/" + itr->icon + ":14:14:0:0|t";
			break;
		}
}

void VIP::GetNextVIP(Player* player, std::string &vipTitle, std::string &vipIcon)
{
	for (std::vector<VIPTemplate>::iterator itr = VIPVec.begin(); itr != VIPVec.end(); ++itr)
		if (itr->vipLv == player->vipLevel + 1)
		{
			vipTitle = itr->name;
			vipIcon = "|TInterface/ICONS/" + itr->icon + ":14:14:0:0|t";
			break;
		}
}


uint32 VIP::GetRate(Player* player, VIPRateTypes type)
{
	uint32 len = VIPVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (player->vipLevel == VIPVec[i].vipLv)
		{
			switch (type)
			{
			case VIP_RATE_LOOT:
				return VIPVec[i].lootRate;
			case VIP_RATE_ITEM_EXCHANGE_0:
				return VIPVec[i].exchangeRate_0;
			case VIP_RATE_ITEM_EXCHANGE_1:
				return VIPVec[i].exchangeRate_1;
			case VIP_RATE_ITEM_STRENGTHEN:
				return VIPVec[i].strengthenRate;
			case VIP_RATE_SIGIL:
				return VIPVec[i].sigilRate;
			case VIP_RATE_GEM_REMOVE:
				return VIPVec[i].removeGemRate;
			default:
				return 0.0f;
			}
		}
	}

	return 0.0f;
}

float VIP::GetLootRate(Player* player)
{
	uint32 len = VIPVec.size();
	for (size_t i = 0; i < len; i++)
		if (player->vipLevel == VIPVec[i].vipLv)
			return VIPVec[i].lootRate;
		
	return 1.0f;
}

void VIP::Up(Player* player)
{
	uint32 len = VIPVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (player->vipLevel == VIPVec[i].vipLv - 1)
		{
			if (sReq->Check(player, VIPVec[i].reqId))
			{
				sReq->Des(player, VIPVec[i].reqId);
				sRew->Rew(player, VIPVec[i].rewId);
				LoginDatabase.DirectPExecute("UPDATE account SET viplevel = viplevel + 1 WHERE id = '%u'", player->GetSession()->GetAccountId());
				player->vipLevel++;
				sCF->SetLootRate(player);

				std::string title	= "";
				std::string icon	= "";

				GetVIPTitle(player, title, icon);

				std::ostringstream oss;
				oss << "����µĻ�Ա�ȼ�!" << icon << title;
				player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
				player->CastSpell(player, 61456, true, NULL, NULL, player->GetGUID());

				const char*  msg = sString->Format(sString->GetText(CORE_STR_TYPES(STR_VIP_UP)), sCF->GetNameLink(player).c_str(), title.c_str());
				sWorld->SendScreenMessage(msg);
				return;
			}
		}
	}
}
uint32 VIP::GetReqId(Player* player)
{
	uint32 len = VIPVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (player->vipLevel == VIPVec[i].vipLv - 1)
		{
			return VIPVec[i].reqId;
		}
	}

	return 0;
}

uint32 VIP::GetTimeRewId(Player* player)
{
	uint32 len = VIPVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (player->vipLevel == VIPVec[i].vipLv)
		{
			return VIPVec[i].timeRewId;
		}
	}

	return 0;
}

std::string VIP::GetGossipText(Player* player, bool next)
{
	uint32 len = VIPVec.size();

	if (next)
	{
		for (std::vector<VIPTemplate>::iterator itr = VIPVec.begin(); itr != VIPVec.end(); ++itr)
			if (itr->vipLv == player->vipLevel + 1)
				return itr->gossipText;
	}
	else
	{
		for (std::vector<VIPTemplate>::iterator itr = VIPVec.begin(); itr != VIPVec.end(); ++itr)
			if (itr->vipLv == player->vipLevel)
				return itr->gossipText;
	}

	return "";
}

void VIP::AddGossip(Player* player, Object* obj)
{
	std::string title = "";
	std::string icon = "";

	GetNextVIP(player, title, icon);

	player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GetGossipText(player), SENDER_VIP_CURR, GOSSIP_ACTION_INFO_DEF);

	uint32 maxLevel = 0;

	for (auto itr = VIPVec.begin(); itr != VIPVec.end(); itr++)
		if (maxLevel < itr->vipLv)
			maxLevel = itr->vipLv;

	if (player->vipLevel < maxLevel)
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "\n<<���������Ա�ȼ�>>\n", SENDER_VIP_UP, GOSSIP_ACTION_INFO_DEF, sReq->Notice(player, GetReqId(player), "����", title), sReq->Golds(GetReqId(player)), false);
	
	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}


class VIPLogin : PlayerScript
{
public:
	VIPLogin() : PlayerScript("VIPLogin") {}
	void OnLogin(Player* player)
	{
		QueryResult result = LoginDatabase.PQuery("SELECT vipLevel FROM account WHERE id = '%u'", player->GetSession()->GetAccountId());
		if (!result)
			player->vipLevel = 0;
		else
		{
			Field* fields = result->Fetch();
			player->vipLevel = fields[0].GetInt32();
		}
	}
};

void AddSC_VIP_LOGIN()
{
	new VIPLogin();
}

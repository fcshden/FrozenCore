#pragma execution_character_set("utf-8")
#include "LuckDraw.h"
#include "../GCAddon/GCAddon.h"
#include "../Requirement/Requirement.h"
#include "../Talisman/Talisman.h"
#include "../Switch/Switch.h"
#include "../CommonFunc/CommonFunc.h"

std::unordered_map<uint32, LuckDrawTemplate> LuckDrawMap;

void LuckDraw::Load()
{
	LuckDrawMap.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID,几率,物品数量 FROM __抽奖" :
		"SELECT itemId,chance,itemCount FROM _luckdraw");
	if (!result)
		return;
	do
	{
		Field* fields = result->Fetch();
		uint32 itemId = fields[0].GetUInt32();

		LuckDrawTemplate Temp;
		Temp.chance = fields[1].GetFloat();
		Temp.count = fields[2].GetInt32();
		LuckDrawMap.insert(std::make_pair(itemId, Temp));
	} while (result->NextRow());
}

int32 LuckDraw::GetCount(uint32 itemId)
{
	std::unordered_map<uint32, LuckDrawTemplate>::iterator iter = LuckDrawMap.find(itemId);

	if (iter != LuckDrawMap.end())
		return iter->second.count;

	return 1;
}

void LuckDraw::SendCheckInfo(Player* player)
{

	bool check1 = sReq->Check(player, atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 1).c_str()), 1, false);
	bool check10 = sReq->Check(player, atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 2).c_str()), 1, false);

	std::ostringstream oss;

	oss << "CHECK#";

	if (check10)
		oss << "2";
	else if (check1)
		oss << "1";
	else
		oss << "0";

	sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW_V3", oss.str());

}

void LuckDraw::OpenPanel(Player* player)
{
	uint32 reqId = atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 1).c_str());

	SendCheckInfo(player);

	std::ostringstream oss;
	
	oss << "SHOW#";

	for (std::unordered_map<uint32, LuckDrawTemplate>::iterator iter = LuckDrawMap.begin(); iter != LuckDrawMap.end(); iter++)
		oss << iter->first << "-" << sTalisman->GetIcon(iter->first) << "-" << abs(iter->second.count) << ":";

	sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW_V3", oss.str());
}

void LuckDraw::Stop(Player* player)
{
	SendCheckInfo(player);

	uint32 itemId = 0;

	std::vector<std::pair<int, float>> vtMap;
	for (auto it = LuckDrawMap.begin(); it != LuckDrawMap.end(); it++)
		vtMap.push_back(std::make_pair(it->first, it->second.chance));

	sort(vtMap.begin(), vtMap.end(),
		[](const std::pair<int, float> &x, const std::pair<int, float> &y) -> int {
		return x.second < y.second;
	});

	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->second;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->second;
		if (rand <= sum)
		{
			itemId = it->first;
			break;
		}
	}

	std::ostringstream oss;
	oss << "STOP#" << itemId;
	sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW_V3", oss.str());

	int32 count = GetCount(itemId);

	player->AddItem(itemId, abs(count));

	if (count < 0)
	{
		std::ostringstream oss1;
		oss1 << "[幸运抽奖]" << sCF->GetNameLink(player) << "抽到了" << sCF->GetItemLink(itemId) << " X " << abs(count);
		sWorld->SendScreenMessage(oss1.str().c_str());
	}
}

void LuckDraw::SendData(Player* player)
{
	std::ostringstream oss;
	for (std::unordered_map<uint32, LuckDrawTemplate>::iterator iter = LuckDrawMap.begin(); iter != LuckDrawMap.end(); iter++)
	{
		if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(iter->first))
			player->GetSession()->SendPacket(&pProto->queryData);
		oss << iter->first << "-" << iter->second.count << " ";
	}
		

	sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW", oss.str());
}

void LuckDraw::SendUpdateData(Player* player)
{
	sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW_UPDATE", " ");
}

void LuckDraw::Rew(Player* player, uint32 rewCount)
{
	uint32 itemId = 0;

	std::vector<std::pair<int, float>> vtMap;
	for (auto it = LuckDrawMap.begin(); it != LuckDrawMap.end(); it++)
		vtMap.push_back(std::make_pair(it->first, it->second.chance));

	sort(vtMap.begin(), vtMap.end(),
		[](const std::pair<int, float> &x, const std::pair<int, float> &y) -> int {
		return x.second < y.second;
	});

	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->second;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->second;
		if (rand <= sum)
		{
			itemId = it->first;
			break;
		}
	}

	int32 count = GetCount(itemId);

	player->AddItem(itemId, abs(count));

	std::ostringstream oss;
	oss << itemId << " " << count << " " << rewCount;
	sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW_REW", oss.str());

	if (count < 0)
	{
		std::ostringstream oss;
		oss << "[幸运抽奖]" << sCF->GetNameLink(player) << "抽到了" << sCF->GetItemLink(itemId) << " X " << abs(count);
		sWorld->SendScreenMessage(oss.str().c_str());
	}
}

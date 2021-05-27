#pragma execution_character_set("utf-8")
#include "BlackMarket.h"
#include "../../GCAddon/GCAddon.h"
#include "../../Requirement/Requirement.h"

std::vector<BlackMarketTemplate> BlackMarketVec;

void BlackMarket::Load()
{
	BlackMarketVec.clear();
	
	if (QueryResult result = WorldDatabase.PQuery("SELECT id,itmemId,itemCount,reqId FROM _ui_blackmarket"))
	{
		do
		{
			Field* fields = result->Fetch();
	
			BlackMarketTemplate Temp;
			Temp.id			= fields[0].GetUInt32();
			Temp.itemId		= fields[1].GetUInt32();
			Temp.itemCount	= fields[2].GetUInt32();
			Temp.reqId		= fields[3].GetUInt32();
			Temp.enable		= true;
			BlackMarketVec.push_back(Temp);
		} while (result->NextRow());
	}
}

void BlackMarket::Start()
{
	Load();

	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator itr = smap.begin(); itr != smap.end(); ++itr)
		if (Player* pl = itr->second->GetPlayer())
			SendData(pl);

	sWorld->SendScreenMessage("[黑市]已经开放！");
}

void BlackMarket::Stop()
{
	for (auto iter = BlackMarketVec.begin(); iter != BlackMarketVec.end(); iter++)
		iter->enable = false;

	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator itr = smap.begin(); itr != smap.end(); ++itr)
		if (Player* pl = itr->second->GetPlayer())
			SendData(pl);

	sWorld->SendScreenMessage("[黑市]已经关闭！");
}

void BlackMarket::SendData(Player* player)
{
	for (auto iter = BlackMarketVec.begin(); iter != BlackMarketVec.end(); iter++)
		SendData(player, iter->id, iter->itemId, iter->itemCount, iter->reqId, iter->enable);
}

void BlackMarket::SendData(Player* player, uint32 id, uint32 itemId, uint32 itemCount, uint32 reqId, bool enable)
{
	if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(itemId))
		player->GetSession()->SendPacket(&pProto->queryData);

	std::ostringstream oss;
	oss << id			<< " ";
	oss << itemId		<< " ";
	oss << itemCount	<< " ";
	oss << reqId		<< " ";

	if (enable)
		oss << 1;
	else
		oss << 0;

	sGCAddon->SendPacketTo(player, "GC_S_BLACKMARKET", oss.str());
}

void BlackMarket::Update(Player* player, uint32 id)
{
	for (auto iter = BlackMarketVec.begin(); iter != BlackMarketVec.end(); iter++)
	{
		if (iter->id == id)
		{
			if (!sReq->Check(player, iter->reqId))
				return;

			if (!iter->enable)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("你不能购买此物品！");
				return;
			}

			player->AddItem(iter->itemId, iter->itemCount);

			iter->enable = false;
			
			SessionMap const& smap = sWorld->GetAllSessions();
			for (SessionMap::const_iterator itr = smap.begin(); itr != smap.end(); ++itr)
				if (Player* pl = itr->second->GetPlayer())
					SendData(pl, iter->id, iter->itemId, iter->itemCount, iter->reqId, iter->enable);

			break;
		}
	}
}

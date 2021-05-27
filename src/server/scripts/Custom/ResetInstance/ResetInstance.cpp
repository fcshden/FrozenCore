#pragma execution_character_set("utf-8")
#include "ResetInstance.h"
#include "../Requirement/Requirement.h"
#include "../FunctionCollection/FunctionCollection.h"

std::vector<ResetInsTemplate> ResetInsVec;

uint8 ResetIns::getDiff(uint32 action) {
	return (uint8)((action) >> 24);
}

uint32 ResetIns::getMapId(uint32 action) {
	return (uint32)((action) & 0xFFFFFF);
}

uint32 ResetIns::actionValue(uint8 diff, uint32 mapid) {
	return (uint32)((diff << 24) | (mapid & 0xFFFFFF));
}


void ResetIns::Load()
{
	ResetInsVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 地图ID,难度,需求模板ID from _副本_重置需求" :
		"SELECT mapid,diff,reqId from _instance_reset");
	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			ResetInsTemplate Temp;
			Temp.mapid = fields[0].GetUInt32();
			Temp.diff = fields[1].GetUInt8();
			Temp.reqId = fields[2].GetUInt32();
			ResetInsVec.push_back(Temp);
		} while (result->NextRow());
	}
}

uint32 ResetIns::GetReqId(uint32 mapid, uint8 diff)
{
	uint32 len = ResetInsVec.size();
	for (size_t i = 0; i < len; i++)
	{
		if (mapid == ResetInsVec[i].mapid && diff == ResetInsVec[i].diff)
			return ResetInsVec[i].reqId;
	}

	return 0;
}

std::string ResetIns::GetGossipText(uint32 mapid, uint8 diff)
{
	uint32 len = ResetInsVec.size();
	for (size_t i = 0; i < len; i++)
		if (mapid == ResetInsVec[i].mapid && diff == ResetInsVec[i].diff)
			if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapid))
			{
				std::string text = "";
				text = mapEntry->name[4];
				switch (diff)
				{
				case 0:
					//text += "[普通]";
					break;
				case 1:
					mapEntry->IsRaid() ? text += "[25]" : text += "[5H]";
					break;
				case 2:
					text += "[10H]";
					break;
				case 3:
					text += "[25H]";
					break;
				default:
					break;
				}

				

				return text;
			}

	return "";
}

void ResetIns::AddGossip(Player* player, Object* obj)
{
	uint32 count = 0;

	for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
	{
		BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(player->GetGUIDLow(), Difficulty(i));
		for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end(); ++itr)
		{
			if (MapEntry const* mapEntry = sMapStore.LookupEntry(itr->first))
			{
				std::string text = GetGossipText(mapEntry->MapID, i);
				uint32 reqId = GetReqId(mapEntry->MapID, i);
				if (!text.empty())
				{
					if (count > 25)
						continue;
					count++;
					player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, text, SENDER_RESET_INS, actionValue(i, mapEntry->MapID), sReq->Notice(player, reqId, "重置", text), sReq->Golds(reqId), false);
				}
			}
		}
	}

	if (obj->ToCreature())
		player->SEND_GOSSIP_MENU(obj->GetEntry(), obj->GetGUID());
	else
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());

	//if (count == 1)
	//	player->PlayerTalkClass->ClearMenus();
}

void ResetIns::Action(Player* player, uint32 action)
{
	

	Difficulty diff = Difficulty(sResetIns->getDiff(action));
	uint32 mapid = sResetIns->getMapId(action);
	uint32 reqId = sResetIns->GetReqId(mapid, diff);

	if (player->GetMapId() == mapid)
	{
		player->GetSession()->SendNotification("请先离开副本！");
		return;
	}

	if (sReq->Check(player, reqId))
	{
		sReq->Des(player, reqId);
		sResetIns->ResetInstance(player, diff, mapid);
	}
}

void ResetIns::ResetInstance(Player* player, Difficulty diff, uint32 mapId)
{

	for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
	{
		BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(player->GetGUIDLow(), Difficulty(i));
		for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end();)
		{
			InstanceSave* save = itr->second.save;
			MapEntry const* mapEntry = sMapStore.LookupEntry(itr->first);
			if (mapEntry && itr->first != player->GetMapId() && (!mapId || mapId == itr->first) && (diff == -1 || diff == save->GetDifficulty()))
			{
				std::ostringstream oss;
				oss << GetGossipText(mapEntry->MapID, i) << "已重置";
				ChatHandler(player->GetSession()).PSendSysMessage(oss.str().c_str());
				sInstanceSaveMgr->PlayerUnbindInstance(player->GetGUIDLow(), itr->first, diff, true, player);
				itr = m_boundInstances.begin();
			}
			else
				++itr;
		}
	}
}


class ResetInsNPC : public CreatureScript
{
public:
	ResetInsNPC() : CreatureScript("ResetInsNPC") { }

	bool OnGossipHello(Player* player, Creature* creature) override
	{
		player->PlayerTalkClass->ClearMenus();
		sResetIns->AddGossip(player, creature);
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		player->PlayerTalkClass->ClearMenus();
		sResetIns->Action(player, action);
		return true;
	}
};

void AddSC_ResetInsNPC()
{
	new ResetInsNPC();
}

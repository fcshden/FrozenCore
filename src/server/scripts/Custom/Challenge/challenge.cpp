#pragma execution_character_set("utf-8")
#include "challenge.h"
#include "Group.h"
#include "MapManager.h"
#include "../CommonFunc/CommonFunc.h"
#include "../AuthCheck/AuthCheck.h"
#include "../Requirement/Requirement.h"
#include "../String/myString.h"
#include "../Switch/Switch.h"

std::vector<ChallengeTemplate> ChallengeVec;
std::vector<PlayerChallengeTemplate> PlayerChallengeVec;

void ChallengeMod::Load()
{
	ChallengeVec.clear();

	if (QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		//		0		1			2			3
		"SELECT 地图ID, 挑战等级, 需求模板ID, 菜单文本 FROM _副本_挑战模式" :
		//		0		1			2			3
		"SELECT MapId, ChallengeLv, ReqId, GossipText FROM _challenge"))
	{
		do
		{
			Field* fields = result->Fetch();
			ChallengeTemplate Temp;
			Temp.MapId = fields[0].GetUInt32();
			Temp.Level = fields[1].GetUInt32();
			Temp.ReqId = fields[2].GetInt32();
			Temp.GossipText = fields[3].GetString();
			ChallengeVec.push_back(Temp);
		} while (result->NextRow());
	}
}

void ChallengeMod::ReplaceChallenge(Player* player, uint32 mapId, uint32 challengeLv)
{
	player->SetDungeonDifficulty(Difficulty(BIND_INSTANCE_DIFF));
	player->SetRaidDifficulty(Difficulty(BIND_INSTANCE_DIFF));
	player->ChallengeLv = challengeLv;

	bool exsist = false;
	for (auto itr = PlayerChallengeVec.begin(); itr != PlayerChallengeVec.end(); itr++)
	{
		if (player->GetGUIDLow() == itr->guid && mapId == itr->mapId)
		{
			itr->challengeLv = challengeLv;
			exsist = true;
		}
	}

	if (!exsist)
	{
		PlayerChallengeTemplate temp;
		temp.guid = player->GetGUIDLow();
		temp.mapId = mapId;
		temp.challengeLv = challengeLv;
		PlayerChallengeVec.push_back(temp);
	}
}

bool ChallengeMod::HasChallenge(Player* player, uint32 mapId, uint32 challengeLv)
{
	for (auto itr = PlayerChallengeVec.begin(); itr != PlayerChallengeVec.end(); itr++)
		if (player->GetGUIDLow() == itr->guid && mapId == itr->mapId && itr->challengeLv == challengeLv)
			return true;

	return false;
}

void ChallengeMod::ResetChallenge(uint32 guidlow, uint32 mapId)
{
	for (auto itr = PlayerChallengeVec.begin(); itr != PlayerChallengeVec.end();)
	{
		if (guidlow == itr->guid && mapId == itr->mapId)
			itr = PlayerChallengeVec.erase(itr);
		else
			++itr;
	}
}


int32 ChallengeMod::GetReqId(uint32 mapId, uint32 challengeLv)
{
	for (auto itr = ChallengeVec.begin(); itr != ChallengeVec.end(); itr++)
		if (mapId == itr->MapId && challengeLv == itr->Level)
			return itr->ReqId;

	return 0;
}

std::string ChallengeMod::GetGossipText(uint32 mapId, uint32 challengeLv)
{
	for (auto itr = ChallengeVec.begin(); itr != ChallengeVec.end(); itr++)
		if (mapId == itr->MapId && challengeLv == itr->Level)
			return itr->GossipText;

	return "";
}

void ChallengeMod::ChangeLevel(Player* leader, uint32 triggerId, uint32 challengeLv)
{
    AreaTriggerTeleport const* at = sObjectMgr->GetAreaTriggerTeleport(triggerId);
	if (!at)
		return;

	uint32 mapId = at->target_mapId;
	int32 reqId = GetReqId(mapId, challengeLv);
	bool teleport = true;

	Group* group = leader->GetGroup();

	//检测队伍成员是否在FB中
	if (group)
	{
		for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
		{
			if (Player* member = itr->GetSource())
			{
				if (member->GetMap()->IsDungeon())
				{
					teleport = false;
					ChatHandler(member->GetSession()).PSendSysMessage("你在副本中，选择难度失败");
					if (leader->GetGUID() != member->GetGUID())
						ChatHandler(leader->GetSession()).PSendSysMessage("%s在副本中，选择难度失败", sCF->GetNameLink(member).c_str());
				}

				if (reqId > 0)
				{
					if (!HasChallenge(member, mapId, challengeLv) && !sReq->Check(member, abs(reqId)))
					{
						teleport = false;
						ChatHandler(member->GetSession()).PSendSysMessage("未达到进入条件，选择难度失败");
						if (leader->GetGUID() != member->GetGUID())
							ChatHandler(leader->GetSession()).PSendSysMessage("未达到进入条件，选择难度失败");
					}
				}
			}

		}

		if (reqId < 0)
		{
			if (!HasChallenge(leader, mapId, challengeLv) && !sReq->Check(leader, abs(reqId)))
			{
				teleport = false;
				ChatHandler(leader->GetSession()).PSendSysMessage("未达到进入条件，选择难度失败");
			}
		}
	}
	else if (!HasChallenge(leader, mapId, challengeLv) && !sReq->Check(leader, abs(reqId)))
	{
		teleport = false;
		ChatHandler(leader->GetSession()).PSendSysMessage("未达到进入条件，选择难度失败");
	}

	if (!teleport)
		return;

	if (group)
	{
		group->SetDungeonDifficulty(Difficulty(BIND_INSTANCE_DIFF));
		group->SetRaidDifficulty(Difficulty(BIND_INSTANCE_DIFF));

		for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
		{
			if (Player* member = itr->GetSource())
			{
				bool haschallenge = HasChallenge(member, mapId, challengeLv);

				if (!haschallenge)
				{
					if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
					{
						sCF->ResetInstance(member, Difficulty(0), mapEntry->IsRaid(), mapId);
						sCF->ResetInstance(member, Difficulty(1), mapEntry->IsRaid(), mapId);
					}
				}

				if (!haschallenge && reqId > 0)
					sReq->Des(member, abs(reqId));

				ReplaceChallenge(member, mapId, challengeLv);
				member->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
			}
		}

		if (!HasChallenge(leader, mapId, challengeLv) && reqId < 0)
			sReq->Des(leader, abs(reqId));
	}
	else
	{
		bool haschallenge = HasChallenge(leader, mapId, challengeLv);

		if (!haschallenge)
		{
			if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
			{
				sCF->ResetInstance(leader, Difficulty(0), mapEntry->IsRaid(), mapId);
				sCF->ResetInstance(leader, Difficulty(1), mapEntry->IsRaid(), mapId);
			}
		}

		if (!haschallenge)
			sReq->Des(leader, abs(reqId));

		ReplaceChallenge(leader, mapId, challengeLv);
		leader->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
	}
}

bool ChallengeMod::IsChallengeMap(Player* player, uint32 mapId)
{
	if (player->GetMap()->IsDungeon() && player->GetMapId() != 230)
		return false;

	for (auto itr = ChallengeVec.begin(); itr != ChallengeVec.end(); itr++)
		if (mapId == itr->MapId)
			return true;

	return false;
}

bool ChallengeMod::AddGossipOrTele(Player* player, uint32 triggerId)
{
	if (!player->GetMap()->IsDungeon())
		player->ChallengeLv = 0;

    AreaTriggerTeleport const* at = sObjectMgr->GetAreaTriggerTeleport(triggerId);
	if (!at)
		return false;

	uint32 mapId = at->target_mapId;

	if (!IsChallengeMap(player, mapId))
	{
		//player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
		return false;
	}

	if (Group* group = player->GetGroup())
	{
		if (Player* leader = ObjectAccessor::FindPlayerInOrOutOfWorld(group->GetLeaderGUID()))
		{
			if (leader->GetGUID() != player->GetGUID())
			{
				if (leader->GetMapId() == mapId)
				{
					bool haschallenge = HasChallenge(player, mapId, leader->GetMap()->challengeLv);

					int32 reqId = GetReqId(mapId, leader->GetMap()->challengeLv);

					if (!haschallenge && reqId > 0)
					{
						if (sReq->Check(player, abs(reqId)))
						{
							sReq->Des(player, abs(reqId));

							if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
							{
								sCF->ResetInstance(player, Difficulty(0), mapEntry->IsRaid(), mapId);
								sCF->ResetInstance(player, Difficulty(1), mapEntry->IsRaid(), mapId);
							}
							ReplaceChallenge(player, mapId, leader->GetMap()->challengeLv);
							player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
						}
					}
					else
					{
						if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
						{
							sCF->ResetInstance(player, Difficulty(0), mapEntry->IsRaid(), mapId);
							sCF->ResetInstance(player, Difficulty(1), mapEntry->IsRaid(), mapId);
						}
						ReplaceChallenge(player, mapId, leader->GetMap()->challengeLv);
						player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
					}
				}
				else
					ChatHandler(player->GetSession()).PSendSysMessage("等待队长选择难度");

				return true;
			}
		}
	}

	player->PlayerTalkClass->ClearMenus();


	//团队副本不允许切换难度
	//for (auto itr = ChallengeVec.begin(); itr != ChallengeVec.end(); itr++)
	//	if (mapId == itr->MapId)
	//	{
	//		if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
	//		{
	//			if (HasChallenge(player, itr->MapId, itr->Level) && mapEntry->IsRaid())
	//			{
	//				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, itr->GossipText + " - 已绑定该难度", triggerId, itr->Level);
	//				player->PlayerTalkClass->GetGossipMenu().SetMenuId(7891);
	//				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
	//				return true;
	//			}
	//		}
	//	}

	std::string text = "";

	for (auto itr = ChallengeVec.begin(); itr != ChallengeVec.end(); itr++)
		if (mapId == itr->MapId)
			if (HasChallenge(player, itr->MapId, itr->Level))
				text = itr->GossipText;


	if (!text.empty())
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, text + " - 重置该难度", triggerId, 10000, "重置" + text + "\n将解除绑定，确定重置吗？", 0, false);

	for (auto itr = ChallengeVec.begin(); itr != ChallengeVec.end(); itr++)
		if (mapId == itr->MapId)
		{
			if (HasChallenge(player, itr->MapId, itr->Level))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, itr->GossipText + " - 已绑定该难度", triggerId, itr->Level);
			else// if (sReq->Check(player, abs(itr->ReqId), 1, false))
				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, itr->GossipText, triggerId, itr->Level, sReq->Notice(player, abs(itr->ReqId), "选择", itr->GossipText), sReq->Golds(abs(itr->ReqId)), false);
		}

	player->PlayerTalkClass->GetGossipMenu().SetMenuId(7891);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());

	return true;
}


class ChallengePlayerScript : public PlayerScript
{
public:
	ChallengePlayerScript() : PlayerScript("ChallengePlayerScript") {}

	void OnGossipSelect(Player* player, uint32 menu_id, uint32 triggerId, uint32 action) override
	{
		if (menu_id != 7891)
			return;

		if (action == 10000)
		{
            AreaTriggerTeleport const* at = sObjectMgr->GetAreaTriggerTeleport(triggerId);
			if (!at)
				return;

			uint32 mapId = at->target_mapId;

			if (Group* group = player->GetGroup())
			{
				for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
					if (Player* member = itr->GetSource())
					{
						if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
						{
							sCF->ResetInstance(member, Difficulty(0), mapEntry->IsRaid(), mapId);
							sCF->ResetInstance(member, Difficulty(1), mapEntry->IsRaid(), mapId);
						}

						sChallengeMod->ResetChallenge(member->GetGUIDLow(), mapId);
					}

			}
			else
			{
				if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
				{
					sCF->ResetInstance(player, Difficulty(0), mapEntry->IsRaid(), mapId);
					sCF->ResetInstance(player, Difficulty(1), mapEntry->IsRaid(), mapId);
				}

				sChallengeMod->ResetChallenge(player->GetGUIDLow(), mapId);
			}

			sChallengeMod->AddGossipOrTele(player, triggerId);
		}
		else
			sChallengeMod->ChangeLevel(player, triggerId, action);
	}
};

void AddSC_Challenge()
{
	new ChallengePlayerScript();
}

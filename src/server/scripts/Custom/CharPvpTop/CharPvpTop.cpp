#pragma execution_character_set("utf-8")
#include "CharPvpTop.h"
#include "MapManager.h"
#include "../Switch/Switch.h"
#include "../CommonFunc/CommonFunc.h"
#include "../PvP/PvP.h"
#include "../String/myString.h"
#include "../GCAddon/GCAddon.h"
#include "../GS/GS.h"

CharPvpTop::CharPvpTop()
{
	topevent = false;
	topGobGuid = 0;
	isfirst = false;
	pvpcount = 0;
}

CharPvpTop::~CharPvpTop()
{

}
void CharPvpTop::LoadTopSys()
{
	VCTopSys.clear();
	topbufflists.clear();

	QueryResult areaidcustom = WorldDatabase.PQuery("SELECT entry,name,itemid,buff from _活动_比武大会");
	if (areaidcustom)
	{
		int nCount = 1;
		do
		{
			Field * fields = areaidcustom->Fetch();
			CTopSys tmpmorph;

			tmpmorph.entry = fields[0].GetUInt32();
			tmpmorph.text = fields[1].GetString();
			tmpmorph.itemid = fields[2].GetUInt32();
			tmpmorph.buffs = fields[3].GetUInt32();

			topbufflists.push_back(tmpmorph.buffs);
			VCTopSys.insert(CTopSys_t::value_type(nCount, tmpmorph));

			nCount++;

		} while (areaidcustom->NextRow());
		sLog->outString(">> 读取自定义功能数据表1_topsys,共%u条数据读取加载...", nCount);
	}
	else
		sLog->outString(">> 读取自定义功能数据表1_topsys,共0条数据读取加载...");
}

GameObject* CharPvpTop::SpawnGob(uint32 guid, bool created)
{

	GameObject* creature = NULL;

	GameObjectData const* data = sObjectMgr->GetGOData(guid);
	if (!data)
		return creature;
	//Get map object
	Map* map = sMapMgr->CreateBaseMap(data->mapid);
	if (!map)
		return creature;

	if (created)
	{
		creature = new GameObject;
		if (!creature->Create(sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT), data->id, map, data->phaseMask, data->posX, data->posY, data->posZ, data->orientation, G3D::Quat(), 0, GO_STATE_READY))
		{
			delete creature;
			return NULL;
		}

		map->AddToMap(creature);
		creature->setActive(true);
		creature->SetPhaseMask(1, true);
		topGobGuid = creature->GetGUID();
	}
	else
	{
		creature = map->GetGameObject(topGobGuid);
	}
	return creature;
}


void CharPvpTop::RollTopTeamID()
{
	if (!topevent)
		return;

	UpdateCharTopData();

	if (!pvpcount)
	{
		if (uint32(sSwitch->GetValue(TOP_97)) > GetTopMax())
		{
			sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(TOP_STR_102), sSwitch->GetValue(TOP_97)));
			sGameEventMgr->StopEvent(sSwitch->GetValue(TOP_93), true);
			return;
		}
	}
	else
	{
		sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(TOP_STR_100), pvpcount + 1));
	}
	pvpcount++;
	uint32 maxcount = _charTopDataMap.size() / 2;
	for (uint32 i = 1; i < maxcount + 1; ++i)
	{
		uint32 pl1, pl2;
		uint32 rollid1 = irand(0, _charTopDataMap.size() - 1);
		uint32 myid1 = 0;
		for (std::vector<uint32>::iterator itr1 = _charTopDataMap.begin(); itr1 != _charTopDataMap.end(); itr1++)
		{
			if (myid1 == rollid1)
			{
				pl1 = *itr1;
				_charTopDataMap.erase(itr1);
				break;
			}
			myid1++;
		}

		uint32 rollid2 = irand(0, _charTopDataMap.size() - 1);
		uint32 myid2 = 0;
		for (std::vector<uint32>::iterator itr2 = _charTopDataMap.begin(); itr2 != _charTopDataMap.end(); itr2++)
		{
			if (myid2 == rollid2)
			{
				pl2 = *itr2;
				_charTopDataMap.erase(itr2);
				break;
			}
			myid2++;
		}

		AddTopTeam(i, pl1, pl2);
		SengPVPgo(i);
	}
}

void CharPvpTop::UpdateCharTopData()
{
	_charTopDataMap.clear();
	_charTopMaxMap.clear();
	SessionMap::const_iterator itr1;
	for (itr1 = sWorld->GetAllSessions().begin(); itr1 != sWorld->GetAllSessions().end(); ++itr1)
	{
		if (itr1->second && itr1->second->GetPlayer() && itr1->second->GetPlayer()->IsInWorld())
		{
			if (itr1->second->GetPlayer()->GetAreaId() != sSwitch->GetValue(TOP_94) && itr1->second->GetPlayer()->m_topmc == 999) //不在区域并且 才报名)
			{
				itr1->second->GetPlayer()->m_topmc = 0; //无名次
				itr1->second->GetPlayer()->m_lasttopmc = 0; //无名次
				continue;
			}

			if (itr1->second->GetPlayer()->m_topmc != 999) //没有决斗出名次
				continue;

			uint32 guid = itr1->second->GetPlayer()->GetGUIDLow();
			_charTopDataMap.push_back(guid);
			_charTopMaxMap.push_back(guid);
		}
	}
}

void CharPvpTop::SengPVPgo(uint32 teamid)
{
	Player * pl1 = sObjectMgr->GetPlayerByLowGUID(VCtopCustom[teamid].pl1);
	Player * pl2 = sObjectMgr->GetPlayerByLowGUID(VCtopCustom[teamid].pl2);

	if (!pl1 || !pl2)
		return;

	if (pl1->duel || pl2->duel)
		return;

	pl1->m_topteam = pl2->m_topteam = teamid;

	GameObject* obj = pl1->GetMap()->GetGameObject(topGobGuid);
	if (!obj)
		return;

	DuelInfo* duel = new DuelInfo;
	duel->initiator = pl1;
	duel->opponent = pl2;
	duel->startTime = 0;
	duel->startTimer = 0;
	duel->isMounted = 0; // Mounted Duel
	duel->istop = true;
	pl1->duel = duel;

	DuelInfo* duel2 = new DuelInfo;
	duel2->initiator = pl1;
	duel2->opponent = pl1;
	duel2->startTime = 0;
	duel2->startTimer = 0;
	duel2->isMounted = 0; // Mounted Duel
	duel2->istop = true;
	pl2->duel = duel2;

	pl1->SetUInt64Value(PLAYER_DUEL_ARBITER, obj->GetGUID());
	pl2->SetUInt64Value(PLAYER_DUEL_ARBITER, obj->GetGUID());

	time_t now = time(NULL);
	pl1->duel->startTimer = now;
	pl2->duel->startTimer = now;
	pl1->setpvptime = true;
	pl2->setpvptime = true;
	pl1->SendDuelCountdown(10000);
	pl2->SendDuelCountdown(10000);
	return;
}

void CharPvpTop::PVPupdate()
{
	time_t now = time(NULL);
	if (topevent) //比武开始
	{
		if (now >= pvptime + sSwitch->GetValue(TOP_95))
		{
			SessionMap::const_iterator itr1;
			for (itr1 = sWorld->GetAllSessions().begin(); itr1 != sWorld->GetAllSessions().end(); ++itr1)
			{
				if (itr1->second && itr1->second->GetPlayer() && itr1->second->GetPlayer()->IsInWorld())
				{
					if (itr1->second->GetPlayer()->GetAreaId() == sSwitch->GetValue(TOP_94))
					{
						if (itr1->second->GetPlayer()->duel && Isintop(itr1->second->GetPlayer()->GetGUIDLow()))
						{
							if (itr1->second->GetPlayer()->GetHealthPct() < itr1->second->GetPlayer()->duel->opponent->GetHealthPct())
								itr1->second->GetPlayer()->DuelComplete(DUEL_FLED);
							else if (itr1->second->GetPlayer()->GetHealthPct() == itr1->second->GetPlayer()->duel->opponent->GetHealthPct())
							{
								if (itr1->second->GetPlayer()->GetHealth() < itr1->second->GetPlayer()->duel->opponent->GetHealth())
									itr1->second->GetPlayer()->DuelComplete(DUEL_FLED);
							}

						}
					}
				}
			}
			RollTopTeamID();
			pvptime = now;
		}
	}
}

void CharPvpTop::SendTopTitle(bool apple)
{
	if (apple)
	{
		std::list<uint64> guids;
		for (SessionMap::const_iterator itr2 = sWorld->GetAllSessions().begin(); itr2 != sWorld->GetAllSessions().end(); ++itr2)
		{
			if (itr2->second && itr2->second->GetPlayer() && itr2->second->GetPlayer()->IsInWorld())
			{
				if (itr2->second->GetPlayer()->m_toptitle != "")
				{
					itr2->second->GetPlayer()->m_toptitle = "";
					guids.push_back(itr2->second->GetPlayer()->GetGUID());
				}
				itr2->second->GetPlayer()->m_lasttopmc = 0;
				SendAndClearTopBuff(itr2->second->GetPlayer());
			}
		}

		if (guids.size())
		{
			for (std::list<uint64>::const_iterator itr = guids.begin(); itr != guids.end(); ++itr)
			{
				Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(*itr);
				if (player)
				{
					sWorld->UpdateTopPlayerData(player->GetGUIDLow(), "");
					for (SessionMap::const_iterator itr22 = sWorld->GetAllSessions().begin(); itr22 != sWorld->GetAllSessions().end(); ++itr22)
					{
						if (itr22->second && itr22->second->GetPlayer() && itr22->second->GetPlayer()->IsInWorld())
							itr22->second->SendNameQueryOpcode(player->GetGUID());
					}
					player->ToggleDND();
					player->m_toptime = time(NULL);
					player->m_sendtoptitle = true;
				}
			}
		}

		sWorld->UpdateAllTopPlayerData();
		CharacterDatabase.PExecute("update _xlchar set lasttop = 0,toptitle =''");
	}
	else
	{
		std::list<uint64> guids;
		//sWorld->UpdateGlobalPlayerData(player->GetGUIDLow(), PLAYER_UPDATE_DATA_NAME, player->GetName());
		for (SessionMap::const_iterator itr2 = sWorld->GetAllSessions().begin(); itr2 != sWorld->GetAllSessions().end(); ++itr2)
		{
			if (itr2->second && itr2->second->GetPlayer() && itr2->second->GetPlayer()->IsInWorld())
			{
				SendAndClearTopBuff(itr2->second->GetPlayer());

				if (itr2->second->GetPlayer()->m_lasttopmc)
				{
					const CTopSys *cccc = FindTopSys(itr2->second->GetPlayer()->m_lasttopmc);
					if (cccc)
					{
						if (cccc->text != "")
						{
							guids.push_back(itr2->second->GetPlayer()->GetGUID());
							itr2->second->GetPlayer()->m_toptitle = cccc->text;
						}
						if (cccc->buffs)
							itr2->second->GetPlayer()->AddAura(cccc->buffs, itr2->second->GetPlayer());
					}

				}
			}
		}

		if (guids.size())
		{
			for (std::list<uint64>::const_iterator itr = guids.begin(); itr != guids.end(); ++itr)
			{
				Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(*itr);
				if (player)
				{
					sWorld->UpdateTopPlayerData(player->GetGUIDLow(), player->m_toptitle);
					for (SessionMap::const_iterator itr22 = sWorld->GetAllSessions().begin(); itr22 != sWorld->GetAllSessions().end(); ++itr22)
					{
						if (itr22->second && itr22->second->GetPlayer() && itr22->second->GetPlayer()->IsInWorld())
							itr22->second->SendNameQueryOpcode(player->GetGUID());
					}
					player->ToggleDND();
					player->m_toptime = time(NULL);
					player->m_sendtoptitle = true;
				}
			}
		}
	}
}

void CharPvpTop::SendAndClearTopBuff(Player * pl, bool onlyph)
{
	uint32 buffid = 0;
	const CTopSys *cccc = FindTopSys(pl->m_lasttopmc);
	if (cccc && cccc->buffs)
		buffid = cccc->buffs;

	if (!buffid)
	{
		for (std::vector<uint32>::iterator itr1 = topbufflists.begin(); itr1 != topbufflists.end(); itr1++)
		{
			if (pl->HasAura(*itr1))
				pl->RemoveAurasDueToSpell(*itr1);
		}
	}
	else
	{
		for (std::vector<uint32>::iterator itr1 = topbufflists.begin(); itr1 != topbufflists.end(); itr1++)
		{
			if (buffid == *itr1 && !pl->HasAura(*itr1))
				pl->AddAura(buffid, pl);
			else if (buffid != *itr1 && pl->HasAura(*itr1))
				pl->RemoveAurasDueToSpell(*itr1);
		}
	}
}


class custom_top_group : public CreatureScript
{
public:
	custom_top_group() : CreatureScript("npc_top")
	{
	}

	bool OnGossipHello(Player* player, Creature* creature) override
	{
		player->PlayerTalkClass->ClearMenus();

		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "比武大会排行榜", 1, 4);

		if (player->m_topmc == 999)
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "你已报名", 1, 1);
		}
		else if (player->m_topmc > 0 && player->m_topmc < 999)
		{
			if (IsEventActive(sSwitch->GetValue(TOP_93)))
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "比武大会已经开始", 1, 1);
			}
			else
			{
				const CTopSys *cccc = sCharPvpTop->FindTopSys(player->m_topmc);
				if (cccc && cccc->itemid)
				{
					if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(cccc->itemid))
					{
						player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "领取比武大会奖励", 1, 2);
					}
					else
					{
						creature->MonsterSay("奖励物品ID设置出错...", LANG_UNIVERSAL, 0);
						return false;
					}
				}
				else
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "比武大会报名", 1, 3);
			}

		}
		else if (!player->m_topmc)
		{
			if (!IsEventActive(sSwitch->GetValue(TOP_93)))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "比武大会报名", 1, 3);
			else
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "比武大会已经开始", 1, 1);
		}


		player->SEND_GOSSIP_MENU(20000, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		if (action == 1)
			OnGossipHello(player, creature);
		else if (action == 2)
		{
			const CTopSys *cccc = sCharPvpTop->FindTopSys(player->m_topmc);
			if (cccc && cccc->itemid)
			{
				if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(cccc->itemid))
					player->AddItem(cccc->itemid, 1);
			}

			player->m_topmc = 0;
			CharacterDatabase.PExecute("update _xlchar set top = %u where guid = %u", player->m_topmc, player->GetGUIDLow());
			creature->MonsterWhisper("比武大会奖励领取成功...", player);
			OnGossipHello(player, creature);
			return false;
		}
		else if (action == 3)
		{
			player->m_topmc = 999;
			CharacterDatabase.PExecute("update _xlchar set top = %u where guid = %u", player->m_topmc, player->GetGUIDLow());
			creature->MonsterWhisper("比武大会报名成功...", player);
			OnGossipHello(player, creature);
			return false;
		}
		else if (action == 4)
		{
			player->PlayerTalkClass->ClearMenus();
			if (sCharPvpTop->m_playertopph.size())
			{
				for (int i = 1; i < sCharPvpTop->m_playertopph.size() + 1; i++)
				{
					if (i > 10)
						continue;

					uint32 guid = sCharPvpTop->m_playertopph[i];
					if (guid)
					{
						GlobalPlayerData const* playerData = sWorld->GetGlobalPlayerData(guid);
						if (playerData)
						{
							std::ostringstream hsfdata;

							bool hastouxian = false;
							const CTopSys *cccc = sCharPvpTop->FindTopSys(i);
							if (cccc && cccc->text != "")
								hastouxian = true;

							if (hastouxian)
								hsfdata << cccc->text << ";" << playerData->name.c_str();
							else
								hsfdata << "第" << i << "名;" << playerData->name.c_str();

							player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, hsfdata.str().c_str(), 1, 5);
						}

					}
				}
			}
			else
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "暂无排行榜", 1, 5);

			player->SEND_GOSSIP_MENU(20000, creature->GetGUID());
		}
		else if (action == 5)
		{
			OnGossipHello(player, creature);
			return false;
		}
		return true;
	}
};

void AddSC_CharPvpTop()
{
	new custom_top_group();
}

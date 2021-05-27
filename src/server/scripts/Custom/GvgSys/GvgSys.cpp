#pragma execution_character_set("utf-8")
#include "GvgSys.h"
#include "MapManager.h"
#include "GuildMgr.h"
#include "Guild.h"
#include "../Switch/Switch.h"
#include "../CommonFunc/CommonFunc.h"
#include "../PvP/PvP.h"
#include "../String/myString.h"
#include "../GCAddon/GCAddon.h"
#include "../GS/GS.h"
#include "Pet.h"

GvgSys::GvgSys()
{
	m_guildId1 = 0;
	m_guildId2 = 0;
	GCevent = false;
	Vgvgconf.clear();
}

GvgSys::~GvgSys()
{

}

void Player::GCPlayerInTeam(bool action)
{
	if (action)
	{
		if (isfirstingvg) //��һ�ε�½��������
		{
			isfirstingvg = false;
			m_playereventdam[sSwitch->GetValue(GVG_109)] = 0;
		}
	}

	ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(GetGcRaceOrRace(action));

	if (rEntry  && action)
		setFaction(rEntry->FactionID);
	else
		setFactionForRace(getRace());

	if (Pet * pet = GetPet())
		pet->setFaction(getFaction());
}

uint8 Player::GetGcRaceOrRace(bool action)
{
	uint8 playerrace = getRace();
	if (sGvgSys->getGuildId1() == GetGuildId()) //�سǱ�����
	{
		if (GetTeamId() == 1)
			playerrace = RACE_HUMAN;
		else
			playerrace = getRace();

		if (sSwitch->GetValue(GVG_178))
		{
			if (action)
				AddAura(sSwitch->GetValue(GVG_178), this);
		}
	}

	if (sGvgSys->getGuildId2() == GetGuildId()) //���Ǳ䲿��
	{
		if (GetTeamId() == 0)
			playerrace = RACE_ORC;
		else
			playerrace = getRace();

		if (sSwitch->GetValue(GVG_179))
		{
			if (action)
				AddAura(sSwitch->GetValue(GVG_179), this);
		}
	}

	if (!action)
	{
		if (sSwitch->GetValue(GVG_178))
			RemoveAurasDueToSpell(sSwitch->GetValue(GVG_178));
		if (sSwitch->GetValue(GVG_179))
			RemoveAurasDueToSpell(sSwitch->GetValue(GVG_179));
	}
	return playerrace;
}

bool Player::IsInDistGCNPC()
{
	if (!GetGuild())
		return false;

	Creature* c1 = FindNearestCreature(sSwitch->GetValue(GVG_117), 2.0f);
	Creature* c2 = FindNearestCreature(sSwitch->GetValue(GVG_116), 2.0f);

	if (c1 && GetGuildId() == sGvgSys->getGuildId2())
	{
		return true;
	}

	if (c2 && GetGuildId() == sGvgSys->getGuildId1())
	{
		return true;
	}
	return false;
}

void Player::IsInDistTELENPC()
{
	if (!GetGuild())
		return;

	Creature* c1 = FindNearestCreature(sSwitch->GetValue(GVG_118), 3.0f);
	if (c1 && GetGuildId() == sGvgSys->getGuildId2())
	{
		uint32 Chance = urand(4, 10);

		const gvgconf * aaaaad = sGvgSys->Findgvg(Chance);
		if (aaaaad)
		{
			TeleportTo(aaaaad->mapid, aaaaad->m_x, aaaaad->m_y, aaaaad->m_z, aaaaad->m_o);
		}
	}
}

void GvgSys::LoadgvgSys()
{
	Vgvgconf.clear();
	QueryResult resultBuffs = WorldDatabase.Query("SELECT id,mapid,m_x,m_y,m_z,m_o,min_x,max_x,min_y,max_y,min_z,max_z,areaids,itemid FROM _�_����ս");
	if (resultBuffs)
	{
		gvgconf tmpspell;
		uint32 ccccc = 0;
		do
		{
			Field *fields = resultBuffs->Fetch();

			tmpspell.id = fields[0].GetUInt32();
			tmpspell.mapid = fields[1].GetUInt32();
			tmpspell.m_x = fields[2].GetFloat();
			tmpspell.m_y = fields[3].GetFloat();
			tmpspell.m_z = fields[4].GetFloat();
			tmpspell.m_o = fields[5].GetFloat();
			tmpspell.min_x = fields[6].GetFloat();
			tmpspell.max_x = fields[7].GetFloat();
			tmpspell.min_y = fields[8].GetFloat();
			tmpspell.max_y = fields[9].GetFloat();
			tmpspell.min_z = fields[10].GetFloat();
			tmpspell.max_z = fields[11].GetFloat();
			tmpspell.areaids = fields[12].GetString();
			tmpspell.itemid = fields[13].GetUInt32();

			Vgvgconf.insert(gvgconf_t::value_type(tmpspell.id, tmpspell));
			ccccc++;
		} while (resultBuffs->NextRow());
		sLog->outString(">> ��ȡ�Զ��幦�����ݱ� _�_����ս,��%u�����ݶ�ȡ����...", ccccc);
	}
	else
		sLog->outString(">> ��ȡ�Զ��幦�����ݱ� _�_����ս,��0�����ݶ�ȡ����...");

	QueryResult result2 = CharacterDatabase.PQuery("SELECT guild1,guild2,data FROM GuildvsGuild where guid = 1");
	if (result2)
	{
		m_guildId1 = result2->Fetch()[0].GetUInt32();
		m_guildId2 = result2->Fetch()[1].GetUInt32();
		m_gvgtime = result2->Fetch()[2].GetUInt32();
	}
	else
	{
		uint32 guid = 1;
		CharacterDatabase.PExecute("INSERT INTO GuildvsGuild (guid) VALUES ('%u')", guid);
	}
}

void GvgSys::StartEventSys(uint16 event_id)
{
	if (event_id == sSwitch->GetValue(GVG_109)) //����ϵͳ
	{
		if (getGuildId1() > 0 && getGuildId2() > 0)
		{
			Guild * pguild1 = sGuildMgr->GetGuildById(getGuildId1());
			Guild * pguild2 = sGuildMgr->GetGuildById(getGuildId2());
			GCevent = true;
			GCtime = sSwitch->GetValue(GVG_110);

			const gvgconf * gcnpc = Findgvg(3);

			std::set<uint32> moderators;
			Tokenizer tokens(gcnpc->areaids, '#');

			for (Tokenizer::const_iterator i = tokens.begin(); i != tokens.end(); ++i)
			{
				uint32 moderator_acc = atol(*i);
				moderators.insert(moderator_acc);
			}

			SessionMap::const_iterator itr;
			for (itr = sWorld->GetAllSessions().begin(); itr != sWorld->GetAllSessions().end(); ++itr)
			{
				if (itr->second && itr->second->GetPlayer() && itr->second->GetPlayer()->IsInWorld())
				{
					if (moderators.find(itr->second->GetPlayer()->GetAreaId()) != moderators.end())
					{
						itr->second->GetPlayer()->GCPlayerInTeam(true);
						if (itr->second->GetPlayer()->GetGuildId() != getGuildId1() && itr->second->GetPlayer()->GetGuildId() != getGuildId2())
						{
							itr->second->GetPlayer()->TeleportTo(itr->second->GetPlayer()->m_homebindMapId, itr->second->GetPlayer()->m_homebindX, itr->second->GetPlayer()->m_homebindY, itr->second->GetPlayer()->m_homebindZ, itr->second->GetPlayer()->GetOrientation());
						}
					}
				}
			}

			if (pguild1 && pguild2)
				sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(GVG_STR_1153), pguild2->GetName().c_str(), pguild1->GetName().c_str()));
		}
	}
}

bool GvgSys::IsInAreaGC(Player * pl)
{
	const gvgconf * aaaaad = Findgvg(3);

	if (!aaaaad)
		return false;

	std::set<uint32> moderators;
	Tokenizer tokens(aaaaad->areaids, '#');


	for (Tokenizer::const_iterator i = tokens.begin(); i != tokens.end(); ++i)
	{
		uint32 moderator_acc = atol(*i);

		moderators.insert(moderator_acc);
	}

	if (moderators.find(pl->GetAreaId()) != moderators.end())
		return true;

	return false;
}

bool GvgSys::IsInDistGC(Player * pl)
{
	if (!pl->IsAlive())
		return false;

	const gvgconf * aaaaad = Findgvg(3);

	if (!aaaaad)
		return false;

	std::set<uint32> moderators;
	Tokenizer tokens(aaaaad->areaids, '#');


	for (Tokenizer::const_iterator i = tokens.begin(); i != tokens.end(); ++i)
	{
		uint32 moderator_acc = atol(*i);

		moderators.insert(moderator_acc);
	}

	if (moderators.find(pl->GetAreaId()) == moderators.end())
	{
		pl->livedisc = true;
		pl->goindisc = false;
		return false;
	}

	if (pl->GetPositionX() >= aaaaad->min_x && aaaaad->max_x >= pl->GetPositionX() && pl->GetPositionY() >= aaaaad->min_y && aaaaad->max_y >= pl->GetPositionY() && pl->GetPositionZ() >= aaaaad->min_z && aaaaad->max_z >= pl->GetPositionZ())
	{
		if (!pl->goindisc) //����û�п���
		{
			pl->goindisc = true;
			pl->livedisc = false;
			ChatHandler(pl->GetSession()).PSendSysMessage("���빤��ս����,���ڹ���ģʽΪ���������Ա"); //���빥��������ʾ
		}
		return true;
	}
	else
	{
		if (!pl->livedisc)
		{
			ChatHandler(pl->GetSession()).PSendSysMessage("�뿪����ս����"); //�뿪����������ʾ
			pl->livedisc = true;
			pl->goindisc = false;
		}
		return false;
	}
}

void GvgSys::UpdateGvGevent()
{
	if (IsGuildvsGuild() && IsGCevent())
	{
		const gvgconf * gcnpc = Findgvg(3);

		std::set<uint32> moderators;
		Tokenizer tokens(gcnpc->areaids, '#');
		for (Tokenizer::const_iterator i = tokens.begin(); i != tokens.end(); ++i)
		{
			uint32 moderator_acc = atol(*i);
			moderators.insert(moderator_acc);
		}

		time_t now = time(NULL);

		bool gc1 = false;
		bool gc2 = false;

		SessionMap::const_iterator itr1;
		for (itr1 = sWorld->GetAllSessions().begin(); itr1 != sWorld->GetAllSessions().end(); ++itr1)  //�سǹ����Ա
		{
			if (itr1->second && itr1->second->GetPlayer() && itr1->second->GetPlayer()->IsInWorld() && itr1->second->GetPlayer()->GetGuildId() == getGuildId1())
			{
				if (IsInDistGC(itr1->second->GetPlayer())) //���سǵĵط�������һ������
					gc1 = true;
			}
		}

		SessionMap::const_iterator itr2;
		for (itr2 = sWorld->GetAllSessions().begin(); itr2 != sWorld->GetAllSessions().end(); ++itr2)  //���ǹ����Ա
		{
			if (itr2->second && itr2->second->GetPlayer() && itr2->second->GetPlayer()->IsInWorld() && itr2->second->GetPlayer()->GetGuildId() == getGuildId2())
			{
				if (IsInDistGC(itr2->second->GetPlayer()))  //���سǵĵط�������һ������
					gc2 = true;
			}
		}

		if (!gc2 && gc1) //����ֻ���س����
		{
			if (GCpoint >= uint32(sSwitch->GetValue(GVG_111)))
				GCpoint = GCpoint - sSwitch->GetValue(GVG_111);
			else
				GCpoint = 0;

			if (GCpoint < 100 && GCtime < uint32(sSwitch->GetValue(GVG_110)))
				GCtime = GCtime + 1;
		}

		if (!gc1 && gc2) //����ֻ�й������
		{
			if (GCpoint <= uint32(100 - sSwitch->GetValue(GVG_111)))
				GCpoint = GCpoint + sSwitch->GetValue(GVG_111);
			else
				GCpoint = 100;
		}

		if (GCpoint == 100 && GCtime > 0)
		{
			GCtime = GCtime - 1;

			if (GCtime <= 10)
			{
				SessionMap::const_iterator itr3;
				for (itr3 = sWorld->GetAllSessions().begin(); itr3 != sWorld->GetAllSessions().end(); ++itr3)  //���ǹ����Ա
				{
					if (itr3->second && itr3->second->GetPlayer() && itr3->second->GetPlayer()->IsInWorld())
					{
						if (moderators.find(itr3->second->GetPlayer()->GetAreaId()) != moderators.end())
							sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(GVG_STR_1154), GCtime));
					}
				}
			}

			if (GCtime == 0) //����ʤ��
			{
				Guild * pguild2 = sGuildMgr->GetGuildById(m_guildId2);
				if (pguild2)
					sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(GVG_STR_1155), pguild2->GetName().c_str()));

				GCtime = sSwitch->GetValue(GVG_110);
				GCpoint = 0;
				setGuildId1(m_guildId2); //ռ�칫��ĳ�ʤ����
				setGuildId2(0);          //���Ƿ���Ϊ0;

				SessionMap::const_iterator itr3;
				for (itr3 = sWorld->GetAllSessions().begin(); itr3 != sWorld->GetAllSessions().end(); ++itr3)  //���ǹ����Ա
				{
					if (itr3->second && itr3->second->GetPlayer() && itr3->second->GetPlayer()->IsInWorld())
					{
						if (moderators.find(itr3->second->GetPlayer()->GetAreaId()) != moderators.end())
						{
							SendGVGItem(itr3->second->GetPlayer());
							itr3->second->GetPlayer()->GCPlayerInTeam(false);
						}

					}
				}

				CharacterDatabase.PExecute("update GuildvsGuild set guild1 = %u,guild2 =%u,data = %u  where guid = 1", m_guildId1, m_guildId2, time(NULL));
				GCevent = false;
			}
		}

		SessionMap::const_iterator itr5;
		for (itr5 = sWorld->GetAllSessions().begin(); itr5 != sWorld->GetAllSessions().end(); ++itr5)  //���ǹ����Ա
		{
			if (itr5->second && itr5->second->GetPlayer() && itr5->second->GetPlayer()->IsInWorld())
			{
				itr5->second->GetPlayer()->SendUpdateWorldState(sSwitch->GetValue(GVG_112), 1);
				itr5->second->GetPlayer()->SendUpdateWorldState(sSwitch->GetValue(GVG_113), 1);
				itr5->second->GetPlayer()->SendUpdateWorldState(sSwitch->GetValue(GVG_114), GCpoint);
				itr5->second->GetPlayer()->SendUpdateWorldState(sSwitch->GetValue(GVG_115), GCtime);
			}
		}
	}
}

void GvgSys::StopEventSys(uint16 event_id)
{
	if (GCevent)
	{
		if (event_id == sSwitch->GetValue(GVG_109))
		{
			if (GCpoint == 100)
			{
				Guild * pguild2 = sGuildMgr->GetGuildById(getGuildId2());
				if (pguild2)
					sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(GVG_STR_1155), pguild2->GetName().c_str()));


				uint32 m_guild1 = getGuildId1();
				setGuildId1(getGuildId2()); //ռ�칫��ĳ�ʤ����
				setGuildId2(0);          //���Ƿ���Ϊ0;
				CharacterDatabase.PExecute("update GuildvsGuild set guild1 = %u,guild2 =%u,data = %u  where guid = 1", getGuildId1(), getGuildId2(), time(NULL));
			}
			else
			{
				Guild * pguild2 = sGuildMgr->GetGuildById(getGuildId1());
				if (pguild2)
					sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(GVG_STR_1155), pguild2->GetName().c_str()));

				setGuildId1(getGuildId1()); //ռ�칫��ĳ�ʤ����
				setGuildId2(0);          //���Ƿ���Ϊ0;
				CharacterDatabase.PExecute("update GuildvsGuild set guild1 = %u,guild2 =%u  where guid = 1", getGuildId1(), getGuildId2());
			}

			const gvgconf * gcnpc = Findgvg(3);
			std::set<uint32> moderators;
			Tokenizer tokens(gcnpc->areaids, '#');
			for (Tokenizer::const_iterator i = tokens.begin(); i != tokens.end(); ++i)
			{
				uint32 moderator_acc = atol(*i);
				moderators.insert(moderator_acc);
			}

			SessionMap::const_iterator itr3;
			for (itr3 = sWorld->GetAllSessions().begin(); itr3 != sWorld->GetAllSessions().end(); ++itr3)  //���ǹ����Ա
			{
				if (itr3->second && itr3->second->GetPlayer() && itr3->second->GetPlayer()->IsInWorld())
				{
					if (moderators.find(itr3->second->GetPlayer()->GetAreaId()) != moderators.end())
					{
						SendGVGItem(itr3->second->GetPlayer());
						itr3->second->GetPlayer()->GCPlayerInTeam(false);
						itr3->second->GetPlayer()->SendUpdateWorldState(sSwitch->GetValue(GVG_112), 0);
						itr3->second->GetPlayer()->SendUpdateWorldState(sSwitch->GetValue(GVG_113), 0);
					}
				}
			}
			GCevent = false;
			GCpoint = 0;
			GCtime = sSwitch->GetValue(GVG_110);
		}
	}
}

void GvgSys::SendGVGItem(Player * pl)
{
	if (pl->m_playereventdam[sSwitch->GetValue(GVG_109)] < uint32(sSwitch->GetValue(GVG_136)))
		return;

	if (pl->GetGuildId() == sGvgSys->getGuildId1()) //ʤ����������
		pl->AddItem(sSwitch->GetValue(GVG_137), 1);
	else
		pl->AddItem(sSwitch->GetValue(GVG_138), 1);

	pl->m_playereventdam[sSwitch->GetValue(GVG_109)] = 0;
	pl->isfirstingvg = true;
}



void GCtijiao(Player * player, Creature* creature)
{
	char * tmp = new char[800];

	Guild * pguild1 = sGuildMgr->GetGuildById(sGvgSys->getGuildId1());
	if (pguild1)
	{
		std::string PlayerNewName;
		sObjectMgr->GetPlayerNameByGUID(pguild1->GetLeaderGUID(), PlayerNewName);
		Player* Leader = ObjectAccessor::FindPlayer(pguild1->GetLeaderGUID());
		//if (Leader)
		sprintf(tmp, "%s|cFF9900CC%s|r%s|cFF9900CC%s|r%s", ("|cFF0000CC����:[|r"), PlayerNewName.c_str(), ("|cFF0000CC]|r\n|cFF0000CC����:[|r"), pguild1->GetName().c_str(), ("|cFF0000CC]|r"));

	}
	else
		sprintf(tmp, "%s", ("|cFF0000CC����:[|cFF9900CC �� |r|cFF0000CC]|r\n|cFF0000CC����:[|r|cFF9900CC �� |r|cFF0000CC]|r"));

	player->ADD_GOSSIP_ITEM(0, tmp, 1000, 1);

	sprintf(tmp, "%s", ("|TInterface\\BUTTONS\\WHITE8X8.blp:1:200|t"));

	player->ADD_GOSSIP_ITEM(0, tmp, 1000, 1);

	sprintf(tmp, "%s", ("|cFF0000CC��ǰ���빥��ս����:|r\n|cFF9900CCÿ��ֻ��һ���������빫��ս|r"));

	player->ADD_GOSSIP_ITEM(0, tmp, 1000, 1);

	if (sGvgSys->getGuildId2())
	{
		Guild * pguild2 = sGuildMgr->GetGuildById(sGvgSys->getGuildId2());
		if (pguild2)
			sprintf(tmp, "|cFF0000CC[|cFF9900CC%s|r|cFF0000CC]", pguild2->GetName().c_str());
		else
			sprintf(tmp, "|cFF0000CC[|cFF9900CC%s|r|cFF0000CC]", (" �� "));
	}
	else
		sprintf(tmp, "|cFF0000CC[|cFF9900CC%s|r|cFF0000CC]", (" �� "));

	player->ADD_GOSSIP_ITEM(0, tmp, 1000, 1);

	sprintf(tmp, "%s", ("|TInterface\\BUTTONS\\WHITE8X8.blp:1:200|t"));

	player->ADD_GOSSIP_ITEM(0, tmp, 1000, 1);

	ItemTemplate const * item1 = sObjectMgr->GetItemTemplate(sSwitch->GetValue(GVG_119));
	if (item1)
	{
		ItemDisplayInfoEntry const * Itemdisplay = sItemDisplayInfoStore.LookupEntry(item1->DisplayInfoID);
		if (Itemdisplay)
		{
			if (player->HasItemCount(sSwitch->GetValue(GVG_119), 1))
				sprintf(tmp, "%s\n|TInterface\\Icons\\%s.blp :30|t%s x1   |TInterface\\RAIDFRAME\\ReadyCheck-Ready.blp:16|t", ("|cFF0000CC���빥����Ҫ��Ʒ:|r"), Itemdisplay->inventoryIcon, item1->Name1.c_str());
			else
				sprintf(tmp, "%s\n|TInterface\\Icons\\%s.blp :30|t%s x1   |TInterface\\RAIDFRAME\\ReadyCheck-NotReady.blp:16|t", ("|cFF0000CC���빥����Ҫ��Ʒ:|r"), Itemdisplay->inventoryIcon, item1->Name1.c_str());
		}
		else
		{
			if (player->HasItemCount(sSwitch->GetValue(GVG_119), 1))
				sprintf(tmp, "%s\n|TInterface\\Icons\\%s.blp :30|t%s x1   |TInterface\\RAIDFRAME\\ReadyCheck-Ready.blp:16|t", ("|cFF0000CC���빥����Ҫ��Ʒ:|r"), "Ability_Racial_PackHobgoblin", item1->Name1.c_str());
			else
				sprintf(tmp, "%s\n|TInterface\\Icons\\%s.blp :30|t%s x1   |TInterface\\RAIDFRAME\\ReadyCheck-NotReady.blp:16|t", ("|cFF0000CC���빥����Ҫ��Ʒ:|r"), "Ability_Racial_PackHobgoblin", item1->Name1.c_str());
		}
	}
	else
	{
		sprintf(tmp, "%s", ("|cFF0000CC���빥����Ʒ���ô���|r"));
	}
	player->ADD_GOSSIP_ITEM(1, tmp, 1000, 1);

	sprintf(tmp, "%s", ("|TInterface\\BUTTONS\\WHITE8X8.blp:1:200|t"));

	player->ADD_GOSSIP_ITEM(0, tmp, 1000, 1);

	sprintf(tmp, "|cFF0000CC%s|r\n\n|cFF9900CC%s|r", ("��Ҫ���빥��ó���"), ("�����˱����ǹ���᳤"));

	player->ADD_GOSSIP_ITEM(2, tmp, 1, 1000);

	player->SEND_GOSSIP_MENU(20001, creature->GetGUID());

	delete[]tmp;

	return;
}

void GCsy(Player * player, Creature* creature)
{
	char * tmp = new char[800];

	Guild * pguild1 = sGuildMgr->GetGuildById(sGvgSys->getGuildId1());
	if (pguild1)
	{
		std::string PlayerNewName;
		sObjectMgr->GetPlayerNameByGUID(pguild1->GetLeaderGUID(), PlayerNewName);

		sprintf(tmp, "%s\n%s|cFF9900CC%s|r%s\n%s|cFF9900CC%s|r%s", ("|cFF0000CC��ǰռ���������:|r"), ("|cFF0000CC�᳤:[|r"), PlayerNewName.c_str(), ("|cFF0000CC]|r"), ("|cFF0000CC����:[|r"), pguild1->GetName().c_str(), ("|cFF0000CC]|r"));
	}
	else
		sprintf(tmp, "%s|cFF9900CC%s|r%s", ("|cFF0000CC��ǰռ������:[|r"), ("��"), ("|cFF0000CC]|r"));
	player->ADD_GOSSIP_ITEM(0, tmp, 1002, 1);

	sprintf(tmp, "%s", ("|TInterface\\BUTTONS\\WHITE8X8.blp:1:200|t"));

	player->ADD_GOSSIP_ITEM(0, tmp, 1002, 1);

	sprintf(tmp, "%s", ("|cFF0000CC��ȡÿ�ս���|r"));

	player->ADD_GOSSIP_ITEM(2, tmp, 1, 1002);

	player->SEND_GOSSIP_MENU(20001, creature->GetGUID());

	delete[]tmp;

	return;
}

class GCnpcsay : public CreatureScript
{
public:
	GCnpcsay() : CreatureScript("gvg_npc"){}
	bool OnGossipHello(Player* player, Creature* creature)
	{
		if (player->IsInCombat() || player->isDead())
		{
			player->GetSession()->SendAreaTriggerMessage("�������޷����ҶԻ�.");
			return true;
		}
		player->CLOSE_GOSSIP_MENU();
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\ICONS\\Ability_Paladin_ShieldoftheTemplar.blp:30|t���ṥ���ύ", 1000, 1);
		//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\ICONS\\Trade_BlackSmithing.blp:30|t������ֲֿ�", 1001, 1);
		if (sSwitch->GetValue(GVG_120))
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\ICONS\\Spell_unused2.blp:30|t�����Աÿ������", 1002, 1);

		//player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface\\ICONS\\Achievement_Arena_2v2_7.blp:30|t������Ϣ��ѯ", 1003, 1);
		player->SEND_GOSSIP_MENU(20001, creature->GetGUID());
		return true;
	}
	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
	{
		player->PlayerTalkClass->ClearMenus();

		if (sender == 1000)
			GCtijiao(player, creature);
		else if (sender == 1002)
			GCsy(player, creature);

		if (action == 1000)
		{
			if (sGvgSys->getGuildId1() == 0)
			{
				Guild * pguid = sGuildMgr->GetGuildById(player->GetGuildId());
				if (pguid)
				{
					if (pguid->GetLeaderGUID() == player->GetGUID())
					{
						if (player->HasItemCount(sSwitch->GetValue(GVG_119), 1))
						{
							player->DestroyItemCount(sSwitch->GetValue(GVG_119), 1, true);
							sGvgSys->setGuildId1(player->GetGuildId());
							sGvgSys->m_gvgtime = time(NULL);
							sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(GVG_STR_1158), pguid->GetName().c_str()));
							CharacterDatabase.PExecute("update GuildvsGuild set guild1 = %u,guild2 =%u,data=%u  where guid = 1", sGvgSys->getGuildId1(), sGvgSys->getGuildId2(), sGvgSys->m_gvgtime);
						}
						else
							ChatHandler(player->GetSession()).PSendSysMessage("��û�й���������Ʒ");
					}
					else
						ChatHandler(player->GetSession()).PSendSysMessage("�㲻�ǹ���᳤");
				}
				else
					ChatHandler(player->GetSession()).PSendSysMessage("��û�м����κι���");
			}
			else
			{
				if (sGvgSys->getGuildId2())
					ChatHandler(player->GetSession()).PSendSysMessage("���������������빥��");
				else
				{
					Guild * pguid = sGuildMgr->GetGuildById(player->GetGuildId());
					if (pguid)
					{
						if (sGvgSys->GCevent)
						{
							ChatHandler(player->GetSession()).PSendSysMessage("����ս������,�����ύ");
							GCtijiao(player, creature);
							return true;
						}

						if (player->GetGuildId() == sGvgSys->getGuildId1())
							ChatHandler(player->GetSession()).PSendSysMessage("�㲻��������");
						else
						{
							if (pguid->GetLeaderGUID() == player->GetGUID())
							{
								if (player->HasItemCount(sSwitch->GetValue(GVG_119), 1))
								{
									player->DestroyItemCount(sSwitch->GetValue(GVG_119), 1, true);
									sGvgSys->setGuildId2(player->GetGuildId());
									Guild * pguild1 = sGuildMgr->GetGuildById(sGvgSys->getGuildId1());
									if (pguild1)
										sWorld->SendServerMessage(SERVER_MSG_STRING, sString->Format(sString->GetText(GVG_STR_1156), pguid->GetName().c_str(), pguild1->GetName().c_str()));

									CharacterDatabase.PExecute("update GuildvsGuild set guild1 = %u,guild2 =%u  where guid = 1", sGvgSys->getGuildId1(), sGvgSys->getGuildId2());
								}
								else
									ChatHandler(player->GetSession()).PSendSysMessage("��û�й���������Ʒ");

							}
							else
								ChatHandler(player->GetSession()).PSendSysMessage("�㲻�ǹ���᳤");
						}
					}
					else
						ChatHandler(player->GetSession()).PSendSysMessage("��û�м����κι���");
				}
			}

			GCtijiao(player, creature);
			return true;
		}
		else if (action == 1002)
		{
			Guild * pguild1 = sGuildMgr->GetGuildById(sGvgSys->getGuildId1());
			if (pguild1)
			{
				if (Guild * plguild = sGuildMgr->GetGuildById(player->GetGuildId()))
				{
					if (player->GetGuildId() == sGvgSys->getGuildId1())
					{
						if (player->inguildtime && player->inguildtime < sGvgSys->m_gvgtime) //�ڹ���ʤ��ǰ���빫��
						{
							time_t now = time(NULL);
							int32 shengyutime = player->lqguildtime + sSwitch->GetValue(GVG_120) - now;
							if (shengyutime < 0)
							{
								const gvgconf * gcnpc = sGvgSys->Findgvg(3);
								if (gcnpc && gcnpc->itemid)
									player->AddItem(gcnpc->itemid, 1);

								player->lqguildtime = now;
							}
							else if (shengyutime > 0)
							{
								std::string timessss = secsToTimeString(shengyutime, true);
								ChatHandler(player->GetSession()).PSendSysMessage("����Ҫ%s�ſ�����ȡ����", timessss.c_str());
							}
						}
						else
							ChatHandler(player->GetSession()).PSendSysMessage(("����ս֮ǰ���빫�����Ҳſ�����ȡ"));
					}
					else
						ChatHandler(player->GetSession()).PSendSysMessage(("�㲻���������ĳ�Ա"));
				}
				else
					ChatHandler(player->GetSession()).PSendSysMessage(("��û�й���"));
			}
			else
				ChatHandler(player->GetSession()).PSendSysMessage(("����û�б�ռ��"));

			GCsy(player, creature);
			return true;
		}

		return true;
	}
};

void AddSC_GvgSys()
{
	new GCnpcsay();
}

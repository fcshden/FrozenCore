#pragma execution_character_set("utf-8")
#include "../CommonFunc/CommonFunc.h"
#include "ServerAnnounce.h"
#include "../String/myString.h"
#include "../Switch/Switch.h"

std::vector<ServerAnnounceTemplate> ServerAnnounceVec;

void ServerAnnounce::Load()
{
	ServerAnnounceVec.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品或物体ID,动作类型,提示位置,提示文本 FROM __全服提示" :
		"SELECT entry,flag,announceType,text FROM _server_announce");
	if (!result)
		return;
	do
	{
		Field* fields = result->Fetch();
		ServerAnnounceTemplate Temp;
		Temp.entry			= fields[0].GetUInt32();
		Temp.flag			= fields[1].GetUInt8();
		Temp.announceType	= fields[2].GetUInt8();
		Temp.text			= fields[3].GetString();		
		ServerAnnounceVec.push_back(Temp);
	} while (result->NextRow());
}

const char* ServerAnnounce::Format(const char *format, ...)
{
	va_list ap;
	char str[2048];
	va_start(ap, format);
	vsnprintf(str, 2048, format, ap);
	va_end(ap);
	const char* c_str = str;
	return c_str;
}

std::string ServerAnnounce::GetNameLink(Player* player)
{
	return sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_SEVER_ANNOUNCE));
}

std::string ServerAnnounce::GetItemLink(uint32 entry)
{
	const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp) return "";

	std::string name = temp->Name1;

	std::ostringstream oss;
	oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec <<
		"|Hitem:" << entry << ":0:0:0:0:0:0:0:0:0|h[" << name << "]|h|r";

	return oss.str();

}

void ServerAnnounce::Announce(Player* announcer, uint32 entry, uint8 flag)
{
	uint32 len = ServerAnnounceVec.size();

	for (size_t i = 0; i < len; i++)
		if (entry == ServerAnnounceVec[i].entry && flag == ServerAnnounceVec[i].flag)
		{
			uint8 announceType = ServerAnnounceVec[i].announceType;
			std::string name; 

			//0 打开gameobject 1 打开物品 2获得物品 3生物刷新
			switch (flag)
			{
			case 0:
				name = sObjectMgr->GetGameObjectTemplate(entry)->name;
				break;
			case 1:
			case 2:
				name = GetItemLink(entry);
				break;
			default:
				break;
			}

			//0 左下角提示 1 左下和中间
			switch (announceType)
			{
			case 0:
				sWorld->SendServerMessage(SERVER_MSG_STRING, Format(ServerAnnounceVec[i].text.c_str(), GetNameLink(announcer).c_str(), name.c_str()));
				break;
			case 1:
				sWorld->SendScreenMessage(Format(ServerAnnounceVec[i].text.c_str(), GetNameLink(announcer).c_str(), name.c_str()));
				break;
			}
		}
}


void ServerAnnounce::CreatureRespawn(Creature* creature)
{
	if (!creature)
		return;

	uint32 len = ServerAnnounceVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (creature->GetEntry() == ServerAnnounceVec[i].entry && 3 == ServerAnnounceVec[i].flag)
		{
			std::ostringstream oss;

			if (AreaTableEntry const* area = GetAreaEntryByAreaID(creature->GetAreaId()))
				oss << "[" << area->area_name[4] << "]";

			oss << "[" << creature->GetCreatureTemplate()->Name << "]";
			oss << ServerAnnounceVec[i].text;

			switch (ServerAnnounceVec[i].announceType)
			{
			case 0:
				sWorld->SendGlobalText(oss.str().c_str(), NULL);
				break;
			case 1:
			{
				SessionMap const& smap = sWorld->GetAllSessions();
				for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
					if (Player* player = iter->second->GetPlayer())
						if (player->GetSession())
						{
							if (player->IsSpectator() || !player->IsInWorld())
								continue;
							player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
						}
			}
			break;
			case 3:
			{
				sWorld->SendGlobalText(oss.str().c_str(), NULL);
				SessionMap const& smap = sWorld->GetAllSessions();
				for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
					if (Player* player = iter->second->GetPlayer())
						if (player->GetSession())
						{
							if (player->IsSpectator() || !player->IsInWorld())
								continue;
							player->GetSession()->SendAreaTriggerMessage(oss.str().c_str());
						}
			}
			break;
			}
		}
	}	
}
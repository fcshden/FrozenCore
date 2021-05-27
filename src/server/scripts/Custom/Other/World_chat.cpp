#pragma execution_character_set("utf-8")
#include "../CommonFunc/CommonFunc.h"
#include "../DataLoader/DataLoader.h"
#include "../Requirement/Requirement.h"
#include "../Switch/Switch.h"
#include "../String/myString.h"
#include "../VIP/VIP.h"

class World_Chat : public CommandScript
{
public:
	World_Chat() : CommandScript("World_Chat") { }

	static bool HandleWorldChatCommand(ChatHandler * pChat, const char * msg)
	{
		if (!*msg)
			return true;

		Player * player = pChat->GetSession()->GetPlayer();

		if (!player)
			return true;

		int32 value = sSwitch->GetValue(ST_WORLD_CHAT);

		//-1 无限制 0 关闭 >0 消耗 <-1 冷却时间


		if (value == 0)
		{
			player->GetSession()->SendNotification("世界聊天功能未开启!");
			return true;
		}
		
		if (value > 0)
		{
			if (!sReq->Check(player, value))
				return true;

			sReq->Des(player, value);
		}
		
		if (value < -1)
		{
			if (player->canWorldChat)
			{
				player->worldChatTimer = value * IN_MILLISECONDS;
				player->canWorldChat = false;
			}else
			{
				uint32 cd = abs(player->worldChatTimer) / IN_MILLISECONDS;

				std::ostringstream oss;
				oss << "世界聊天将在" << cd << "s后冷却！";
				player->GetSession()->SendNotification(oss.str().c_str());
				return true;
			}
		}
	
		const char*  text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_WORLD_CHAT)), sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_WORLD_CHAT)).c_str(), msg);
		sWorld->SendServerMessage(SERVER_MSG_STRING, text);
		return true;
	}

	static bool HandleFactionChatCommand(ChatHandler * pChat, const char * msg)
	{
		if (!*msg)
			return true;

		Player * player = pChat->GetSession()->GetPlayer();

		if (!player)
			return true;

		int32 value = sSwitch->GetValue(ST_FACTION_CHAT);

		//-1 无限制 0 关闭 >0 消耗 <-1 冷却时间


		if (value == 0)
		{
			player->GetSession()->SendNotification("阵营聊天功能未开启!");
			return true;
		}

		if (value > 0)
		{
			if (!sReq->Check(player, value))
				return true;

			sReq->Des(player, value);
		}

		if (value < -1)
		{
			if (player->canFactionChat)
			{
				player->factionChatTimer = value * IN_MILLISECONDS;
				player->canFactionChat = false;
			}
			else
			{
				uint32 cd = abs(player->factionChatTimer) / IN_MILLISECONDS;

				std::ostringstream oss;
				oss << "阵营聊天将在" << cd << "s后冷却！";
				player->GetSession()->SendNotification(oss.str().c_str());
				return true;
			}
		}


		const char*  text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_FACTION_CHAT)), sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_FACTION_CHAT)).c_str(), msg);
		sWorld->SendFactionMessage(SERVER_MSG_STRING, text, player->GetTeamId());
		return true;
	}

	static bool HandleCustomFactionChatCommand(ChatHandler * pChat, const char * msg)
	{
		if (!*msg)
			return true;

		Player * player = pChat->GetSession()->GetPlayer();

		if (!player)
			return true;
		
		if (!player->faction)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("你还没有加入任何门派！");
			return true;
		}

		const char*  text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_CUSTOM_FACTION_CHAT)), sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_WORLD_CHAT)).c_str(), msg);

		WorldPacket data(SMSG_SERVER_MESSAGE, 50);
		data << uint32(SERVER_MSG_STRING);
		data << text;

		for (SessionMap::const_iterator itr = sWorld->GetAllSessions().begin(); itr != sWorld->GetAllSessions().end(); ++itr)
		{
			if (itr->second &&
				itr->second->GetPlayer() &&
				itr->second->GetPlayer()->IsInWorld() &&
				itr->second->GetPlayer()->faction == player->faction)
			{
				itr->second->SendPacket(&data);
			}
		}
		return true;
	}

    std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> WorldChatCommandTable =
		{
			{ "s", SEC_PLAYER, true, &HandleWorldChatCommand, "" },
			{ "z", SEC_PLAYER, true, &HandleFactionChatCommand, "" },
			{ "m", SEC_PLAYER, true, &HandleCustomFactionChatCommand, "" }
		};

		return WorldChatCommandTable;
	}
};

class LoginAnnounce : PlayerScript
{
public:
	LoginAnnounce() : PlayerScript("LoginAnnounce") {}
	void OnLogin(Player* player) override
	{
		if (!sSwitch->GetOnOff(ST_LOG_IN))
			return;
		std::string text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_LOGIN)), sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_LOG_IN)).c_str());
		sWorld->SendGlobalText(text.c_str(), NULL);
	}

    void OnFirstLogin(Player* player) override
    {
        std::string text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_LOGIN)), sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_LOG_IN)).c_str());
        player->GetTeamId() == TEAM_ALLIANCE ? text = text + "\n" + (std::string)sString->Format(sString->GetText(CORE_STR_TYPES(STR_LOGIN_A)), sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_LOG_IN)).c_str()) : text = text + "\n" + (std::string)sString->Format(sString->GetText(CORE_STR_TYPES(STR_LOGIN_H)), sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_LOG_IN)).c_str());
    }

	void OnLogout(Player* player) override
	{
		if (!sSwitch->GetOnOff(ST_LOG_OUT))
			return;

		std::string text = sString->Format(sString->GetText(CORE_STR_TYPES(STR_LOGOUT)), sCF->GetPlayerTotalName(player, sSwitch->GetFlag(ST_LOG_OUT)).c_str());

		sWorld->SendGlobalText(text.c_str(), NULL);
	}
};

void AddSC_cs_world_chat()
{
	new World_Chat();
	new LoginAnnounce();
}

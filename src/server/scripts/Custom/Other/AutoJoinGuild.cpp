#include "GuildMgr.h"
#include "../Switch/Switch.h"

class AutoJoinGuild : public PlayerScript
{
public:
	AutoJoinGuild() : PlayerScript("AutoJoinGuild") {}
	void OnLogin(Player* player)
	{
		if (!sSwitch->GetOnOff(ST_AUTO_JOIN_GUILD) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_FACTION) || player->HasAtLoginFlag(AT_LOGIN_CHANGE_RACE)) 
			return;

		if (player->realTeam == TEAM_ALLIANCE)
		{
			uint32 AllyGuild = atoi(sSwitch->GetFlagByIndex(ST_AUTO_JOIN_GUILD, 1).c_str());

			if (player->GetGuildId() != AllyGuild)
			{
				Guild* targetGuild = sGuildMgr->GetGuildById(AllyGuild);
				if (!targetGuild)
					return;
				targetGuild->AddMember(player->GetGUID());
			}
		}else
		{
			uint32 HoderGuild = atoi(sSwitch->GetFlagByIndex(ST_AUTO_JOIN_GUILD, 2).c_str());

			if (player->GetGuildId() != HoderGuild)
			{
				Guild* targetGuild = sGuildMgr->GetGuildById(HoderGuild);
				if (!targetGuild)
					return;
				targetGuild->AddMember(player->GetGUID());
			}
		}
	}
};


void AddSC_AutoJoinGuild()
{
	new AutoJoinGuild();
}

#include "CFBG.h"
#include "../CommonFunc/CommonFunc.h"
#include "../CustomEvent/FixedTimeBG/FixedTimeBG.h"

void CFBG::SetFaction(Player* player)
{
	switch (player->getRace())
	{
	case 1:
	case 3:
	case 4:
	case 7:
	case 11:
		player->setFactionForRace(RACE_ORC);
	break;
	case 2:
	case 5:
	case 6:
	case 8:
	case 10:
		player->setFactionForRace(RACE_HUMAN);
	break;
	}
}


bool CFBG::SendCFBGChat(Player* player, uint32 msgtype, std::string message)
{
	float distance = msgtype == CHAT_MSG_SAY ? sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_SAY) : sWorld->getFloatConfig(CONFIG_LISTEN_RANGE_YELL);
	if (Battleground* pBattleGround = player->GetBattleground())
	{
		if (pBattleGround->isArena())
			return false;
		
		if(!sFTB->GetCFFlag(pBattleGround->GetBgTypeID()))
			return false;

		for (Battleground::BattlegroundPlayerMap::const_iterator itr = pBattleGround->GetPlayers().begin(); itr != pBattleGround->GetPlayers().end(); ++itr)
		{
			if (Player* pPlayer = ObjectAccessor::FindPlayer(itr->first))
			{
				if (player->GetDistance2d(pPlayer->GetPositionX(), pPlayer->GetPositionY()) <= distance)
				{
					WorldPacket data;
					if (msgtype == CHAT_MSG_SAY || msgtype == CHAT_MSG_BATTLEGROUND || msgtype == CHAT_MSG_YELL)
					{
						if (pPlayer->GetTeamId() == player->GetTeamId())
							ChatHandler::BuildChatPacket(data, CHAT_MSG_SAY, Language(LANG_UNIVERSAL), player, pPlayer, message);
						else
							ChatHandler::BuildChatPacket(data, CHAT_MSG_SAY, Language(LANG_TITAN), player, pPlayer, message);
						pPlayer->GetSession()->SendPacket(&data);
					}
				}
			}
		}
		return true;
	}
	else
		return false;
}


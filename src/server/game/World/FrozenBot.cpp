#pragma execution_character_set("utf-8")
#include "FrozenBot.h"
#include "Player.h"
#include "Config.h"


FrozenBot::FrozenBot()
{
}

FrozenBot::~FrozenBot()
{
}


void FrozenBot::LogoutPlayerBot(uint64 guid, bool Save)
{
	if (Player *pPlayerBot = ObjectAccessor::FindPlayer(guid))
	{
		if (pPlayerBot && pPlayerBot->m_bot)
		{
			WorldSession* pPlayerBotWorldSession = pPlayerBot->GetSession();
			uint32 guids = pPlayerBot->GetGUIDLow();
			uint32 accids = pPlayerBot->GetSession()->GetAccountId();
			pPlayerBotWorldSession->LogoutPlayer(Save);
			delete pPlayerBotWorldSession;

			CharacterDatabase.PExecute("UPDATE characters SET online = 0 WHERE guid = %u", guids);
			LoginDatabase.PExecute("UPDATE account SET online = 0 WHERE id = %u", accids);
			m_playerBots.erase(guid);
			m_playerBottime.erase(guid);
		}
	}
}

void FrozenBot::PlBotupdate()
{
	//BOT上线
	if (_charBotLoginCallback.ready())
	{
		SQLQueryHolder* param;
		_charBotLoginCallback.get(param);
		HandlePlayerBotLogin((LoginQueryHolder*)param);
		_charBotLoginCallback.cancel();
	}

	std::set<uint64> guids;
	for (PlayerBotMap::const_iterator itr = m_playerBots.begin(); itr != m_playerBots.end(); ++itr)
	{
		m_playerBottime[itr->first] += 1;
		if (m_playerBottime[itr->first] >= sConfigMgr->GetIntDefault("Frozen.BotoffTime", 10))
			guids.insert(itr->first);
	}

	if (!guids.empty())
	{
		for (std::set<uint64>::const_iterator itr1 = guids.begin(); itr1 != guids.end(); ++itr1)
			LogoutPlayerBot(*itr1, true);
	}
}


//===============bot========================
void FrozenBot::AddPlayerBot(uint32 accid, uint32 playerGuid)
{
	if (QueryResult aaaaa = CharacterDatabase.PQuery("SELECT guid FROM character_banned WHERE guid = %u", playerGuid))
		return;

	uint64 guid = MAKE_NEW_GUID(playerGuid, 0, HIGHGUID_PLAYER);

	if (Player * botplr = ObjectAccessor::FindPlayerInOrOutOfWorld(guid))
		return;


	LoginQueryHolder *holder = new LoginQueryHolder(accid, guid);
	if (!holder->Initialize())
	{
		delete holder;
		return;
	}
	_charBotLoginCallback = CharacterDatabase.DelayQueryHolder((SQLQueryHolder*)holder);
}

void FrozenBot::HandlePlayerBotLogin(LoginQueryHolder* holder)
{
	if (!holder)
		return;

	LoginQueryHolder *lqh = (LoginQueryHolder *)holder;
	if (!lqh)
	{
		if (holder)
			delete holder;
		return;
	}

	WorldSession *botSession = new WorldSession(lqh->GetAccountId(), NULL, SEC_PLAYER, 2, 0, LocaleConstant(4), 0, 0, 0, 0);
	if (!botSession)
	{
		if (holder)
			delete holder;
		if (botSession)
			delete botSession;
		return;
	}

	botSession->SetAddress("playbot");
	botSession->Setexpansion(2);
	uint64 guid = lqh->GetGuid();
	if (!guid)
	{
		if (holder)
			delete holder;
		if (botSession)
			delete botSession;
		return;
	}

	botSession->HandlePlayerLoginFromDB(lqh);
	Player *botPlayer = botSession->GetPlayer();

	if (!botPlayer)
	{
		if (holder)
			delete holder;
		if (botSession)
			delete botSession;
		return;
	}

	m_playerBots[guid] = botPlayer;
}

/*
 * Copyright (C) 
 * Copyright (C) 
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/// \addtogroup world The World
/// @{
/// \file

#ifndef __Frozen_H
#define __Frozen_H

#include "Common.h"
#include "Timer.h"
#include <ace/Singleton.h>
#include <ace/Atomic_Op.h>
#include "SharedDefines.h"
#include "QueryResult.h"
#include "Callback.h"
#include "QueryResult.h"
#include <map>
#include <set>
#include <list>

class Object;
class WorldPacket;
class WorldSession;
class Player;
class WorldSocket;
class SystemMgr;
class LoginQueryHolder;

/// The Bot
class FrozenBot
{
	
    public:

		FrozenBot();
		~FrozenBot();

		typedef std::map<uint64, Player*> PlayerBotMap;
		PlayerBotMap m_playerBots;

		typedef std::map<uint64, uint32> PlayerBotTime;
		PlayerBotTime m_playerBottime;

		void PlBotupdate();
		void AddPlayerBot(uint32 accid, uint32 playerGuid);
		void LogoutPlayerBot(uint64 guid, bool Save);
		void HandlePlayerBotLogin(LoginQueryHolder* holder);
		QueryResultHolderFuture _charBotLoginCallback;

		uint32 GetMaxBotCount() const { return m_MaxbotCount; }

		/// Increase/Decrease number of players
		inline void IncreaseBotCount()
		{
			uint32 botcout = m_playerBots.size();
			m_MaxbotCount = std::max(m_MaxbotCount, botcout);
		}
		uint32 m_MaxbotCount;
};

#define sFbot ACE_Singleton<FrozenBot, ACE_Null_Mutex>::instance()
#endif
/// @}

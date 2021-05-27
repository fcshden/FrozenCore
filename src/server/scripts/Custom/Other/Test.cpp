#pragma execution_character_set("utf-8")
#include "../AuthCheck/AuthCheck.h"
#include "../Switch/Switch.h"
#include "../LeaderReward/LeaderReward.h"
#include <fstream>
#include <windows.h>
#include "../QuickResponse/QuickResponse.h"
#include "../Command/CustomCommand.h"
#include "../Armory/Armory.h"
#include "../SignIn/SignIn.h"
#include "../GCAddon/GCAddon.h"
#include "../CommonFunc/CommonFunc.h"
#include "../ItemSet/ItemSet.h"
#include "../CustomEvent/Event.h"
#include "../Rank/Rank.h"
#include "AccountMgr.h"
#include "../Faker/Faker.h"
#include "../Scripts/CustomScripts.h"
#include "../ItemMod/ItemMod.h"
#include "DBCStore.h"
#include "DBCStores.h"
#include "../PvP/PvP.h"
#include "../AntiFarm/AntiFarm.h"
#include "../Talisman/Talisman.h"
#include "../ItemMod/NoPatchItem.h"
#include "../Quest/QuestMod.h"
#include "MapManager.h"
#include "../EquipmentManager/EquipmentManager.h"
#include "../CustomEvent/BattleIC/BattleIC.h"
#include "../UnitMod/CharMod/CharMod.h"
#include "Group.h"
#include "../Recruit/Recruit.h"
#include "LootItemStorage.h"
#include "../AesEncryptor/aes_encryptor.h"
#include <tchar.h>
#include "../Requirement/Requirement.h"
#include "../LuckDraw/LuckDraw.h"
#include "../Reward/Reward.h"
#include "../../../../authserver/Server/TOTP.h"
#include "../MapMod/MapMod.h"

void EncodeString(LPCTSTR lpszText, LPTSTR *lpszReturn, LPCTSTR lpszKey)
{
	int nTextLen = 0;
	char *cPos = NULL;
	char *pDest = NULL;
	if (lpszReturn)    // 加密
	{
		nTextLen = ::_tcslen(lpszText);
		pDest = new char[nTextLen + 3];    // ==\0
	}
	else    // 解密
	{
		// 查找自定的中止标记
		cPos = (LPTSTR)lpszText;
		while (true)    // 从这里可以看到，除非搜索到我们自定的中止标记，否则会一直搜索下去
		{
			if (*cPos == '=')
				if (cPos[1] == '=')
					if (cPos[2] == '\0')
						break;
			cPos++;
		}
		if (!cPos)    // 没有找到结束标记，也不是加密
			return;
		nTextLen = cPos - lpszText;
		pDest = (LPTSTR)lpszText;
	}

	int nKeyLen = ::_tcslen(lpszKey);
	int i = 0;
	int k = 0;
	int t = nTextLen;
	int cn = 0;
	for (int a = 0; a < nKeyLen; a++)    // 将密钥所有值加起来
		cn += lpszKey[a];

	for (; i < nTextLen; i++)
	{
		if (lpszReturn)    // 加密
		{
			pDest[i] = lpszText[i] + t;
			pDest[i] = pDest[i] ^ lpszKey[k];
			pDest[i] = pDest[i] ^ cn;
		}
		else    // 解密，顺序与加密时相反
		{
			pDest[i] = lpszText[i] ^ cn;
			pDest[i] = pDest[i] ^ lpszKey[k];
			pDest[i] = pDest[i] - t;
		}
		k++;
		t--;
		if (k >= nKeyLen)
			k = 0;
		if (t <= 0)
			t = nTextLen;
	}

	if (!cPos)
	{
		memcpy(pDest + nTextLen, _T("==\0"), 3 * sizeof(TCHAR));
		*lpszReturn = pDest;
	}
	else
		memset(pDest + nTextLen, _T('\0'), sizeof(TCHAR));
}


uint32 cal(uint32 x)
{
	uint32 count = 0;

	for (size_t i = 1; i < x + 1; i++)
	{
		count += i;
	}

	return count;
}

uint32 HL(uint16 h, uint16 l){ return (uint32)((h << 16) | (l & 0xFFFF)); }
uint32 H(uint32 HL){ return (uint32)(HL & 0xFFFF); }
uint32 L(uint32 HL){ return (uint32)(HL >> 16); }




class TestCommand : public CommandScript
{
public:
	TestCommand() : CommandScript("TestCommand") { }

	static void Line()
	{
		printf("\n");
		printf("\n");
	}

    std::vector<ChatCommand> GetCommands() const
	{
		static std::vector<ChatCommand> TestCommandTable =
		{
			{ "t", SEC_CONSOLE, true, &HandleTestCommand, "" },
			{ "t1", SEC_CONSOLE, true, &HandleTest1Command, "" },
			{ "t2", SEC_CONSOLE, true, &HandleTest2Command, "" },
			{ "t3", SEC_CONSOLE, true, &HandleTest3Command, "" }
		};

		return TestCommandTable;
	}
	/* 1 + ... + x*/


	static bool HandleTestCommand(ChatHandler* handler, const char* args)
	{
		uint32 i = uint32(atol(args));
		Player* player = handler->GetSession()->GetPlayer();

		switch (i)
		{
		case 1:
		{

		}
		break;
		case 2:
		{

		}
		break;
		case 3:
		{

		}
		break;
		case 4:
		{

		}
		break;
		default:
			break;
		}

		return true;
	}

	static bool HandleTest1Command(ChatHandler* handler, const char* args)
	{
		uint32 i = uint32(atol(args));

		Player* player = handler->GetSession()->GetPlayer();

		for (auto itr = FakerSessions.begin(); itr != FakerSessions.end(); itr++)
		{
			WorldSession * s = *itr;
			if (s && s->GetPlayer() && s->GetPlayer()->IsInWorld())
			{
				WorldPacket* const packet = new WorldPacket(CMSG_BATTLEMASTER_JOIN, 8 + 4 + 4 + 1);
				*packet << uint64(0);
				*packet << uint32(BATTLEGROUND_WS);
				*packet << uint32(0);
				*packet << uint8(0);
				s->QueuePacket(packet);
			}
		}

		return true;
	}

	static bool HandleTest2Command(ChatHandler* handler, const char* args)
	{
		uint32 i = uint32(atol(args));

		Player* player = handler->GetSession()->GetPlayer();


		return true;
	}

	static bool HandleTest3Command(ChatHandler* handler, const char* args)
	{

		uint32 i = uint32(atol(args));

		Player* player = handler->GetSession()->GetPlayer();

		if (Player* target = player->GetSelectedPlayer())
		{
			if (!target->IsFaker)
				return true;

			for (auto itr = FakerSessions.begin(); itr != FakerSessions.end();)
				if ((*itr)->GetPlayer()->GetGUID() == target->GetGUID())
					itr = FakerSessions.erase(itr);
				else
					itr++;

			if (WorldSession * s = target->GetSession())
			{
				s->LogoutPlayer(true);
				delete s;
			}
		}

		

		return true;
	}
};

void AddSC_Test_Command()
{
	new TestCommand();
}

#pragma execution_character_set("utf-8")
#include "CustomCommand.h"
#include "Chat.h"
#include "Transport.h"
#include "../CommonFunc/CommonFunc.h"
#include "Chat.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Pet.h"
#include "Player.h"
#include "ReputationMgr.h"
#include "ScriptMgr.h"
#include "../Switch/Switch.h"
#include "../StatPoints/StatPoints.h"
#include "../FunctionCollection/FunctionCollection.h"
#include "../VIP/VIP.h"
#include "../String/myString.h"
#include "../CustomEvent/Event.h"
#include "../CharNameMod/CharNameMod.h"
#include "../Reward/Reward.h"
#include "../DBCCreate/DBCCreate.h"
#include "../Skill/Skill.h"
#include "../Rank/Rank.h"
#include "../SpiritPower/SpiritPower.h"
#include "../Morph/Morph.h"
#include "../ItemMod/ItemMod.h"
#include "../Faker/Faker.h"
#include "../Talisman/Talisman.h"
#include "AccountMgr.h"
#include "../GCAddon/GCAddon.h"
#include "../CDK/CDK.h"
#include "../UI/Rune/Rune.h"
#include "../UI/BlackMarket/BlackMarket.h"
#include "../AuthCheck/AuthCheck.h"
#include "group.h"
#include <fstream>
#include "MapManager.h"
#include "..\..\server\scripts\Custom\DataLoader\DataLoader.h"

std::vector<CommandTemplate> CommandVec;

void CustomCommand::Load()
{
	return;
	CommandVec.clear();
	QueryResult result = WorldDatabase.PQuery("SELECT ID,command FROM _command");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		CommandTemplate Temp;
		Temp.ID = fields[0].GetUInt32();
		Temp.command = fields[1].GetString();

		CommandVec.push_back(Temp);
	} while (result->NextRow());
}

void CustomCommand::DoCommandByID(Player* player, uint32 ID)
{
	std::string command = "";

	for (std::vector<CommandTemplate>::iterator iter = CommandVec.begin(); iter != CommandVec.end(); ++iter)
	{
		if (ID == iter->ID)
		{
			command = iter->command;
			break;
		}
	}

	DoCommand(player, command);
}

void CustomCommand::DoCommand(Player* player, std::string command)
{
	if (command.empty())
		return;

	player->CastStop();

	uint64 oriTarget = player->GetTarget();

	std::vector<std::string> commandsVec = sCF->SplitStr(command, "#");

	AccountTypes security = player->GetSession()->GetSecurity();

	player->GetSession()->SetSecurity(SEC_CONSOLE);

	for (std::vector<std::string>::iterator iter = commandsVec.begin(); iter != commandsVec.end(); ++iter)
	{
		std::string command = *iter;

		std::string::size_type p = command.find("&");

		if (p == std::string::npos)
			player->SetSelection(player->GetGUID());
		else
		{
			command = sCF->SplitStr(command, "&")[1];
			player->SetSelection(oriTarget);
		}
			
		if (!ChatHandler(player->GetSession()).ParseCommands(command.c_str(), false))
		{
			std::ostringstream oss;
			oss << "命令配置出错:" << (*iter);
			player->GetSession()->SendNotification(oss.str().c_str());
		}
	}

	player->SetSelection(oriTarget);

	player->GetSession()->SetSecurity(security);	
}

class custom_commandscript : public CommandScript
{
public:
	custom_commandscript() : CommandScript("custom_commandscript") { }

    std::vector<ChatCommand> GetCommands() const override
	{
        static std::vector<ChatCommand> cdkCommandTable =
		{
			{ "create", SEC_CONSOLE, true, &HandleCreateCDKCommand, "" },
			{ "out",	SEC_CONSOLE, true, &HandleOutCDKCommand, "" }
		};

        static std::vector<ChatCommand> addCommandTable =
		{
			{ "xp",				SEC_GAMEMASTER,			false,		&HandleAddXPCommand,				"" },
			{ "token",			SEC_GAMEMASTER,			false,		&HandleAddTokenCommand,				"" },
			{ "sps",			SEC_GAMEMASTER,			false,		&HandleAddStatPointCommand,			"" },
			{ "tps",			SEC_GAMEMASTER,			false,		&HandleAddTalentPointCommand,		"" },
			{ "viptps",			SEC_GAMEMASTER,			false,		&HandleAddVIPTalentPointCommand,	"" },
			{ "cps",			SEC_GAMEMASTER,			false,		&HandleAddCommercePointCommand,		"" },
			{ "pos",			SEC_GAMEMASTER,			false,		&HandleAddPosCommand,				"" },
			{ "inerpos",		SEC_GAMEMASTER,			false,		&HandleAddInerPosCommand,			"" },
			{ "skill",			SEC_GAMEMASTER,			false,		&HandleAddSkillCommand,				"" },
			{ "rankvalue",		SEC_GAMEMASTER,			false,		&HandleAddRankValueCommand,			"" },
			{ "spiritpower",	SEC_GAMEMASTER,			false,		&HandleAddSpiritPowerCommand,		"" },
			{ "maxspiritpower", SEC_GAMEMASTER,			false,		&HandleAddMaxSpiritPowerCommand,	"" },
			{ "gob",			SEC_GAMEMASTER,			false,		&HandleAddGameObjectCommand,		"" },
			{ "items",			SEC_GAMEMASTER,			false,		&HandleAddItemsCommand,				"" },
			{ "talismanvalue",	SEC_GAMEMASTER,			false,		&HandleAddTalismanValueCommand,		"" },
			{ "faker",			SEC_GAMEMASTER,			false,		&HandleAddFakerCommand,				"" },
			{ "rune",			SEC_GAMEMASTER,			false,		&HandleAddRuneCommand,				"" },
			{ "aura",			SEC_GAMEMASTER,			false,		&HandleAddAuraCommand,				"" }
		};

        static std::vector<ChatCommand> rewCommandTable =
		{
			{ "ip",			SEC_GAMEMASTER,			false,		&HandleRewIpCommand,				"" },
			{ "rewId",		SEC_GAMEMASTER,			false,		&HandleRewIdCommand,				"" }
		};

        static std::vector<ChatCommand> setCommandTable =
		{
			{ "viplevel",			SEC_GAMEMASTER,			false,		&HandleSetVIPCommand,				"" },
            { "acc",                SEC_GAMEMASTER,         false,      &HandleSetCCCommand,                "" },
			{ "faction",			SEC_GAMEMASTER,			false,		&HandleSetFactionCommand,			"" },
			{ "prefix",				SEC_GAMEMASTER,			false,		&HandleSetNamePrefixCommand,		"" },
			{ "suffix",				SEC_GAMEMASTER,			false,		&HandleSetNameSuffixCommand,		"" },
			{ "skill",				SEC_GAMEMASTER,			false,		&HandleSetSkillCommand,				"" },
			{ "duel",				SEC_GAMEMASTER,			false,		&HandleSetDuelCommand,				"" },
			{ "maxspiritpower",		SEC_GAMEMASTER,			false,		&HandleSetMaxSpiritPowerCommand,	"" },
			{ "morph",				SEC_GAMEMASTER,			false,		&HandleSetMorphCommand,				"" },
			{ "scale",				SEC_GAMEMASTER,			false,		&HandleSetScaleCommand,				"" },
			{ "display",			SEC_GAMEMASTER,			false,		&HandleSetDisplayCommand,			"" },
			{ "name",				SEC_GAMEMASTER,			false,		&HandleSetNameCommand,				"" },
			{ "subname",			SEC_GAMEMASTER,			false,		&HandleSetSubNameCommand,			"" },
			{ "npcflag",			SEC_GAMEMASTER,			false,		&HandleSetNPCFlagCommand,			"" }
		};

        static std::vector<ChatCommand> resetCommandTable =
		{
			{ "sps",		SEC_GAMEMASTER,			false,		&HandleResetStatPointsCommand,		"" },
			{ "daily",		SEC_GAMEMASTER,			false,		&HandleResetDailyQuestCommand,		"" },
			{ "weekly",		SEC_GAMEMASTER,			false,		&HandleResetWeeklyQuestCommand,		"" }
		};

        static std::vector<ChatCommand> dbcCommandTable =
		{
			{ "item",					SEC_CONSOLE,		true,	&HandleCreateItemDBCCommand,					"" },
			{ "charstartoutfit",		SEC_CONSOLE,		true,	&HandleCreateCharStartOutfitDBCCommand,			"" },
			{ "itemdisplayinfo",		SEC_CONSOLE,		true,	&HandleCreateItemDisplayInfoDBCCommand,			"" },
			{ "itemextendedcost",		SEC_CONSOLE,		true,	&HandleCreateItemExtendedCostDBCCommand,		"" },
			{ "spell",					SEC_CONSOLE,		true,	&HandleCreateSpellDBCCommand,					"" },
			{ "spellitemenchantment",	SEC_CONSOLE,		true,	&HandleCreateSpellItemEnchantmentDBCCommand,	"" },
			{ "talent",					SEC_CONSOLE,		true,	&HandleCreateTalentDBCCommand,					"" },
			{ "spellicon",				SEC_CONSOLE,		true,	&HandleCreateSpellIconDBCCommand,				"" }
		};

        static std::vector<ChatCommand> sqlCommandTable =
		{
			{ "charstartoutfit",		SEC_CONSOLE,		true,	&HandleCreateCharStartOutfitSQLCommand,			"" },
			{ "itemdisplayinfo",		SEC_CONSOLE,		true,	&HandleCreateItemDisplayInfoSQLCommand,			"" },
			{ "itemextendedcost",		SEC_CONSOLE,		true,	&HandleCreateItemExtendedCostSQLCommand,		"" },
			{ "spell",					SEC_CONSOLE,		true,	&HandleCreateSpellSQLCommand,					"" },
			{ "spellitemenchantment",	SEC_CONSOLE,		true,	&HandleCreateSpellItemEnchantmentSQLCommand,	"" },
			{ "talent",					SEC_CONSOLE,		true,	&HandleCreateTalentSQLCommand,					"" },
			{ "spellicon",				SEC_CONSOLE,		true,	&HandleCreateSpellIconSQLCommand,				"" }
		};

        static std::vector<ChatCommand> commandTable =
		{
			{ "_vendor",	SEC_GAMEMASTER,			false,		&HandleVendorCommand,				"" },
			{ "_add",		SEC_GAMEMASTER,			false,		NULL,								"", addCommandTable },
			{ "_reward",	SEC_GAMEMASTER,			false,		NULL,								"", rewCommandTable },
			{ "_set",		SEC_GAMEMASTER,			false,		NULL,								"", setCommandTable },
			{ "_reset",		SEC_GAMEMASTER,			false,		NULL,								"", resetCommandTable },
			{ "_tele",		SEC_GAMEMASTER,			false,		&HandleTeleCommand,					"" },
			{ "jk",			SEC_PLAYER,				false,		&HandleJKCommand,					"" },
			{ "_summon",	SEC_GAMEMASTER,			false,		&HandleSummonCommand,				"" },
			{ "_db_clear",	SEC_CONSOLE,			true,		&HandleDBClearCommand,				"" },
			{ "pl",			SEC_CONSOLE,			true,		&HandlePlayerDataCommand,			"" },
			{ "_loot",		SEC_CONSOLE,			true,		&HandleAOELootCommand,				"" },
			{ "dbc",		SEC_GAMEMASTER,			true,		NULL,								"", dbcCommandTable },
			{ "sql",		SEC_GAMEMASTER,			true,		NULL,								"", sqlCommandTable },
			{ "_morpet",	SEC_GAMEMASTER,			false,		&HandlePetMorphCommand,				"" },
			{ "cdk",		SEC_CONSOLE,			true,		NULL,								"", cdkCommandTable },
			{ "_blackmarket",	SEC_GAMEMASTER,		false,		&HandleBlackMarketCommand,			"" },
			{ "outpos",		SEC_GAMEMASTER,			false,		&HandleOutPosCommand,				"" },
			{ "emote",		SEC_GAMEMASTER,			false,		&HandleEmoteCommand,				"" },
			{ "clearbag",	SEC_GAMEMASTER,			false,		&HandleClearBagCommand,				"" },
			{ "_group",		SEC_GAMEMASTER,			false,		&HandleGroupCommand,				"" },
			{ "_heal",		SEC_GAMEMASTER,			false,		&HandleHealCommand,					"" },
			{ "_level",		SEC_GAMEMASTER,			false,		&HandleLevelCommand,				"" },
			{ "_enchantlua", SEC_CONSOLE,			true,		&HandleEnchantLuaCommand,			"" },
			{ "_go",		SEC_CONSOLE,			true,		&HandleGoCommand,					"" }
		};
		return commandTable;
	}

	static bool HandleGoCommand(ChatHandler* handler, char const* args)
	{
		if (!*args)
			return false;

		Player* player = handler->GetSession()->GetPlayer();

		char* goX = strtok((char*)args, " ");
		char* goY = strtok(NULL, " ");
		char* goZ = strtok(NULL, " ");
		char* id = strtok(NULL, " ");
		char* s_diff = strtok(NULL, " ");
		char* s_challengelv = strtok(NULL, " ");

		if (!goX || !goY)
			return false;

		float x = (float)atof(goX);
		float y = (float)atof(goY);
		float z;
		float ort = player->GetOrientation();
		uint32 mapId = id ? (uint32)atoi(id) : player->GetMapId();

		if (goZ)
		{
			z = (float)atof(goZ);
			if (!MapManager::IsValidMapCoord(mapId, x, y, z))
			{
				handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
				handler->SetSentErrorMessage(true);
				return false;
			}
		}
		else
		{
			if (!MapManager::IsValidMapCoord(mapId, x, y))
			{
				handler->PSendSysMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
				handler->SetSentErrorMessage(true);
				return false;
			}
			Map const* map = sMapMgr->CreateBaseMap(mapId);
			z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));
		}

		// stop flight if need
		if (player->IsInFlight())
		{
			player->GetMotionMaster()->MovementExpired();
			player->CleanupAfterTaxiFlight();
		}
		// save only in non-flight case
		else
			player->SaveRecallPosition();

		player->SetDungeonDifficulty(Difficulty(atoi(s_diff)));
		player->ChallengeLv = atoi(s_challengelv);

		player->TeleportTo(mapId, x, y, z, ort);
		return true;
	}


	static bool HandleEnchantLuaCommand(ChatHandler* handler, char const* args)
	{
		ofstream outfile("GhostEnchantData.lua");

		outfile << "GhostEnchantData = {" << std::endl;

		for (uint32 id = 0; id < sSpellItemEnchantmentStore.GetNumRows(); id++)
		{

			SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(id);

			if (!info)
				continue;

			outfile << "[\"" << info->ID << "\"] = \"" << info->description[4] << "\"," << std::endl;

		}

		outfile << "}" << std::endl;
		outfile.close();

		if (handler->GetSession())
			ChatHandler(handler->GetSession()).PSendSysMessage("附魔信息导出完成，保存于核心根目录GhostEnchantData.Lua，将其复制到Interface/Addons/GhostPanel文件夹下");
		else
			sLog->outString("附魔信息导出完成，保存于核心根目录GhostEnchantData.Lua，将其复制到Interface/Addons/GhostPanel文件夹下");

		return true;
	}

	static bool HandleGroupCommand(ChatHandler* handler, char const* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player)
			return false;

		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("命令格式._group summon/revive");
			return false;
		}

		Group* group = player->GetGroup();

		if (!group)
			return true;

		std::string param = (char*)args;

		if (param == "summon")
		{
			for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
				if (Player* member = itr->GetSource())
					if (member->IsInWorld() && member->GetGUID() != player->GetGUID())
						member->TeleportTo(player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());
			return true;
		}

		if (param == "revive")
		{
			for (GroupReference* itr = group->GetFirstMember(); itr != NULL; itr = itr->next())
				if (Player* member = itr->GetSource())
					if (member->IsInWorld() && !member->IsAlive())
					{
						member->ResurrectPlayer(1.0f);
						member->SpawnCorpseBones();
					}

			return true;
		}

		ChatHandler(handler->GetSession()).PSendSysMessage("命令格式._group summon/revive");
		return false;
	}

	static bool HandleHealCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player)
			return false;

		if (!*args)
		{	
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._heal 生物编号 血量");
			return false;
		}

		char* s_entry = strtok((char*)args, " ");
		char* s_health = strtok(NULL, " ");

		if (!s_entry || !s_health)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._heal 生物编号 血量");
			return false;
		}

		uint32 entry = atoi(s_entry);
		uint32 health = atoi(s_health);

		if (Creature* c = player->FindNearestCreature(entry, 1000.0f))
			c->ModifyHealth(health);

		return true;
	}

	static bool HandleLevelCommand(ChatHandler* handler, const char* args)
	{
		Player* player = handler->GetSession()->GetPlayer();

		if (!player)
			return false;

		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._level 等级");
			return false;
		}

		uint32 level = atoi(args);
		if (player->getLevel() >= level)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("你当前等级%u大于或等于要提升的等级%u", player->getLevel(), level);
			return true;
		}

		std::string cmd = ".char level " + std::string(args);
		sCustomCommand->DoCommand(player, cmd);

		return true;
	}

	static bool HandleBlackMarketCommand(ChatHandler* handler, char const* args)
	{
		if (!*args)
			return false;


		std::string param = (char*)args;

		if (param == "on")
		{
			sBlackMarket->Start();
			return true;
		}

		if (param == "off")
		{
			sBlackMarket->Stop();
			return true;
		}

		handler->SendSysMessage(LANG_USE_BOL);
		handler->SetSentErrorMessage(true);
		return false;
	}

	static bool HandleSetScaleCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		float Scale = (float)atof((char*)args);
		if (Scale > 10.0f || Scale < 0.1f)
		{
			handler->SendSysMessage(LANG_BAD_VALUE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		Unit* target = handler->getSelectedUnit();
		if (!target || target->GetTypeId() != TYPEID_UNIT)
		{
			handler->SendSysMessage(LANG_SELECT_CREATURE);
			handler->SetSentErrorMessage(true);
			return false;
		}


		target->SetObjectScale(Scale);
		WorldDatabase.PExecute("Update creature_template Set scale = %f WHERE entry = %u", Scale, target->GetEntry());
		ChatHandler(handler->GetSession()).PSendSysMessage("[%s][Scale]设置为%f", target->GetName().c_str(), Scale);
		return true;
	}

	static bool HandleOutPosCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		OutPos(atoi(args));
		return true;
	}

	static bool HandleEmoteCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;
		Unit* target = handler->getSelectedUnit();
		
		if (!target)
			target = handler->GetSession()->GetPlayer();

		target->HandleEmoteCommand(atoi(args));

		return true;
	}

	static bool HandleClearBagCommand(ChatHandler* handler, const char* args)
	{
		if (Player* player = handler->GetSession()->GetPlayer())
			for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
				if (Bag* pBag = player->GetBagByPos(i))
					for (uint32 j = 0; j < pBag->GetBagSize(); j++)
						if (Item* item = player->GetItemByPos(i, j))
							player->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
		
		return true;
	}

	static bool HandleSetDisplayCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		uint32 displayId = atoi(args);

		Unit* target = handler->getSelectedUnit();
		if (!target || target->GetTypeId() != TYPEID_UNIT)
		{
			handler->SendSysMessage(LANG_SELECT_CREATURE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		target->SetDisplayId(displayId);
		WorldDatabase.PExecute("Update creature_template Set modelid1 = %u,modelid2 = 0,modelid3 = 0,modelid4 = 0 WHERE entry = %u", displayId, target->GetEntry());
		ChatHandler(handler->GetSession()).PSendSysMessage("[%s][DisplayId]设置为%d", target->GetName().c_str(), displayId);
		return true;
	}

	static bool HandleSetNPCFlagCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		uint32 npcflag = atoi(args);

		Unit* target = handler->getSelectedUnit();
		if (!target || target->GetTypeId() != TYPEID_UNIT)
		{
			handler->SendSysMessage(LANG_SELECT_CREATURE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		target->SetUInt32Value(UNIT_NPC_FLAGS, npcflag);
		WorldDatabase.PExecute("Update creature_template Set npcflag = %u WHERE entry = %u", npcflag, target->GetEntry());
		ChatHandler(handler->GetSession()).PSendSysMessage("[%s][NpcFlag]设置为%d", target->GetName().c_str(), npcflag);
		return true;
	}

	static bool HandleSetSubNameCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		std::string subName = args;

		Unit* target = handler->getSelectedUnit();
		if (!target || target->GetTypeId() != TYPEID_UNIT)
		{
			handler->SendSysMessage(LANG_SELECT_CREATURE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(target->GetEntry()))
		{
			WorldPacket queryData;
			queryData.Initialize(SMSG_CREATURE_QUERY_RESPONSE, 1);
		
			queryData << uint32(target->GetEntry());                 // creature entry
			queryData << ci->Name;
			queryData << uint8(0) << uint8(0) << uint8(0);           // name2, name3, name4, always empty
			queryData << subName;
			queryData << ci->IconName;                               // "Directions" for guard, string for Icons 2.3.0
			queryData << uint32(ci->type_flags);                     // flags
			queryData << uint32(ci->type);                           // CreatureType.dbc
			queryData << uint32(ci->family);                         // CreatureFamily.dbc
			queryData << uint32(ci->rank);                           // Creature Rank (elite, boss, etc)
			queryData << uint32(ci->KillCredit[0]);                  // new in 3.1, kill credit
			queryData << uint32(ci->KillCredit[1]);                  // new in 3.1, kill credit
			queryData << uint32(ci->Modelid1);                       // Modelid1
			queryData << uint32(ci->Modelid2);                       // Modelid2
			queryData << uint32(ci->Modelid3);                       // Modelid3
			queryData << uint32(ci->Modelid4);                       // Modelid4
			queryData << float(ci->ModHealth);                       // dmg/hp modifier
			queryData << float(ci->ModMana);                         // dmg/mana modifier
			queryData << uint8(ci->RacialLeader);
			for (uint32 i = 0; i < MAX_CREATURE_QUEST_ITEMS; ++i)
				queryData << uint32(0);              // itemId[6], quest drop
			queryData << uint32(ci->movementId);                     // CreatureMovementInfo.dbc
		
			handler->GetSession()->SendPacket(&queryData);
		}
		
		WorldDatabase.PExecute("Update creature_template Set subname = '%s' WHERE entry = %u", subName.c_str(), target->GetEntry());
		ChatHandler(handler->GetSession()).PSendSysMessage("[%s][SubName]设置为%s", target->GetName().c_str(), subName.c_str());

		return true;
	}

	static bool HandleSetNameCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		std::string name = args;

		Unit* target = handler->getSelectedUnit();
		if (!target || target->GetTypeId() != TYPEID_UNIT)
		{
			handler->SendSysMessage(LANG_SELECT_CREATURE);
			handler->SetSentErrorMessage(true);
			return false;
		}

		ChatHandler(handler->GetSession()).PSendSysMessage("[%s][Name]设置为%s", target->GetName().c_str(), name.c_str());
		target->SetName(name);

		if (CreatureTemplate const* ci = sObjectMgr->GetCreatureTemplate(target->GetEntry()))
		{
			WorldPacket queryData;
			queryData.Initialize(SMSG_CREATURE_QUERY_RESPONSE, 1);
		
			queryData << uint32(target->GetEntry());                  // creature entry
			queryData << name;
			queryData << uint8(0) << uint8(0) << uint8(0);           // name2, name3, name4, always empty
			queryData << ci->SubName;
			queryData << ci->IconName;                               // "Directions" for guard, string for Icons 2.3.0
			queryData << uint32(ci->type_flags);                     // flags
			queryData << uint32(ci->type);                           // CreatureType.dbc
			queryData << uint32(ci->family);                         // CreatureFamily.dbc
			queryData << uint32(ci->rank);                           // Creature Rank (elite, boss, etc)
			queryData << uint32(ci->KillCredit[0]);                  // new in 3.1, kill credit
			queryData << uint32(ci->KillCredit[1]);                  // new in 3.1, kill credit
			queryData << uint32(ci->Modelid1);                       // Modelid1
			queryData << uint32(ci->Modelid2);                       // Modelid2
			queryData << uint32(ci->Modelid3);                       // Modelid3
			queryData << uint32(ci->Modelid4);                       // Modelid4
			queryData << float(ci->ModHealth);                       // dmg/hp modifier
			queryData << float(ci->ModMana);                         // dmg/mana modifier
			queryData << uint8(ci->RacialLeader);
			for (uint32 i = 0; i < MAX_CREATURE_QUEST_ITEMS; ++i)
				queryData << uint32(0);              // itemId[6], quest drop
			queryData << uint32(ci->movementId);                     // CreatureMovementInfo.dbc
		
			handler->GetSession()->SendPacket(&queryData);
		}

		WorldDatabase.PExecute("Update creature_template Set name = '%s' WHERE entry = %u", name.c_str(), target->GetEntry());
		
		return true;
	}

	static bool HandleDBClearCommand(ChatHandler* handler, const char* args)
	{
		QueryResult result = LoginDatabase.PQuery("SELECT MAX(id) FROM account");

		if (!result)
			return true;

		uint32 max = result->Fetch()[0].GetUInt32();

		for (size_t i = 0; i < max + 1; i++)
			AccountMgr::DeleteAccount(i);

		LoginDatabase.DirectExecute("TRUNCATE account_rank");
		sLog->outString(">> Delete account_rank..");

		LoginDatabase.DirectExecute("TRUNCATE account_spells");
		sLog->outString(">> Delete account_spells..");

		CharacterDatabase.DirectExecute("TRUNCATE account_instance_times");
		sLog->outString(">> Delete account_instance_times..");

		CharacterDatabase.DirectExecute("TRUNCATE challenge");
		sLog->outString(">> Delete challenge..");

		CharacterDatabase.DirectExecute("TRUNCATE character_talisman");
		sLog->outString(">> Delete character_talisman..");

		CharacterDatabase.DirectExecute("TRUNCATE characters_extra_equipments");
		sLog->outString(">> Delete characters_extra_equipments..");

		CharacterDatabase.DirectExecute("TRUNCATE chatacters_anticheat");
		sLog->outString(">> Delete chatacters_anticheat..");

		CharacterDatabase.DirectExecute("TRUNCATE chatacters_token_monitor");
		sLog->outString(">> Delete chatacters_token_monitor..");

		CharacterDatabase.DirectExecute("TRUNCATE character_recruit");
		sLog->outString(">> Delete character_recruit..");

		CharacterDatabase.DirectExecute("TRUNCATE character_lottery");
		sLog->outString(">> Delete character_lottery..");

		CharacterDatabase.DirectExecute("TRUNCATE character_day_item");
		sLog->outString(">> Delete character_day_item..");

		CharacterDatabase.DirectExecute("TRUNCATE character_achievement_progress");
		sLog->outString(">> Delete character_custom_skill..");

		CharacterDatabase.DirectExecute("TRUNCATE characters_talent_req");
		sLog->outString(">> Delete characters_talent_req..");

		CharacterDatabase.DirectExecute("TRUNCATE character_achievement");
		sLog->outString(">> Delete character_achievement..");

		CharacterDatabase.DirectExecute("TRUNCATE guild");
		sLog->outString(">> Delete guild..");

		CharacterDatabase.DirectExecute("TRUNCATE guild_bank_eventlog");
		sLog->outString(">> Delete guild_bank_eventlog..");

		CharacterDatabase.DirectExecute("TRUNCATE guild_bank_item");
		sLog->outString(">> Delete guild_bank_item..");

		CharacterDatabase.DirectExecute("TRUNCATE guild_bank_right");
		sLog->outString(">> Delete guild_bank_right..");

		CharacterDatabase.DirectExecute("TRUNCATE guild_bank_tab");
		sLog->outString(">> Delete guild_bank_tab..");

		CharacterDatabase.DirectExecute("TRUNCATE guild_eventlog");
		sLog->outString(">> Delete guild_eventlog..");

		CharacterDatabase.DirectExecute("TRUNCATE guild_member");
		sLog->outString(">> Delete guild_member..");

		CharacterDatabase.DirectExecute("TRUNCATE guild_member_withdraw");
		sLog->outString(">> Delete guild_member_withdraw..");

		CharacterDatabase.DirectExecute("TRUNCATE guild_rank");
		sLog->outString(">> Delete guild_rank..");

		CharacterDatabase.DirectExecute("TRUNCATE character_rank");
		sLog->outString(">> Delete character_rank..");

		sLog->outString(">> 账号库、角色库清理完毕！");
		return true;
	}

	static bool HandlePlayerDataCommand(ChatHandler* handler, const char* args)
	{
		uint32 playerCount = 0;
		uint32 fakerCount = 0;

		std::vector<std::string> IPVec;

		SessionMap::const_iterator itr;
		for (itr = sWorld->GetAllSessions().begin(); itr != sWorld->GetAllSessions().end(); ++itr)
		{
			if (itr->second &&
				itr->second->GetPlayer() &&
				itr->second->GetPlayer()->IsInWorld())
			{
				std::string ip = itr->second->GetRemoteAddress();

				if (std::find(IPVec.begin(), IPVec.end(), ip) == IPVec.end())
					IPVec.push_back(ip);

				playerCount++;
			}
				
		}

		fakerCount = sWorld->GetPlayerCount() - playerCount;
		if (handler->GetSession())
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("服务器运行时间: %s", secsToTimeString(sWorld->GetUptime()).append("."));
			ChatHandler(handler->GetSession()).PSendSysMessage("玩家数量[%u]，IP数量[%u]，假人数量[%u]", playerCount, IPVec.size(), fakerCount);
		}	
		else
		{
			sLog->outString("服务器运行时间: %s", secsToTimeString(sWorld->GetUptime()).append("."));
			sLog->outString("玩家数量[%u]，IP数量[%u]，假人数量[%u]", playerCount, IPVec.size(), fakerCount);
		}
			
		return true;
	}


	static bool HandleCreateCDKCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			if (handler->GetSession())
				ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:.cdk create [count] [rewId] [comment]");
			else
				sLog->outString("语法格式:.cdk create [count] [rewId] [comment]");

			return false;
		}

		char* s_count = strtok((char*)args, " ");
		char* s_rewId = strtok(NULL, " ");
		char* s_comment = strtok(NULL, " ");

		if (!s_count || !s_rewId || !s_comment)
		{
			if (handler->GetSession())
				ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:.cdk create [count] [rewId] [comment]");
			else
				sLog->outString("语法格式:.cdk create [count] [rewId] [comment]");

			return false;
		}

		uint32 count = atoi(s_count);
		uint32 rewId = atoi(s_rewId);
		std::string comment = s_comment;
		sCDK->Create(count, rewId, comment);

		std::ostringstream oss;
		sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
			oss << "[兑换码工具][__兑换码]表新增" << count << "个兑换码，备注名[" << comment << "]，奖励模板ID[" << rewId << "]" :
			oss << "[兑换码工具][_cdk]表新增" << count << "个兑换码，备注名[" << comment << "]，奖励模板ID[" << rewId << "]";

		if (handler->GetSession())
			ChatHandler(handler->GetSession()).PSendSysMessage(oss.str().c_str());
		else
			sLog->outString(oss.str().c_str());

		return true;
	}

	static bool HandleOutCDKCommand(ChatHandler* handler, const char* args)
	{
		sCDK->OutPut();
		
		std::ostringstream oss;
		oss << "[兑换码工具]兑换码导出完成，保存于核心根目录兑换码.txt";

		if (handler->GetSession())
			ChatHandler(handler->GetSession()).PSendSysMessage(oss.str().c_str());
		else
			sLog->outString(oss.str().c_str());

		return true;
	}

	static bool HandleAddAuraCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._add aura mapid/-zoneid/all(正数 - mapid;负数 - zoneid;all - 全服) spell_id(正数-本方阵营;负值-敌方阵营)");
			return false;
		}

		Player* player = handler->GetSession()->GetPlayer();

		if (!player)
			return false;

		char* s_zone = strtok((char*)args, " ");
		char* s_spell = strtok(NULL, " ");

		if (!s_zone || !s_spell)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._add aura mapid/-zoneid/all(正数 - mapid;负数 - zoneid;all - 全服) spell_id(正数-本方阵营;负值-敌方阵营)");
			return false;
		}

		int32 spellid = atoi(s_spell);
		int32 zoneid = atoi(s_zone);

		const SpellInfo * spellInfo = sSpellMgr->GetSpellInfo(abs(spellid));

		if (!spellInfo)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("光环技能 -%u 不存在", abs(spellid));
			return false;
		}
			
		SessionMap const& smap = sWorld->GetAllSessions();
		for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
			if (Player* pl = iter->second->GetPlayer())
			{
				if (pl->GetSession())
				{
					if (pl->IsSpectator())
						continue;

					if (pl->isDead())
						continue;

					if (spellid > 0 && pl->GetTeamId() != player->GetTeamId())
						continue;

					if (spellid < 0 && pl->GetTeamId() == player->GetTeamId())
						continue;

					if (strcmp("all", s_zone) == 0)
					{
						pl->AddAura(abs(spellid), pl);
						continue;
					}

					if (zoneid >= 0 && pl->GetMapId() != zoneid)
						continue;

					if (zoneid < 0 && pl->GetZoneId() != abs(zoneid))
						continue;

					pl->AddAura(abs(spellid), pl);
				}
			}

		return true;

		return true;
	}

	static bool HandleAddPosCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._add pos 备注，如._add pos 主城");
			return false;
		}

		Player* player = handler->GetSession()->GetPlayer();

		uint32 posId = 1;

		if (QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
			"SELECT max(坐标ID) FROM _模板_坐标" :
			"SELECT max(posId) FROM _position"))
			posId = result->Fetch()[0].GetUInt32() + 1;

		WorldDatabase.DirectPExecute(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
			"INSERT INTO _模板_坐标(备注,坐标ID,地图ID,X坐标,Y坐标,Z坐标,O坐标) VALUES ('%s','%u','%u','%f','%f','%f','%f')" :
			"INSERT INTO _position(comment,posId,map,x,y,z,o) VALUES ('%s','%u','%u','%f','%f','%f','%f')", args, posId, player->GetMapId(), player->GetPositionX(), player->GetPositionY(), player->GetPositionZ(), player->GetOrientation());

		std::ostringstream oss;
		sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
			oss << "[_模板_坐标]新增ID:" << posId << "备注:" << args :
			oss << "[_position]新增ID:" << posId << "备注:" << args;

		ChatHandler(player->GetSession()).PSendSysMessage(oss.str().c_str());

		sCustomCommand->DoCommand(player, ".rl all");

		return true;
	}

	static bool HandleAddInerPosCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._add inerpos 备注，如._add inerpos posId1 posId2 count");
			return false;
		}

		Player* player = handler->GetSession()->GetPlayer();

		uint32 posId1 = atol(strtok((char*)args, " "));
		uint32 posId2 = atoi(strtok(NULL, " "));
		uint32 count = atoi(strtok(NULL, " "));

		//sEvent->PrintPos(player, posId1, posId2, count, true);
		sCustomCommand->DoCommand(player, ".rl all");
		return true;
	}

	static bool HandleAddSkillCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		uint32 skillId = atol(strtok((char*)args, " "));
		int unlearn = atoi(strtok(NULL, " "));

		if (unlearn == -1)
			sCustomSkill->LearnSkill(target, skillId, false);
		else
			sCustomSkill->LearnSkill(target, skillId);
			
		return true;
	}

	static bool HandleAddFakerCommand(ChatHandler* handler, const char* args)
	{
		sFaker->Add(true);
		return true;
	}

	static bool HandleAddRuneCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._add rune [spellid]");
			return false;
		}

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		if (!args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._add rune [spellid]");
			return false;
		}

		uint32 spellid = atoi(args);
		sRune->Add(target, spellid);
		return true;
	}

	static bool HandleAddItemsCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();

		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		uint32 categoryId = atoi(args);

		sItemMod->AddCategoryItem(handler->GetSession()->GetPlayer(), target, categoryId);

		return true;
	}

	static bool HandleAddTalismanValueCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();

		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		uint32 value = atoi(args);

		sTalisman->AddTalismanValue(target, value, false);
		sTalisman->SaveTalisManValue(target);

		return true;
	}

	static bool HandleCreateItemDBCCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateItemDBC();
		return true;
	}
	
	static bool HandleCreateItemDisplayInfoDBCCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateItemDisplayInfoDBC();
		return true;
	}

	static bool HandleCreateItemDisplayInfoSQLCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateItemDisplayInfoSql();
		return true;
	}

	static bool HandleCreateItemExtendedCostDBCCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateItemExtendCostDBC();
		return true;
	}

	static bool HandleCreateItemExtendedCostSQLCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateItemExtendCostSql();
		return true;
	}

	static bool HandleCreateSpellDBCCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateSpellDBC();
		return true;
	}

	static bool HandleCreateSpellSQLCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateSpellSql();
		return true;
	}

	static bool HandleCreateSpellItemEnchantmentDBCCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateSpellItemEnchantmentDBC();
		return true;
	}

	static bool HandleCreateSpellItemEnchantmentSQLCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateSpellItemEnchantmentSql();
		return true;
	}

	static bool HandleCreateCharStartOutfitDBCCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateCharStartOutfitDBC();
		return true;
	}

	static bool HandleCreateCharStartOutfitSQLCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateCharStartOutfitSql();
		return true;
	}

	static bool HandleCreateTalentDBCCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateTalentDBC();
		return true;
	}

	static bool HandleCreateTalentSQLCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateTalentSql();
		return true;
	}

	static bool HandleCreateSpellIconDBCCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateSpellIconDBC();
		return true;
	}

	static bool HandleCreateSpellIconSQLCommand(ChatHandler* handler, const char* args)
	{
		sDBCCreate->GenerateSpellIconSql();
		return true;
	}

	static bool HandleAddXPCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 xp = atoi(args);

		if (xp < 0)
			return false;

		uint32 curXP = target->GetUInt32Value(PLAYER_XP);
		uint32 nextLvlXP = target->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
		uint32 newXP = curXP + xp;
		uint32 level = target->getLevel();

		bool flag = false;

		while (newXP >= nextLvlXP && level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
		{
			newXP -= nextLvlXP;

			if (level < sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
			{
				if (level < sSwitch->GetValue(ST_XP_MAX_LV))
					target->GiveLevel(level + 1);
				else
					flag = true;
			}

			level = target->getLevel();
			nextLvlXP = target->GetUInt32Value(PLAYER_NEXT_LEVEL_XP);
		}

		target->SetUInt32Value(PLAYER_XP, newXP);

		if (flag)
			target->SetUInt32Value(PLAYER_XP, target->GetUInt32Value(PLAYER_NEXT_LEVEL_XP));

		std::ostringstream oss;
		oss << sCF->GetNameLink(target) << "获得" << "[经验] X " << xp;
		ChatHandler(target->GetSession()).PSendSysMessage(oss.str().c_str());
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage(oss.str().c_str());

		return true;
	}

	static bool HandleAddRankValueCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		uint32 value = (uint32)atoi(args);
		if (value == 0)
			return false;

		ChatHandler(target->GetSession()).PSendSysMessage("你获得了%d点服务器经验值", value);

		sRank->Update(target, value, true);
		return true;
	}

	static bool HandleAddSpiritPowerCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 value = atoi(args);

		if (value == 0)
			return false;
		if (value > 0)
			sSpiritPower->Update(target, value, true);
		else
			sSpiritPower->Update(target, abs(value), false);

		return true;
	}

	static bool HandleAddMaxSpiritPowerCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 value = atoi(args);

		if (value == 0)
			return false;

		int32 max = target->MaxSpiritPower + value;

		if (max <= 0)
			max = 0;

		sSpiritPower->UpdateMax(target, max);

		return true;
	}

	static bool HandleAddTokenCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 token = atoi(args);
		std::ostringstream oss;

		if (token > 0)
		{
			sCF->UpdateTokenAmount(target, token, true, "[命令]获得");
			oss << sCF->GetNameLink(target) << "获得[" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "] X " << token;
		}
		else
		{
			sCF->UpdateTokenAmount(target, abs(token), false, "[命令]消耗");
			oss << sCF->GetNameLink(target) << "扣除[" << sString->GetText(CORE_STR_TYPES(STR_TOKEN)) << "] X " << abs(token);
		}

		ChatHandler(target->GetSession()).PSendSysMessage(oss.str().c_str());
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage(oss.str().c_str());

		return true;
	}

	static bool HandleSetNamePrefixCommand(ChatHandler* handler, const char* args)
	{

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		std::string namePrefix;

		if (*args)
			namePrefix = args;
		else
			namePrefix = "";

		sCharNameMod->UpdatePrefix(target, namePrefix);

		std::ostringstream oss;
		oss << sCF->GetNameLink(target) << "获得[姓名前缀]" << namePrefix;
		ChatHandler(target->GetSession()).PSendSysMessage(oss.str().c_str());
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage(oss.str().c_str());

		return true;
	}

	static bool HandleSetNameSuffixCommand(ChatHandler* handler, const char* args)
	{
		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		std::string nameSuffix;

		if (*args)
			nameSuffix = args;
		else
			nameSuffix = "";

		sCharNameMod->UpdateSuffix(target, nameSuffix);

		std::ostringstream oss;
		oss << sCF->GetNameLink(target) << "获得[姓名后缀]" << nameSuffix;
		ChatHandler(target->GetSession()).PSendSysMessage(oss.str().c_str());
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage(oss.str().c_str());

		return true;
	}

	static bool HandleAddStatPointCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 statpoint = atoi(args);

		if (statpoint < 0)
			return false;

		uint32 total = 0;

		for (size_t i = 0; i < MAX_STAT_POINTS_TYPE; i++)
			total += target->stat_points[i];

		total += statpoint;

		if (total >= sSwitch->GetValue(ST_SPS_LIMIT))
		{
			ChatHandler(target->GetSession()).PSendSysMessage("斗气点数已达到最大值%d", sSwitch->GetValue(ST_SPS_LIMIT));
			statpoint = sSwitch->GetValue(ST_SPS_LIMIT) - (total - statpoint);
		}

		if (statpoint <= 0)
			return false;

		target->stat_points[SPT_TOTLAL] += statpoint;

		sStatPoints->UpdateDB(target);

		std::ostringstream oss;
		oss << sCF->GetNameLink(target) << "获得[斗气点] X " << statpoint;
		ChatHandler(target->GetSession()).PSendSysMessage(oss.str().c_str());
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage(oss.str().c_str());

		return true;
	}

	static bool HandleResetStatPointsCommand(ChatHandler* handler, const char* args)
	{
		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		sStatPoints->ResetPoints(target);

		ChatHandler(target->GetSession()).PSendSysMessage("%s[斗气点]已经重置", sCF->GetNameLink(target).c_str());
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage("%s[斗气点]已经重置", sCF->GetNameLink(target).c_str());

		return true;
	}

	static bool HandleResetDailyQuestCommand(ChatHandler* handler, const char* args)
	{
		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		target->ResetDailyQuestStatus();
		return true;
	}

	static bool HandleResetWeeklyQuestCommand(ChatHandler* handler, const char* args)
	{
		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		target->ResetWeeklyQuestStatus();
		return true;
	}

	static bool HandleRewIdCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._rew rewId 数值");
			return false;
		}

		if (Player* target = handler->getSelectedPlayerOrSelf())
			sRew->Rew(target, atoi(args));

		return true;
	}

	static bool HandleRewIpCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._rew ip rewId 数值 或 ._rew ip item 数值");
			return false;
		}

		char* type = strtok((char*)args, " ");
		char* value = strtok(NULL, " ");
		if (!type || !value)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._rew ip rewId 数值 或 ._rew ip item 数值");
			return false;
		}

		int i = 0;

		if (strcmp(type, "rewId") == 0)		i = 1;
		else if (strcmp(type, "item") == 0)	i = 2;

		if (i == 0)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._rew ip rewId 数值 或 ._rew ip item 数值");
			return false;
		}

		uint32 id = atoi(value);

		std::vector<std::string> IpVec;
		IpVec.clear();

		SessionMap const& smap = sWorld->GetAllSessions();
		for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
			if (Player* pl = iter->second->GetPlayer())
			{
				std::string ip = pl->GetSession()->GetRemoteAddress();

				if (std::find(IpVec.begin(), IpVec.end(), ip) == IpVec.end())
				{
					if(i == 1) sRew->Rew(pl, id);
					else if (i == 2) pl->AddItem(id, 1);
					IpVec.push_back(ip);
				}
			}

		if (handler->GetSession())
			ChatHandler(handler->GetSession()).PSendSysMessage("IP奖励已发放");

		return true;
	}

	static bool HandleVendorCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;
		handler->GetSession()->SendCommandListInventory(atoi(args));
		return true;
	}

	
	static bool HandleTeleCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		uint32 posId = atoi(args);

		Player* target = handler->GetSession()->GetPlayer();

		std::unordered_map<uint32, PosTemplate>::iterator it = PosMap.find(posId);

		if (it != PosMap.end())
			target->TeleportTo(it->second.map, it->second.x, it->second.y, it->second.z, it->second.o);
		else
			ChatHandler(handler->GetSession()).PSendSysMessage("传送点不存在");

		return true;
	}

	static bool HandleJKCommand(ChatHandler* handler, const char* args)
	{
		Player* target = handler->GetSession()->GetPlayer();

		if (!target)
			return true;

		if (target->IsAlive())
		{
			target->GetSession()->SendNotification("死亡状态才可以使用该命令！");
			return true;
		}
	
		target->TeleportTo(target->m_homebindMapId, target->m_homebindX, target->m_homebindY, target->m_homebindZ, target->GetOrientation());
		return true;
	}

	static void AoeLoot(Player* player, float range)
	{
		if (!player->IsAlive())
		{
			ChatHandler(player->GetSession()).PSendSysMessage("你已经死亡");
			return;
		}

		if (player->IsNonMeleeSpellCast(false))
			player->InterruptNonMeleeSpells(false);

		std::list<Creature*> list;
		player->GetCreatureListInGrid(list, range);
		if (!list.empty())
		{
			for (std::list<Creature*>::iterator itr = list.begin(); itr != list.end(); ++itr)
			{
				Creature* c = *itr;

				if (c->IsAlive())
					continue;

				uint64 guid = c->GetGUID();
				Loot* loot = &c->loot;
				float range = 100.0f;

				WorldPacket p1(CMSG_LOOT);
				p1 << guid;
				player->GetSession()->HandleLootFarOpcode(p1, range);

				for (size_t lootSlot = 0; lootSlot < loot->GetMaxSlotInLootFor(player); lootSlot++)
				{
					WorldPacket p(CMSG_AUTOSTORE_LOOT_ITEM);
					p << lootSlot;
					player->GetSession()->HandleAutostoreLootItemFarOpcode(p, range);
				}

				WorldPacket p2(CMSG_LOOT_MONEY);
				player->GetSession()->HandleLootMoneyFarOpcode(p2, range);
			}
		}
	}

	static bool HandleAOELootCommand(ChatHandler* handler, const char* args)
	{
		if (Player* player = handler->GetSession()->GetPlayer())
			AoeLoot(player, 1000.0f);

		return true;
	}

	static bool HandleSummonCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._summon id(正数 - Creature Entry;负数 - Gameobject Entry) time(Creature或Gameobject持续的时间，单位 秒");
			return false;
		}

		char* s_entry = strtok((char*)args, " ");
		char* s_time	= strtok(NULL, " ");

		if (!s_entry || !s_time)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._summon id(正数 - Creature Entry;负数 - Gameobject Entry) time(Creature或Gameobject持续的时间，单位 秒");
			return false;
		}

		int32 entry = atoi(s_entry);
		uint32 time = atoi(s_time);

		Player* target = handler->GetSession()->GetPlayer();

		if (entry > 0)
			target->SummonCreature(entry, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, time * IN_MILLISECONDS);
		else if (entry < 0)
			target->SummonGameObject(abs(entry), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), target->GetOrientation(), 0, 0, 0, 0, time);
		
		return true;
	}

	static bool HandlePetMorphCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->GetSession()->GetPlayer();

		if (!target)
			return false;

		uint32 displayID = atoi(args);

		CreatureModelInfo const* minfo = sObjectMgr->GetCreatureModelInfo(displayID);

		if (!minfo)
		{
			ChatHandler(target->GetSession()).PSendSysMessage("%u - 不是一个有效的模型ID", displayID);
			return true;
		}

		if (Pet * pet = target->GetPet())
		{
			pet->SetDisplayId(displayID);
			pet->SetNativeDisplayId(displayID);
			CharacterDatabase.DirectPExecute("UPDATE character_pet SET modelid = '%u' WHERE owner = '%u' AND entry = '%u'", displayID, target->GetGUIDLow(), pet->GetEntry());
			ChatHandler(target->GetSession()).PSendSysMessage("改变宠物模型为 - %u", displayID);
			return true;
		}else
			ChatHandler(target->GetSession()).PSendSysMessage("你没有召唤一个宠物", displayID);

		return true;
	}

	static bool HandleSetFactionCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Unit* target = handler->getSelectedUnit();
		if (!target)
		{
			handler->SetSentErrorMessage(true);
			return false;
		}

		uint32 faction = atoi(args);

		target->setFaction(faction);

		if (target->GetTypeId() == TYPEID_UNIT)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("[%s][Faction]设置为%d", target->GetName().c_str(), faction);
			WorldDatabase.PExecute("Update creature_template Set faction = %u WHERE entry = %u", faction, target->GetEntry());
		}
		else if (target->GetTypeId() == TYPEID_PLAYER)
		{
			ChatHandler(target->ToPlayer()->GetSession()).PSendSysMessage("[%s][阵营]设置为%d", target->GetName().c_str(), faction);
			if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
				ChatHandler(handler->GetSession()).PSendSysMessage("[%s][阵营]设置为%d", target->GetName().c_str(), faction);
		}

		return true;
	}

	static bool HandleSetDuelCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 duel = atoi(args);

		target->SetInt32Value(PLAYER_DUEL_TEAM, duel);

		ChatHandler(target->GetSession()).PSendSysMessage("[Duel组别]设置为%d", duel);
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage("[Duel组别]设置为%d", duel);

		return true;
	}

	static bool HandleSetMorphCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 morphId = atoi(args);

		target->RemoveAura(MORPH_SPELLID);
		//sMorph->SetMorphId(target, morphId);
		//sMorph->SetMorphDuration(target, sMorph->GetDuration(morphId));
		target->CastSpell(target, MORPH_SPELLID);

		return true;
	}

	static bool HandleSetMaxSpiritPowerCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 maxspiritpower = atoi(args);

		sSpiritPower->UpdateMax(target, maxspiritpower);
		return true;
	}

	static bool HandleAddTalentPointCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 tp = atoi(args);

		if (tp < 0)
			return false;

		uint32 extraTps = sCF->GetExtraTalentPoints(target);
		uint32 maxExTps = sSwitch->GetValue(ST_EXTRA_TPS);

		if (extraTps + tp >= maxExTps)
		{
			target->SetFreeTalentPoints(target->GetFreeTalentPoints() + uint32(maxExTps - extraTps));
			target->SendTalentsInfoData(false);

			ChatHandler(target->GetSession()).PSendSysMessage("%s获得额外%u个天赋点", sCF->GetNameLink(target).c_str(), uint32(maxExTps - extraTps));
			if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
				ChatHandler(handler->GetSession()).PSendSysMessage("%s获得额外%u个天赋点", sCF->GetNameLink(target).c_str(), uint32(maxExTps - extraTps));

			if (sSwitch->GetOnOff(ST_TP_ACCOUNT_BIND))
				LoginDatabase.DirectPExecute("UPDATE account SET extraTalentPoints = '%u' WHERE id = '%u'", maxExTps, target->GetSession()->GetAccountId());
			else
				CharacterDatabase.DirectPExecute("UPDATE characters SET extraTalentPoints = '%u' WHERE guid = '%u'", maxExTps, target->GetGUIDLow());
			return true;
		}

		ChatHandler(target->GetSession()).PSendSysMessage("%s获得额外%u个天赋点", sCF->GetNameLink(target).c_str(), tp);
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage("%s获得额外%u个天赋点", sCF->GetNameLink(target).c_str(), tp);

		target->SetFreeTalentPoints(target->GetFreeTalentPoints() + tp);
		target->SendTalentsInfoData(false);

		if (sSwitch->GetOnOff(ST_TP_ACCOUNT_BIND))
			LoginDatabase.DirectPExecute("UPDATE account SET extraTalentPoints = extraTalentPoints + '%u' WHERE id = '%u'", tp, target->GetSession()->GetAccountId());
		else
			CharacterDatabase.DirectPExecute("UPDATE characters SET extraTalentPoints = extraTalentPoints + '%u' WHERE guid = '%u'", tp, target->GetGUIDLow());
		return true;
	}

	static bool HandleAddVIPTalentPointCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 tp = atoi(args);

		if (tp < 0)
			return false;

		uint32 extraTps = sCF->GetExtraTalentPoints(target);
		uint32 maxExTps = 0;

		switch (target->vipLevel)
		{
		case 1: maxExTps = 3; break;
		case 2: maxExTps = 8; break;
		case 3: maxExTps = 15; break;
		case 4: maxExTps = 24; break;
		case 5: maxExTps = 36; break;
		case 6: maxExTps = 50; break;
		default:break;	
		}

		if (extraTps + tp >= maxExTps)
		{
			target->SetFreeTalentPoints(target->GetFreeTalentPoints() + uint32(maxExTps - extraTps));
			target->SendTalentsInfoData(false);

			ChatHandler(target->GetSession()).PSendSysMessage("%s获得额外%u个天赋点", sCF->GetNameLink(target).c_str(), uint32(maxExTps - extraTps));
			if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
				ChatHandler(handler->GetSession()).PSendSysMessage("%s获得额外%u个天赋点", sCF->GetNameLink(target).c_str(), uint32(maxExTps - extraTps));

			if (sSwitch->GetOnOff(ST_TP_ACCOUNT_BIND))
				LoginDatabase.DirectPExecute("UPDATE account SET extraTalentPoints = '%u' WHERE id = '%u'", maxExTps, target->GetSession()->GetAccountId());
			else
				CharacterDatabase.DirectPExecute("UPDATE characters SET extraTalentPoints = '%u' WHERE guid = '%u'", maxExTps, target->GetGUIDLow());
			return true;
		}

		ChatHandler(target->GetSession()).PSendSysMessage("%s获得额外%u个天赋点", sCF->GetNameLink(target).c_str(), tp);
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage("%s获得额外%u个天赋点", sCF->GetNameLink(target).c_str(), tp);

		target->SetFreeTalentPoints(target->GetFreeTalentPoints() + tp);
		target->SendTalentsInfoData(false);

		if (sSwitch->GetOnOff(ST_TP_ACCOUNT_BIND))
			LoginDatabase.DirectPExecute("UPDATE account SET extraTalentPoints = extraTalentPoints + '%u' WHERE id = '%u'", tp, target->GetSession()->GetAccountId());
		else
			CharacterDatabase.DirectPExecute("UPDATE characters SET extraTalentPoints = extraTalentPoints + '%u' WHERE guid = '%u'", tp, target->GetGUIDLow());
		return true;
	}


	static bool HandleAddCommercePointCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 cp = atoi(args);

		if (cp < 0)
			return false;

		uint32 extraCps = 0;

		QueryResult result = CharacterDatabase.PQuery("SELECT extraPrimaryTradeSkills FROM characters WHERE guid = '%u'", target->GetGUIDLow());
		if (result)
		{
			Field* fields = result->Fetch();
			extraCps = fields[0].GetInt32();
		}


		if (extraCps + cp > uint32(sSwitch->GetValue(ST_EXTRA_CPS_LIMIT)))
		{
			CharacterDatabase.DirectPExecute("UPDATE characters SET extraPrimaryTradeSkills ='%u' WHERE guid = '%u'", sSwitch->GetValue(ST_EXTRA_CPS_LIMIT), target->GetGUIDLow());
			target->maxPrimaryTradeSkills = target->maxPrimaryTradeSkills + uint32(sSwitch->GetValue(ST_EXTRA_CPS_LIMIT) - extraCps);
			if (target->maxPrimaryTradeSkills >= sCF->GetCommercePoints(target))
				target->SetFreePrimaryProfessions(target->maxPrimaryTradeSkills - sCF->GetCommercePoints(target));

			ChatHandler(target->GetSession()).PSendSysMessage("%s获得额外%u个商业技能点", sCF->GetNameLink(target).c_str(), uint32(sSwitch->GetValue(ST_EXTRA_CPS_LIMIT) - extraCps));
			if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
				ChatHandler(handler->GetSession()).PSendSysMessage("%s获得额外%u个商业技能点", sCF->GetNameLink(target).c_str(), uint32(sSwitch->GetValue(ST_EXTRA_CPS_LIMIT) - extraCps));

			return true;
		}

		target->maxPrimaryTradeSkills += cp;
		if (target->maxPrimaryTradeSkills >= sCF->GetCommercePoints(target))
			target->SetFreePrimaryProfessions(target->maxPrimaryTradeSkills - sCF->GetCommercePoints(target));
		CharacterDatabase.PExecute("update characters set extraPrimaryTradeSkills = extraPrimaryTradeSkills + '%u' where guid = %u", cp, target->GetGUIDLow());

		ChatHandler(target->GetSession()).PSendSysMessage("%s获得额外%u个商业技能点", sCF->GetNameLink(target).c_str(), cp);
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage("%s获得额外%u个商业技能点", sCF->GetNameLink(target).c_str(), cp);

		return true;
	}

    static bool HandleSetCCCommand(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        Player* target = handler->getSelectedPlayer();
        if (!target)
        {
            handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
            handler->SetSentErrorMessage(true);
            return false;
        }

        int32 vip = atoi(args);
        if (vip < 1)
        {
            ChatHandler(target->GetSession()).PSendSysMessage("爆率不能低于1倍");
            return false;
        }
        sDataLoader->AddLootPl(target, vip);
        sCF->SetLootRate(target);

        ChatHandler(target->GetSession()).PSendSysMessage("%s获得%u倍爆率，小退后在上生效", sCF->GetNameLink(target).c_str(), vip);
        return true;
    }

	static bool HandleSetVIPCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
			return false;

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		int32 vip = atoi(args);

		if (vip < 1)
			return false;

        if (target->vipLevel >= vip)
        {
            ChatHandler(handler->GetSession()).PSendSysMessage("%sVIP等级已经大于或者等于这个等级了无法在使用.", sCF->GetNameLink(target).c_str());
            return false;
        }

		if (vip <= target->vipLevel)
			vip = target->vipLevel;

		LoginDatabase.DirectPExecute("UPDATE account SET viplevel = '%u' WHERE id = '%u'", vip, target->GetSession()->GetAccountId());
		target->vipLevel = vip;

		ChatHandler(target->GetSession()).PSendSysMessage("%s获得VIP等级%u", sCF->GetNameLink(target).c_str(), vip);
		if (handler->GetSession()->GetPlayer()->GetGUID() != target->GetGUID())
			ChatHandler(handler->GetSession()).PSendSysMessage("%s获得VIP等级%u", sCF->GetNameLink(target).c_str(), vip);


		std::string title = "";
		std::string icon = "";

		sVIP->GetVIPTitle(target, title, icon);
		target->CastSpell(target, 61456, true, NULL, NULL, target->GetGUID());

        
		const char*  msg = sString->Format(sString->GetText(CORE_STR_TYPES(STR_VIP_UP)), sCF->GetNameLink(target).c_str(), title.c_str());
		sWorld->SendScreenMessage(msg);

		return true;
	}

	static bool HandleSetSkillCommand(ChatHandler* handler, const char* args)
	{
		if (!*args)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._set skill 专业中文拼音缩写 数值，如._set skill cy 450");
			return false;
		}	

		Player* target = handler->getSelectedPlayer();
		if (!target)
		{
			handler->SendSysMessage(LANG_PLAYER_NOT_FOUND);
			handler->SetSentErrorMessage(true);
			return false;
		}

		char* skillName = strtok((char*)args, " ");
		char* value = strtok(NULL, " ");
		if (!skillName || !value)
		{
			ChatHandler(handler->GetSession()).PSendSysMessage("语法格式:._set skill 专业中文拼音缩写 数值，如._set skill cy 450");
			return false;
		}
			
		uint32 skillid = 0;

		if (strcmp(skillName, "cy") == 0)		skillid = 182;
		else if (strcmp(skillName, "ck") == 0)	skillid = 186;
		else if (strcmp(skillName, "bp") == 0)	skillid = 393;
		else if (strcmp(skillName, "dz") == 0)	skillid = 164;
		else if (strcmp(skillName, "cf") == 0)	skillid = 197;
		else if (strcmp(skillName, "zp") == 0)	skillid = 165;
		else if (strcmp(skillName, "gc") == 0)	skillid = 202;
		else if (strcmp(skillName, "lj") == 0)	skillid = 171;
		else if (strcmp(skillName, "mw") == 0)	skillid = 773;
		else if (strcmp(skillName, "fm") == 0)	skillid = 333;
		else if (strcmp(skillName, "zb") == 0)	skillid = 129;
		else if (strcmp(skillName, "jj") == 0)	skillid = 333;
		else if (strcmp(skillName, "pr") == 0)	skillid = 185;
		else if (strcmp(skillName, "dy") == 0)	skillid = 356;

		int32 point = atoi(value);

		if (skillid == 0 || point <= 0)
			return false;

		if (target->HasSkill(skillid) && target->GetSkillValue(skillid) < point)
			target->SetSkill(skillid, 1, point, point);

		return true;
	}

	//._add id r count z_offset scale ori temp

	static bool HandleAddGameObjectCommand(ChatHandler* handler, const char* args)
	{
		uint32 objectId = atol(strtok((char*)args, " "));
		uint32 count = atol(strtok(NULL, " "));
		float r = (float)atof(strtok(NULL, " "));
		float z_offset = (float)atof(strtok(NULL, " "));
		float o = (float)atof(strtok(NULL, " "));
		uint32 temp = atol(strtok(NULL, " "));

		Player* player = handler->GetSession()->GetPlayer();

		for (size_t i = 0; i < count; i++)
		{
			float x = player->GetPositionX() + (i * r)  * cos(player->GetOrientation());
			float y = player->GetPositionY() + (i * r)  * sin(player->GetOrientation());
			float z = player->GetPositionZ() + z_offset;

			if (temp == 0)
			{
				player->SummonGameObject(objectId, x, y, z, o, 0, 0, 0, 0, 30);
				handler->PSendSysMessage("创建临时GAMEOBJECT");
				continue;
			}
				
			Map* map = player->GetMap();
			GameObject* object = sObjectMgr->IsGameObjectStaticTransport(objectId) ? new StaticTransport() : new GameObject();
			uint32 guidLow = sObjectMgr->GenerateLowGuid(HIGHGUID_GAMEOBJECT);

			if (!object->Create(guidLow, objectId, map, player->GetPhaseMaskForSpawn(), x, y, z, o, G3D::Quat(), 0, GO_STATE_READY))
			{
				delete object;
				return false;
			}

			object->SaveToDB(map->GetId(), (1 << map->GetSpawnMode()), player->GetPhaseMaskForSpawn());
			delete object;

			object = sObjectMgr->IsGameObjectStaticTransport(objectId) ? new StaticTransport() : new GameObject();
			if (!object->LoadGameObjectFromDB(guidLow, map))
			{
				delete object;
				return false;
			}
			sObjectMgr->AddGameobjectToGrid(guidLow, sObjectMgr->GetGOData(guidLow));
			handler->PSendSysMessage("添加GAMEOBJECT至数据库");
		}

		return true;
	}
};

void AddSC_custom_commandscript()
{
	new custom_commandscript();
}

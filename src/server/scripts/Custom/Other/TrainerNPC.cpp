#pragma execution_character_set("utf-8")
#include "../PrecompiledHeaders/ScriptPCH.h"

const uint32 PALADIN = 5492;
const uint32 WARRIOR = 914;
const uint32 PRIEST = 5484;
const uint32 MAGE = 331;
const uint32 DRUID = 5504;
const uint32 DEATH_KNIGHT = 29196;
const uint32 HUNTER = 5516;
const uint32 ROGUE = 918;
const uint32 SHAMAN = 20407;
const uint32 WARLOCK = 5496;

class TRAINER_NPC : public CreatureScript
{
public:
	TRAINER_NPC() : CreatureScript("TRAINER_NPC") { }
	bool OnGossipHello(Player* player, Creature* creature) override
	{
		player->PlayerTalkClass->ClearMenus();
		switch (player->getClass())
		{
		case CLASS_WARRIOR:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Warrior_SavageBlow:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_PALADIN:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Holy_AuraOfLight:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_HUNTER:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Hunter_ChimeraShot2:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_ROGUE:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Rogue_DeadlyBrew:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_PRIEST:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Holy_Heal:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_DEATH_KNIGHT:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Shadow_DeadofNight:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_SHAMAN:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Shaman_Stormstrike:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_MAGE:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Frost_IceStorm:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_WARLOCK:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Warlock_DemonicPower:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		case CLASS_DRUID:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Druid_Bash:30:30:0:0|t职业", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
			break;
		default:
			break;
		}
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_DualWield:30:30:0:0|t武器", GOSSIP_SENDER_MAIN + 4, GOSSIP_ACTION_INFO_DEF);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Coin_06:30:30:0:0|t商业", GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
		player->GetTeamId() == TEAM_ALLIANCE ? player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Mount_Gryphon_01:30:30:0:0|t骑术", GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF) : player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Ability_Mount_Wyvern_01:30:30:0:0|t骑术", GOSSIP_SENDER_MAIN + 3, GOSSIP_ACTION_INFO_DEF);
		if (!player->HasSpell(63624))
			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Shadow_Charm:30:30:0:0|t天赋", GOSSIP_SENDER_MAIN + 5, GOSSIP_ACTION_INFO_DEF, "确定要学习双天赋吗？", 1000 * GOLD, false);
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Achievement_BG_winWSG:30:30:0:0|t重置天赋", GOSSIP_SENDER_MAIN + 6, GOSSIP_ACTION_INFO_DEF, "确定要重置天赋吗？", 50 * GOLD, false);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		player->PlayerTalkClass->ClearMenus();

		if (sender == GOSSIP_SENDER_MAIN + 100 && action == GOSSIP_ACTION_INFO_DEF + 100)
		{
			OnGossipHello(player, creature);
			return true;
		}

		switch (sender)
		{
		case GOSSIP_SENDER_MAIN + 1:
		{
			switch (player->getClass())
			{
			case CLASS_WARRIOR:
				player->GetSession()->SendTrainerList(creature->GetGUID(), WARRIOR);
				player->NpcTrainerId = WARRIOR;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_PALADIN:
				player->GetSession()->SendTrainerList(creature->GetGUID(), PALADIN);
				player->NpcTrainerId = PALADIN;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_HUNTER:
				player->GetSession()->SendTrainerList(creature->GetGUID(), HUNTER);
				player->NpcTrainerId = HUNTER;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_ROGUE:
				player->GetSession()->SendTrainerList(creature->GetGUID(), ROGUE);
				player->NpcTrainerId = ROGUE;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_PRIEST:
				player->GetSession()->SendTrainerList(creature->GetGUID(), PRIEST);
				player->NpcTrainerId = PRIEST;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_DEATH_KNIGHT:
				player->GetSession()->SendTrainerList(creature->GetGUID(), DEATH_KNIGHT);
				player->NpcTrainerId = DEATH_KNIGHT;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_SHAMAN:
				player->GetSession()->SendTrainerList(creature->GetGUID(), SHAMAN);
				player->NpcTrainerId = SHAMAN;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_MAGE:
				player->GetSession()->SendTrainerList(creature->GetGUID(), MAGE);
				player->NpcTrainerId = MAGE;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_WARLOCK:
				player->GetSession()->SendTrainerList(creature->GetGUID(), WARLOCK);
				player->NpcTrainerId = WARLOCK;
				player->CLOSE_GOSSIP_MENU();
				break;
			case CLASS_DRUID:
				player->GetSession()->SendTrainerList(creature->GetGUID(), DRUID);
				player->NpcTrainerId = DRUID;
				player->CLOSE_GOSSIP_MENU();
				break;
			default:
				break;
			}
		}
		break;
		case GOSSIP_SENDER_MAIN + 2:
		{
			//player->GetSession()->SendTrainerList(creature->GetGUID(), 28698);
			//player->NpcTrainerId = 28698;
			//break;
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_Alchemy:30:30:0:0|t炼金", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_BlackSmithing:30:30:0:0|t锻造", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 2);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_Engraving:30:30:0:0|t附魔", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 3);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_Engineering:30:30:0:0|t工程", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 4);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_Herbalism:30:30:0:0|t草药", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 5);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Inscription_Tradeskill01:30:30:0:0|t铭文", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 6);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Jewelcrafting_DragonsEye02:30:30:0:0|t珠宝", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 7);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_LeatherWorking:30:30:0:0|t制皮", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 8);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_Mining:30:30:0:0|t采矿", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 9);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_LeatherScrap_10:30:30:0:0|t剥皮", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 10);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_Tailoring:30:30:0:0|t裁缝", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 11);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/INV_Misc_Food_64:30:30:0:0|t烹饪", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 12);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Spell_Holy_SealOfSacrifice:30:30:0:0|t急救", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 13);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/Trade_Fishing:30:30:0:0|t钓鱼", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 14);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "|TInterface/ICONS/misc_arrowleft:30:30:0:0|t返回", GOSSIP_SENDER_MAIN + 100, GOSSIP_ACTION_INFO_DEF + 100);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
		}
		break;
		case GOSSIP_SENDER_MAIN + 3:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 31238);
			player->NpcTrainerId = 31238;
			break;
		case GOSSIP_SENDER_MAIN + 4:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 11867);
			player->NpcTrainerId = 11867;
			break;
		case GOSSIP_SENDER_MAIN + 5:
			player->ModifyMoney(-1000 * GOLD);
			player->CastSpell(player, 63680, true, NULL, NULL, player->GetGUID());
			player->CastSpell(player, 63624, true, NULL, NULL, player->GetGUID());
			player->learnSpell(63624);
			player->CastSpell(player, 63707);
			ChatHandler(player->GetSession()).PSendSysMessage("你学会了双天赋");
			player->CLOSE_GOSSIP_MENU();
			break;
		case GOSSIP_SENDER_MAIN + 6:
			player->resetTalents(true);
			player->SendTalentsInfoData(false);
			player->GetSession()->SendAreaTriggerMessage("已重置角色天赋");
			player->ModifyMoney(-50 * GOLD);
			player->CLOSE_GOSSIP_MENU();
			break;
		default:
			break;
		}
		switch (action)
		{
		case GOSSIP_ACTION_INFO_DEF + 1:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 33630);
			player->NpcTrainerId = 33630;
			break;
		case GOSSIP_ACTION_INFO_DEF + 2:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28694);
			player->NpcTrainerId = 28694;
			break;
		case GOSSIP_ACTION_INFO_DEF + 3:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 33633);
			player->NpcTrainerId = 33633;
			break;
		case GOSSIP_ACTION_INFO_DEF + 4:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28697);
			player->NpcTrainerId = 28697;
			break;
		case GOSSIP_ACTION_INFO_DEF + 5:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28704);
			player->NpcTrainerId = 28704;
			break;
		case GOSSIP_ACTION_INFO_DEF + 6:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28702);
			player->NpcTrainerId = 28702;
			break;
		case GOSSIP_ACTION_INFO_DEF + 7:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28701);
			player->NpcTrainerId = 28701;
			break;
		case GOSSIP_ACTION_INFO_DEF + 8:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28700);
			player->NpcTrainerId = 28700;
			break;
		case GOSSIP_ACTION_INFO_DEF + 9:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28698);
			player->NpcTrainerId = 28698;
			break;
		case GOSSIP_ACTION_INFO_DEF + 10:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28696);
			player->NpcTrainerId = 28696;
			break;
		case GOSSIP_ACTION_INFO_DEF + 11:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28699);
			player->NpcTrainerId = 28699;
			break;
		case GOSSIP_ACTION_INFO_DEF + 12:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28705);
			player->NpcTrainerId = 28705;
			break;
		case GOSSIP_ACTION_INFO_DEF + 13:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28706);
			player->NpcTrainerId = 28706;
			break;
		case GOSSIP_ACTION_INFO_DEF + 14:
			player->GetSession()->SendTrainerList(creature->GetGUID(), 28742);
			player->NpcTrainerId = 28742;
			break;
		default:
			break;
		}
		return true;
	}
};

void AddSC_TRAINER_NPC()
{
	new TRAINER_NPC();
}

#pragma execution_character_set("utf-8")
#include "CommonFunc.h"
#include "../PrecompiledHeaders/ScriptPCH.h"
#include "BattlegroundMgr.h"
#include "AchievementMgr.h"
#include "AccountMgr.h"
#include "Group.h"
#include "../Requirement/Requirement.h"
#include "../Reward/Reward.h"
#include "../DataLoader/DataLoader.h"
#include "../SellReward/SellReward.h"
#include "../ItemMod/ItemMod.h"
#include "../VIP/VIP.h"
#include "../HonorRank/HonorRank.h"
#include "../Switch/Switch.h"
#include "../GCAddon/GCAddon.h"
#include "../ItemSet/ItemSet.h"
#include "../CustomEvent/Event.h"
#include "../Recovery/Recovery.h"
#include "../AntiCheat/AntiCheat.h"
#include "../String/myString.h"
#include "../TalentReq/TalentReq.h"
#include <fstream>

bool CommonFunc::IsInclude(uint32 mask, uint32 totalMask)
{
	return (totalMask & mask) == mask;
}

bool CampAttackStartFlag = false;

void CommonFunc::StartCamAttack()
{
	CampAttackStartFlag = true;
}

uint32 CommonFunc::GetExtraTalentPoints(Player* player)
{
	QueryResult resultExtraTPs;

	if (sSwitch->GetOnOff(ST_TP_ACCOUNT_BIND))
		resultExtraTPs = LoginDatabase.PQuery("SELECT extraTalentPoints FROM account WHERE id = '%u'", player->GetSession()->GetAccountId());
	else 
		resultExtraTPs = CharacterDatabase.PQuery("SELECT extraTalentPoints FROM characters WHERE guid = '%u'", player->GetGUIDLow());

	if (!resultExtraTPs) 
		return 0;
	else
	{
		Field* fields = resultExtraTPs->Fetch();
		return fields[0].GetInt32();
	}
}

uint32 CommonFunc::GetTokenAmount(Player* player)
{
	QueryResult result = LoginDatabase.PQuery("SELECT tokenAmount FROM account WHERE id = '%u'", player->GetSession()->GetAccountId());
	if (!result) return 0;
	else
	{
		Field* fields1 = result->Fetch();
		return fields1[0].GetInt32();
	}
}

uint32 CommonFunc::GetTotalTokenAmount(Player* player)
{
	QueryResult result = LoginDatabase.PQuery("SELECT totalTokenAmount FROM account WHERE id = '%u'", player->GetSession()->GetAccountId());
	if (!result) return 0;
	else
	{
		Field* fields1 = result->Fetch();
		return fields1[0].GetInt32();
	}
}

void CommonFunc::UpdateTokenAmount(Player* player, uint32 amount, bool ins, std::string action)
{
	if (ins)
		LoginDatabase.DirectPExecute("UPDATE account SET tokenAmount = tokenAmount + '%u' WHERE id = '%u'", amount, player->GetSession()->GetAccountId());
	else
		LoginDatabase.DirectPExecute("UPDATE account SET tokenAmount = tokenAmount - '%u' WHERE id = '%u'", amount, player->GetSession()->GetAccountId());

	sGCAddon->SendTokenUpdateData(player, amount, ins);

	//积分监视
	SQLTransaction trans = CharacterDatabase.BeginTransaction();
	PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_TOKEN);
	stmt->setString(0, sAntiCheat->GetTimeString());
	stmt->setString(1, player->GetName());
	stmt->setUInt32(2, player->GetGUIDLow());
	stmt->setUInt32(3, player->GetSession()->GetAccountId());
	stmt->setString(4, action);
	if (ins)
		stmt->setInt32(5, amount);
	else
		stmt->setInt32(5, -amount);
	trans->Append(stmt);
	CharacterDatabase.CommitTransaction(trans);
}

void CommonFunc::SetOnlineRewardedCount(Player* player, uint32 count)
{
	player->onlineRewardedCount = count;
	CharacterDatabase.DirectPExecute("UPDATE characters SET onlineRewardedCount = '%u' WHERE guid = '%u'", count, player->GetGUIDLow());
}

//弹窗
void CommonFunc::SendAcceptOrCancel(Player* player, uint32 id, std::string text, bool quest)
{
	if (quest)
	{
		WorldPacket data(SMSG_QUEST_CONFIRM_ACCEPT, (4 + text.size() + 8));
		data << uint32(id);
		data << text;
		data << uint64(player->GetGUID());
		player->GetSession()->SendPacket(&data);
		return;
	}
	
	WorldPacket data(SMSG_GOSSIP_MESSAGE, 100);
	data << uint64(player->GetGUID());
	data << uint32(id);
	data << uint32(1);
	data << uint32(1);
	data << uint32(1);
	data << uint8(1);
	data << uint8(0);
	data << uint32(0);
	data << "";
	data << text;
	player->GetSession()->SendPacket(&data);
}

//弹窗事件处理
bool CommonFunc::DoAciotnAfterAccept(Player* player, uint32 id, bool quest)
{
	if (quest)
	{
		switch (id)
		{
		case 1000000://buy item 
			if (sReq->Check(player, player->buy_reqId, player->buy_count))
			{
				if (player->AddItem(player->buy_item, player->buy_count))
					sReq->Des(player, player->buy_reqId, player->buy_count);
				else
					player->GetSession()->SendNotification("购买失败，背包已满或物品唯一");

				player->buy_item = 0;
				player->buy_count = 0;
				player->buy_reqId = 0;
			}
			return true;
		default:
			return false;
		}
		return false;
	}

	switch (id)
	{
	case 994://进入地图
		if (sReq->Check(player, player->enter_map_req))
		{
			sReq->Des(player, player->enter_map_req);
			player->TeleportTo(player->enter_map_at->target_mapId, player->enter_map_at->target_X, player->enter_map_at->target_Y, player->enter_map_at->target_Z, player->enter_map_at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
		}
		return true;
	case 995://天赋消耗
		sTalentReq->DoAction(player);
		return true;
	case 996://buy item 
		if (sReq->Check(player, player->buy_reqId,player->buy_count))
		{
			if (player->AddItem(player->buy_item, player->buy_count))
				sReq->Des(player, player->buy_reqId, player->buy_count);
			else
				player->GetSession()->SendNotification("背包已满");

			player->buy_item  = 0;
			player->buy_count = 0;
			player->buy_reqId = 0;
		}
		return true;
	case 997://购买坐骑
		if (sReq->Check(player, player->mountReqId))
		{
			sReq->Des(player, player->mountReqId);
			player->learnSpell(player->mountSpellId);
			player->mountSpellId = 0;
			player->mountReqId = 0;
		}
		return true;
	case 999://购买幻化
		if (sReq->Check(player, player->trans_reqId))
		{
			if (Item* item = player->AddItemById(player->trans_item, 1))
			{
				sReq->Des(player, player->trans_reqId);
				//添加幻化标记
				//player->ApplyEnchantment(item, EnchantmentSlot(PERM_ENCHANTMENT_SLOT), false);
				//item->SetEnchantment(EnchantmentSlot(PERM_ENCHANTMENT_SLOT), TRANS_FLAG_ENCHANT_ID, 0, 0);
				//player->ApplyEnchantment(item, EnchantmentSlot(PERM_ENCHANTMENT_SLOT), true);
			}
			else
			{
				player->GetSession()->SendNotification("背包已满");
			}
			player->trans_reqId = 0;
			player->trans_item = 0;
		}
		return true;
	default:
		return false;
	}
	return false;
}

//播放声音

void CommonFunc::PlayCustomSound(Player* player, uint32 soundId)
{
	if (!sSoundEntriesStore.LookupEntry(soundId)) return;
	WorldPacket data(SMSG_PLAY_SOUND, 4);
	data << uint32(soundId);
	sWorld->SendGlobalMessage(&data);
}

void CommonFunc::CompleteQuest(Player* player, uint32 questId)
{
	Quest const* quest = sObjectMgr->GetQuestTemplate(questId);
	if (quest && player->GetQuestStatus(questId) == QUEST_STATUS_INCOMPLETE)
		player->CompleteQuest(questId);
}

//获取物品图标、名字及链接
std::string CommonFunc::GetItemIcon(uint32 entry, uint32 width, uint32 height, int x, int y)
{
	std::ostringstream ss;
	ss << "|TInterface";
	const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
	const ItemDisplayInfoEntry* dispInfo = NULL;
	if (temp)
	{
		dispInfo = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);
		if (dispInfo)
			ss << "/ICONS/" << dispInfo->inventoryIcon;
	}
	if (!dispInfo)
		ss << "/InventoryItems/WoWUnknownItem01";
	ss << ":" << width << ":" << height << ":" << x << ":" << y << "|t";
	return ss.str();
}

std::string CommonFunc::GetItemLink(Item* item, WorldSession* session)
{


	int loc_idx = session->GetSessionDbLocaleIndex();
	const ItemTemplate* temp = item->GetTemplate();
	std::string name = temp->Name1;

	if (int32 itemRandPropId = item->GetItemRandomPropertyId())
	{
		char* const* suffix = NULL;
		if (itemRandPropId < 0)
		{
			const ItemRandomSuffixEntry* itemRandEntry = sItemRandomSuffixStore.LookupEntry(-item->GetItemRandomPropertyId());
			if (itemRandEntry)
				suffix = itemRandEntry->nameSuffix;
		}
		else
		{
			const ItemRandomPropertiesEntry* itemRandEntry = sItemRandomPropertiesStore.LookupEntry(item->GetItemRandomPropertyId());
			if (itemRandEntry)
				suffix = itemRandEntry->nameSuffix;
		}
		if (suffix)
		{
			std::string test(suffix[(name != temp->Name1) ? loc_idx : DEFAULT_LOCALE]);
			if (!test.empty())
			{
				name += ' ';
				name += test;
			}
		}
	}

	std::ostringstream oss;
	oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec <<
		"|Hitem:" << temp->ItemId << ":" <<
		item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT) << ":" <<
		item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT) << ":" <<
		item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_2) << ":" <<
		item->GetEnchantmentId(SOCK_ENCHANTMENT_SLOT_3) << ":" <<
		item->GetEnchantmentId(BONUS_ENCHANTMENT_SLOT) << ":" <<
		item->GetItemRandomPropertyId() << ":" << item->GetItemSuffixFactor() << ":" <<
		(uint32)item->GetOwner()->getLevel() << "|h[" << name << "]|h|r";

	return oss.str();
}

std::string CommonFunc::GetItemLink(uint32 entry)
{
	const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
	if (!temp) return "";

	std::string name = temp->Name1;

	std::ostringstream oss;
	oss << "|c" << std::hex << ItemQualityColors[temp->Quality] << std::dec <<
		"|Hitem:" << entry << ":0:0:0:0:0:0:0:0:0|h[" << name << "]|h|r";

	return oss.str();

}

//获取成就点
uint32 CommonFunc::GetAchievementPoints(Player* player)
{
	CompletedAchievementMap cam = player->getAchievementMgr()->get_m_completedAchievements();
	uint32 points = 0;
	for (CompletedAchievementMap::const_iterator itr = cam.begin(); itr != cam.end(); ++itr)
		if (AchievementEntry const* completedAchievements = sAchievementStore.LookupEntry(itr->first))
			points += completedAchievements->points;
	return points;
}

//队伍奖励
void CommonFunc::GroupReward(Player* player, uint32 rewardTemplateId, uint32 chance)
{
	Group* grp = player->GetGroup();
	if (!grp)
	{
		if (urand(1, 100) <= chance) sRew->Rew(player, rewardTemplateId);
		return;
	}
	for (GroupReference* itr = grp->GetFirstMember(); itr != NULL; itr = itr->next())
		if (Player* member = itr->GetSource())
		{
			if (member->GetDistance(player) > 75.0f) continue;
			if (urand(1, 100) <= chance) sRew->Rew(member, rewardTemplateId);
		}
}

//additem 包包满时sendmail
void CommonFunc::AddOrMailItem(Player* player, uint32 itemId, uint32 count)
{

}

void CommonFunc::SetSpeed(Player* player, float multi)
{
	player->SetSpeed(MOVE_WALK, multi, true);
	player->SetSpeed(MOVE_RUN, multi, true);
	player->SetSpeed(MOVE_SWIM, multi, true);
	player->SetSpeed(MOVE_FLIGHT, multi, true);
}

void CommonFunc::SendMsGToAll(std::string str, uint32 type)
{
	SessionMap const& smap = sWorld->GetAllSessions();//获取所有在线玩家
	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
		if (Player* player = iter->second->GetPlayer())
		{
			if (player->GetSession())
			{
				switch (type)
				{
				case 0:
					player->GetSession()->SendAreaTriggerMessage(str.c_str());
					break;
				case 1:
					player->GetSession()->SendNotification(str.c_str());
					break;
				default:
					break;
				}
			}
		}
}

void CommonFunc::GetRace(Player* player, std::string &race, std::string &raceIcon, bool fakeplayer, uint8 fakerace,uint8 gender)
{
	uint8 player_race = fakeplayer ? fakerace : player->getRace();
	uint8 player_gender = fakeplayer ? gender : player->getGender();


	switch (player_race)
	{
	case RACE_BLOODELF:
		race = "|cFFFF1717[血精灵]|r";
		if (player_race == GENDER_MALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Bloodelf_Male:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Bloodelf_Female:14:14:0:-2|t";
		break;
	case RACE_DRAENEI:
		race = "|cFF0177EC[德莱尼]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Draenei_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Draenei_Male:14:14:0:-2|t";
		break;
	case RACE_DWARF:
		race = "|cFF0177EC[矮人]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Dwarf_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Dwarf_Male:14:14:0:-2|t";
		break;
	case RACE_GNOME:
		race = "|cFF0177EC[侏儒]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Gnome_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Gnome_Male:14:14:0:-2|t";
		break;
	case RACE_HUMAN:
		race = "|cFF0177EC[人类]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Human_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Human_Male:14:14:0:-2|t";
		break;
	case RACE_NIGHTELF:
		race = "|cFF0177EC[暗夜精灵]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Nightelf_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Nightelf_Male:14:14:0:-2|t";
		break;
	case RACE_ORC:
		race = "|cFFFF1717[兽人]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Orc_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Orc_Male:14:14:0:-2|t";
		break;
	case RACE_TAUREN:
		race = "|cFFFF1717[牛头人]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Tauren_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Tauren_Male:14:14:0:-2|t";
		break;
	case RACE_TROLL:
		race = "|cFFFF1717[巨魔]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Troll_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Troll_Male:14:14:0:-2|t";
		break;
	case RACE_UNDEAD_PLAYER:
		race = "|cFFFF1717[亡灵]|r";
		if (player_race == GENDER_FEMALE)
			raceIcon = "|TInterface/ICONS/Achievement_Character_Undead_Female:14:14:0:-2|t";
		else
			raceIcon = "|TInterface/ICONS/Achievement_Character_Undead_Male:14:14:0:-2|t";
		break;
	}
}

void CommonFunc::GetClass(Player* player, std::string &_class, std::string &classIcon, bool fakeplayer, uint8 fakeclass)
{
	uint8 player_class = fakeplayer ? fakeclass : player->getClass();

	switch (player_class)
	{
	case CLASS_DEATH_KNIGHT:
		_class = "|cffC41F3B[死亡骑士]|r";
		classIcon = "|TInterface\\icons\\Spell_Deathknight_ClassIcon:14:14:0:-2|t";
		break;
	case CLASS_DRUID:
		_class = "|cffFF7D0A[德鲁伊]|r";
		classIcon = "|TInterface\\icons\\Ability_Druid_Maul:14:14:0:-2|t";
		break;
	case CLASS_HUNTER:
		_class = "|cffABD473[猎人]|r";
		classIcon = "|TInterface\\icons\\INV_Weapon_Bow_07:14:14:0:-2|t";
		break;
	case CLASS_MAGE:
		_class = "|cff69CCF0[法师]|r";
		classIcon = "|TInterface\\icons\\INV_Staff_13:14:14:0:-2|t";
		break;
	case CLASS_PALADIN:
		_class = "|cffF58CBA[圣骑士]|r";
		classIcon = "|TInterface\\icons\\INV_Hammer_01:14:14:0:-2|t";
		break;
	case CLASS_PRIEST:
		_class = "|cffFFFFFF[牧师]|r";
		classIcon = "|TInterface\\icons\\INV_Staff_30:14:14:0:-2|t";
		break;
	case CLASS_ROGUE:
		_class = "|cffFFF569[盗贼]|r";
		classIcon = "|TInterface\\icons\\INV_ThrowingKnife_04:14:14:0:-2|t";
		break;
	case CLASS_SHAMAN:
		_class = "|cff0070DE[萨满祭司]|r";
		classIcon = "|TInterface\\icons\\Spell_Nature_BloodLust:14:14:0:-2|t";
		break;
	case CLASS_WARLOCK:
		_class = "|cff9482C9[术士]|r";
		classIcon = "|TInterface\\icons\\Spell_Nature_FaerieFire:14:14:0:-2|t";
		break;
	case CLASS_WARRIOR:
		_class = "|cffC79C6E[战士]|r";
		classIcon = "|TInterface\\icons\\INV_Sword_27.png:14:14:0:-2|t";
		break;
	}
}

std::string CommonFunc::GetNameLink(Player* player)
{
	std::string name = player->GetName();
	return "|Hplayer:" + name + "|h[" + name + "]|h";
}

void CommonFunc::GetNameLink(Player* player, std::string &nameLink, std::string &nameLinkWithColor, bool fakeplayer, uint8 fakeclass, std::string fakename)
{
	std::string color = "";

	uint8 player_class = fakeplayer ? fakeclass : player->getClass();

	switch (player_class)
	{
	case CLASS_WARRIOR:
		color = "|cFFC79C6E";
		break;
	case CLASS_PALADIN:
		color = "|cFFF58CBA";
		break;
	case CLASS_HUNTER:
		color = "|cFFABD473";
		break;
	case CLASS_ROGUE:
		color = "|cFFFFF569";
		break;
	case CLASS_PRIEST:
		color = "|cFFFFFFFF";
		break;
	case CLASS_DEATH_KNIGHT:
		color = "|cFFC41F3B";
		break;
	case CLASS_SHAMAN:
		color = "|cFF0070DE";
		break;
	case CLASS_MAGE:
		color = "|cFF69CCF0";
		break;
	case CLASS_WARLOCK:
		color = "|cFF9482C9";
		break;
	case CLASS_DRUID:
		color = "|cFFFF7d0A";
		break;
	default:
		break;
	}
	std::string name = fakeplayer ? fakename : player->namePrefix + player->GetName() + player->nameSuffix;

	nameLink = "|Hplayer:" + name + "|h[" + name + "]|h";

	nameLinkWithColor = color + "|Hplayer:" + name + "|h[" + name + "]|h|r";
}

void CommonFunc::GetHRTitle(Player* player, std::string &hrTitle, std::string &hrIcon, bool fakeplayer, uint8 fakehr)
{
	if (fakeplayer)
	{
		switch (fakehr)
		{
		case 28:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_14:14:14:0:-2|t"; hrTitle = "|cFFFF1717[高阶督军]|r"; break;
		case 27:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_13:14:14:0:-2|t"; hrTitle = "|cFFFF1717[督军]|r"; break;
		case 26:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_12:14:14:0:-2|t"; hrTitle = "|cFFFF1717[将军]|r"; break;
		case 25:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_11:14:14:0:-2|t"; hrTitle = "|cFFFF1717[中将]|r"; break;
		case 24:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_10:14:14:0:-2|t"; hrTitle = "|cFFFF1717[勇士]|r"; break;
		case 23:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_09:14:14:0:-2|t"; hrTitle = "|cFFFF1717[百夫长]|r"; break;
		case 22:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_08:14:14:0:-2|t"; hrTitle = "|cFFFF1717[军团士兵]|r"; break;
		case 21:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_07:14:14:0:-2|t"; hrTitle = "|cFFFF1717[血卫士]|r"; break;
		case 20:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_06:14:14:0:-2|t"; hrTitle = "|cFFFF1717[石头守卫]|r"; break;
		case 19:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_05:14:14:0:-2|t"; hrTitle = "|cFFFF1717[一等军士长]|r"; break;
		case 18:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_04:14:14:0:-2|t"; hrTitle = "|cFFFF1717[高阶军士]|r"; break;
		case 17:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_03:14:14:0:-2|t"; hrTitle = "|cFFFF1717[中士]|r"; break;
		case 16:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_02:14:14:0:-2|t"; hrTitle = "|cFFFF1717[步兵]|r"; break;
		case 15:hrIcon = "|TInterface/ICONS/Achievement_PVP_H_01:14:14:0:-2|t"; hrTitle = "|cFFFF1717[斥候]|r"; break;
		case 14:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_14:14:14:0:-2|t"; hrTitle = "|cFF0177EC[大元帅]|r"; break;
		case 13:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_13:14:14:0:-2|t"; hrTitle = "|cFF0177EC[元帅]|r"; break;
		case 12:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_12:14:14:0:-2|t"; hrTitle = "|cFF0177EC[统帅]|r"; break;
		case 11:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_11:14:14:0:-2|t"; hrTitle = "|cFF0177EC[司令]|r"; break;
		case 10:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_10:14:14:0:-2|t"; hrTitle = "|cFF0177EC[少校]|r"; break;
		case 9:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_09:14:14:0:-2|t"; hrTitle = "|cFF0177EC[护卫骑士]|r"; break;
		case 8:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_08:14:14:0:-2|t"; hrTitle = "|cFF0177EC[骑士队长]|r"; break;
		case 7:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_07:14:14:0:-2|t"; hrTitle = "|cFF0177EC[骑士中尉]|r"; break;
		case 6:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_06:14:14:0:-2|t"; hrTitle = "|cFF0177EC[骑士]|r"; break;
		case 5:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_05:14:14:0:-2|t"; hrTitle = "|cFF0177EC[士官长]|r"; break;
		case 4:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_04:14:14:0:-2|t"; hrTitle = "|cFF0177EC[军士长]|r"; break;
		case 3:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_03:14:14:0:-2|t"; hrTitle = "|cFF0177EC[中士]|r"; break;
		case 2:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_02:14:14:0:-2|t"; hrTitle = "|cFF0177EC[下士]|r"; break;
		case 1:hrIcon = "|TInterface/ICONS/Achievement_PVP_A_01:14:14:0:-2|t"; hrTitle = "|cFF0177EC[列兵]|r"; break;
		}

		return;
	}

	if (player->GetTeamId() == TEAM_ALLIANCE)
	{
		if (player->HasTitle(14))		{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_14:14:14:0:-2|t"; hrTitle = "|cFF0177EC[大元帅]|r";		}
		else if (player->HasTitle(13))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_13:14:14:0:-2|t"; hrTitle = "|cFF0177EC[元帅]|r";		}
		else if (player->HasTitle(12))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_12:14:14:0:-2|t"; hrTitle = "|cFF0177EC[统帅]|r";		}
		else if (player->HasTitle(11))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_11:14:14:0:-2|t"; hrTitle = "|cFF0177EC[司令]|r";		}
		else if (player->HasTitle(10))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_10:14:14:0:-2|t"; hrTitle = "|cFF0177EC[少校]|r";		}
		else if (player->HasTitle(9))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_09:14:14:0:-2|t"; hrTitle = "|cFF0177EC[护卫骑士]|r";	}
		else if (player->HasTitle(8))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_08:14:14:0:-2|t"; hrTitle = "|cFF0177EC[骑士队长]|r";	}
		else if (player->HasTitle(7))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_07:14:14:0:-2|t"; hrTitle = "|cFF0177EC[骑士中尉]|r";	}
		else if (player->HasTitle(6))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_06:14:14:0:-2|t"; hrTitle = "|cFF0177EC[骑士]|r";		}
		else if (player->HasTitle(5))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_05:14:14:0:-2|t"; hrTitle = "|cFF0177EC[士官长]|r";		}
		else if (player->HasTitle(4))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_04:14:14:0:-2|t"; hrTitle = "|cFF0177EC[军士长]|r";		}
		else if (player->HasTitle(3))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_03:14:14:0:-2|t"; hrTitle = "|cFF0177EC[中士]|r";		}
		else if (player->HasTitle(2))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_02:14:14:0:-2|t"; hrTitle = "|cFF0177EC[下士]|r";		}
		else							{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_01:14:14:0:-2|t"; hrTitle = "|cFF0177EC[列兵]|r";		}
		if (player->IsGameMaster())		{ hrIcon = "|TInterface/ICONS/Achievement_PVP_A_16:14:14:0:-2|t"; hrTitle = "|cFF0177EC[GM]|r";			}
	}
	else
	{
		if (player->HasTitle(28))		{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_14:14:14:0:-2|t"; hrTitle = "|cFFFF1717[高阶督军]|r";	}
		else if (player->HasTitle(27))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_13:14:14:0:-2|t"; hrTitle = "|cFFFF1717[督军]|r";		}
		else if (player->HasTitle(26))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_12:14:14:0:-2|t"; hrTitle = "|cFFFF1717[将军]|r";		}
		else if (player->HasTitle(25))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_11:14:14:0:-2|t"; hrTitle = "|cFFFF1717[中将]|r";		}
		else if (player->HasTitle(24))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_10:14:14:0:-2|t"; hrTitle = "|cFFFF1717[勇士]|r";		}
		else if (player->HasTitle(23))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_09:14:14:0:-2|t"; hrTitle = "|cFFFF1717[百夫长]|r";		}
		else if (player->HasTitle(22))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_08:14:14:0:-2|t"; hrTitle = "|cFFFF1717[军团士兵]|r";	}
		else if (player->HasTitle(21))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_07:14:14:0:-2|t"; hrTitle = "|cFFFF1717[血卫士]|r";		}
		else if (player->HasTitle(20))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_06:14:14:0:-2|t"; hrTitle = "|cFFFF1717[石头守卫]|r";	}
		else if (player->HasTitle(19))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_05:14:14:0:-2|t"; hrTitle = "|cFFFF1717[一等军士长]|r";	}
		else if (player->HasTitle(18))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_04:14:14:0:-2|t"; hrTitle = "|cFFFF1717[高阶军士]|r";	}
		else if (player->HasTitle(17))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_03:14:14:0:-2|t"; hrTitle = "|cFFFF1717[中士]|r";		}
		else if (player->HasTitle(16))	{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_02:14:14:0:-2|t"; hrTitle = "|cFFFF1717[步兵]|r";		}
		else							{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_01:14:14:0:-2|t"; hrTitle = "|cFFFF1717[斥候]|r";		}
		if (player->IsGameMaster())		{ hrIcon = "|TInterface/ICONS/Achievement_PVP_H_16:14:14:0:-2|t"; hrTitle = "|cFFFF1717[GM]|r";			}
	}
}

std::string CommonFunc::GetHRTitle(uint32 rank)
{
	std::string hrTitle = "";
	std::string hrTitle1 = "";

	switch (rank)
	{
	case 14:hrTitle1 = "|cFFFF1717[高阶督军]|r"; break;
	case 13:hrTitle1 = "|cFFFF1717[督军]|r"; break;
	case 12:hrTitle1 = "|cFFFF1717[将军]|r"; break;
	case 11:hrTitle1 = "|cFFFF1717[中将]|r"; break;
	case 10:hrTitle1 = "|cFFFF1717[勇士]|r"; break;
	case 9: hrTitle1 = "|cFFFF1717[百夫长]|r"; break;
	case 8: hrTitle1 = "|cFFFF1717[军团士兵]|r"; break;
	case 7: hrTitle1 = "|cFFFF1717[血卫士]|r"; break;
	case 6: hrTitle1 = "|cFFFF1717[石头守卫]|r"; break;
	case 5: hrTitle1 = "|cFFFF1717[一等军士长]|r"; break;
	case 4: hrTitle1 = "|cFFFF1717[高阶军士]|r"; break;
	case 3: hrTitle1 = "|cFFFF1717[中士]|r"; break;
	case 2: hrTitle1 = "|cFFFF1717[步兵]|r"; break;
	case 1: hrTitle1 = "|cFFFF1717[斥候]|r"; break;
	}

	switch (rank)
	{
	case 14:hrTitle = "|cFF0177EC[大元帅]|r"; break;
	case 13:hrTitle = "|cFF0177EC[元帅]|r"; break;
	case 12:hrTitle = "|cFF0177EC[统帅]|r"; break;
	case 11:hrTitle = "|cFF0177EC[司令]|r"; break;
	case 10:hrTitle = "|cFF0177EC[少校]|r"; break;
	case 9: hrTitle = "|cFF0177EC[护卫骑士]|r"; break;
	case 8: hrTitle = "|cFF0177EC[骑士队长]|r"; break;
	case 7: hrTitle = "|cFF0177EC[骑士中尉]|r"; break;
	case 6: hrTitle = "|cFF0177EC[骑士]|r"; break;
	case 5: hrTitle = "|cFF0177EC[士官长]|r"; break;
	case 4: hrTitle = "|cFF0177EC[军士长]|r"; break;
	case 3: hrTitle = "|cFF0177EC[中士]|r"; break;
	case 2: hrTitle = "|cFF0177EC[下士]|r"; break;
	case 1: hrTitle = "|cFF0177EC[列兵]|r"; break;
	}

	return hrTitle + "或" + hrTitle1;
}

void CommonFunc::UpdatePlayerNameWithHR(Player* player)
{
	//std::ostringstream oss;
	//oss << player->GetName();
	//oss << " ";
	//oss << GetHRTitle(player);
	//sWorld->UpdateGlobalNameData(player->GetGUIDLow(), player->GetName(), oss.str());
	//sWorld->UpdateGlobalPlayerData(player->GetGUIDLow(), PLAYER_UPDATE_DATA_NAME, oss.str());
	//player->ToggleDND();
	//player->canUpdateName = true;
}

uint32 CommonFunc::GetHrTitleId(Player* player)
{
	uint32 id;
	if (player->GetTeamId() == TEAM_ALLIANCE)
	{
		if (player->HasTitle(14)) id = 14;
		else if (player->HasTitle(13)) id = 13;
		else if (player->HasTitle(12)) id = 12;
		else if (player->HasTitle(11)) id = 11;
		else if (player->HasTitle(10)) id = 10;
		else if (player->HasTitle(9)) id = 9;
		else if (player->HasTitle(8)) id = 8;
		else if (player->HasTitle(7)) id = 7;
		else if (player->HasTitle(6)) id = 6;
		else if (player->HasTitle(5)) id = 5;
		else if (player->HasTitle(4)) id = 4;
		else if (player->HasTitle(3)) id = 3;
		else if (player->HasTitle(2)) id = 2;
		else if (player->HasTitle(1)) id = 1;
		else id = 0;
	}
	else
	{
		if (player->HasTitle(28)) id = 28;
		else if (player->HasTitle(27)) id = 27;
		else if (player->HasTitle(26)) id = 26;
		else if (player->HasTitle(25)) id = 25;
		else if (player->HasTitle(24)) id = 24;
		else if (player->HasTitle(23)) id = 23;
		else if (player->HasTitle(22)) id = 22;
		else if (player->HasTitle(21)) id = 21;
		else if (player->HasTitle(20)) id = 20;
		else if (player->HasTitle(19)) id = 19;
		else if (player->HasTitle(18)) id = 18;
		else if (player->HasTitle(17)) id = 17;
		else if (player->HasTitle(16)) id = 16;
		else if (player->HasTitle(15)) id = 15;
		else id = 0;
	}
	return id;
}

uint32 CommonFunc::GetOnlineIpCount(Player* player)
{
	uint32 count = 0;
	std::string IP = player->GetSession()->GetRemoteAddress();

	SessionMap const& smap = sWorld->GetAllSessions();
	for (SessionMap::const_iterator iter = smap.begin(); iter != smap.end(); ++iter)
		if (Player* pPlayer = iter->second->GetPlayer())
		{
			if (pPlayer->GetSession())
			{
				if (strcmp(IP.c_str(), pPlayer->GetSession()->GetRemoteAddress().c_str()) == 0)
					count++;
			}
		}

	return count;
}

uint32 CommonFunc::GetBgIpCount(Player* player, Battleground* bg)
{
	uint32 count = 0;
	std::string IP = player->GetSession()->GetRemoteAddress();

	Battleground::BattlegroundPlayerMap const& pl = bg->GetPlayers();
	for (Battleground::BattlegroundPlayerMap::const_iterator itr = pl.begin(); itr != pl.end(); ++itr)
	{
		if (itr->second->GetSession())
		{
			if (strcmp(IP.c_str(), itr->second->GetSession()->GetRemoteAddress().c_str()) == 0)
				count++;
		}
	}
	return count;
}

bool CommonFunc::onlineIsAllowed(Player* player)
{
	if (GetOnlineIpCount(player) > uint32(sSwitch->GetValue(ST_ONLINE_PLAYERS)))
		return false;
	else
		return true;
}

bool CommonFunc::joinBgIsAllowed(Player* player, Battleground* bg)
{
	uint32 count = GetBgIpCount(player, bg);
	uint32 max = sSwitch->GetValue(ST_BG_PLAYERS);

	if (count > max)
	{
		std::ostringstream oss;
		oss << "加入战场的玩家数量已达到上限，加入失败！" << count << "/" << max;
		player->GetSession()->SendNotification(oss.str().c_str());
		return false;
	}
		return true;
}

void CommonFunc::updateHRTitle(Player* player)
{
	uint32 playerHrTitleLevel = getPlayerHrTitleLevel(player);
	uint32 accountHrTitleLevel = getAccountHrTitleLevel(player);

	if (playerHrTitleLevel == accountHrTitleLevel)
		return;

	if (playerHrTitleLevel < accountHrTitleLevel)
	{
		CharTitlesEntry const* titleInfo;

		for (uint32 i = playerHrTitleLevel + 1; i < accountHrTitleLevel + 1; i++)
		{
			player->realTeam == TEAM_ALLIANCE ? titleInfo = sCharTitlesStore.LookupEntry(i) : titleInfo = sCharTitlesStore.LookupEntry(i + 14);
			if (!titleInfo) return;
			player->SetTitle(titleInfo);
		}
	}
	else
		LoginDatabase.DirectPExecute("UPDATE account SET hrLevel = '%u' WHERE id = '%u'", playerHrTitleLevel, player->GetSession()->GetAccountId());
}

uint32 CommonFunc::getPlayerHrTitleLevel(Player* player)
{
	if (player->HasTitle(14) || player->HasTitle(28)) return 14;
	else if (player->HasTitle(13) || player->HasTitle(27)) return 13;
	else if (player->HasTitle(12) || player->HasTitle(26)) return 12;
	else if (player->HasTitle(11) || player->HasTitle(25)) return 11;
	else if (player->HasTitle(10) || player->HasTitle(24)) return 10;
	else if (player->HasTitle(9) || player->HasTitle(23)) return 9;
	else if (player->HasTitle(8) || player->HasTitle(22)) return 8;
	else if (player->HasTitle(7) || player->HasTitle(21)) return 7;
	else if (player->HasTitle(6) || player->HasTitle(20)) return 6;
	else if (player->HasTitle(5) || player->HasTitle(19)) return 5;
	else if (player->HasTitle(4) || player->HasTitle(18)) return 4;
	else if (player->HasTitle(3) || player->HasTitle(17)) return 3;
	else if (player->HasTitle(2) || player->HasTitle(16)) return 2;
	else if (player->HasTitle(1) || player->HasTitle(15)) return 1;
	else return 0;
}

uint32 CommonFunc::getAccountHrTitleLevel(Player* player)
{
	QueryResult result = LoginDatabase.PQuery("SELECT hrLevel FROM account WHERE id = '%u'", player->GetSession()->GetAccountId());
	if (!result) return 0;
	else
	{
		Field* fields = result->Fetch();
		return fields[0].GetInt32();
	}
}

void CommonFunc::SetLootRate(Player* player)
{
	player->SetCustomLootRate(1.0f);

	float totalRate = 1.0f;
	float factionRate = 1.0f;
	float playerRate = 1.0f;
	float hrRate = 1.0f;
	float vipRate = 1.0f;

	player->realTeam == TEAM_ALLIANCE ? factionRate = sSwitch->GetValue(ST_LOOT_RATE_A) : factionRate = sSwitch->GetValue(ST_LOOT_RATE_H);

	std::unordered_map<uint32, float>::iterator iter = PlayerDropRateMap.find(player->GetGUIDLow());
	if (iter != PlayerDropRateMap.end())
		playerRate = iter->second;

	if (!HRUpVec.empty())
		hrRate = sHR->GetRate(player, HR_RATE_LOOT);

	if (!VIPVec.empty())
		vipRate = sVIP->GetLootRate(player);

	if (vipRate == 0)
		vipRate = 1.0f;

	if (hrRate == 0)
		hrRate = 1.0f;

	if (factionRate == 0)
		factionRate = 1.0f;

	totalRate = factionRate * playerRate * hrRate * vipRate;

	player->SetCustomLootRate(totalRate);
}

uint32 CommonFunc::GetGemCountByEntry(Player* player, uint32 itemEntry)
{
	uint32 count = 0;
	for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i)
		if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			count += pItem->GetGemCountWithID(itemEntry);
	return count;
}

void CommonFunc::ResetInstance(Player* player, Difficulty diff, bool isRaid, uint32 mapId)
{
	for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
	{
		BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(player->GetGUIDLow(), Difficulty(i));
		for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end();)
		{
			InstanceSave* save = itr->second.save;
			MapEntry const* mapEntry = sMapStore.LookupEntry(itr->first);
			if (mapEntry && mapEntry->IsRaid() == isRaid && itr->first != player->GetMapId() && (!mapId || mapId == itr->first) && (diff == -1 || diff == save->GetDifficulty()))
			{
				sInstanceSaveMgr->PlayerUnbindInstance(player->GetGUIDLow(), itr->first, diff, true, player);
				itr = m_boundInstances.begin();
			}
			else
				++itr;
		}
	}
}

std::vector<std::string> CommonFunc::SplitStr(std::string str, std::string pattern)
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;
	int size = str.size();
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

std::string CommonFunc::GetPlayerTotalName(Player* player, std::string flag, bool fakeplayer, uint8 fake_class, uint8 fake_race, uint8 fake_hr, std::string fake_name, uint32 vip,uint8 gender)
{
	//1-职业 2-种族 3-VIP 4-军衔 5-名字
	// 1 | 2 | 3 | 4 | 5
	
	std::string totalName = "";

	std::string rcTitle = "";
	std::string rcIcon = "";
	std::string gossip = "";

	//获取职业
	std::string _class = "";
	std::string classIcon = "";
	fakeplayer ? GetClass(NULL,_class,classIcon,true,fake_class) : GetClass(player, _class, classIcon);

	//获取种族
	std::string race = "";
	std::string raceIcon = "";
	fakeplayer ? GetRace(NULL, race, raceIcon, true, fake_race, gender) : GetRace(player, race, raceIcon);

	//获取军衔
	std::string hrTitle = "";
	std::string hrIcon = "";
	fakeplayer ? GetHRTitle(NULL, hrTitle, hrIcon, true, fake_hr) : GetHRTitle(player, hrTitle, hrIcon);

	std::string vipTitle = "";
	std::string vipIcon = ""; 
	fakeplayer ? sVIP->GetVIPTitle(NULL, vipTitle, vipIcon,true,vip) : sVIP->GetVIPTitle(player, vipTitle, vipIcon);

	std::string nameLink = "";
	std::string nameLinkWithColor = "";
	fakeplayer ? GetNameLink(NULL, nameLink, nameLinkWithColor,true,fake_class,fake_name) : GetNameLink(player, nameLink, nameLinkWithColor);

	std::vector<std::string> NameTypeVec = sCF->SplitStr(flag, "|");
	for (size_t i = 0; i < NameTypeVec.size(); i++)
	{
		uint32 nameType = (uint32)atoi(NameTypeVec[i].c_str());

		switch (nameType)
		{
		case 1:
			player->realTeam == TEAM_ALLIANCE ? totalName += "|cFF0177EC[联盟]|r" : totalName += "|cFFFF1717[部落]|r";
			break;
		case 110:
			totalName += classIcon;
			break;
		case 101:
			totalName += _class;
			break;
		case 111:
			totalName += classIcon;
			totalName += _class;
			break;
		case 210:
			totalName += raceIcon;
			break;
		case 201:
			totalName += race;
			break;
		case 211:
			totalName += raceIcon;
			totalName += race;
			break;
		case 310:
			totalName += vipIcon;
			break;
		case 301:
			totalName += vipTitle;
			break;
		case 311:
			totalName += vipIcon;
			totalName += vipTitle;
			break;
		case 410:
			totalName += hrIcon;
			break;
		case 401:
			totalName += hrTitle;
			break;
		case 411:
			totalName += hrIcon;
			totalName += hrTitle;
			break;
		case 510:
			break;
			totalName += rcIcon;
		case 501:
			totalName += rcTitle;
			break;
		case 511:
			totalName += rcIcon;
			totalName += rcTitle;
			break;
		case 60:
			totalName += nameLink;
			break;
		case 61:
			totalName += nameLinkWithColor;
			break;
		}
	}

	return totalName;
}

uint32 CommonFunc::GetCommercePoints(Player* player)
{
	player->tradeSkillCount = 0;
	if (player->HasSpell(2366)) player->tradeSkillCount++;
	if (player->HasSpell(2575)) player->tradeSkillCount++;
	if (player->HasSpell(8613)) player->tradeSkillCount++;
	if (player->HasSpell(2018)) player->tradeSkillCount++;
	if (player->HasSpell(3908)) player->tradeSkillCount++;
	if (player->HasSpell(2108)) player->tradeSkillCount++;
	if (player->HasSpell(4036)) player->tradeSkillCount++;
	if (player->HasSpell(2259)) player->tradeSkillCount++;
	if (player->HasSpell(45357)) player->tradeSkillCount++;
	if (player->HasSpell(7411)) player->tradeSkillCount++;
	if (player->HasSpell(25229)) player->tradeSkillCount++;

	return player->tradeSkillCount;
}

void CommonFunc::SetCommercePoints(Player* player)
{
	QueryResult result = CharacterDatabase.PQuery("SELECT extraPrimaryTradeSkills FROM characters where guid = %u", player->GetGUIDLow());
	if (result)
		player->maxPrimaryTradeSkills += result->Fetch()[0].GetUInt32();
	if (player->maxPrimaryTradeSkills >= GetCommercePoints(player))
		player->SetFreePrimaryProfessions(player->maxPrimaryTradeSkills - GetCommercePoints(player));
}



//QQMsg
#define QQMSG_PORT		5000
#define QQMSG_GROUP		"创世魔兽玩家群"
std::queue<std::string> QQMsgQueue;

std::string GetQQMsgTime()
{
	time_t t = time(NULL);
	tm localTime;
	localtime_r(&t, &localTime);
	int hour = localTime.tm_hour;
	int min = localTime.tm_min;
	int sec = localTime.tm_sec;
	std::ostringstream oss;
	oss << hour << ":" << min;
	return oss.str();
}

DWORD WINAPI QQMsgThread(LPVOID i)
{
	WSADATA wsaData;

	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		WSACleanup();
		return 0;
	}

	struct sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	srv_addr.sin_port = ntohs(QQMSG_PORT);

	uint32 count = 0;

	while (true)
	{
		if (!QQMsgQueue.empty())
		{
			SOCKET cli_sock = socket(PF_INET, SOCK_STREAM, 0);
			if (cli_sock == INVALID_SOCKET)
				continue;

			if (connect(cli_sock, (LPSOCKADDR)&srv_addr, sizeof(srv_addr)) == SOCKET_ERROR)
				continue;

			count++;

			std::ostringstream oss;
			oss << "【游戏消息 " << "第" << count << "条 " << GetQQMsgTime() << "】" << QQMsgQueue.front();
			QQMsgQueue.pop();

			std::string msg = oss.str();
			url_encode(msg);
			

			char s[1024];
			sprintf(s, "GET /openqq/send_group_message?name=%s&content=%s HTTP/1.1\r\nHost: 127.0.0.1:%u\r\n\r\n", QQMSG_GROUP, msg.c_str(), QQMSG_PORT);
			send(cli_sock, s, sizeof(s), 0);
			closesocket(cli_sock);

			Sleep(1000);
		}
	}

	return 0;
}

class QQMsgWorldScript : public WorldScript
{
public:
	QQMsgWorldScript() : WorldScript("QQMsgWorldScript") {}

	void OnStartup()
	{
		CreateThread(NULL, 0, QQMsgThread, NULL, 0, NULL);
	}
};

void AddSC_QQMsg()
{
	//new QQMsgWorldScript();
}

std::string GetDBName(std::string str)
{
	std::string pattern = ";";

	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;
	int size = str.size();
	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result[4];
}
void SplitStr(const char* delim, char* args, char** arg1, char** arg2)
{
	char* p1 = strtok(args, delim);
	char* p2 = strtok(NULL, delim);

	if (!p2)
	{
		p2 = p1;
		p1 = NULL;
	}

	if (arg1)
		*arg1 = p1;

	if (arg2)
		*arg2 = p2;
}
void EncodeStr(std::string& c)
{
	int a[] = { 3, 5, 9, 2, 4, 7, 3 };
	for (int i = 0, j = 0; c[j]; j++, i = (i + 1) % 7){
		c[j] += a[i];
		//if (c[j] > 122) c[j] -= 90;
	}
}
void DecodeStr(std::string& c)
{
	int a[] = { 3, 5, 9, 2, 4, 7, 3 };
	for (int i = 0, j = 0; c[j]; j++, i = (i + 1) % 7){
		c[j] -= a[i];
		//if (c[j] < 32) c[j] += 90;
	}
}
char *url_encode(const char *s, int len, int *new_length)
{
	unsigned char hexchars[] = "0123456789ABCDEF";

	register int x, y;
	unsigned char *str;

	str = (unsigned char *)malloc(3 * len + 1);
	for (x = 0, y = 0; len--; x++, y++)
	{
		str[y] = (unsigned char)s[x];
		if (str[y] == ' ')
		{
			str[y] = '+';
		}
		else if ((str[y] < '0' && str[y] != '-' && str[y] != '.')
			|| (str[y] < 'A' && str[y] > '9')
			|| (str[y] > 'Z' && str[y] < 'a' && str[y] != '_')
			|| (str[y] > 'z'))
		{
			str[y++] = '%';
			str[y++] = hexchars[(unsigned char)s[x] >> 4];
			str[y] = hexchars[(unsigned char)s[x] & 15];
		}
	}
	str[y] = '\0';
	if (new_length) {
		*new_length = y;
	}

	return ((char *)str);
}
void url_encode(std::string &s)
{
	char *buf = url_encode(s.c_str(), s.length(), NULL);
	if (buf)
	{
		s = buf;
		free(buf);
	}
}

void OutString(std::string s)
{
	ofstream outfile("data.txt");

	outfile << s << std::endl;

	outfile.close();
}

void OutPos(uint32 posId)
{
	auto itr = PosMap.find(posId);
	if (itr != PosMap.end())
	{
		std::ostringstream oss;

		oss << "PosId " << posId << ":" << itr->second.map << "," << itr->second.x << "," << itr->second.y << "," << itr->second.z << "," << itr->second.o;

		ofstream outfile("data.txt");

		outfile << oss.str() << std::endl;

		outfile.close();
	}
	
}

void PrintStr(std::string s)
{
	sWorld->SendServerMessage(SERVER_MSG_STRING, s.c_str());
}

void CommonFunc::TeleToTarget(Player* player, Player* target)
{
	if (!target)
		return;

	Map* map = target->GetMap();

	if (!map)
		return;

	if (map->IsBattlegroundOrArena())
	{
		if (!player->GetMap()->IsBattlegroundOrArena())
			player->SetEntryPoint();

		player->SetBattlegroundId(target->GetBattlegroundId(), target->GetBattlegroundTypeId(), PLAYER_MAX_BATTLEGROUND_QUEUES, false, false, TEAM_NEUTRAL);
	}
	else if (map->IsDungeon())
	{
		// if the GM is bound to another instance, he will not be bound to another one
		InstancePlayerBind* bind = sInstanceSaveMgr->PlayerGetBoundInstance(player->GetGUIDLow(), target->GetMapId(), target->GetDifficulty(map->IsRaid()));
		if (!bind)
			if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(target->GetInstanceId()))
				sInstanceSaveMgr->PlayerBindToInstance(player->GetGUIDLow(), save, !save->CanReset(), player);

		if (map->IsRaid())
			player->SetRaidDifficulty(target->GetRaidDifficulty());
		else
			player->SetDungeonDifficulty(target->GetDungeonDifficulty());
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

	player->TeleportTo(target->GetMapId(), target->GetPositionX(), target->GetPositionY(), target->GetPositionZ() + 0.25f, player->GetOrientation(), TELE_TO_GM_MODE);
	player->SetPhaseMask(target->GetPhaseMask() | 1, false);
}

void CommonFunc::TeleToTarget(Player* player, Player* target, float x, float y, float z)
{
	if (!target)
		return;

	Map* map = target->GetMap();

	if (!map)
		return;

	if (map->IsBattlegroundOrArena())
	{
		if (!player->GetMap()->IsBattlegroundOrArena())
			player->SetEntryPoint();

		player->SetBattlegroundId(target->GetBattlegroundId(), target->GetBattlegroundTypeId(), PLAYER_MAX_BATTLEGROUND_QUEUES, false, false, TEAM_NEUTRAL);
	}
	else if (map->IsDungeon())
	{
		// if the GM is bound to another instance, he will not be bound to another one
		InstancePlayerBind* bind = sInstanceSaveMgr->PlayerGetBoundInstance(player->GetGUIDLow(), target->GetMapId(), target->GetDifficulty(map->IsRaid()));
		if (!bind)
			if (InstanceSave* save = sInstanceSaveMgr->GetInstanceSave(target->GetInstanceId()))
				sInstanceSaveMgr->PlayerBindToInstance(player->GetGUIDLow(), save, !save->CanReset(), player);

		if (map->IsRaid())
			player->SetRaidDifficulty(target->GetRaidDifficulty());
		else
			player->SetDungeonDifficulty(target->GetDungeonDifficulty());
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

	player->TeleportTo(target->GetMapId(), x, y, z + 0.25f, player->GetOrientation(), TELE_TO_GM_MODE);
	player->SetPhaseMask(target->GetPhaseMask() | 1, false);
}

std::string SecTimeString(uint64 timeInSecs, bool shortText)
{
	uint64 secs = timeInSecs % MINUTE;
	uint64 minutes = timeInSecs % HOUR / MINUTE;
	uint64 hours = timeInSecs % DAY / HOUR;
	uint64 days = timeInSecs / DAY;

	std::ostringstream ss;
	if (days)
		ss << days << (shortText ? "天" : "天");
	if (hours)
		ss << hours << (shortText ? "时" : "小时");
	if (minutes)
		ss << minutes << (shortText ? "分" : "分钟");
	if (secs || (!days && !hours && !minutes))
		ss << secs << (shortText ? "秒" : "秒钟");

	std::string str = ss.str();

	if (!shortText && !str.empty() && str[str.size() - 1] == ' ')
		str.resize(str.size() - 1);

	return str;
}

uint32 GetIpCount()
{
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
		}
	}

	return IPVec.size();
}

bool IsGCValidString(std::string s, std::string description, WorldSession* session, WorldPacket& recvData)
{
	if (s.empty())
		return true;

	int num = s.size();
	int i = 0;
	while (i < num)
	{
		int size = 1;

		if (s[i] & 0x80)
		{
			char temp = s[i];
			temp <<= 1;
			do{
				temp <<= 1;
				++size;
			} while (temp & 0x80);
		}

		std::string subs;
		subs = s.substr(i, size);

		switch (size)
		{
		case 1:
			//if (subs[0] < 32 || subs[0] > 126)
			//{
			//	if (session)
			//	{
			//		sLog->outString("IP:%s AccountId:%u CharGUID:%u Opcode:%s SizeType:%u", session->GetRemoteAddress().c_str(), session->GetAccountId(), session->GetGuidLow(), description, size);
			//		sLog->outChar("IP:%s AccountId:%u CharGUID:%u Opcode:%s SizeType:%u", session->GetRemoteAddress().c_str(), session->GetAccountId(), session->GetGuidLow(), description, size);
			//	}
			//	recvData.rfinish();
			//	return false;
			//}
			break;
		case 3:
		{
			uint32 x = ((subs[0] & 0xFF) << 16) | ((subs[1] & 0xFF) << 8) | (subs[2] & 0xFF);

			bool IsMark = false;

			switch (x)
			{
			case 0xE38082://。
			case 0xE38090://【】
			case 0xE38091:
			case 0xEFBC8C://，
			case 0xE2809C://“”
			case 0xE2809D:
			case 0xEfBC9B://；
			case 0xE38081://、
			case 0xEFBC9F://？
			case 0xEFBC88://（）
			case 0xEFBC89:
			case 0xefbc87://＇
			case 0xe288b6://∶
			case 0xefbd9e://～
			case 0xe38085://々
			case 0xefb99f://﹟
			case 0xefbd80://｀
			case 0xefb9a9://﹩
			case 0xe3808e://『』
			case 0xe3808f:
			case 0xe38096://〖〗
			case 0xe38097:
			case 0xefbcbb://［］
			case 0xefbcbd:
			case 0xe3808a://《》
			case 0xe3808b:
			case 0xefb99b://﹛﹜
			case 0xefb99c:
			case 0xe3808c://「」
			case 0xe3808d:
			case 0xe285a0://Ⅰ-Ⅹ
			case 0xe285a1:
			case 0xe285a2:
			case 0xe285a3:
			case 0xe285a4:
			case 0xe285a5:
			case 0xe285a6:
			case 0xe285a7:
			case 0xe285a8:
			case 0xe285a9:
			case 0xe2978b://○
			case 0xe29787://◇
			case 0xe296a1://□
			case 0xe296b3://△
			case 0xe296bd://▽
			case 0xe29886://☆
			case 0xe2978f://●
			case 0xe29786://◆
			case 0xe296b2://▲
			case 0xe296bc://▼
			case 0xe29980://♀
			case 0xe29982://♂
			case 0xe2889a://?√
			case 0xe2978e://◎
			case 0xe28a99://⊙
			case 0xe28690://←→
			case 0xe28692://
				IsMark = true;
			}

			if (!IsMark && (x < 0xE4B880 || x > 0xE9BEA5))
			{
				if (session)
                    sLog->outString("Possiable String Hack --> IP:%s AccountId:%u CharGUID:%u Opcode:%s SizeType:%u", session->GetRemoteAddress().c_str(), session->GetAccountId(), session->GetGuidLow(), description.c_str(), size);
				recvData.rfinish();
				return false;
			}
		}
		break;
		default:
			if (session)
                sLog->outString("Possiable String Hack --> IP:%s AccountId:%u CharGUID:%u Opcode:%s SizeType:%u", session->GetRemoteAddress().c_str(), session->GetAccountId(), session->GetGuidLow(), description.c_str(), size);
			recvData.rfinish();
			return false;
		}

		i += size;
	}

	return true;
}

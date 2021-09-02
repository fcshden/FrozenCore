#pragma execution_character_set("utf-8")
#include "GCAddon.h"
#include "../ItemMod/ItemMod.h"
#include "../Requirement/Requirement.h"
#include "../Reward/Reward.h"
#include "../DataLoader/DataLoader.h"
#include "../CommonFunc/CommonFunc.h"
#include "Object.h"
#include "../VIP/VIP.h"
#include "../HonorRank/HonorRank.h"
#include "../ExtraEquipment/ExtraEquipment.h"
#include "../String/myString.h"
#include "../Switch/Switch.h"
#include "../Transmogrification/Transmogrification.h"
#include "../Armory/Armory.h"
#include "../SignIn/SignIn.h"
#include "../Talisman/Talisman.h"
#include "../Rank/Rank.h"
#include "../SpiritPower/SpiritPower.h"
#include "../LuckDraw/LuckDraw.h"
#include "../Recovery/Recovery.h"
#include "../StatPoints/StatPoints.h"
#include "../Market/Market.h"
#include "../AntiFarm/AntiFarm.h"
#include "../Faction/Faction.h"
#include "../Reincarnation/Reincarnation.h"
#include "../UI/Rune/Rune.h"
#include "../UI/BlackMarket/BlackMarket.h"
#include "../GS/GS.h"
#include "../SoulStone/SoulStone.h"
#include "../SoulStoneEx/SoulStoneEx.h"

void GCAddon::Load()
{
}

std::string GCAddon::SplitStr(std::string msg, uint32 index)
{
	std::string::size_type idx = msg.find(" ");

	if (idx != std::string::npos)
	{
		std::vector<std::string> vec = sCF->SplitStr(msg, " ");

		if (index >= vec.size())
			index = 0;

		return vec[index];
	}
	else
		return msg;
}

bool IsOpcode(std::string opcode, std::string _opcode)
{
	return strcmp(opcode.c_str(), _opcode.c_str()) == 0;
}

bool GCAddon::OnRecv(Player* player, std::string msg)
{
	stripLineInvisibleChars(msg);
	std::string opcode = SplitStr(msg, 0);

	if (IsOpcode(opcode, "GC_C_LUCKDRAW_V3"))
	{
		uint32 action = atoi(SplitStr(msg, 1).c_str());

		if (action != 1 && action != 10)
			return true;

		uint32 reqId = 0;

		if (action == 1)
			reqId = atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 1).c_str());
		else
			reqId = atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 2).c_str());

		if (action == 1)
		{
			if (!sReq->Check(player, reqId))
				return true;

			if (player->LuckDrawTotalCount == 0)
			{
				player->LuckDrawTotalCount = 1;
				player->LuckDrawCount = 1;
				player->LuckDrawTimer = 0;
			}
			else
				return true;

			sReq->Des(player, reqId);
		}
		else if (action == 10)
		{
			if (!sReq->Check(player, reqId))
				return true;

			if (player->LuckDrawTotalCount == 0)
			{
				player->LuckDrawTotalCount = 10;
				player->LuckDrawCount = 10;
				player->LuckDrawTimer = 0;
			}
			else
				return true;

			sReq->Des(player, reqId);
		}

		sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW_V3", "START");

		return true;
	}
	else if (IsOpcode(opcode, "GC_C_RECOVERY"))
	{
		uint32 action = atoi(SplitStr(msg, 1).c_str());
		uint32 id = atoi(SplitStr(msg, 2).c_str());

		if (action == 0)
			sRecovery->SendCategoryMsg(player, id);
		else if (action == 1)
			sRecovery->Action(player, id);
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_STATPOINTS"))
	{
        //sLog->outString("op = %s", msg.c_str());
		uint32 action = atoi(SplitStr(msg, 1).c_str());
		uint32 id = atoi(SplitStr(msg, 2).c_str());

		if (action == 0)
			sStatPoints->Ins(player, id);
		else if (action == 1)
			sStatPoints->Des(player, id);
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_ANTIFARM"))
	{
        if (player->AntiFarmCount == 0)
        {
            sAntiFarm->Action(player, AF_CHECK_KILL);
            return true;
        }
        uint32 num = atoi(SplitStr(msg, 1).c_str());
		sAntiFarm->DoCheck(player, num);
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_RELOAD"))
	{
		sGCAddon->SendAllData(player);
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_CHECK_REQ_POP"))
	{
		uint32 reqId = atoi(SplitStr(msg, 1).c_str());
	
		SendReqCheck(player, reqId, true);
	
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_CHECK_REQ_PANEL"))
	{
		uint32 reqId = atoi(SplitStr(msg, 1).c_str());
	
		SendReqCheck(player, reqId, false);
	
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_BUY_RUNE"))
	{
		uint32 page = atoi(SplitStr(msg, 1).c_str());
		uint32 id = atoi(SplitStr(msg, 2).c_str());
		sRune->Update(player, page, id);
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_BUY_BLACKMARKET"))
	{
		uint32 id = atoi(SplitStr(msg, 1).c_str());
		sBlackMarket->Update(player, id);
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_BUY_LUCKDRAW"))
	{
		uint32 count = atoi(SplitStr(msg, 1).c_str());
		
		if (count != 1 && count != 10)
		{
			ChatHandler(player->GetSession()).PSendSysMessage("错误的抽奖次数");
			sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW_STOP", "");
			return true;
		}
			
		uint32 reqId = 0;
	
		if (count == 1)
			reqId = atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 1).c_str());
		else
			reqId = atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 2).c_str());
	
		if (sReq->Check(player, reqId))
		{
			sReq->Des(player, reqId);
			player->UI_LuckDrawCount = count;
			player->UI_LuckDrawUpdateTimer = 100;
			player->UI_LuckDrawRewCount = 0;
		}else
			sGCAddon->SendPacketTo(player, "GC_S_LUCKDRAW_STOP", "");
	
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_TRANSMOG"))
	{
		SendTransMogData(player);
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_BUY_TRANSMOG"))
	{
		uint32 id = atoi(SplitStr(msg, 1).c_str());
		uint32 fakeEntry = atoll(SplitStr(msg, 2).c_str());
	
		EquipmentSlots slot = EQUIPMENT_SLOT_END;
	
		switch (id)
		{
		case 1:slot = EQUIPMENT_SLOT_HEAD; break;
		case 2:slot = EQUIPMENT_SLOT_SHOULDERS; break;
		case 3:slot = EQUIPMENT_SLOT_BODY; break;
		case 4:slot = EQUIPMENT_SLOT_CHEST; break;
		case 5:slot = EQUIPMENT_SLOT_WAIST; break;
		case 6:slot = EQUIPMENT_SLOT_LEGS; break;
		case 7:slot = EQUIPMENT_SLOT_FEET; break;
		case 8:slot = EQUIPMENT_SLOT_WRISTS; break;
		case 9:slot = EQUIPMENT_SLOT_HANDS; break;
		case 10:slot = EQUIPMENT_SLOT_BACK; break;
		case 11:slot = EQUIPMENT_SLOT_MAINHAND; break;
		case 12:slot = EQUIPMENT_SLOT_OFFHAND; break;
		case 13:slot = EQUIPMENT_SLOT_RANGED; break;
		case 14:slot = EQUIPMENT_SLOT_TABARD; break;
		default:return true;
		}
	
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
		{
			if (ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(fakeEntry))
			{
				if (sTransmogrification->PresetTransmog(player, item, fakeEntry, slot))
					SendTransMogData(player);
				else
					ChatHandler(player->GetSession()).PSendSysMessage("幻化失败，相关部位不能幻化此物品");
			}
		}
	
		return true;
	}
	else if (IsOpcode(opcode, "GC_C_TALISMAN"))
	{
		uint32 action = atoi(SplitStr(msg, 1).c_str());

		if (action == 0)
		{
			uint32 ID = atoi(SplitStr(msg, 2).c_str());
			uint32 entry = atoi(SplitStr(msg, 3).c_str());
			sTalisman->EquipTalisman(player, ID, entry);
		}
		else if (action == 1)
			sTalisman->SendPacket(player);
	}
	else if (IsOpcode(opcode, "GC_TRANS_BUY"))
	{
		int32 i = atoi(SplitStr(msg, 1).c_str());

		uint32 entry = abs(i);

		ItemTemplate const* pProto = sObjectMgr->GetItemTemplate(entry);
		if (!pProto)
		{
			player->GetSession()->SendNotification("该物品不存在");
			return true;
		}

		uint32 reqId = GetTransReqId(entry);

		if (!sReq->Check(player, reqId))
			return true;

		EquipmentSlots slot = EQUIPMENT_SLOT_END;

		switch (pProto->InventoryType)
		{
		case INVTYPE_HEAD:
			slot = EQUIPMENT_SLOT_HEAD;
			break;
		case INVTYPE_SHOULDERS:
			slot = EQUIPMENT_SLOT_SHOULDERS;
			break;
		case INVTYPE_BODY:
			slot = EQUIPMENT_SLOT_BODY;
			break;
		case INVTYPE_CHEST:
		case INVTYPE_ROBE:
			slot = EQUIPMENT_SLOT_CHEST;
			break;
		case INVTYPE_WAIST:
			slot = EQUIPMENT_SLOT_WAIST;
			break;
		case INVTYPE_LEGS:
			slot = EQUIPMENT_SLOT_LEGS;
			break;
		case INVTYPE_FEET:
			slot = EQUIPMENT_SLOT_FEET;
			break;
		case INVTYPE_WRISTS:
			slot = EQUIPMENT_SLOT_WRISTS;
			break;
		case INVTYPE_HANDS:
			slot = EQUIPMENT_SLOT_HANDS;
			break;
		case INVTYPE_CLOAK:
			slot = EQUIPMENT_SLOT_BACK;
			break;
		case INVTYPE_WEAPON:
		case INVTYPE_WEAPONMAINHAND:
		case INVTYPE_2HWEAPON:
			slot = EQUIPMENT_SLOT_MAINHAND;
			break;
		case INVTYPE_WEAPONOFFHAND:
		case INVTYPE_SHIELD:
			slot = EQUIPMENT_SLOT_OFFHAND;
			break;
		case INVTYPE_RANGEDRIGHT:
		case INVTYPE_RANGED:
			slot = EQUIPMENT_SLOT_OFFHAND;
			break;
		default:
			break;
		}

		if (slot == EQUIPMENT_SLOT_END)
		{
			player->GetSession()->SendNotification("该物品不能用于幻化");
			return true;
		}

		if (entry > 56806 && !player->HasItemCount(entry, 1, true))
		{
			std::ostringstream oss;
			oss << "请先购买" << sCF->GetItemLink(entry);
			player->GetSession()->SendNotification(oss.str().c_str());
			return true;
		}

		if (i < 0)
			slot = EQUIPMENT_SLOT_OFFHAND;

		if (Item* itemTransmogrified = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
		{
			if (sTransmogrification->PresetTransmog(player, itemTransmogrified, entry, slot))
			{
				player->GetSession()->SendAreaTriggerMessage("幻化成功");
				sReq->Des(player, reqId);
			}
			else
				player->GetSession()->SendNotification("目标部位不允许幻化成该物品，幻化失败");
		}
		else
			player->GetSession()->SendNotification("该部位未装备物品，幻化失败");

		return true;
	}else if (IsOpcode(opcode, "GC_C_ITEMENTRY"))
	{
		sGCAddon->SendItemEntryData(player, atoi(SplitStr(msg, 1).c_str()));
		return true;
	}
	else if (IsOpcode(opcode, "SSC_ITEM_TO_SLOT"))
	{
		sLog->outString("收到魂玉操作码5");
		sLog->outString(msg.c_str());
		std::string str = SplitStr(msg, 1);
		std::vector<std::string> vec = sSoulStone->split(str, "#");

		uint32 page = atoi(vec[0].c_str());
		uint32 slot = atoi(vec[1].c_str());
		uint32 item = atoi(vec[2].c_str());
		sLog->outString("page = %s, slot = %d, item = %d", vec[0].c_str(), slot, item);
		sSoulStone->CanInSetToSlot(player, page, slot, item, str);
	}
	else if (IsOpcode(opcode, "SSC_REMOVE_SLOT_ITEM"))
	{
		sLog->outString("收到魂玉操作码4");
		std::string str = SplitStr(msg, 1);
		std::vector<std::string> vec = sSoulStone->split(str, "#");

		uint32 page = atoi(vec[0].c_str());
		uint32 slot = atoi(vec[1].c_str());

		sSoulStone->CanRemoveSlot(player, page, slot, str);
	}
	else if (IsOpcode(opcode, "SSC_LIMIT"))
	{
		if (SplitStr(msg, 1) == "VAL")
		{
			sLog->outString("收到魂玉操作码1");
			sSoulStone->SendMutualData(player);
		}
	}
	else if (IsOpcode(opcode, "SSC_ACTI"))
	{
		if (SplitStr(msg, 1) == "ISOK")
		{
			sLog->outString("收到魂玉操作码2");
			sSoulStone->SendAllActiData(player);
		}
	}
	else if (IsOpcode(opcode, "SSC_BUY_PAGE"))
	{
		sLog->outString("收到魂玉操作码3");
		std::string str = SplitStr(msg, 1);
		uint32 page = atoi(str.c_str());

		if (!sSoulStone->IsBuyPageReq(player, page))
		{
			return false;
		}
	}
	else if (IsOpcode(opcode, "SSCEX_ITEM_TO_SLOT"))
	{
		sLog->outString("收到魂玉扩展操作码5");
		sLog->outString(msg.c_str());
		std::string str = SplitStr(msg, 1);
		std::vector<std::string> vec = sSoulStoneEx->split(str, "#");

		uint32 page = atoi(vec[0].c_str());
		uint32 slot = atoi(vec[1].c_str());
		uint32 item = atoi(vec[2].c_str());
		sLog->outString("page = %s, slot = %d, item = %d", vec[0].c_str(), slot, item);
		sSoulStoneEx->CanInSetToSlot(player, page, slot, item, str);
	}
	else if (IsOpcode(opcode, "SSCEX_REMOVE_SLOT_ITEM"))
	{
		sLog->outString("收到魂玉扩展操作码4");
		std::string str = SplitStr(msg, 1);
		std::vector<std::string> vec = sSoulStoneEx->split(str, "#");

		uint32 page = atoi(vec[0].c_str());
		uint32 slot = atoi(vec[1].c_str());

		sSoulStoneEx->CanRemoveSlot(player, page, slot, str);
	}
	else if (IsOpcode(opcode, "SSCEX_LIMIT"))
	{
		if (SplitStr(msg, 1) == "VAL")
		{
			sLog->outString("收到魂玉扩展操作码1");
			sSoulStoneEx->SendMutualData(player);
		}
	}
	else if (IsOpcode(opcode, "SSCEX_ACTI"))
	{
		if (SplitStr(msg, 1) == "ISOK")
		{
			sLog->outString("收到魂玉扩展操作码2");
			sSoulStoneEx->SendAllActiData(player);
		}
	}
	else if (IsOpcode(opcode, "SSCEX_BUY_PAGE"))
	{
		sLog->outString("收到魂玉扩展操作码3");
		std::string str = SplitStr(msg, 1);
		uint32 page = atoi(str.c_str());

		if (!sSoulStoneEx->IsBuyPageReq(player, page))
		{
			return false;
		}
	}
	return false;
}

std::string GCAddon::GetStrByIndex(uint32 index, std::string buff)
{
	std::string::size_type idx = buff.find("#");

	if (idx != std::string::npos)
	{
		std::vector<std::string> vec = sCF->SplitStr(buff, "#");

		if (index > vec.size())
			return vec[0];
		else
			return vec[index - 1];
	}
	else
		return buff;
}

uint32 GCAddon::GetItemEntry(std::string itemLink)
{
	std::string::size_type idx = itemLink.find(":");

	if (idx != std::string::npos)
	{
		std::vector<std::string> vec = sCF->SplitStr(itemLink, ":");

		if (vec.size() > 2)
			return atoi(vec[1].c_str());
	}

	return 0;
}

std::string GCAddon::GetItemLink(Item* item, WorldSession* session)
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

std::string GCAddon::GetItemIcon(uint32 entry)
{
	//if(ItemEntry const* dbcitem = sItemStore.LookupEntry(entry))
	//	return "";

	const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
	const ItemDisplayInfoEntry* dispInfo = NULL;
	if (temp)
	{
		dispInfo = sItemDisplayInfoStore.LookupEntry(temp->DisplayInfoID);
		if (dispInfo)
			return  dispInfo->inventoryIcon;
	}

	return "";
}

std::string GCAddon::GetReqString(Player* player, uint32 reqId)
{
	return "";
}

std::string GCAddon::GetRewString(Player* player, uint32 rewId)
{
	std::ostringstream oss;

	std::unordered_map<uint32, RewTemplate>::iterator itr = RewMap.find(rewId);
	if (itr != RewMap.end())
	{
		if (itr->second.hrPoints > 0)
			oss << sSwitch->GetValue(ST_BG_QUEQUE_INTERVALS) << "-" << GetItemIcon(sSwitch->GetValue(ST_BG_QUEQUE_INTERVALS)) << "-" << itr->second.hrPoints << ":";

		if (itr->second.arenaPoints > 0)
			oss << sSwitch->GetValue(ST_TALISMAN) << "-" << GetItemIcon(sSwitch->GetValue(ST_TALISMAN)) << "-" << itr->second.arenaPoints << ":";


		if (itr->second.tokenCount > 0)
			oss << sSwitch->GetValue(ST_TOKEN_ID) << "-" << GetItemIcon(sSwitch->GetValue(ST_TOKEN_ID)) << "-" << itr->second.tokenCount << ":";

		if (itr->second.xp > 0)
			oss << sSwitch->GetValue(ST_CF_GROUP) << "-" << GetItemIcon(sSwitch->GetValue(ST_CF_GROUP)) << "-" << itr->second.xp << ":";

		if (itr->second.goldCount > 0)
			oss << sSwitch->GetValue(ST_BG_FIXTIME_ENABLE) << "-" << GetItemIcon(sSwitch->GetValue(ST_BG_FIXTIME_ENABLE)) << "-" << itr->second.goldCount / GOLD << ":";
		
		for (auto it = itr->second.ItemDataVec.begin(); it != itr->second.ItemDataVec.end(); it++)
			oss << it->itemId << "-" << GetItemIcon(it->itemId) << "-" << it->itemCount << ":";
	}

	return oss.str();
}

uint32 GCAddon::GetTransReqId(uint32 itemId)
{
	std::unordered_map<uint32, uint32>::iterator iter = BuyTransItemMap.find(itemId);
	if (iter != BuyTransItemMap.end())
		return iter->second;

	iter = BuyTransItemMap.find(0);
	if (iter != BuyTransItemMap.end())
		return iter->second;

	return 0;
}

//#include "mail.h"
// subject 邮件标题
// text		邮件内容
//guid		玩家guid
//itemEntry 物品ID
//itemCount 物品数量

void SendMail(std::string subject, std::string text,uint32 guid,uint32 itemEntry,uint32 itemCount)
{

	MailSender sender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM);

	SQLTransaction trans = CharacterDatabase.BeginTransaction();

	MailDraft draft(subject, text);

	if (Item* item = Item::CreateItem(itemEntry, itemCount))
	{
		item->SaveToDB(trans);
		draft.AddItem(item);
	}

	draft.SendMailTo(trans, MailReceiver(guid), sender);

	CharacterDatabase.CommitTransaction(trans);
}

void SendMail(std::string subject, std::string text, std::string name, uint32 itemEntry, uint32 itemCount)
{

	uint32 guid = sWorld->GetGlobalPlayerGUID(name);

	MailSender sender(MAIL_NORMAL, 0, MAIL_STATIONERY_GM);

	SQLTransaction trans = CharacterDatabase.BeginTransaction();

	MailDraft draft(subject, text);

	if (Item* item = Item::CreateItem(itemEntry, itemCount))
	{
		item->SaveToDB(trans);
		draft.AddItem(item);
	}

	draft.SendMailTo(trans, MailReceiver(guid), sender);

	CharacterDatabase.CommitTransaction(trans);
}

class GCAddonPlayerScript : PlayerScript
{
public:
	GCAddonPlayerScript() : PlayerScript("GCAddonPlayerScript") {}

	void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 /*newArea*/) override
	{
		sGCAddon->SendCharData(player);
	}
};

void AddSC_GCAddon()
{
	new GCAddonPlayerScript();
}

void GCAddon::SendAllData(Player* player)
{
	SendDBData(player);
	SendCharData(player);
	SendTokenData(player);
	sTalisman->SendTalisManValue(player);
	sRank->SendPacket(player);
	sSpiritPower->SendPacket(player);
	sGS->SendVisableGSData(player, true);
}

void GCAddon::SendDBData(Player* player)
{
	std::ostringstream oss;
	oss << atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 1).c_str()) << " ";
	oss << atoi(sSwitch->GetFlagByIndex(ST_LUCKDRAW, 2).c_str());
	SendPacketTo(player, "GC_S_OTHER_DATA", oss.str());

	SendVIPData(player);
	SendHRData(player);
	SendFactionData(player);
	SendReincarnationData(player);
	SendRankData(player);
	SendReqData(player);
	SendRewData(player);
	SendEnchantData(player);
	sRune->SendData(player);
	sBlackMarket->SendData(player);
	sLuckDraw->SendData(player);
	sGS->SendSpellGSData(player);
	SendItemDayLimitData(player);
}

void GCAddon::SendReqData(Player* player)
{
	for (auto iter = ReqMap.begin(); iter != ReqMap.end(); iter++)
	{
		std::ostringstream oss;

		oss << iter->first << "#";
		oss << iter->second.meetLevel << "#";
		oss << iter->second.meetVipLevel << "#";
		oss << iter->second.meetHRRank << "#";
		oss << iter->second.meetFaction << "#";
		oss << iter->second.meetRankLevel << "#";
		oss << iter->second.reincarnation << "#";
		oss << iter->second.meetAchievementPoints << "#";
		oss << iter->second.desGoldCount << "#";
		oss << iter->second.desTokenCount << "#";
		oss << iter->second.desXp << "#";
		oss << iter->second.desHRPoints << "#";
		oss << iter->second.desArenaPoints << "#";
		oss << iter->second.desSpiritPower << "#";

		for (size_t i = 0; i < 10; i++)
		{
			uint32 entry = iter->second.desItem[i];
			uint32 count = iter->second.desItemCount[i];

			oss << entry << "#";
			oss << count << "#";
		}

		if (iter->second.MapDataVec.empty())
			oss << "0";
		else
		{
			for (auto i = iter->second.MapDataVec.begin(); i != iter->second.MapDataVec.end(); i++)
			{
				std::string name = GetMapNameById(i->map);
				std::string zone = GetZoneNameById(i->zone);
				if (!zone.empty())
					name += "/" + zone;
				std::string area = GetAreaNameById(i->area);
				if (!zone.empty())
					name += "/" + area;
				oss << name << ",";
			}
		}
		oss << "#";
		
		if (iter->second.SpellDataVec.empty())
			oss << "0";
		else
		{
			for (auto i = iter->second.SpellDataVec.begin(); i != iter->second.SpellDataVec.end(); i++)
			{
				int32 spell = *i;

				if (SpellInfo const*  spellInfo = sSpellMgr->GetSpellInfo(abs(spell)))
				{
					if (spell > 0)
						oss << 1 << "*" << std::string(spellInfo->SpellName[4]) << ",";
					else
						oss << -1 << "*" << std::string(spellInfo->SpellName[4]) << ",";
				}
					
			}
		}
		oss << "#";

		if (iter->second.QuestDataVec.empty())
			oss << "0";
		else
		{
			for (auto i = iter->second.QuestDataVec.begin(); i != iter->second.QuestDataVec.end(); i++)
			{
				int32 quest = *i;

				if (Quest const* questProto = sObjectMgr->GetQuestTemplate(abs(quest)))
				{
					if (quest > 0)
						oss << 1 << "*" << questProto->GetTitle() << "$";
					else
						oss << -1 << "*" << questProto->GetTitle() << "$";
				}
					
			}
		}
		oss << "#";

		if (iter->second.AchieveDataVec.empty())
			oss << "0";
		else
		{
			for (auto i = iter->second.AchieveDataVec.begin(); i != iter->second.AchieveDataVec.end(); i++)
			{
				uint32 entryId = *i;
				if (AchievementEntry const* achieve = sAchievementStore.LookupEntry(entryId))
					oss << std::string(achieve->name[4]) << ",";
			}
		}

		oss << "#";
		//command
		uint32 count = 0;
		for (auto itr = iter->second.CommandDataVec.begin(); itr != iter->second.CommandDataVec.end(); itr++)
		{
			if (!itr->command.empty())
			{
				oss << itr->des << "#";
				oss << itr->icon << "#";
				count++;
			}
		}
		for (size_t i = count; i < 10; i++)
		{
			oss << 0 << "#";
			oss << 0 << "#";
		}

		SendPacketTo(player, "GC_S_REQ", oss.str());
	}
}

void GCAddon::SendRewData(Player* player)
{
	for (auto iter = RewMap.begin(); iter != RewMap.end(); iter++)
	{
		std::ostringstream oss;

		oss << iter->first << "#";
		oss << iter->second.goldCount << "#";
		oss << iter->second.tokenCount << "#";
		oss << iter->second.xp << "#";
		oss << iter->second.statPoints << "#";
		oss << iter->second.hrPoints << "#"; 
		oss << iter->second.arenaPoints << "#";

		//item
		uint32 count = 0;
		for (auto itr = iter->second.ItemDataVec.begin(); itr != iter->second.ItemDataVec.end(); itr++)
		{
			if (count > 10)
				continue;

			oss << itr->itemId << "#";
			oss << itr->itemCount << "#";
			count++;
		}
		for (size_t i = count; i < 10; i++)
		{
			oss << 0 << "#";
			oss << 0 << "#";
		}
			
		//spell
		count = 0;
		for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
		{
			if (*itr < 0 || count > 10)
				continue;

			oss << *itr << "#";
			count++;
		}
		for (size_t i = count; i < 10; i++)
			oss << 0 << "#";

		//aura
		count = 0;
		for (auto itr = iter->second.SpellDataVec.begin(); itr != iter->second.SpellDataVec.end(); itr++)
		{
			if (*itr > 0 || count > 10)
				continue;

			oss << abs(*itr) << "#";
			count++;
		}
		for (size_t i = count; i < 10; i++)
			oss << 0 << "#";

		//command
		count = 0;
		for (auto itr = iter->second.CommandDataVec.begin(); itr != iter->second.CommandDataVec.end(); itr++)
		{
			if (!itr->command.empty())
			{
				oss << itr->des << "#";
				oss << itr->icon << "#";
				count++;
			}
		}
		for (size_t i = count; i < 10; i++)
		{
			oss << 0 << "#";
			oss << 0 << "#";
		}
			
		SendPacketTo(player, "GC_S_REW", oss.str());
	}
}

void GCAddon::SendVIPData(Player* player)
{
	for (auto iter = VIPVec.begin(); iter != VIPVec.end(); ++iter)
	{
		std::ostringstream oss;
		oss << iter->vipLv << " ";
		oss << iter->icon << " ";
		oss << iter->name << " ";
		oss << iter->reqId << " ";
		oss << iter->rewId;
		SendPacketTo(player, "GC_S_VIP", oss.str());
	}
}

void GCAddon::SendHRData(Player* player)
{
	for (auto iter = HRUpVec.begin(); iter != HRUpVec.end(); ++iter)
	{
		std::ostringstream oss;
		oss << iter->title << " ";
		oss << iter->reqId << " ";
		oss << iter->rewId;
		SendPacketTo(player, "GC_S_HR", oss.str());
	}
}

void GCAddon::SendFactionData(Player* player)
{
	std::ostringstream oss;
	for (auto iter = FactionDataMap.begin(); iter != FactionDataMap.end(); ++iter)
		oss << iter->first << "-" << iter->second.name << " ";
	SendPacketTo(player, "GC_S_FACTION", oss.str());
}

void GCAddon::SendReincarnationData(Player* player)
{
	std::ostringstream oss;
	for (auto iter = ReincarnationMap.begin(); iter != ReincarnationMap.end(); ++iter)
		oss << iter->first << "-" << iter->second.gossipText << " ";
	SendPacketTo(player, "GC_S_REINCARNATION", oss.str());
}

void GCAddon::SendRankData(Player* player)
{
	std::ostringstream oss;
	for (auto iter = RankDataMap.begin(); iter != RankDataMap.end(); ++iter)
		oss << iter->first << "-" << iter->second.name << " ";
	SendPacketTo(player, "GC_S_RANK", oss.str());
}

void GCAddon::SendEnchantData(Player* player)
{
	for (auto itr = GCAddonEnchantGroupVec.begin(); itr != GCAddonEnchantGroupVec.end(); itr++)
	{
		SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(*itr);

		if (!info)
			continue;

		std::ostringstream oss;
		oss << info->ID << "^";
		oss << info->description[4];
		SendPacketTo(player, "GC_S_ENCHANT", oss.str());
	}

	//主背包
	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			SendItemEnchantData(player, item);
			

	//额外三个背包
	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = player->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = player->GetItemByPos(i, j))
					SendItemEnchantData(player, item);

	//银行
	for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			SendItemEnchantData(player, item);
				
	//银行背包
	for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
		if (Bag* pBag = player->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = player->GetItemByPos(i, j))
					SendItemEnchantData(player, item);
}

void GCAddon::SendItemEnchantData(Player* player, Item* item)
{
	if (!item->IsNoPatch())
		return;

	for (size_t slot = PROP_ENCHANTMENT_SLOT_0; slot < MAX_ENCHANTMENT_SLOT; slot++)
	{
		std::ostringstream oss;
		oss << item->GetGUIDLow() << " ";
		oss << slot - 6 << " ";
		oss << item->GetEnchantmentId(EnchantmentSlot(slot)) << " ";
		SendPacketTo(player, "GC_S_ITEMGUID", oss.str());
	}
}

void GCAddon::SendItemEntryData(Player* player, uint32 entry)
{
	auto iter = UIItemEntryMap.find(entry);

	if (iter != UIItemEntryMap.end())
	{
		std::ostringstream oss;
		oss << iter->first << "^";
		oss << iter->second.des << "^";
		oss << iter->second.heroText << "^";
		oss << iter->second.daylimit << "^";
		oss << iter->second.maxGems << "^";
		oss << iter->second.exchange1 << "^";
		oss << iter->second.exchangeReqId1 << "^";
		oss << iter->second.exchange2 << "^";
		oss << iter->second.exchangeReqId2 << "^";
		oss << iter->second.unbindReqId << "^";
		oss << iter->second.useReqId << "^";
		oss << iter->second.equipReqId << "^";
		oss << iter->second.buyReqId << "^";
		oss << iter->second.sellRewId << "^";
		oss << iter->second.recoveryRewId << "^";
		oss << iter->second.gs;
		SendPacketTo(player, "GC_S_ITEMENTRY", oss.str());
	}
}

void GCAddon::SendItemDayLimitData(Player* player)
{
	for (auto itr = player->PDayLimitItemMap.begin(); itr != player->PDayLimitItemMap.end(); itr++)
	{
		std::ostringstream oss;
		oss << itr->first << " ";
		oss << itr->second;
		SendPacketTo(player, "GC_S_DAYLIMIT", oss.str());
	}
}

void GCAddon::SendCharData(Player* player)
{
	std::ostringstream oss;
	oss << player->vipLevel			<< " ";
	oss << sHR->GetHRTiteId(player) << " ";
	oss << player->faction			<< " ";
	oss << player->rankLevel		<< " ";
	oss << player->reincarnationLv	<< " ";
	oss << player->GetMapId()		<< " ";
	oss << player->GetZoneId()		<< " ";
	oss << player->GetAreaId()		<< " ";

	SendPacketTo(player, "GC_S_CHAR", oss.str());
}


void GCAddon::SendTokenData(Player* player)
{
	std::ostringstream oss;
	oss << sCF->GetTokenAmount(player);
	SendPacketTo(player, "GC_S_TOKEN", oss.str());
}

void GCAddon::SendTokenUpdateData(Player* player, int64 count, bool add)
{
	std::ostringstream oss;
	add ? oss << count : oss << -count;
	SendPacketTo(player, "GC_S_TOKEN_UPDATE", oss.str());
}

//check map achieve spell quest
void GCAddon::SendReqCheck(Player* player, uint32 req, bool pop)
{
	std::ostringstream oss;
	
	oss << req;

	oss << " ";

	if (sReq->CheckMap(player, req, false))
		oss << 1;
	else
		oss << -1;
	
	oss << " ";

	if (sReq->CheckQuest(player, req, false))
		oss << 1;
	else
		oss << -1;

	oss << " ";

	if (sReq->CheckSpell(player, req, false))
		oss << 1;
	else
		oss << -1;

	oss << " ";

	if (sReq->CheckAcheive(player, req, false))
		oss << 1;
	else
		oss << -1;

	if (pop)
		SendPacketTo(player, "GC_S_REQ_CHECK_POP", oss.str());
	else
		SendPacketTo(player, "GC_S_REQ_CHECK_PANEL", oss.str());
}

void GCAddon::SendTransMogData(Player* player)
{
	std::ostringstream oss;
	
	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HEAD))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_SHOULDERS))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BODY))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_CHEST))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WAIST))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_LEGS))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_FEET))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_WRISTS))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_HANDS))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_BACK))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_MAINHAND))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_OFFHAND))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_RANGED))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID()) << " ";
	else
		oss << 0 << "-" << 0 << " ";

	if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_TABARD))
		oss << item->GetEntry() << "-" << sTransmogrification->GetFakeEntry(item->GetGUID());
	else
		oss << 0 << "-" << 0;

	SendPacketTo(player, "GC_S_TRANSMOG", oss.str());
}

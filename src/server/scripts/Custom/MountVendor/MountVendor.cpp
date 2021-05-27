#pragma execution_character_set("utf-8")
#include "MountVendor.h"
#include "../Requirement/Requirement.h"
#include "../CommonFunc/CommonFunc.h"
#include "../DataLoader/DataLoader.h"

std::vector<MountVendorTemplate> MountVendorVec;

void MountVendor::Load()
{
	MountVendorVec.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 坐骑技能ID,购买需求模板ID,是否售卖 FROM _坐骑_预览与售卖" :
		"SELECT mountSpellId,reqId,onSale FROM _mount_vendor");
	
	if (!result) 
		return;
	do
	{
		Field* fields = result->Fetch();
		MountVendorTemplate MountVendorTemp;
		MountVendorTemp.spellId = fields[0].GetUInt32();
		MountVendorTemp.reqId = fields[1].GetUInt32();
		MountVendorTemp.onSale = fields[2].GetBool();
		MountVendorVec.push_back(MountVendorTemp);
	} while (result->NextRow());
}

class MountVendorScript : public CreatureScript
{
public:
	MountVendorScript() : CreatureScript("MountVendorScript") { }


	bool OnGossipHello(Player* player, Creature* creature) override
	{
		player->PlayerTalkClass->ClearMenus();
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "陆地坐骑", GOSSIP_SENDER_MAIN + 1, GOSSIP_ACTION_INFO_DEF);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "飞行坐骑", GOSSIP_SENDER_MAIN + 2, GOSSIP_ACTION_INFO_DEF);
		player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		player->PlayerTalkClass->ClearMenus();
		player->mountReqId = 0;
		switch (sender)
		{
		case GOSSIP_SENDER_MAIN:
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "查看", GOSSIP_SENDER_MAIN + 3, action);
			if (OnSale(action))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "购买", GOSSIP_SENDER_MAIN + 4, action);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "返回", GOSSIP_SENDER_MAIN + 5, action);
			player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
			break;
		case GOSSIP_SENDER_MAIN + 1://陆地坐骑列表
			AddMountList(player, false);
			player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
			break;
		case GOSSIP_SENDER_MAIN + 2://飞行坐骑列表
			AddMountList(player, true);
			player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
			break;
		case GOSSIP_SENDER_MAIN + 3://查看
		{
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TAXI, "查看", GOSSIP_SENDER_MAIN + 3, action);
			if (OnSale(action))
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_TRAINER, "购买", GOSSIP_SENDER_MAIN + 4, action);
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "返回", GOSSIP_SENDER_MAIN + 5, action);
			player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
			if (player->HasAura(action))
				break;
			if (player->IsMounted())
			{
				player->Dismount();
				player->RemoveAurasByType(SPELL_AURA_MOUNTED);
			}

			std::ostringstream oss;
			oss << "该坐骑效果将在" << MountTryTime << "S后去除";
			player->GetSession()->SendNotification(oss.str().c_str());
			player->AddAura(action, player);
			player->mountSpellId = action;
			player->isTryMount = true;
			player->mountTimer = 0;
			ChatHandler(player->GetSession()).PSendSysMessage(GetMountDescription(action).c_str());
		}
			break;
		case GOSSIP_SENDER_MAIN + 4://购买
		{
			player->mountReqId = GetReqId(action);
			player->mountSpellId = action;
			std::string notice = sReq->Notice(player, player->mountReqId, "购买\n", GetMountName(action));
			sCF->SendAcceptOrCancel(player, 997, notice);
		}		
			break;
		case GOSSIP_SENDER_MAIN + 5://返回
			//OnGossipHello(player, creature);
			if (IsFlyMount(action))
			{
				player->PlayerTalkClass->ClearMenus();
				AddMountList(player, true);
				player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
			}
			else
			{
				player->PlayerTalkClass->ClearMenus();
				AddMountList(player, false);
				player->SEND_GOSSIP_MENU(creature->GetEntry(), creature->GetGUID());
			}
			break;
		default:
			break;
		}

		return true;
	}

	void AddMountList(Player* player, bool flymount)
	{
		uint32 len = MountVendorVec.size();

		uint32 flycount = 0;
		uint32 noflycount = 0;

		for (size_t i = 0; i < len; i++)
		{
			SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(MountVendorVec[i].spellId);
			if (!spellInfo)
				continue;
			if (player->HasSpell(spellInfo->Id))
				continue;

			if (flymount)
			{
				if (!IsFlyMount(MountVendorVec[i].spellId))
					continue;

				flycount++;

				if (flycount < 32)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, spellInfo->SpellName[4], GOSSIP_SENDER_MAIN, MountVendorVec[i].spellId);
			}
			else
			{
				if (IsFlyMount(MountVendorVec[i].spellId))
					continue;

				noflycount++;

				if (noflycount < 32)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_VENDOR, spellInfo->SpellName[4], GOSSIP_SENDER_MAIN, MountVendorVec[i].spellId);
			}
		}
	}

	bool OnSale(uint32 spellId)
	{
		uint32 len = MountVendorVec.size();

		for (size_t i = 0; i < len; i++)
		{
			if (spellId == MountVendorVec[i].spellId)
				return MountVendorVec[i].onSale;
		}

		return false;
	}

	bool IsFlyMount(uint32 spellId)
	{
		SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
		if (!spellInfo)
			return false;

		for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
			if (spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED)
				return true;

		return false;
	}	

	uint32 GetReqId(uint32 spellId)
	{
		uint32 len = MountVendorVec.size();

		for (size_t i = 0; i < len; i++)
		{
			if (spellId == MountVendorVec[i].spellId)
				return MountVendorVec[i].reqId;
		}

		return 0;
	}

	std::string GetMountName(uint32 spellId)
	{
		SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
		if (!spellInfo)
			return "";
		return spellInfo->SpellName[4];
	}

	std::string GetMountDescription(uint32 spellId)
	{
		SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(spellId);
		if (!spellInfo)
			return "";

		uint32 speed = 0;

		for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
			if (spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MOD_INCREASE_MOUNTED_SPEED)
				speed = spellInfo->Effects[i].BasePoints + 1;

		for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
			if (spellInfo->Effects[i].ApplyAuraName == SPELL_AURA_MOD_INCREASE_MOUNTED_FLIGHT_SPEED)
				speed = spellInfo->Effects[i].BasePoints + 1;
		
		std::ostringstream oss;
		oss << "[|cFFFF1717" << spellInfo->SpellName[4] << "|r]";

		if (IsFlyMount(spellId))
			oss << " 飞行坐骑，";
		else
			oss << " 陆地坐骑，";

		oss << "移动速度提高|cFFFF1717" << speed << "|r%%。";

		std::unordered_map<uint32, MountAllowedTemplate>::iterator iter = MountAllowedMap.find(spellInfo->Id);

		if (iter != MountAllowedMap.end())
		{
			bool ban = false;

			for (size_t i = 0; i < BAN_MAP_MAX; i++)
			{
				if (MapEntry const* mapEntry = sMapStore.LookupEntry(iter->second.BanMap[i]))
				{
					ban = true;
					oss << "[|cFFFF1717" << mapEntry->name[4] << "|r]";
				}
			}
					
			if (!iter->second.indoor)
			{
				ban = true;
				oss << "[" << "|cFFFF1717室内|r" << "]";
			}
			
			if (!iter->second.instance)
			{
				ban = true;
				oss << "[" << "|cFFFF1717副本|r" << "]";
			}
				

			if (!iter->second.battleground)
			{
				ban = true;
				oss << "[" << "|cFFFF1717战场|r" << "]";
			}

			if (ban)
				oss << "无法使用这种坐骑。";
		}

		return oss.str();
	}
};

void AddSC_MountVendor()
{
	new MountVendorScript();
}

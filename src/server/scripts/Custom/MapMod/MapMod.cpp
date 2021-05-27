#pragma execution_character_set("utf-8")
#include "MapMod.h"
#include "../Requirement/Requirement.h"
#include "../EquipmentManager/EquipmentManager.h"
#include "../CommonFunc/CommonFunc.h"
#include <random>
#include <algorithm>
#include <vector>

std::vector<MapModTemplate> MapModVec;
std::unordered_map<uint32, std::unordered_map<uint8, bool>> MapItemMap;
std::vector<MapModPlayerTemplate> MapModPlayerVec;
void MapMod::Load()
{
	MapModVec.clear();
    MapItemMap.clear();
    EquipmentVec.clear();
    QueryResult result;
    if (result = WorldDatabase.PQuery(
        //		0	 1	   2	3	4	 5	  6	   7	8	9	10	 11	 12	  13   14  15	16	 17	 18		19
        "SELECT 头部,颈部,肩部,衬衣,胸部,腰部,腿部,靴子,手腕,手部,戒指,戒指,饰品,饰品,背部,主手,副手,远程,战袍,地图ID FROM _地图装备控制"))
    {
        do
        {
            Field* fields = result->Fetch();
            std::unordered_map<uint8, bool> slots;

            for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
                slots.insert(std::make_pair(i, fields[i].GetBool()));

            MapItemMap.insert(std::make_pair(fields[19].GetUInt32(), slots));

        } while (result->NextRow());
    }

	result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?

		//		0		1		2		3			4		5				6		7		8			9			10				11		12		13
		"SELECT 地图ID,地域ID,区域ID,进入需求模板ID,副本难度,怪物类型,等级,生命值,物理伤害值或倍率,法术伤害倍率,治疗效果倍率,减伤百分比,抗性值,击杀奖励模板ID,"
		//14			15				16		17		18			19			20						21				
		"击杀奖励几率,击杀是否全服提示,护甲值,副本挑战等级,攻击间隔,击杀奖励法宝值,击杀奖励自定义等级值,离开副本后是否重置,"
		//	22    23		24		25		26	 27		28			29				30		  31				32				33				34
		"掉落ID1,掉落ID2,掉落ID3,掉落ID4,掉落ID5,光环组,随机光环数,生命值倍率,队伍击杀奖励模板ID,队伍击杀奖励几率,击杀召唤物体ID,是否加载原掉落,随机技能组模板ID FROM _属性调整_地图" :

		//		0	1	2		3	4		5		6	7		8			9		10		11			12			13
		"SELECT Map,Area,Zone,ReqId,Diff,ModType,Level,Health,MeleeDmg,SpellDmgMod,HealMod,ReduceDmgPct,Resistance,KillRewId,"
		//		14		15			16		17			18			19				20				21					
		"KillRewChance,KillAnnounce,Armor,ChallengeLv,AttackTime,AddTalismanValue,AddRankValue,ResetOnLeave,"
		//	22		23		24		25			26			27		28			29			30		31					32					33				34	
		"LootId_1,LootId_2,LootId_3,LootId_4,LootId_5,AuraData,RandomAuraCount,HpMod,KillGroupRewId,KillGroupRewChance,KillRewGameObject,SrcLoot,RandSpellGroupId FROM _map");

	if (!result) 
		return;

	do
	{
		Field* fields = result->Fetch();
		MapModTemplate MapModTemp;
		MapModTemp.Map				= fields[0].GetUInt32();
		MapModTemp.Area				= fields[1].GetUInt32();
		MapModTemp.Zone				= fields[2].GetUInt32();
		MapModTemp.ReqId			= fields[3].GetUInt32();
		MapModTemp.Diff				= fields[4].GetUInt32();
		MapModTemp.ModType			= fields[5].GetUInt32();
		MapModTemp.Level			= fields[6].GetUInt8();
		MapModTemp.Health			= fields[7].GetUInt32();
		MapModTemp.MeleeDmg			= fields[8].GetFloat();
		MapModTemp.SpellDmgMod		= fields[9].GetFloat();
		MapModTemp.HealMod			= fields[10].GetFloat();
		MapModTemp.ReduceDmgPct		= fields[11].GetFloat();
		MapModTemp.Resistance		= fields[12].GetInt32();
		MapModTemp.KillRewId		= fields[13].GetUInt32();
		MapModTemp.KillRewChance	= fields[14].GetFloat();
		MapModTemp.KillAnnounce		= fields[15].GetBool();
		MapModTemp.Armor			= fields[16].GetInt32();
		MapModTemp.ChallengeLv		= fields[17].GetUInt32();
		MapModTemp.AttackTime		= fields[18].GetUInt32();
		MapModTemp.AddTalismanValue = fields[19].GetInt32();
		MapModTemp.AddRankValue		= fields[20].GetInt32();
		MapModTemp.ResetOnLeave		= fields[21].GetBool();

		for (size_t i = 0; i < MAX_CUSTOM_LOOT_COUNT; i++)
			MapModTemp.LootId[i] = fields[22 + i].GetUInt32();

		Tokenizer auraData(fields[27].GetString(), '#');
		for (Tokenizer::const_iterator itr = auraData.begin(); itr != auraData.end(); ++itr)
			if (SpellInfo const*  spellInfo = sSpellMgr->GetSpellInfo(abs(atoi(*itr))))
				MapModTemp.AuraVec.push_back(atoi(*itr));
			
		MapModTemp.RandomAuraCount = fields[28].GetUInt32();

		MapModTemp.HpMod				= fields[29].GetFloat();
		MapModTemp.KillGroupRewId		= fields[30].GetUInt32();
		MapModTemp.KillGroupRewChance	= fields[31].GetFloat();
		MapModTemp.KillRewGameObject	= fields[32].GetUInt32();
		MapModTemp.SrcLoot				= fields[33].GetBool();
		MapModTemp.RandSpellGroupId		= fields[34].GetUInt32();
		MapModVec.push_back(MapModTemp);
	} while (result->NextRow());


		//EQUIPMENT_SLOT_HEAD = 0,
		//EQUIPMENT_SLOT_NECK = 1,
		//EQUIPMENT_SLOT_SHOULDERS = 2,
		//EQUIPMENT_SLOT_BODY = 3,
		//EQUIPMENT_SLOT_CHEST = 4,
		//EQUIPMENT_SLOT_WAIST = 5,
		//EQUIPMENT_SLOT_LEGS = 6,
		//EQUIPMENT_SLOT_FEET = 7,
		//EQUIPMENT_SLOT_WRISTS = 8,
		//EQUIPMENT_SLOT_HANDS = 9,
		//EQUIPMENT_SLOT_FINGER1 = 10,
		//EQUIPMENT_SLOT_FINGER2 = 11,
		//EQUIPMENT_SLOT_TRINKET1 = 12,
		//EQUIPMENT_SLOT_TRINKET2 = 13,
		//EQUIPMENT_SLOT_BACK = 14,
		//EQUIPMENT_SLOT_MAINHAND = 15,
		//EQUIPMENT_SLOT_OFFHAND = 16,
		//EQUIPMENT_SLOT_RANGED = 17,
		//EQUIPMENT_SLOT_TABARD = 18,


	MapModPlayerVec.clear();
	if (result = WorldDatabase.PQuery("SELECT 地图ID, 副本难度, 副本挑战等级, 获得光环组 FROM _属性调整_地图_玩家"))
	{
		do
		{
			Field* fields = result->Fetch();
			MapModPlayerTemplate Temp;
			Temp.Map = fields[0].GetUInt32();
			Temp.Diff = fields[1].GetUInt8();
			Temp.ChallengeLv = fields[2].GetUInt32();

			Tokenizer data1(fields[3].GetString(), ' ');
			for (Tokenizer::const_iterator itr = data1.begin(); itr != data1.end(); ++itr)
				Temp.AuraVec.push_back(uint32(atol(*itr)));

			MapModPlayerVec.push_back(Temp);
		} while (result->NextRow());
	}
}

void MapMod::ReApplyMapAura(Player* player)
{
	for (auto itr = MapModPlayerVec.begin(); itr != MapModPlayerVec.end(); itr++)
		for (auto i = itr->AuraVec.begin(); i != itr->AuraVec.end(); i++)
			if (player->HasAura(*i))
				player->RemoveAurasDueToSpell(*i);

	for (auto itr = MapModPlayerVec.begin(); itr != MapModPlayerVec.end(); itr++)
		if (player->GetMapId() == itr->Map && player->GetMap()->challengeLv == itr->ChallengeLv && player->GetMap()->GetDifficulty() == itr->Diff)
			for (auto i = itr->AuraVec.begin(); i != itr->AuraVec.end(); i++)
				if (!player->HasAura(*i))
					player->AddAura(*i, player);
}

void MapMod::RemoveItem(Player* player)
{
	auto itr = MapItemMap.find(player->GetMapId());

	if (itr == MapItemMap.end())
		return;

	SQLTransaction trans = CharacterDatabase.BeginTransaction();

	std::list<Item*> mailItems;

	for (size_t i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
	{
		auto x = itr->second.find(i);
		if (x == itr->second.end())
			continue;

		if (x->second)
			continue;

		if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			ItemPosCountVec dest;
			uint8 msg = player->CanStoreItem(NULL_BAG, NULL_SLOT, dest, pItem, false);
			if (msg == EQUIP_ERR_OK)
			{
				player->RemoveItem(INVENTORY_SLOT_BAG_0, i, true);
				player->StoreItem(dest, pItem, true);
			}
			else
			{
				player->MoveItemFromInventory(INVENTORY_SLOT_BAG_0, i, true);
				SQLTransaction trans = CharacterDatabase.BeginTransaction();
				pItem->DeleteFromInventoryDB(trans);
				pItem->SaveToDB(trans);
				mailItems.push_back(pItem);
			}
		}
	}

	while (!mailItems.empty())
	{
		MailDraft draft("遗失的物品", "请收好你的物品");
		for (uint8 i = 0; !mailItems.empty() && i < MAX_MAIL_ITEMS; ++i)
		{
			draft.AddItem(mailItems.front());
			mailItems.pop_front();
		}

		draft.SendMailTo(trans, player, MailSender(player, MAIL_STATIONERY_GM), MAIL_CHECK_MASK_COPIED);
	}

	player->UpdateTitansGrip();

	CharacterDatabase.CommitTransaction(trans);
}

bool MapMod::CanEquipItem(Player* player, uint8 slot, uint32 itemId)
{
	if (slot >= EQUIPMENT_SLOT_END)
		return true;

	auto itr = MapItemMap.find(player->GetMapId());

	if (itr == MapItemMap.end())
		return true;

	if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
		if (proto->Map == player->GetMapId())
			return true;

	auto x = itr->second.find(slot);
	if (x->second)
		return true;

	return false;
}

bool MapMod::OnEnterMap(Player* player)
{
	uint32 map = player->GetMapId();
	uint32 diff = player->GetMap()->GetDifficulty();
	uint32 ChallengeLv = player->GetMap()->challengeLv;

	for (auto itr = MapModVec.begin(); itr != MapModVec.end(); itr++)
	{
		if (diff != itr->Diff || ChallengeLv != itr->ChallengeLv || itr->Zone != 0 || itr->Area != 0)
			continue;

		if (map != itr->Map)
			continue;

		if (sReq->Check(player, itr->ReqId))
		{
			sReq->Des(player, itr->ReqId);
			return true;
		}
		else
		{
			player->GetSession()->SendNotification("你没有资格进入该地图！");
			player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
			return false;
		}
	}

	return true;
}

bool MapMod::OnEnterZone(Player* player, uint32 zone)
{
	uint32 diff = player->GetMap()->GetDifficulty();
	uint32 ChallengeLv = player->GetMap()->challengeLv;

	for (auto itr = MapModVec.begin(); itr != MapModVec.end(); itr++)
	{
		if (diff != itr->Diff || ChallengeLv != itr->ChallengeLv || itr->Area != 0)
			continue;

		if (zone != itr->Zone)
			continue;

		if (sReq->Check(player, itr->ReqId))
		{
			sReq->Des(player, itr->ReqId);
			return true;
		}
		else
		{
			player->GetSession()->SendNotification("你没有资格进入该地域！");
			player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
			return false;
		}
	}

	return true;
}

bool MapMod::OnEnterArea(Player* player, uint32 area)
{
	uint32 diff = player->GetMap()->GetDifficulty();
	uint32 ChallengeLv = player->GetMap()->challengeLv;

	for (auto itr = MapModVec.begin(); itr != MapModVec.end(); itr++)
	{
		if (diff != itr->Diff || ChallengeLv != itr->ChallengeLv)
			continue;

		if (area != itr->Area)
			continue;

		if (sReq->Check(player, itr->ReqId))
		{
			sReq->Des(player, itr->ReqId);
			return true;
		}
		else
		{
			player->GetSession()->SendNotification("你没有资格进入该区域！");
			player->TeleportTo(player->m_homebindMapId, player->m_homebindX, player->m_homebindY, player->m_homebindZ, player->GetOrientation());
			return false;
		}
	}

	return true;
}

void MapMod::SetMod(Creature* creature)
{
	if (creature->GetEntry() == 1964 || creature->IsGuardian() || creature->IsHunterPet() || creature->IsTotem())
		return;
		
	////小动物之类
	//if (creature->IsCivilian())
	//	return;

	uint32 map	= creature->GetMapId();
	uint32 zone = creature->GetZoneId();
	uint32 area = creature->GetAreaId();
	uint32 diff = creature->GetMap()->GetDifficulty();
	uint32 ChallengeLv = creature->GetMap()->challengeLv;

	uint32 ModType = 0;

	if (creature->IsDungeonBoss() || creature->isWorldBoss())
		ModType = 1;

	uint32 len = MapModVec.size();

	for (size_t i = 0; i < len; i++)
	{
		if (ChallengeLv == MapModVec[i].ChallengeLv && diff == MapModVec[i].Diff && (map == MapModVec[i].Map && MapModVec[i].Zone == 0 && MapModVec[i].Area == 0 || map == MapModVec[i].Map && zone == MapModVec[i].Zone && MapModVec[i].Area == 0 || map == MapModVec[i].Map && zone == MapModVec[i].Zone && area == MapModVec[i].Area))
		{
			if (ModType == 0 && (MapModVec[i].ModType != 0 && MapModVec[i].ModType != 2))
				continue;

			if (ModType == 1 && (MapModVec[i].ModType != 1 && MapModVec[i].ModType != 2))
				continue;

			creature->C_Level			= MapModVec[i].Level;
			creature->C_Health			= MapModVec[i].Health;
			creature->C_HpMod			= MapModVec[i].HpMod;
			creature->C_MeleeDmg		= MapModVec[i].MeleeDmg;
			creature->C_SpellDmgMod		= MapModVec[i].SpellDmgMod;
			creature->C_HealMod			= MapModVec[i].HealMod;
			creature->C_ReduceDmgPct	= MapModVec[i].ReduceDmgPct;
			creature->C_Resistance		= MapModVec[i].Resistance;
			creature->C_SrcLoot			= MapModVec[i].SrcLoot;

			for (size_t j = 0; j < MAX_CUSTOM_LOOT_COUNT; j++)
				creature->C_LootId[j] = MapModVec[i].LootId[j];

			std::vector<uint32> RandomAuraVec;

			for (auto itr = MapModVec[i].AuraVec.begin(); itr != MapModVec[i].AuraVec.end(); itr++)
			{
				if (*itr > 0)
					creature->C_AuraVec.push_back(*itr);
				else
					RandomAuraVec.push_back(abs(*itr));
			}
			
			if (!RandomAuraVec.empty())
			{
				int32 delcount = RandomAuraVec.size() - MapModVec[i].RandomAuraCount;

				for (size_t i = 0; i < delcount; i++)
				{
					if (RandomAuraVec.empty())
						break;

                    std::default_random_engine generator{ std::random_device{}() };
                    std::shuffle(std::begin(RandomAuraVec), std::end(RandomAuraVec), generator);

					RandomAuraVec.erase(RandomAuraVec.begin());
				}
			}

			if (!RandomAuraVec.empty())
				for (size_t i = 0; i < RandomAuraVec.size(); i++)
					creature->C_AuraVec.push_back(RandomAuraVec[i]);
			
			creature->C_KillRewId			= MapModVec[i].KillRewId;
			creature->C_KillRewChance		= MapModVec[i].KillRewChance;
			creature->C_KillGroupRewId		= MapModVec[i].KillGroupRewId;
			creature->C_KillGroupRewChance	= MapModVec[i].KillGroupRewChance;
			creature->C_KillAnnounce		= MapModVec[i].KillAnnounce;
			creature->C_Armor				= MapModVec[i].Armor;
			creature->C_AttackTime			= MapModVec[i].AttackTime;
			creature->C_AddTalismanValue	= MapModVec[i].AddTalismanValue;
			creature->C_AddAddRankValue		= MapModVec[i].AddRankValue;
			creature->C_KillRewGameObject	= MapModVec[i].KillRewGameObject;
			creature->RandSpellGroupId		= MapModVec[i].RandSpellGroupId;
			break;
		}
	}
}

void MapMod::ResetInstance(Player* player, Difficulty diff, uint32 mapId)
{
	if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
	{
		std::string text = mapEntry->name[4];

		switch (diff)
		{
		case 0:
			break;
		case 1:
			mapEntry->IsRaid() ? text += "[25]" : text += "[5H]";
			break;
		case 2:
			text += "[10H]";
			break;
		case 3:
			text += "[25H]";
			break;
		default:
			break;
		}

		text += " 已被重置";

		for (uint8 i = 0; i < MAX_DIFFICULTY; ++i)
		{
			BoundInstancesMap const& m_boundInstances = sInstanceSaveMgr->PlayerGetBoundInstances(player->GetGUIDLow(), Difficulty(i));
			for (BoundInstancesMap::const_iterator itr = m_boundInstances.begin(); itr != m_boundInstances.end();)
			{
				InstanceSave* save = itr->second.save;
				MapEntry const* mapEntry = sMapStore.LookupEntry(itr->first);
				if (mapEntry && itr->first != player->GetMapId() && (!mapId || mapId == itr->first) && (diff == -1 || diff == save->GetDifficulty()))
				{
					ChatHandler(player->GetSession()).PSendSysMessage(text.c_str());
					sInstanceSaveMgr->PlayerUnbindInstance(player->GetGUIDLow(), itr->first, diff, true, player);
					itr = m_boundInstances.begin();
				}
				else
					++itr;
			}
		}
	}
}

void MapMod::PopOrTele(Player* player, uint32 triggerId)
{
    AreaTriggerTeleport const* at = sObjectMgr->GetAreaTriggerTeleport(triggerId);
	if (!at)
		return;

	uint32 mapId = at->target_mapId;
	uint32 len = MapModVec.size();
	uint32 reqId = 0;

	for (size_t i = 0; i < len; i++)
		if (mapId == MapModVec[i].Map && MapModVec[i].Zone == 0 && MapModVec[i].Area == 0)
			if (MapModVec[i].ReqId > 0)
				reqId = MapModVec[i].ReqId;

	if (reqId == 0)
		player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, at->target_Orientation, TELE_TO_NOT_LEAVE_TRANSPORT);
	else
	{
		if (MapEntry const* mapEntry = sMapStore.LookupEntry(mapId))
		{
			sCF->SendAcceptOrCancel(player, 994, sReq->Notice(player, reqId, "进入", mapEntry->name[4]));
			player->enter_map_at = at;
			player->enter_map_req = reqId;
		}
	}
}

class MapModPlayerScript : public PlayerScript
{
public:
	MapModPlayerScript() : PlayerScript("MapModPlayerScript") {}

	void OnMapChanged(Player* player) override
	{
		sMapMod->ReApplyMapAura(player);

		sMapMod->RemoveItem(player);

        if (player->GetMap())
            player->InitMapTempItems(player->GetMapId());

		//if (!sMapMod->OnEnter(player, 0))
			//return;

		sEM->Update(player, 0);

		Map* map = player->LastMap;

		if (!map || !map->IsDungeon())
			return;

		uint32 MapId = map->GetId();
		uint32 Diff = map->GetDifficulty();
		uint32 ChallengeLv = map->challengeLv;

		uint32 len = MapModVec.size();

		for (size_t i = 0; i < len; i++)
		{
			if (ChallengeLv == MapModVec[i].ChallengeLv && Diff == MapModVec[i].Diff && MapId == MapModVec[i].Map)
			{
				if (MapModVec[i].ResetOnLeave)
				{
					sMapMod->ResetInstance(player, map->GetDifficulty(), MapId);
					return;
				}
			}
		}
	}
    void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 newArea)
    {
        player->InitAreaTempItems(newArea);
    }
};

void AddSC_MapModPlayerScript()
{
	new MapModPlayerScript();
}

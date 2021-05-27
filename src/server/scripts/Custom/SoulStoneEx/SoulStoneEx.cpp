
#include "SoulStoneEx.h"
#include "Object.h"
#include "../GCAddon/GCAddon.h"
#include "../CommonFunc/CommonFunc.h"


#pragma execution_character_set("UTF-8")


void SoulStoneEx::Load()
{
    _SoulStoneExPlayerDataMap.clear();

    QueryResult result = WorldDatabase.PQuery("SELECT 玩家GUID,页面,插槽1物品ID,插槽2物品ID,插槽3物品ID,插槽4物品ID,插槽5物品ID,插槽6物品ID,是否激活页面属性 FROM 玩家魂玉扩展记录表");

    if (result)
    {
        uint32 count = 0;
        do
        {
            Field * fields = result->Fetch();

            SoulStoneExPlayerData td;

            td.guid = fields[0].GetUInt32();
            td.page = fields[1].GetUInt32();
            td.itemid1 = fields[2].GetUInt32();
            td.itemid2 = fields[3].GetUInt32();
            td.itemid3 = fields[4].GetUInt32();
            td.itemid4 = fields[5].GetUInt32();
            td.itemid5 = fields[6].GetUInt32();
            td.itemid6 = fields[7].GetUInt32();
            td.itemid7 = fields[8].GetUInt32();

            _SoulStoneExPlayerDataMap.insert({td.guid,td});
            ++count;

        } while (result->NextRow());
        sLog->outString("======加载玩家魂玉扩展记录表 %u 条数据======", count);
    }

    _SoulStoneExBuyReqDataMap.clear();

    QueryResult results = WorldDatabase.PQuery("SELECT 页面,需求物品1,数量1,需求物品2,数量2,需求物品3,数量3,需求物品4,数量4,需求物品5,数量5,VIP,军衔,转生,斗气值,金币,激活给予BUFF的物品ID FROM 魂玉页面购买需求激活表");

    if (results)
    {
        uint32 count = 0;
        do
        {
            Field * fields = results->Fetch();

            SoulStoneExBuyReqData td;

            td.page = fields[0].GetUInt32();
            td.itemid1 = fields[1].GetUInt32();
            td.val1 = fields[2].GetUInt32();
            td.itemid2 = fields[3].GetUInt32();
            td.val2 = fields[4].GetUInt32();
            td.itemid3 = fields[5].GetUInt32();
            td.val3 = fields[6].GetUInt32();
            td.itemid4 = fields[7].GetUInt32();
            td.val4 = fields[8].GetUInt32();
            td.itemid5 = fields[9].GetUInt32();
            td.val5 = fields[10].GetUInt32();
            td.vip = fields[11].GetUInt32();
            td.junxian = fields[12].GetUInt32();
            td.zhuansheng = fields[13].GetUInt32();
            td.douqival = fields[14].GetUInt32();
            td.money = fields[15].GetUInt32() * GOLD;
            td.jihuoshuxingid = fields[16].GetUInt32();

            _SoulStoneExBuyReqDataMap.insert({td.page,td});
            
            ++count;

        } while (results->NextRow());
        sLog->outString("======加载魂玉页面购买需求激活表 %u 条数据======", count);
    }

	_SoulStoneExItemTypeMap.clear();

	QueryResult resultss = WorldDatabase.PQuery("SELECT 物品ID,物品类型ID,页数 FROM 魂玉扩展类型表");

	if (resultss)
	{
		uint32 count = 0;
		do
		{
			Field * fields = resultss->Fetch();

			SoulStoneExItemType td;

			uint32 itemid = fields[0].GetUInt32();
			td.type = fields[1].GetInt32();
			td.page = fields[2].GetInt32();

			_SoulStoneExItemTypeMap.insert({ itemid, td });

			++count;

		} while (resultss->NextRow());
		sLog->outString("======加载魂玉扩展类型表 %u 条数据======", count);
	}

	QueryResult resultssss = WorldDatabase.PQuery("SELECT 参数 FROM __通用配置 WHERE ID = 101");

	if (resultssss)
	{

		Field * fields = resultssss->Fetch();
		limitpageval = uint32(atoi(fields[0].GetString().c_str()));

		sLog->outString("=======魂玉（组合激活版）初始化页面数量为：%d", limitpageval);
	}

    _SoulStoneExExDateMap.clear();

    QueryResult resultsss = WorldDatabase.PQuery("SELECT 页面,插槽1物品ID,插槽2物品ID,插槽3物品ID,插槽4物品ID,插槽5物品ID,插槽6物品ID,激活物品ID FROM 魂玉组合配置表");

    if (resultsss)
    {
        uint32 count = 0;
        do
        {
            Field * fields = resultsss->Fetch();
            SoulStoneExDate td;

            td.page = fields[0].GetUInt32();
            td.itemid1 = fields[1].GetUInt32();
            td.itemid2 = fields[2].GetUInt32();
            td.itemid3 = fields[3].GetUInt32();
            td.itemid4 = fields[4].GetUInt32();
            td.itemid5 = fields[5].GetUInt32();
            td.itemid6 = fields[6].GetUInt32();
            td.itemid7 = fields[7].GetUInt32();

            _SoulStoneExExDateMap.insert({td.page,td});

            ++count;

        } while (resultsss->NextRow());

        sLog->outString("======加载魂玉组合配置表 %u 条数据======", count);
    }
}

//重载 
void SoulStoneEx::ReLoad()
{
    Load();

    SessionMap const& sessions = sWorld->GetAllSessions();
    for (SessionMap::const_iterator it = sessions.begin(); it != sessions.end(); ++it)
        if (Player* player = it->second->GetPlayer())
        {
            SendMutualData(player);
        }
}

void SoulStoneEx::SendMutualData(Player * player)
{
    if (!player)
        return;

	uint32 p = limitpageval;

	if (!p)
	{
		sGCAddon->SendPacketTo(player, "SSSEX_LIMIT_VAL", std::to_string(p));
		return;
	}

    uint32 num = sSoulStoneEx->GetPlayerMaxPage(player);

    std::string val = std::to_string(sSoulStoneEx->GetPlayerMaxPage(player));

	sGCAddon->SendPacketTo(player, "SSSEX_LIMIT_VAL", val);

    for (uint8 i = 1; i< num + 1; ++i)
    {
        SendPlayerDataForPage(player, i);
    }
   
}

void SoulStoneEx::SendPlayerDataForPage(Player * player, uint32 page)
{
    if (!player)
        return;

    uint32 guid = player->GetGUIDLow();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            if (itr->second.page == page)
            {
                std::ostringstream ss1;
                std::ostringstream ss2;
                std::ostringstream ss3;
                std::ostringstream ss4;
                std::ostringstream ss5;
                std::ostringstream ss6;

				ss1 << itr->second.page << "#1#" << itr->second.itemid1 << "#" << sCF->GetItemLink(itr->second.itemid1);
				ss2 << itr->second.page << "#2#" << itr->second.itemid2 << "#" << sCF->GetItemLink(itr->second.itemid2);
				ss3 << itr->second.page << "#3#" << itr->second.itemid3 << "#" << sCF->GetItemLink(itr->second.itemid3);
				ss4 << itr->second.page << "#4#" << itr->second.itemid4 << "#" << sCF->GetItemLink(itr->second.itemid4);
				ss5 << itr->second.page << "#5#" << itr->second.itemid5 << "#" << sCF->GetItemLink(itr->second.itemid5);
				ss6 << itr->second.page << "#6#" << itr->second.itemid6 << "#" << sCF->GetItemLink(itr->second.itemid6);

				sGCAddon->SendPacketTo(player, "SSSEX_XQ_FG", ss1.str());
				sGCAddon->SendPacketTo(player, "SSSEX_XQ_FG", ss2.str());
				sGCAddon->SendPacketTo(player, "SSSEX_XQ_FG", ss3.str());
				sGCAddon->SendPacketTo(player, "SSSEX_XQ_FG", ss4.str());
				sGCAddon->SendPacketTo(player, "SSSEX_XQ_FG", ss5.str());
				sGCAddon->SendPacketTo(player, "SSSEX_XQ_FG", ss6.str());
            }
        }
    }
}

void SoulStoneEx::SavePlayerAllDate(Player * player)
{
    if (!player)
        return;

    uint32 guid = player->GetGUIDLow();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {

			if (itr->second.page == 0)
				continue;

            WorldDatabase.PExecute("REPLACE INTO 玩家魂玉扩展记录表(玩家GUID,页面,插槽1物品ID,插槽2物品ID,插槽3物品ID,插槽4物品ID,插槽5物品ID,插槽6物品ID,是否激活页面属性)VALUES(%u,%u,%u,%u,%u,%u,%u,%u,%u)", itr->second.guid, itr->second.page, itr->second.itemid1, itr->second.itemid2, itr->second.itemid3, itr->second.itemid4, itr->second.itemid5, itr->second.itemid6, itr->second.itemid7);
        }
    }
}

void SoulStoneEx::CreatePlayerDate(Player * player)
{
    if (!player)
        return;

	for (uint32 i = 1; i < limitpageval + 1; ++i)
    {
        SoulStoneExPlayerData td;
        td.guid = player->GetGUIDLow();
        td.page = i;
        td.itemid1 = 0;
        td.itemid2 = 0;
        td.itemid3 = 0;
        td.itemid4 = 0;
        td.itemid5 = 0;
        td.itemid6 = 0;
        td.itemid7 = 0;

        _SoulStoneExPlayerDataMap.insert({ td.guid,td });
    }

    SavePlayerAllDate(player);
}

void SoulStoneEx::SavePlayerSlotToSTL(Player * player, uint32 page, uint32 slot, uint32 itemid)
{
    if (!player)
        return;

    if (slot > 6)
        return;

    uint32 guid = player->GetGUIDLow();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            if (itr->second.page == page)
            {
                switch (slot)
                {
                case 1:
                    itr->second.itemid1 = itemid;
                    return;
                case 2:
                    itr->second.itemid2 = itemid;
                    return;
                case 3:
                    itr->second.itemid3 = itemid;
                    return;
                case 4:
                    itr->second.itemid4 = itemid;
                    return;
                case 5:
                    itr->second.itemid5 = itemid;
                    return;
                case 6:
                    itr->second.itemid6 = itemid;
                    return;
                default:
                    return;
                }
            }
        }
    }

    //SavePlayerSlotToBase(player, page, slot,itemid);
    //SavePlayerAllDate(player);
}

void SoulStoneEx::SavePlayerSlotToBase(Player * player, uint32 page, uint32 slot, uint32 itemid)
{
    if (!player)
        return;

    if (slot > 6)
        return;

	if (page == 0)
		return;

    uint32 guid = player->GetGUIDLow();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            if (itr->second.page == page)
            {
                switch (slot)
                {
                case 1:
                    WorldDatabase.PExecute("REPLACE INTO 玩家魂玉扩展记录表(玩家GUID,页面,插槽1物品ID)VALUES(%u,%u,%u)", guid, page, itemid);
                    return;
                case 2:
                    WorldDatabase.PExecute("REPLACE INTO 玩家魂玉扩展记录表(玩家GUID,页面,插槽2物品ID)VALUES(%u,%u,%u)", guid, page, itemid);
                    return;
                case 3:
                    WorldDatabase.PExecute("REPLACE INTO 玩家魂玉扩展记录表(玩家GUID,页面,插槽3物品ID)VALUES(%u,%u,%u)", guid, page, itemid);
                    return;
                case 4:
                    WorldDatabase.PExecute("REPLACE INTO 玩家魂玉扩展记录表(玩家GUID,页面,插槽4物品ID)VALUES(%u,%u,%u)", guid, page, itemid);
                    return;
                case 5:
                    WorldDatabase.PExecute("REPLACE INTO 玩家魂玉扩展记录表(玩家GUID,页面,插槽5物品ID)VALUES(%u,%u,%u)", guid, page, itemid);
                    return;
                case 6:
                    WorldDatabase.PExecute("REPLACE INTO 玩家魂玉扩展记录表(玩家GUID,页面,插槽6物品ID)VALUES(%u,%u,%u)", guid, page, itemid);
                    return;
                default:
                    break;
                }
            }
        }
    }
}

void SoulStoneEx::AddOrRemovePlayerBuff(Player * player, uint32 itemid, uint32 olditemid, SoulStoneExSlotType t)
{
    sLog->outString("olditemid = %u", olditemid);

    ItemTemplate const * pProto = sObjectMgr->GetItemTemplate(itemid);

    ItemTemplate const * pProts = sObjectMgr->GetItemTemplate(olditemid);

    if (itemid != 0 && !pProto)
        return;

    if (olditemid != 0 && !pProts)
        return;

    if (t == SSEX_FG)
    {
        if (olditemid == 0)
            return;

        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            _Spell const& spellData = pProto->Spells[i];
            _Spell const& spellDatas = pProts->Spells[i];

            // no spell
            if (!spellData.SpellId)
                continue;
            if (Aura * aura = player->GetAura(spellDatas.SpellId))
            {
                player->RemoveAura(aura);
            }

            player->CastSpell(player, spellData.SpellId);
        }
    }

    if (t == SSEX_XQ)
    {
        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            _Spell const& spellData = pProto->Spells[i];

            // no spell
            if (!spellData.SpellId)
                continue;

            player->CastSpell(player, spellData.SpellId);
        }
    }

    if (t == SSEX_CX)
    {
        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            _Spell const& spellDatas = pProts->Spells[i];

            if (Aura * aura = player->GetAura(spellDatas.SpellId))
            {
                player->RemoveAura(aura);
            }
        }
    }
}

void SoulStoneEx::PlayerBuyPage(Player * player, uint32 page)
{
    //这个好像不能放到这里 要放到判定是否有购买需求那里 首位  然后这里最底下 或许要放个 发送激活操作码到客户端
    if (page > GetPlayerMaxPage(player) + 1)
    {
        //提示要按顺序购买页面
        return;
    }


    RemoveReq(player, page);

    SoulStoneExPlayerData td;
    td.guid = player->GetGUIDLow();
    td.page = page;
    td.itemid1 = 0;
    td.itemid2 = 0;
    td.itemid3 = 0;
    td.itemid4 = 0;
    td.itemid5 = 0;
    td.itemid6 = 0;
    td.itemid7 = 0;

    _SoulStoneExPlayerDataMap.insert({ td.guid,td });

    //SavePlayerAllDate(player);

    SendBuyIsOkPage(player,page);
}

uint32 SoulStoneEx::GetPlayerMaxPage(Player * player)
{
    uint32 guid = player->GetGUIDLow();

    uint32 count = 0;

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            ++count;
        }
    }

    return count;
}


uint32 SoulStoneEx::GetItemType(uint32 itemid)
{
	auto itr = _SoulStoneExItemTypeMap.find(itemid);

	if (itr != _SoulStoneExItemTypeMap.end())
		return itr->second.type;

	return 0;
}

int32 SoulStoneEx::GetItemPage(uint32 itemid)
{
	auto itr = _SoulStoneExItemTypeMap.find(itemid);

	if (itr != _SoulStoneExItemTypeMap.end())
		return itr->second.page;

	return -1;
}

//uint32 SoulStoneEx::GetItemType(uint32 itemid)
//{
//    auto itr = _SoulStoneExItemTypeMap.find(itemid);
//
//    if (itr != _SoulStoneExItemTypeMap.end())
//        return itr->second;
//
//    return 0;
//}

uint32 SoulStoneEx::GetPageActi(uint32 page)
{
    auto itr = _SoulStoneExBuyReqDataMap.find(page);

    if (itr != _SoulStoneExBuyReqDataMap.end())
        return itr->second.jihuoshuxingid;

    return 0;
}

uint32 SoulStoneEx::GetOldItemId(Player * player, uint32 page, uint32 slot)
{
    uint32 guid = player->GetGUID();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            if (itr->second.page == page)
            {
                switch (slot)
                {
                case 1:
                    return itr->second.itemid1;
                case 2:
                    return itr->second.itemid2;
                case 3:
                    return itr->second.itemid3;
                case 4:
                    return itr->second.itemid4;
                case 5:
                    return itr->second.itemid5;
                case 6:
                    return itr->second.itemid6;
                default:
                    break;
                }
            }
        }
    }

    return 0;
}

void SoulStoneEx::SendAllActiData(Player * player)
{

	uint32 maxpage = GetPlayerMaxPage(player);

	if (maxpage < limitpageval)
		maxpage = limitpageval;

	for (uint32 i = 1; i < maxpage + 1; ++i)
    {
        if (sSoulStoneEx->IsActi(player, i))
        {
            SendActiDataToClient(player, i);
        }
    }
}

void SoulStoneEx::SendActiDataToClient(Player * player, uint32 page)
{
    std::ostringstream ss;

    //uint32 itemid = GetPageActi(page);
    uint32 itemid = GetExActiId(player, page);

    
    std::string itemlink = sCF->GetItemLink(itemid);

    ss << page << "#" << itemid << "#" << itemlink;
   
	sGCAddon->SendPacketTo(player, "SSSEX_ACTI_VAL", ss.str());

}


void SoulStoneEx::SendUnActiDataToClient(Player * player, uint32 page)
{
	sGCAddon->SendPacketTo(player, "SSSEX_ACTI_UN", std::to_string(page));
}


void SoulStoneEx::SetActi(Player * player, uint32 page)
{
    if (!player)
        return;

    //uint32 Actiid = GetPageActi(page);
    uint32 Actiid = GetExActiId(player,page);
    //尝试在这获得扩展性页面激活ID 对应玩家guid  页面 以及 页面内所有1~6的物品ID 得到激活ID

    if (Actiid != 0)
    {
        ItemTemplate const * pProto = sObjectMgr->GetItemTemplate(Actiid);

        if (!pProto)
        {
            //可以写提示语句
            return;
        }

        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            _Spell const& spellData = pProto->Spells[i];

            // no spell
            if (!spellData.SpellId)
                continue;

            player->CastSpell(player, spellData.SpellId);
        }

        //uint32 itemid7 = GetPageActi(page);
        uint32 itemid7 = GetExActiId(player,page);

        SetActiId(player, page, itemid7);

        SendActiDataToClient(player, page);
    }
}

void SoulStoneEx::RemoveActi(Player * player, uint32 page)
{
    if (!player)
        return;

    //uint32 Actiid = GetPageActi(page);
    uint32 Actiid = GetExActiId(player,page);

    if (Actiid != 0)
    {
        ItemTemplate const * pProto = sObjectMgr->GetItemTemplate(Actiid);

        if (!pProto)
        {
            //可以写提示语句
            return;
        }

        for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
        {
            _Spell const& spellData = pProto->Spells[i];

            // no spell
            if (!spellData.SpellId)
                continue;

            if (Aura * aura = player->GetAura(spellData.SpellId))
            {
                player->RemoveAura(aura);
            }
        }

        SetActiId(player, page, 0);

        SendUnActiDataToClient(player, page);
    }
}


bool SoulStoneEx::CanSetActi(Player * player, uint32 page)
{
    uint32 guid = player->GetGUIDLow();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            if (itr->second.page == page)
            {
                if (itr->second.itemid1 > 0 && itr->second.itemid2 > 0 && itr->second.itemid3 > 0 && itr->second.itemid4 > 0 && itr->second.itemid5 > 0 && itr->second.itemid6 > 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}


bool SoulStoneEx::IsActi(Player * player, uint32 page)
{

    uint32 guid = player->GetGUIDLow();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            if (itr->second.page == page)
            {
                if (itr->second.itemid7 == 0)
                {
                    return false;
                }
            }
        }
    }

    return true;
}


//void SoulStoneEx::CanInSetToSlot(Player * player, uint32 page, uint32 slot, uint32 itemid, std::string msg)
//{
//    if (page > GetPlayerMaxPage(player))
//        return;
//
//    ItemTemplate const * pProto = sObjectMgr->GetItemTemplate(itemid);
//
//    if (!player || !pProto || !IfSoulStoneItem(itemid))
//    {
//        //提示未知问题 
//        return;
//    }
//
//	uint32 maxpage = GetPlayerMaxPage(player);
//
//	if (page > maxpage)
//	{
//		//提示请勿非法使用插件,不能使用未购买的魂玉页面
//		return;
//	}
//
//    uint32 guid = player->GetGUIDLow();
//
//    uint32 newitemtype = GetItemType(itemid);
//
//    uint32 olditemtype = 0;
//
//    uint32 olditemid = 0;
//
//    player->PlayerTalkClass->ClearMenus();
//
//    std::ostringstream ss;
//
//    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
//    {
//        if (itr->first == guid)
//        {
//            if (itr->second.page == page)
//            {
//                switch (slot)
//                {
//                case 1:
//                    olditemtype = GetItemType(itr->second.itemid1);
//                    olditemid = itr->second.itemid1;
//                    break;
//                case 2:
//                    olditemtype = GetItemType(itr->second.itemid2);
//                    olditemid = itr->second.itemid2;
//                    break;
//                case 3:
//                    olditemtype = GetItemType(itr->second.itemid3);
//                    olditemid = itr->second.itemid3;
//                    break;
//                case 4:
//                    olditemtype = GetItemType(itr->second.itemid4);
//                    olditemid = itr->second.itemid4;
//                    break;
//                case 5:
//                    olditemtype = GetItemType(itr->second.itemid5);
//                    olditemid = itr->second.itemid5;
//                    break;
//                case 6:
//                    olditemtype = GetItemType(itr->second.itemid6);
//                    olditemid = itr->second.itemid6;
//                    break;
//                default:
//                    break;
//                }
//            }
//        }
//    }
//
//    
//    if (olditemid == 0)
//    {
//        ss << sCF->GetItemLink(itemid) << pProto->Name1.c_str() << "是否镶嵌到此插槽?";
//        player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_VENDOR, "", GOSSIP_SENDER_SEC_SS, GOSSIP_ACTION_SSEX + 1, ss.str().c_str(), 0, false);
//        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
//    }
//    else
//    {
//        if (ItemTemplate const * pProt = sObjectMgr->GetItemTemplate(olditemid))
//        {
//            ss << sCF->GetItemLink(itemid) << pProto->Name1.c_str() << "是否替换插槽内的" <<sCF->GetItemLink(olditemid) << pProt->Name1.c_str();
//            player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_VENDOR, "", GOSSIP_SENDER_SEC_SS, GOSSIP_ACTION_SSEX + 2, ss.str().c_str(), 0, false);
//            player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
//        }
//    }
//
//    std::ostringstream str;
//
//    str << msg << "#" << sCF->GetItemLink(itemid);
//
//    SetPlayerMsg(player, str.str());
//}

void SoulStoneEx::CanInSetToSlot(Player * player, uint32 page, uint32 slot, uint32 itemid, std::string msg)
{
	if (page > GetPlayerMaxPage(player))
		return;

	player->PlayerTalkClass->ClearMenus();

	ItemTemplate const * pProto = sObjectMgr->GetItemTemplate(itemid);

	if (!player || !pProto || !IfSoulStoneItem(itemid))
	{
		//提示未知问题 
		return;
	}

	uint32 maxpage = GetPlayerMaxPage(player);

	if (page > maxpage)
	{
		//提示请勿非法使用插件,不能使用未购买的魂玉页面
		return;
	}

	uint32 guid = player->GetGUIDLow();

	uint32 newitemtype = GetItemType(itemid);
	uint32 newitempage = GetItemPage(itemid);

	if (newitempage == -1 || newitemtype == 0)
		return;

	if (newitempage != page && newitempage != 0)
	{
		std::ostringstream sstr;
		sstr << sCF->GetItemLink(itemid) << "此魂玉只能镶嵌在第" << newitempage << "页";
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_VENDOR, "", GOSSIP_SENDER_SEC_SS_ERR, GOSSIP_ACTION_SS_ERR, sstr.str(), 0, false);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
		return;
	}

	uint32 olditemid = 0;

	std::ostringstream ss;

	for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
	{
		if (itr->first == guid)
		{
			bool send = false;

			if (itr->second.page == page)
			{
				if (slot == 1)
				{
					if (newitemtype == GetItemType(itr->second.itemid2) || newitemtype == GetItemType(itr->second.itemid3) || newitemtype == GetItemType(itr->second.itemid4) || newitemtype == GetItemType(itr->second.itemid5) || newitemtype == GetItemType(itr->second.itemid6))
					{
						send = true;
					}
				}

				if (slot == 2)
				{
					if (newitemtype == GetItemType(itr->second.itemid1) || newitemtype == GetItemType(itr->second.itemid3) || newitemtype == GetItemType(itr->second.itemid4) || newitemtype == GetItemType(itr->second.itemid5) || newitemtype == GetItemType(itr->second.itemid6))
					{
						send = true;
					}
				}

				if (slot == 3)
				{
					if (newitemtype == GetItemType(itr->second.itemid2) || newitemtype == GetItemType(itr->second.itemid1) || newitemtype == GetItemType(itr->second.itemid4) || newitemtype == GetItemType(itr->second.itemid5) || newitemtype == GetItemType(itr->second.itemid6))
					{
						send = true;
					}
				}

				if (slot == 4)
				{
					if (newitemtype == GetItemType(itr->second.itemid2) || newitemtype == GetItemType(itr->second.itemid3) || newitemtype == GetItemType(itr->second.itemid1) || newitemtype == GetItemType(itr->second.itemid5) || newitemtype == GetItemType(itr->second.itemid6))
					{
						send = true;
					}
				}

				if (slot == 5)
				{
					if (newitemtype == GetItemType(itr->second.itemid2) || newitemtype == GetItemType(itr->second.itemid3) || newitemtype == GetItemType(itr->second.itemid4) || newitemtype == GetItemType(itr->second.itemid1) || newitemtype == GetItemType(itr->second.itemid6))
					{
						send = true;
					}
				}

				if (slot == 6)
				{
					if (newitemtype == GetItemType(itr->second.itemid2) || newitemtype == GetItemType(itr->second.itemid3) || newitemtype == GetItemType(itr->second.itemid4) || newitemtype == GetItemType(itr->second.itemid5) || newitemtype == GetItemType(itr->second.itemid1))
					{
						send = true;
					}
				}
			}

			if (send)
			{
				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_VENDOR, "", GOSSIP_SENDER_SEC_SSEX_ERR, GOSSIP_ACTION_SSEX_ERR, "当前页面只能镶嵌一个此类型魂玉", 0, false);
				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
				return;
			}

			switch (slot)
			{
			case 1:
				olditemid = itr->second.itemid1;
				break;
			case 2:
				olditemid = itr->second.itemid2;
				break;
			case 3:
				olditemid = itr->second.itemid3;
				break;
			case 4:
				olditemid = itr->second.itemid4;
				break;
			case 5:
				olditemid = itr->second.itemid5;
				break;
			case 6:
				olditemid = itr->second.itemid6;
				break;
			default:
				break;
			}

		}
	}


	if (olditemid == 0)
	{
		ss << sCF->GetItemLink(itemid) << pProto->Name1.c_str() << "是否镶嵌到此插槽?";
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_VENDOR, "", GOSSIP_SENDER_SEC_SS, GOSSIP_ACTION_SSEX + 1, ss.str().c_str(), 0, false);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
	}
	else
	{
		if (ItemTemplate const * pProt = sObjectMgr->GetItemTemplate(olditemid))
		{
			ss << sCF->GetItemLink(itemid) << pProto->Name1.c_str() << "是否替换插槽内的" << sCF->GetItemLink(olditemid) << pProt->Name1.c_str();
			player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_VENDOR, "", GOSSIP_SENDER_SEC_SS, GOSSIP_ACTION_SSEX + 2, ss.str().c_str(), 0, false);
			player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());
		}
	}

	std::ostringstream str;

	str << msg << "#" << sCF->GetItemLink(itemid);

	SetPlayerMsg(player, str.str());
}

void SoulStoneEx::CanRemoveSlot(Player * player, uint32 page, uint32 slot, std::string msg)
{
    if (!player)
        return;

	uint32 maxpage = GetPlayerMaxPage(player);

	if (page > maxpage)
	{
		//提示请勿非法使用插件,不能使用未购买的魂玉页面
		return;
	}

    player->PlayerTalkClass->ClearMenus();

    uint32 guid = player->GetGUIDLow();

    uint32 olditemid = 0;

    std::ostringstream ss;

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            if (itr->second.page == page)
            {
                switch (slot)
                {
                case 1:
                    olditemid = itr->second.itemid1;
                    break;
                case 2:
                    olditemid = itr->second.itemid2;
                    break;
                case 3:
                    olditemid = itr->second.itemid3;
                    break;
                case 4:
                    olditemid = itr->second.itemid4;
                    break;
                case 5:
                    olditemid = itr->second.itemid5;
                    break;
                case 6:
                    olditemid = itr->second.itemid6;
                    break;
                default:
                    break;
                } 
            }
        }
    }

    ItemTemplate const * pProt = sObjectMgr->GetItemTemplate(olditemid);

    if (!pProt)
        return;

    ss << "是否拆卸此插槽中的" << sCF->GetItemLink(olditemid) << pProt->Name1.c_str();

    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_VENDOR, "", GOSSIP_SENDER_SEC_SS, GOSSIP_ACTION_SSEX + 3, ss.str().c_str(), 0, false);

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());

    std::ostringstream str;

    str << msg << "#" << std::to_string(olditemid);

    SetPlayerMsg(player, str.str());
}


bool SoulStoneEx::IsData(Player * player)
{

    uint32 guid = player->GetGUIDLow();

    uint32 count = 0;

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->second.guid == guid)
            ++count;
    }

    if (count == 0)
    {
        return false;
    }

    return true;

}

bool SoulStoneEx::IsBuyPageReq(Player * player, uint32 page)
{
    if (!player)
        return false;

    auto itr = _SoulStoneExBuyReqDataMap.find(page);
    if (itr == _SoulStoneExBuyReqDataMap.end())
        return false;

	bool send = false;

	std::ostringstream oss;

	oss << "购买魂玉第" << page << "页：\n";

    if (itr->second.itemid1 > 0 && itr->second.val1 > 0)
    {
        if (!player->HasItemCount(itr->second.itemid1, itr->second.val1))
        {
			//ChatHandler(player->GetSession()).PSendSysMessage("需要物品：%s * %d 个",sCF->GetItemLink(itr->second.itemid1).c_str(),itr->second.val1);
            //return false;
			oss << "需要物品：" << sCF->GetItemLink(itr->second.itemid1) << " * " << itr->second.val1 << "个\n";
			send = true;
        }
    }

    if (itr->second.itemid2 > 0 && itr->second.val2 > 0)
    {
        if (!player->HasItemCount(itr->second.itemid2, itr->second.val2))
        {
			//ChatHandler(player->GetSession()).PSendSysMessage("需要物品：%s * %d 个", sCF->GetItemLink(itr->second.itemid2), itr->second.val2);
            //return false;
			oss << "需要物品：" << sCF->GetItemLink(itr->second.itemid2) << " * " << itr->second.val2 << "个\n";
			send = true;
        }
    }

    if (itr->second.itemid3 > 0 && itr->second.val3 > 0)
    {
        if (!player->HasItemCount(itr->second.itemid3, itr->second.val3))
        {
			//ChatHandler(player->GetSession()).PSendSysMessage("需要物品：%s * %d 个", sCF->GetItemLink(itr->second.itemid3), itr->second.val3);
            //return false;
			oss << "需要物品：" << sCF->GetItemLink(itr->second.itemid3) << " * " << itr->second.val3 << "个\n";
			send = true;
        }
    }
    
    if (itr->second.itemid4 > 0 && itr->second.val4 > 0)
    {
        if (!player->HasItemCount(itr->second.itemid4, itr->second.val4))
        {
			//ChatHandler(player->GetSession()).PSendSysMessage("需要物品：%s * %d 个", sCF->GetItemLink(itr->second.itemid4), itr->second.val4);
            //return false;
			oss << "需要物品：" << sCF->GetItemLink(itr->second.itemid4) << " * " << itr->second.val4 << "个\n";
			send = true;
        }
    }
    
    if (itr->second.itemid5 > 0 && itr->second.val5 > 0)
    {
        if (!player->HasItemCount(itr->second.itemid5, itr->second.val5))
        {
			//ChatHandler(player->GetSession()).PSendSysMessage("需要物品：%s * %d 个", sCF->GetItemLink(itr->second.itemid5), itr->second.val5);
            //return false;
			oss << "需要物品：" << sCF->GetItemLink(itr->second.itemid5) << " * " << itr->second.val5 << "个\n";
			send = true;
        }
    }
    

    if (player->GetMoney() < itr->second.money)
    {
		//ChatHandler(player->GetSession()).PSendSysMessage("需要金币：%d", uint32(itr->second.money / GOLD));
		//return false;
		oss << "需要金币：" << uint32(itr->second.money / GOLD);
		send = true;
    }

	if (send)
	{
		ChatHandler(player->GetSession()).PSendSysMessage(oss.str().c_str());
		return false;
	}

    player->PlayerTalkClass->ClearMenus();

    player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_VENDOR, "", GOSSIP_SENDER_SEC_SS_BUY, GOSSIP_ACTION_SSEX_BUY + page, "是否需要购买第"+std::to_string(page)+"页", 0, false);

    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, player->GetGUID());

    return true;

}

void SoulStoneEx::RemoveReq(Player * player, uint32 page)
{
    if (!player)
        return;

    auto itr = _SoulStoneExBuyReqDataMap.find(page);
    if (itr == _SoulStoneExBuyReqDataMap.end())
        return;

    player->DestroyItemCount(itr->second.itemid1, itr->second.val1,true);

    player->DestroyItemCount(itr->second.itemid2, itr->second.val2,true);

    player->DestroyItemCount(itr->second.itemid3, itr->second.val3,true);

    player->DestroyItemCount(itr->second.itemid4, itr->second.val4,true);

    player->DestroyItemCount(itr->second.itemid5, itr->second.val5,true);

    player->SetMoney(player->GetMoney() - (itr->second.money));

}

void SoulStoneEx::SendBuyIsOkPage(Player * player, uint32 page)
{
    std::string p = std::to_string(page);

	sGCAddon->SendPacketTo(player, "SSSEX_BUY_PAGE", p);

}

void SoulStoneEx::SetActiId(Player * player, uint32 page,uint32 itemid)
{
    uint32 guid = player->GetGUIDLow();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            if (itr->second.page == page)
            {
                itr->second.itemid7 = itemid;
            }
        }
    }

    //SavePlayerAllDate(player);
}

void SoulStoneEx::AgainAddLoseBuff(Player * player)
{
    uint32 maxpage = GetPlayerMaxPage(player);

    uint32 guid = player->GetGUIDLow();

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->first == guid)
        {
            ItemTemplate const * p1 = sObjectMgr->GetItemTemplate(itr->second.itemid1);
            ItemTemplate const * p2 = sObjectMgr->GetItemTemplate(itr->second.itemid2);
            ItemTemplate const * p3 = sObjectMgr->GetItemTemplate(itr->second.itemid3);
            ItemTemplate const * p4 = sObjectMgr->GetItemTemplate(itr->second.itemid4);
            ItemTemplate const * p5 = sObjectMgr->GetItemTemplate(itr->second.itemid5);
            ItemTemplate const * p6 = sObjectMgr->GetItemTemplate(itr->second.itemid6);
            ItemTemplate const * p7 = sObjectMgr->GetItemTemplate(itr->second.itemid7);

            if (p1)
            {
                for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                {
                    _Spell const& spellData = p1->Spells[i];

                    // no spell
                    if (!spellData.SpellId)
                        continue;

                    player->CastSpell(player, spellData.SpellId);
                }
            }

            if (p2)
            {
                for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                {
                    _Spell const& spellData = p2->Spells[i];

                    // no spell
                    if (!spellData.SpellId)
                        continue;

                    player->CastSpell(player, spellData.SpellId);
                }
            }

            if (p3)
            {
                for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                {
                    _Spell const& spellData = p3->Spells[i];

                    // no spell
                    if (!spellData.SpellId)
                        continue;

                    player->CastSpell(player, spellData.SpellId);
                }
            }

            if (p4)
            {
                for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                {
                    _Spell const& spellData = p4->Spells[i];

                    // no spell
                    if (!spellData.SpellId)
                        continue;

                    player->CastSpell(player, spellData.SpellId);
                }
            }

            if (p5)
            {
                for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                {
                    _Spell const& spellData = p5->Spells[i];

                    // no spell
                    if (!spellData.SpellId)
                        continue;

                    player->CastSpell(player, spellData.SpellId);
                }
            }

            if (p6)
            {
                for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                {
                    _Spell const& spellData = p6->Spells[i];

                    // no spell
                    if (!spellData.SpellId)
                        continue;

                    player->CastSpell(player, spellData.SpellId);
                }
            }

            if (p7)
            {
                for (uint8 i = 0; i < MAX_ITEM_PROTO_SPELLS; ++i)
                {
                    _Spell const& spellData = p7->Spells[i];

                    // no spell
                    if (!spellData.SpellId)
                        continue;

                    player->CastSpell(player, spellData.SpellId);
                }
            }
        }
    }
}

void SoulStoneEx::UpdatePlayerDate(Player * player)
{
	uint32 guid = player->GetGUIDLow();

	uint32 count = 0;

	for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
	{
		if (itr->second.guid == guid)
			++count;
	}

	if (limitpageval <= count)
		return;

	uint32 i = limitpageval - count;

	for (; i < limitpageval + 1; ++i)
	{
		SoulStoneExPlayerData td;
		td.guid = player->GetGUIDLow();
		td.page = i;
		td.itemid1 = 0;
		td.itemid2 = 0;
		td.itemid3 = 0;
		td.itemid4 = 0;
		td.itemid5 = 0;
		td.itemid6 = 0;
		td.itemid7 = 0;

		_SoulStoneExPlayerDataMap.insert({ td.guid, td });
	}

	SavePlayerAllDate(player);

}

uint32 SoulStoneEx::GetExActiId(Player * player, uint32 page)
{
    uint32 guid = player->GetGUIDLow(); 

    uint32 _itemid[6] = {0,0,0,0,0,0};

    for (auto itr = _SoulStoneExPlayerDataMap.begin(); itr != _SoulStoneExPlayerDataMap.end(); ++itr)
    {
        if (itr->second.page == page)
        {
            if (itr->second.itemid1 > 0 && itr->second.itemid2 > 0 && itr->second.itemid3 > 0 && itr->second.itemid4 > 0 && itr->second.itemid5 > 0 && itr->second.itemid6 > 0)
            {
                _itemid[0] = itr->second.itemid1;
                _itemid[1] = itr->second.itemid2;
                _itemid[2] = itr->second.itemid3;
                _itemid[3] = itr->second.itemid4;
                _itemid[4] = itr->second.itemid5;
                _itemid[5] = itr->second.itemid6;
            }
        }
    }

    for (uint8 i = 0; i < 6; ++i)
    {
        if (_itemid[i] == 0)
            return 0;
    }

    
    for (auto its = _SoulStoneExExDateMap.begin(); its != _SoulStoneExExDateMap.end(); ++its)
    {
        if (its->second.page == page)
        {
            if (_itemid[0] == its->second.itemid1 && _itemid[1] == its->second.itemid2 && _itemid[2] == its->second.itemid3 && _itemid[3] == its->second.itemid4 && _itemid[4] == its->second.itemid5 && _itemid[5] == its->second.itemid6)
            {
                return its->second.itemid7;
            }
        }
    }

    return 0;
}

std::vector<std::string> SoulStoneEx::split(std::string str, std::string pattern)
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

class SoulStoneExPlayerScript : public PlayerScript
{
public:
    SoulStoneExPlayerScript() : PlayerScript("SoulStonePlayerScript") {}

    void OnPlayerChatAddon(std::string header, std::string msg, uint8 chn, Player* player)
    {
        if (chn == 4) //CHAT_MSG_GUILD = 4
        {
            if (header == "SSC_ITEM_TO_SLOT")
            {
                std::vector<std::string> vec = sSoulStoneEx->split(msg, "#");

                uint32 page = atoi(vec[0].c_str());
                uint32 slot = atoi(vec[1].c_str());
                uint32 item = atoi(vec[2].c_str());

                sSoulStoneEx->CanInSetToSlot(player, page, slot, item, msg);

            }

            if (header == "SSC_REMOVE_SLOT_ITEM")
            {
                std::vector<std::string> vec = sSoulStoneEx->split(msg, "#");

                uint32 page = atoi(vec[0].c_str());
                uint32 slot = atoi(vec[1].c_str());

                sSoulStoneEx->CanRemoveSlot(player, page, slot, msg);
            }

            if (header == "SSC_LIMIT" && msg == "VAL")
            {
                sSoulStoneEx->SendMutualData(player);
            }

            if (header == "SSC_ACTI" && msg == "ISOK")
            {
                sSoulStoneEx->SendAllActiData(player);
            }

            if (header == "SSC_BUY_PAGE")
            {
                uint32 page = atoi(msg.c_str());

                if (!sSoulStoneEx->IsBuyPageReq(player, page))
                {
                    return;
                }

            }
        }
    }

    void OnGossipSelect(Player* player, uint32 menu_id, uint32 sender, uint32 action)
    {
        
        if (sender == GOSSIP_SENDER_SEC_SS)
        {

            if (action == GOSSIP_ACTION_SSEX + 1)
            {
                std::string msg = sSoulStoneEx->GetPlayerMsg(player);
				sLog->outString("msg = %s",msg.c_str());

				sGCAddon->SendPacketTo(player, "SSSEX_XQ_FG", msg);

                std::vector<std::string> vec = sSoulStoneEx->split(msg, "#");

                uint32 page = atoi(vec[0].c_str());
                uint32 slot = atoi(vec[1].c_str());
                uint32 itemid = atoi(vec[2].c_str());

                sSoulStoneEx->SavePlayerSlotToSTL(player, page, slot, itemid);

                sSoulStoneEx->AddOrRemovePlayerBuff(player, itemid);

                player->DestroyItemCount(itemid, 1,true);

                if (sSoulStoneEx->CanSetActi(player, page))
                {
                    sSoulStoneEx->SetActi(player, page);
                }
            }

            if (action == GOSSIP_ACTION_SSEX + 2)
            {
                std::string msg = sSoulStoneEx->GetPlayerMsg(player);

				sGCAddon->SendPacketTo(player, "SSSEX_XQ_FG", msg);

                std::vector<std::string> vec = sSoulStoneEx->split(msg, "#");

                uint32 page = atoi(vec[0].c_str());
                uint32 slot = atoi(vec[1].c_str());
                uint32 itemid = atoi(vec[2].c_str());

                uint32 olditemid = sSoulStoneEx->GetOldItemId(player, page, slot);

                sSoulStoneEx->SavePlayerSlotToSTL(player, page, slot, itemid);

                sSoulStoneEx->AddOrRemovePlayerBuff(player, itemid,olditemid,SSEX_FG);

                player->DestroyItemCount(itemid, 1, true);

                if (sSoulStoneEx->CanSetActi(player, page))
                {
                    sSoulStoneEx->SetActi(player, page);
                }
            }


            if (action == GOSSIP_ACTION_SSEX + 3)
            {
                std::string msg = sSoulStoneEx->GetPlayerMsg(player);

				sGCAddon->SendPacketTo(player,"SSSEX_CX",msg);

                std::vector<std::string> vec = sSoulStoneEx->split(msg, "#");

                uint32 page = atoi(vec[0].c_str());
                uint32 slot = atoi(vec[1].c_str());

                uint32 olditemid = sSoulStoneEx->GetOldItemId(player, page, slot);

                sSoulStoneEx->SavePlayerSlotToSTL(player, page, slot, 0);

                sSoulStoneEx->AddOrRemovePlayerBuff(player, 0, olditemid, SSEX_CX);

                player->AddItem(olditemid, 1);

                if (sSoulStoneEx->IsActi(player, page))
                {
                    sSoulStoneEx->RemoveActi(player, page);
                }
            }

        }

        if (sender == GOSSIP_SENDER_SEC_SS_BUY)
        {
            if (action > GOSSIP_ACTION_SSEX_BUY)
            {
                uint32 page = action - GOSSIP_ACTION_SSEX_BUY;

                sSoulStoneEx->PlayerBuyPage(player, page);
            }
        }

    }

	void OnLogin(Player* player)
    {
        //初始化魂玉信息
        if (!sSoulStoneEx->IsData(player))
        {
            sSoulStoneEx->CreatePlayerDate(player);
        }
		else
		{
			sSoulStoneEx->UpdatePlayerDate(player);
		}
    }
};


class SoulStoneExWorldScript : public WorldScript
{
public:
	SoulStoneExWorldScript() : WorldScript("SoulStoneWorldScript") {}

	void OnAfterConfigLoad(bool /*reload*/)
	{
		sSoulStoneEx->Load();
	}
};

void AddSC_SoulStoneExScripts()
{
    new SoulStoneExPlayerScript();
	new SoulStoneExWorldScript();
}



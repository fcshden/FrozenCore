/*
5.0
Transmogrification 3.3.5a - Gossip menu
By Rochet2

ScriptName for NPC:
Creature_Transmogrify

TODO:
Make DB saving even better (Deleting)? What about coding?

Fix the cost formula
-- Too much data handling, use default costs

Are the qualities right?
Blizzard might have changed the quality requirements.
(TC handles it with stat checks)

Cant transmogrify rediculus items // Foereaper: would be fun to stab people with a fish
-- Cant think of any good way to handle this easily, could rip flagged items from cata DB
*/

#pragma execution_character_set("utf-8")
#include "Transmogrification.h"
#define sT  sTransmogrification
#define GTS session->GetAcoreString // dropped translation support, no one using?

class CS_Transmogrification : public CreatureScript
{
public:
    CS_Transmogrification() : CreatureScript("Creature_Transmogrify") { }

    bool OnGossipHello(Player* player, Creature* creature)
    {
        WorldSession* session = player->GetSession();
        if (sT->GetEnableTransmogInfo())
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Book_11:30:30:-18:0|t 请选择需要幻化的部位", EQUIPMENT_SLOT_END + 9, 0);
        for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
        {
            if (const char* slotName = sT->GetSlotName(slot, session))
            {
                Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
                uint32 entry = newItem ? sT->GetFakeEntry(newItem->GetGUID()) : 0;
                std::string icon = entry ? sT->GetItemIcon(entry,30,30,-18,0) : sT->GetSlotIcon(slot, 30, 30, -18, 0);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, icon+std::string(slotName), EQUIPMENT_SLOT_END, slot);
            }
        }
#ifdef PRESETS
        if (sT->GetEnableSets())
			player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/RAIDFRAME/UI-RAIDFRAME-MAINASSIST:30:30:-18:0|t 幻化管理 ", EQUIPMENT_SLOT_END + 4, 0);
#endif
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Enchant_Disenchant:30:30:-18:0|t 清除所有幻化效果 ", EQUIPMENT_SLOT_END + 2, 0, "您确定要移除所有装备的幻化效果么？ ", 0, false);
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|t 刷新菜单 ", EQUIPMENT_SLOT_END + 1, 0);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action)
    {
        player->PlayerTalkClass->ClearMenus();
        WorldSession* session = player->GetSession();
        switch(sender)
        {
        case EQUIPMENT_SLOT_END: // Show items you can use
            ShowTransmogItems(player, creature, action);
            break;
        case EQUIPMENT_SLOT_END+1: // Main menu
            OnGossipHello(player, creature);
            break;
        case EQUIPMENT_SLOT_END+2: // Remove Transmogrifications
            {
                bool removed = false;
                SQLTransaction trans = CharacterDatabase.BeginTransaction();
                for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                {
                    if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                    {
                        if (!sT->GetFakeEntry(newItem->GetGUID()))
                            continue;
                        sT->DeleteFakeEntry(player, slot, newItem, &trans);
                        removed = true;
                    }
                }
                if (removed)
                {
                    session->SendAreaTriggerMessage(GTS(LANG_ERR_UNTRANSMOG_OK));
                    CharacterDatabase.CommitTransaction(trans);
                }
                else
                    session->SendNotification(LANG_ERR_UNTRANSMOG_NO_TRANSMOGS);
                OnGossipHello(player, creature);
            } break;
        case EQUIPMENT_SLOT_END+3: // Remove Transmogrification from single item
            {
                if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, action))
                {
                    if (sT->GetFakeEntry(newItem->GetGUID()))
                    {
                        sT->DeleteFakeEntry(player, action, newItem);
                        session->SendAreaTriggerMessage(GTS(LANG_ERR_UNTRANSMOG_OK));
                    }
                    else
                        session->SendNotification(LANG_ERR_UNTRANSMOG_NO_TRANSMOGS);
                }
                OnGossipSelect(player, creature, EQUIPMENT_SLOT_END, action);
            } break;
#ifdef PRESETS
        case EQUIPMENT_SLOT_END+4: // Presets menu
            {
                if (!sT->GetEnableSets())
                {
                    OnGossipHello(player, creature);
                    return true;
                }
                if (sT->GetEnableSetInfo())
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Book_11:30:30:-18:0|t 把身上的幻化效果保存为套装 ", EQUIPMENT_SLOT_END + 10, 0);
                for (Transmogrification::presetIdMap::const_iterator it = sT->presetByName[player->GetGUID()].begin(); it != sT->presetByName[player->GetGUID()].end(); ++it)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Statue_02:30:30:-18:0|t"+it->second, EQUIPMENT_SLOT_END+6, it->first);

                if (sT->presetByName[player->GetGUID()].size() < sT->GetMaxSets())
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/GuildBankFrame/UI-GuildBankFrame-NewTab:30:30:-18:0|t 保存套装 ", EQUIPMENT_SLOT_END + 8, 0);
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|t 返回 ", EQUIPMENT_SLOT_END + 1, 0);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            } break;
        case EQUIPMENT_SLOT_END+5: // Use preset
            {
                if (!sT->GetEnableSets())
                {
                    OnGossipHello(player, creature);
                    return true;
                }
                // action = presetID
                for (Transmogrification::slotMap::const_iterator it = sT->presetById[player->GetGUID()][action].begin(); it != sT->presetById[player->GetGUID()][action].end(); ++it)
                {
                    if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, it->first))
                        sT->PresetTransmog(player, item, it->second, it->first);
                }
                OnGossipSelect(player, creature, EQUIPMENT_SLOT_END+6, action);
            } break;
        case EQUIPMENT_SLOT_END+6: // view preset
            {
                if (!sT->GetEnableSets())
                {
                    OnGossipHello(player, creature);
                    return true;
                }
                // action = presetID
                for (Transmogrification::slotMap::const_iterator it = sT->presetById[player->GetGUID()][action].begin(); it != sT->presetById[player->GetGUID()][action].end(); ++it)
                    player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, sT->GetItemIcon(it->second, 30, 30, -18, 0)+sT->GetItemLink(it->second, session), sender, action);

				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Misc_Statue_02:30:30:-18:0|t 使用套装 ", EQUIPMENT_SLOT_END + 5, action, "使用后会让装备变得不可退回，不可交易。\n您确定要继续么？\n\n " + sT->presetByName[player->GetGUID()][action], 0, false);
				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-LeaveItem-Opaque:30:30:-18:0|t 移除套装 ", EQUIPMENT_SLOT_END + 7, action, "你确定要删除已经保存的【 " + sT->presetByName[player->GetGUID()][action] + " 】套装么？ ", 0, false);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|t 返回 ", EQUIPMENT_SLOT_END+4, 0);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            } break;
        case EQUIPMENT_SLOT_END+7: // Delete preset
            {
                if (!sT->GetEnableSets())
                {
                    OnGossipHello(player, creature);
                    return true;
                }
                // action = presetID
                CharacterDatabase.PExecute("DELETE FROM `custom_transmogrification_sets` WHERE Owner = %u AND PresetID = %u",  player->GetGUIDLow(), action);
                sT->presetById[player->GetGUID()][action].clear();
                sT->presetById[player->GetGUID()].erase(action);
                sT->presetByName[player->GetGUID()].erase(action);

                OnGossipSelect(player, creature, EQUIPMENT_SLOT_END+4, 0);
            } break;
        case EQUIPMENT_SLOT_END+8: // Save preset
            {
                if (!sT->GetEnableSets() || sT->presetByName[player->GetGUID()].size() >= sT->GetMaxSets())
                {
                    OnGossipHello(player, creature);
                    return true;
                }
                uint32 cost = 0;
                bool canSave = false;
                for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                {
                    if (!sT->GetSlotName(slot, session))
                        continue;
                    if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                    {
                        uint32 entry = sT->GetFakeEntry(newItem->GetGUID());
                        if (!entry)
                            continue;
                        const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
                        if (!temp)
                            continue;
                        if (!sT->SuitableForTransmogrification(player, temp)) // no need to check?
                            continue;
                        cost += sT->GetSpecialPrice(temp);
                        canSave = true;
                        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, sT->GetItemIcon(entry, 30, 30, -18, 0)+sT->GetItemLink(entry, session), EQUIPMENT_SLOT_END+8, 0);
                    }
                }
                if (canSave)
					player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, "|TInterface/GuildBankFrame/UI-GuildBankFrame-NewTab:30:30:-18:0|t 保存套装 ", 0, 0, "请你设置要保存的套装的名字 ", cost*sT->GetSetCostModifier() + sT->GetSetCopperCost(), true);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|t 刷新菜单 ", sender, action);
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|t 返回 ", EQUIPMENT_SLOT_END+4, 0);
                player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            } break;
        case EQUIPMENT_SLOT_END+10: // Set info
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|t 返回 ", EQUIPMENT_SLOT_END+4, 0);
                player->SEND_GOSSIP_MENU(sT->GetSetNpcText(), creature->GetGUID());
            } break;
#endif
        case EQUIPMENT_SLOT_END+9: // Transmog info
            {
                player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|t 返回 ", EQUIPMENT_SLOT_END+1, 0);
                player->SEND_GOSSIP_MENU(sT->GetTransmogNpcText(), creature->GetGUID());
            } break;
        default: // Transmogrify
            {
                if (!sender && !action)
                {
                    OnGossipHello(player, creature);
                    return true;
                }
                // sender = slot, action = display
                TransmogTrinityStrings res = sT->Transmogrify(player, MAKE_NEW_GUID(action, 0, HIGHGUID_ITEM), sender);
                if (res == LANG_ERR_TRANSMOG_OK)
                    session->SendAreaTriggerMessage(GTS(LANG_ERR_TRANSMOG_OK));
                else
                    session->SendNotification(res);
                // OnGossipSelect(player, creature, EQUIPMENT_SLOT_END, sender);
                // ShowTransmogItems(player, creature, sender);
                player->CLOSE_GOSSIP_MENU(); // Wait for SetMoney to get fixed, issue #10053
            } break;
        }
        return true;
    }

#ifdef PRESETS
    bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code)
    {
        player->PlayerTalkClass->ClearMenus();
        if (sender || action)
            return true; // should never happen
        if (!sT->GetEnableSets())
        {
            OnGossipHello(player, creature);
            return true;
        }
        std::string name(code);
        if (name.find('"') != std::string::npos || name.find('\\') != std::string::npos)
            player->GetSession()->SendNotification(LANG_PRESET_ERR_INVALID_NAME);
        else
        {
            for (uint8 presetID = 0; presetID < sT->GetMaxSets(); ++presetID) // should never reach over max
            {
                if (sT->presetByName[player->GetGUID()].find(presetID) != sT->presetByName[player->GetGUID()].end())
                    continue; // Just remember never to use presetByName[pGUID][presetID] when finding etc!

                int32 cost = 0;
                std::map<uint8, uint32> items;
                for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
                {
                    if (!sT->GetSlotName(slot, player->GetSession()))
                        continue;
                    if (Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                    {
                        uint32 entry = sT->GetFakeEntry(newItem->GetGUID());
                        if (!entry)
                            continue;
                        const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);
                        if (!temp)
                            continue;
                        if (!sT->SuitableForTransmogrification(player, temp))
                            continue;
                        cost += sT->GetSpecialPrice(temp);
                        items[slot] = entry;
                    }
                }
                if (items.empty())
                    break; // no transmogrified items were found to be saved
                cost *= sT->GetSetCostModifier();
                cost += sT->GetSetCopperCost();
                if (!player->HasEnoughMoney(cost))
                {
                    player->GetSession()->SendNotification(LANG_ERR_TRANSMOG_NOT_ENOUGH_MONEY);
                    break;
                }

                std::ostringstream ss;
                for (std::map<uint8, uint32>::iterator it = items.begin(); it != items.end(); ++it)
                {
                    ss << uint32(it->first) << ' ' << it->second << ' ';
                    sT->presetById[player->GetGUID()][presetID][it->first] = it->second;
                }
                sT->presetByName[player->GetGUID()][presetID] = name; // Make sure code doesnt mess up SQL!
                CharacterDatabase.PExecute("REPLACE INTO `custom_transmogrification_sets` (`Owner`, `PresetID`, `SetName`, `SetData`) VALUES (%u, %u, \"%s\", \"%s\")",  player->GetGUIDLow(), uint32(presetID), name.c_str(), ss.str().c_str());
                if (cost)
                    player->ModifyMoney(cost);
                break;
            }
        }
        //OnGossipSelect(player, creature, EQUIPMENT_SLOT_END+4, 0);
        player->CLOSE_GOSSIP_MENU(); // Wait for SetMoney to get fixed, issue #10053
        return true;
    }
#endif

    void ShowTransmogItems(Player* player, Creature* creature, uint8 slot) // Only checks bags while can use an item from anywhere in inventory
    {
        WorldSession* session = player->GetSession();
        Item* oldItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot);
        if (oldItem)
        {
            uint32 limit = 0;
            uint32 price = sT->GetSpecialPrice(oldItem->GetTemplate());
            price *= sT->GetScaledCostModifier();
            price += sT->GetCopperCost();
            std::ostringstream ss;
            ss << std::endl;
            if (sT->GetRequireToken())
                ss << std::endl << std::endl << sT->GetTokenAmount() << " x " << sT->GetItemLink(sT->GetTokenEntry(), session);

            for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; ++i)
            {
                if (limit > MAX_OPTIONS)
                    break;
                Item* newItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i);
                if (!newItem)
                    continue;
                if (!sT->CanTransmogrifyItemWithItem(player, oldItem->GetTemplate(), newItem->GetTemplate()))
                    continue;
                if (sT->GetFakeEntry(oldItem->GetGUID()) == newItem->GetEntry())
                    continue;
                ++limit;
				player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, sT->GetItemIcon(newItem->GetEntry(), 30, 30, -18, 0) + sT->GetItemLink(newItem, session), slot, newItem->GetGUIDLow(), "用这件装备幻化以后将和你绑定，将不能交易！\n你要继续吗？\n\n" + sT->GetItemIcon(newItem->GetEntry(), 40, 40, -15, -10) + sT->GetItemLink(newItem, session) + ss.str(), price, false);
            }

            for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; ++i)
            {
                Bag* bag = player->GetBagByPos(i);
                if (!bag)
                    continue;
                for (uint32 j = 0; j < bag->GetBagSize(); ++j)
                {
                    if (limit > MAX_OPTIONS)
                        break;
                    Item* newItem = player->GetItemByPos(i, j);
                    if (!newItem)
                        continue;
                    if (!sT->CanTransmogrifyItemWithItem(player, oldItem->GetTemplate(), newItem->GetTemplate()))
                        continue;
                    if (sT->GetFakeEntry(oldItem->GetGUID()) == newItem->GetEntry())
                        continue;
                    ++limit;
					player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, sT->GetItemIcon(newItem->GetEntry(), 30, 30, -18, 0) + sT->GetItemLink(newItem, session), slot, newItem->GetGUIDLow(), "用这件装备幻化以后将和你绑定，将不能交易！\n你要继续吗？\n\n" + sT->GetItemIcon(newItem->GetEntry(), 40, 40, -15, -10) + sT->GetItemLink(newItem, session) + ss.str(), price, false);
                }
            }
        }

		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/INV_Enchant_Disenchant:30:30:-18:0|t 移除幻化效果 ", EQUIPMENT_SLOT_END + 3, slot, "你确定要移除这个部位的幻化效果么？ ", 0, false);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/PaperDollInfoFrame/UI-GearManager-Undo:30:30:-18:0|t 刷新菜单 ", EQUIPMENT_SLOT_END, slot);
        player->ADD_GOSSIP_ITEM(GOSSIP_ICON_MONEY_BAG, "|TInterface/ICONS/Ability_Spy:30:30:-18:0|t 返回 ", EQUIPMENT_SLOT_END+1, 0);
        player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
    }
};

class PS_Transmogrification : public PlayerScript
{
public:
    PS_Transmogrification() : PlayerScript("Player_Transmogrify") { }

    void OnLogin(Player* player)
    {
        uint64 playerGUID = player->GetGUID();
        sT->entryMap.erase(playerGUID);
        QueryResult result = CharacterDatabase.PQuery("SELECT GUID, FakeEntry FROM custom_transmogrification WHERE Owner = %u", player->GetGUIDLow());
        if (result)
        {
            do
            {
                uint64 itemGUID = MAKE_NEW_GUID((*result)[0].GetUInt32(), 0, HIGHGUID_ITEM);
                uint32 fakeEntry = (*result)[1].GetUInt32();
                if (sObjectMgr->GetItemTemplate(fakeEntry))
                {
                    sT->dataMap[itemGUID] = playerGUID;
                    sT->entryMap[playerGUID][itemGUID] = fakeEntry;
                }
                else
                {
                    //TC_LOG_ERROR->outError(LOG_FILTER_SQL, "Item entry (Entry: %u, itemGUID: %u, playerGUID: %u) does not exist, ignoring.", fakeEntry, GUID_LOPART(itemGUID), player->GetGUIDLow());
                    // CharacterDatabase.PExecute("DELETE FROM custom_transmogrification WHERE FakeEntry = %u", fakeEntry);
                }
            } while (result->NextRow());

            for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
            {
                if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
                    player->SetVisibleItemSlot(slot, item);
            }
        }

#ifdef PRESETS
        if (sT->GetEnableSets())
            sT->LoadPlayerSets(playerGUID);
#endif
    }

    void OnLogout(Player* player)
    {
        uint32 pGUID = player->GetGUID();
        for (Transmogrification::transmogData::const_iterator it = sT->entryMap[pGUID].begin(); it != sT->entryMap[pGUID].end(); ++it)
            sT->dataMap.erase(it->first);
        sT->entryMap.erase(pGUID);

#ifdef PRESETS
        if (sT->GetEnableSets())
            sT->UnloadPlayerSets(pGUID);
#endif
    }
};

class WS_Transmogrification : public WorldScript
{
public:
    WS_Transmogrification() : WorldScript("WS_Transmogrification") { }

    void OnAfterConfigLoad(bool reload)
    {
        sT->LoadConfig(reload);
    }

    void OnStartup()
    {
        sT->LoadConfig(false);
        //TC_LOG_ERROR->outInfo(LOG_FILTER_SERVER_LOADING, "Deleting non-existing transmogrification entries...");
        CharacterDatabase.Execute("DELETE FROM custom_transmogrification WHERE NOT EXISTS (SELECT 1 FROM item_instance WHERE item_instance.guid = custom_transmogrification.GUID)");

#ifdef PRESETS
        // Clean even if disabled
        // Dont delete even if player has more presets than should
        CharacterDatabase.Execute("DELETE FROM `custom_transmogrification_sets` WHERE NOT EXISTS(SELECT 1 FROM characters WHERE characters.guid = custom_transmogrification_sets.Owner)");
#endif
    }
};

void AddSC_CPWS_Transmogrification()
{
    new CS_Transmogrification();
    new PS_Transmogrification();
    new WS_Transmogrification();
}

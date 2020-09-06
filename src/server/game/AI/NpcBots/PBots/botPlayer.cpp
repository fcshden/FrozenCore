#include "Player.h"
#include "AccountMgr.h"
#include "AchievementMgr.h"
#include "ArenaTeam.h"
#include "ArenaTeamMgr.h"
#include "Battlefield.h"
#include "BattlefieldMgr.h"
#include "BattlefieldWG.h"
#include "Battleground.h"
#include "BattlegroundAV.h"
#include "BattlegroundMgr.h"
#include "CellImpl.h"
#include "Channel.h"
#include "ChannelMgr.h"
#include "CharacterDatabaseCleaner.h"
#include "Chat.h"
#include "Common.h"
#include "ConditionMgr.h"
#include "CreatureAI.h"
#include "DatabaseEnv.h"
#include "DisableMgr.h"
#include "Formulas.h"
#include "GameEventMgr.h"
#include "GossipDef.h"
#include "GridNotifiers.h"
#include "GridNotifiersImpl.h"
#include "Group.h"
#include "GroupMgr.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "GitRevision.h"
#include "revision.h"
#include "InstanceSaveMgr.h"
#include "InstanceScript.h"
#include "Language.h"
#include "LFGMgr.h"
#include "Log.h"
#include "LootItemStorage.h"
#include "MapInstanced.h"
#include "MapManager.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "OutdoorPvP.h"
#include "OutdoorPvPMgr.h"
#include "Pet.h"
#include "PetitionMgr.h"
#include "QuestDef.h"
#include "ReputationMgr.h"
#include "SkillDiscovery.h"
#include "SocialMgr.h"
#include "Spell.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "Transport.h"
#include "UpdateData.h"
#include "UpdateFieldFlags.h"
#include "UpdateMask.h"
#include "Util.h"
#include "Vehicle.h"
#include "Weather.h"
#include "WeatherMgr.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "ArenaSpectator.h"
#include "GameObjectAI.h"
#include "PoolMgr.h"
#include "SavingSystem.h"
#include "TicketMgr.h"
#include "ScriptMgr.h"
#include "GameGraveyard.h"

#include "Config.h"
// NPCBOT
#include "bothelper.h"
#include "BotSystem.h"
// NPCBOT


void Player::UpdateNpcBot(uint32 p_time)
{
    if (m_botTimer > 0)
    {
        if (p_time >= m_botTimer)
            m_botTimer = 0;
        else
            m_botTimer -= p_time;
    }
    else
        RefreshBot(p_time);
}

void Player::LoadBotInfo()
{
    if (sConfigMgr->GetBoolDefault("Bot.EnableNpcBots", true))
    {
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_NPCBOTS);
        stmt->setUInt32(0, GetGUIDLow());
        PreparedQueryResult result = CharacterDatabase.Query(stmt);
        if (result)
        {
            uint32 m_bot_entry = 0;
            uint8 m_bot_race = 0;
            uint8 m_bot_class = 0;
            uint32 equips[18];
            do
            {
                Field* fields = result->Fetch();
                m_bot_entry = fields[0].GetUInt32();
                m_bot_race = fields[1].GetUInt8();
                m_bot_class = fields[2].GetInt8();
                for (uint8 i = 0; i != 18; ++i)
                    equips[i] = fields[i + 4].GetUInt32();

                if (m_bot_entry && m_bot_race && m_bot_class)
                    SetBotMustBeCreated(m_bot_entry, m_bot_race, m_bot_class, equips);

            } while (result->NextRow());
        }
    }
}

void Player::SaveBotInfo()
{
    CharacterDatabase.DirectPExecute("DELETE FROM character_npcbot WHERE owner = %u", GetGUIDLow());

    for (BotInfoMap::const_iterator itr = m_botInfo.begin(); itr != m_botInfo.end(); ++itr)
    {
        BotInfo mb = itr->second;
        //entry is unique for each master's bot so clean it up just in case
        PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_NPCBOT);
        uint8 i = 0;
        stmt->setUInt32(i++, GetGUIDLow());
        stmt->setUInt32(i++, mb.entry);
        stmt->setUInt8(i++, mb.race);
        stmt->setUInt8(i++, mb.bclass);
        stmt->setUInt8(i++, uint8(mb.roles));

        for (uint8 j = 0; j != 18; ++j)
            stmt->setUInt32(i++, mb.equips[j]);

        stmt->setUInt8(i++, uint8(1)); //active
        CharacterDatabase.Execute(stmt);
    }
}

BotInfo const* Player::GetBotInfo(uint32 entry) const
{
    BotInfoMap::const_iterator itr = m_botInfo.find(entry);
    if (itr != m_botInfo.end())
        return &itr->second;
    return NULL;
}

uint32 Player::GetPlayerBotRoles(uint32 entry)
{
    BotInfoMap::const_iterator itr = m_botInfo.find(entry);
    if (itr != m_botInfo.end())
        return itr->second.roles;
    return 0;
}

void Player::SetBotInfo(uint32 entry, uint32 race, uint32 pclass, uint32 roles, uint32 * equips, uint32 active)
{
    BotInfo info;
    info.entry = entry;
    info.race = race;
    info.bclass = pclass;
    info.roles = roles;
    for (uint8 i = 0; i != 18; ++i)
        info.equips[i] = equips[i];
    info.active = active;
    m_botInfo[info.entry] = info;
}

void Player::SetBotActive(uint32 entry, uint32 active)
{
    BotInfoMap::const_iterator itr = m_botInfo.find(entry);
    if (itr != m_botInfo.end())
    {
        m_botInfo[entry].active = active;
    }
}

void Player::SetBotQquips(uint32 entry, uint32 * equips)
{
    BotInfoMap::const_iterator itr = m_botInfo.find(entry);
    if (itr != m_botInfo.end())
    {
        for (int i = 0; i != 18; ++i)
            m_botInfo[entry].equips[i] = equips[i];
    }
}

void Player::SetBotRoles(uint32 entry, uint32 roles)
{
    BotInfoMap::const_iterator itr = m_botInfo.find(entry);
    if (itr != m_botInfo.end())
    {
        m_botInfo[entry].roles = roles;
    }
}

void Player::SetBotRace(uint32 pos, uint32 race)
{
    NpcBotMap const* m_tmp = GetBotMap(pos);
    if (!m_tmp)
        return;
    m_botmap[pos]->m_race = race;
}

void Player::UpdateBotModelid(Creature* bot)
{
    /*	CreatureOutfit co;

        co.race = 1;
        co.gender = bot->getGender();
        //co.displayId = bot->GetNativeDisplayId();
        co.skin = 0;
        co.face = 0;
        co.hair = 0;
        co.haircolor = 0;
        co.facialhair = 0;

        for (uint8 i = 0; i != MAX_CREATURE_OUTFIT_DISPLAYS; ++i)
            co.outfit[i] = 0;

        if (uint32 itemId = GetBotEquip(bot, 3))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[0] = proto->DisplayInfoID;

        if (uint32 itemId = GetBotEquip(bot, 4))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[1] = proto->DisplayInfoID;

        if (uint32 itemId = GetBotEquip(bot, 12))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[2] = proto->DisplayInfoID;


        if (uint32 itemId = GetBotEquip(bot, 5))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[3] = proto->DisplayInfoID;

        if (uint32 itemId = GetBotEquip(bot, 6))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[4] = proto->DisplayInfoID;

        if (uint32 itemId = GetBotEquip(bot, 7))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[5] = proto->DisplayInfoID;

        if (uint32 itemId = GetBotEquip(bot, 8))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[6] = proto->DisplayInfoID;

        if (uint32 itemId = GetBotEquip(bot, 9))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[7] = proto->DisplayInfoID;

        if (uint32 itemId = GetBotEquip(bot, 10))
            if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(itemId))
                co.outfit[8] = proto->DisplayInfoID;

        WorldPacket data(SMSG_MIRRORIMAGE_DATA, 68);
        data << uint64(bot->ToUnit()->GetGUID());
        data << uint32(bot->GetNativeDisplayId()); // displayId
        data << uint8(co.race);             // race
        data << uint8(co.gender);           // gender
        data << uint8(bot->getClass());            // class
        data << uint8(co.skin);             // skin
        data << uint8(co.face);             // face
        data << uint8(co.hair);             // hair
        data << uint8(co.haircolor);        // haircolor
        data << uint8(co.facialhair);       // facialhair
        data << uint32(0);                          // guildId

        // item displays
        for (uint8 i = 0; i != MAX_CREATURE_OUTFIT_DISPLAYS; ++i)
            data << uint32(co.outfit[i]);

        GetSession()->SendPacket(&data);*/
    return;
}

void Player::RefreshBot(uint32 diff)
{
    if (m_botTimer > 0)
        return;

    if (IsInFlight())
        m_botTimer = 3000;

    if (!HaveBot())
        return;

    //BOT REVIVE SUPPORT part 2
    //Revive timer condition (maybe we should check whole party?)
    bool partyInCombat = IsInCombat();
    if (!partyInCombat)
    {
        for (uint8 i = 0; i != GetMaxNpcBots(); ++i)
        {
            if (Creature* bot = m_botmap[i]->m_creature)
            {
                if (bot->IsInCombat())
                {
                    partyInCombat = true;
                    break;
                }
                else if (Creature* pet = bot->GetBotsPet())
                {
                    if (pet->IsInCombat())
                    {
                        partyInCombat = true;
                        break;
                    }
                }
            }
        }
    }

    for (uint8 i = 0; i != GetMaxNpcBots(); ++i)
    {
        uint64 guid = m_botmap[i]->m_guid;
        m_bot = m_botmap[i]->m_creature;

        //BOT REVIVE SUPPORT part 2
        //Do not allow bot to be revived if master is in battle
        if (!partyInCombat)
        {
            if (m_botmap[i]->m_reviveTimer > diff)
            {
                if (!IsInCombat())
                    m_botmap[i]->m_reviveTimer -= diff;
            }
            else if (m_botmap[i]->m_reviveTimer > 0)
                m_botmap[i]->m_reviveTimer = 0;
        }

        if (!m_bot || !m_bot->IsInWorld())
            continue;

        //!!!BOT UPDATE HELPER!!!
        m_bot->SetCanUpdate(true);
        m_bot->IsAIEnabled = true;

        //BOT REVIVE SUPPORT part 3
        //Revive bot if possible
        if (m_botmap[i]->m_reviveTimer == 0)
        {
            if (m_bot->isDead() && IsAlive() && !IsInCombat() && !InArena() && !IsInFlight() &&
                !HasFlag(UNIT_FIELD_FLAGS_2, UNIT_FLAG2_FEIGN_DEATH) &&
                !HasInvisibilityAura() && !HasStealthAura())
            {
                CreateBot(0, 0, 0, true); //revive
                continue;
            }

            //delay next attempt
            m_botmap[i]->m_reviveTimer = 500;
        }

        //BOT MUST DIE SUPPORT
        if (IsInFlight() || !GetGroup() || !GetGroup()->IsMember(m_bot->GetGUID()))//even if bot is dead
        {
            RemoveBot(guid, !IsInFlight(), false);
            continue;
        }

        //TELEPORT SUPPORT
        if (!IsInFlight() && IsAlive() && (m_bot->IsAlive() || m_bot->GetMapId() != GetMapId() || RestrictBots()))
        {
            if (m_bot->GetMapId() != GetMapId() || RestrictBots())
            {
                RemoveBot(guid);
                continue;
            }
            else if (m_bot->GetBotCommandState() != COMMAND_STAY)
            {
                if (GetDistance(m_bot) > SIZE_OF_GRIDS)
                {
                    ////This thing is not confirmed to be stable
                    //m_bot->SetOwnerGUID(0);
                    //m_bot->RemoveFromWorld();
                    //m_bot->Relocate(this);
                    ////Creature::AddToWorld(): Skip AIM_Initialize();
                    //if (m_bot->GetZoneScript())
                    //    m_bot->GetZoneScript()->OnCreatureCreate(m_bot);
                    //sObjectAccessor->AddObject(m_bot);
                    //m_bot->ToUnit()->AddToWorld();
                    //m_bot->SetOwnerGUID(GetGUID());

                    //This thing is unsafe
                    m_bot->SetBotsPetDied();
                    m_bot->OnBotDespawn(NULL);
                    m_bot->InterruptNonMeleeSpells(true);
                    m_bot->AttackStop();
                    m_bot->RemoveAllAttackers();
                    m_bot->DeleteThreatList();
                    m_bot->ClearInCombat();

                    m_bot->RemoveNotOwnSingleTargetAuras();
                    //m_bot->RemoveAllGameObjects();
                    //m_bot->RemoveAllDynObjects();
                    //m_bot->ExitVehicle();
                    //m_bot->UnsummonAllTotems();
                    //m_bot->RemoveAllControlled();
                    m_bot->RemoveAreaAurasDueToLeaveWorld();
                    m_bot->DestroyForNearbyPlayers();
                    m_bot->ClearUpdateMask(true);
                    //sObjectAccessor->RemoveObject(m_bot);
                    m_bot->Relocate(this);
                    //sObjectAccessor->AddObject(m_bot);
                    m_bot->ToUnit()->AddToWorld();
                }
            }
        }

        //Update bots manually and prevent from normal updates
        //This will update bot's AI
        m_bot->Update(diff);
        m_bot->SetCanUpdate(false);

        //Update bot's pet manually and prevent from normal updates
        //This will update pet's AI
        if (Creature* pet = m_bot->GetBotsPet())
        {
            pet->SetCanUpdate(true);
            pet->IsAIEnabled = true;
            pet->Update(diff);
            pet->SetCanUpdate(false);
        }

        m_bot = NULL;
    }//end for botmap

    //BOT CREATION/RECREATION SUPPORT

    if (m_botCreateTimer > diff)
        m_botCreateTimer -= diff;
    else
    {
        m_botCreateTimer = 250;

        if (!IsInFlight() && IsAlive() && !IsInCombat() && GetBotMustBeCreated() && !RestrictBots())
            for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
                if (m_botmap[pos]->m_entry != 0 && m_botmap[pos]->m_guid == 0)
                    CreateBot(m_botmap[pos]->m_entry, m_botmap[pos]->m_race, m_botmap[pos]->m_class);
    }
}

void Player::SetBotMustBeCreated(uint32 m_entry, uint8 m_race, uint8 m_class, uint32 *equips)
{
    if (m_enableNpcBots == false)
    {
        ChatHandler ch(GetSession());
        ch.SendSysMessage("NpcBot system currently disabled. Please contact your administration.");
        ClearBotMustBeCreated(0, 0, true);
        return;
    }
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
    {
        if (m_botmap[pos]->m_entry == 0)
        {
            m_botmap[pos]->m_guid = 0;//we need it to make sure Player::CreateBot will find this slot
            m_botmap[pos]->m_entry = m_entry;
            m_botmap[pos]->m_race = m_race;
            m_botmap[pos]->m_class = m_class;

            for (uint8 i = 0; i != 18; ++i)
                m_botmap[pos]->equips[i] = equips[i];

            break;
        }
    }
}

bool Player::GetBotMustBeCreated()
{
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
    {
        if (m_botmap[pos]->m_entry != 0 &&
            (m_botmap[pos]->m_guid == 0 || !sObjectAccessor->FindUnit(m_botmap[pos]->m_guid)))
        {
            m_botmap[pos]->m_guid = 0;
            return true;
        }
    }
    return false;
}

void Player::ClearBotMustBeCreated(uint64 guidOrSlot, bool guid, bool fully)
{
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
    {
        if ((guid == true && m_botmap[pos]->m_guid == guidOrSlot) ||
            (guid == false && pos == guidOrSlot) ||
            fully)
        {
            m_botmap[pos]->m_guid = 0;
            m_botmap[pos]->m_entry = 0;
            m_botmap[pos]->m_race = 0;
            m_botmap[pos]->m_class = 0;
            m_botmap[pos]->m_creature = NULL;

            for (uint8 i = 0; i != 18; ++i)
                m_botmap[pos]->equips[i] = 0;

            if (!fully)
                break;
        }
    }
}

void Player::RemoveBot(uint64 guid, bool final, bool eraseFromDB)
{
    if (guid == 0) return;
    for (uint8 i = 0; i != GetMaxNpcBots(); ++i)
    {
        if (m_botmap[i]->m_guid == guid)
        {
            m_bot = m_botmap[i]->m_creature;
            break;
        }
    }
    if (!m_bot)
        m_bot = sObjectAccessor->GetObjectInWorld(guid, (Creature*)NULL);
    if (m_bot)
    {
        //do not disband group unless not in dungeon or forced or on logout (Check WorldSession::LogoutPlayer())
        Group* gr = GetGroup();
        if (gr && gr->IsMember(guid))
        {
            if (gr->GetMembersCount() > 2 || /*!GetMap()->Instanceable() || */(final && eraseFromDB))
                gr->RemoveMember(guid);
            else //just cleanup
            {
                PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_MEMBER);
                stmt->setUInt32(0, GUID_LOPART(guid));
                stmt->setUInt32(1, GetGUIDLow());
                CharacterDatabase.Execute(stmt);
            }
        }

        m_bot->SetBotsPetDied();
        m_bot->OnBotDespawn(NULL);
        m_bot->SetCharmerGUID(0);
        //m_bot->SetBotOwner(NULL);
        m_bot->SetIAmABot(false);
        SetMinion((Minion*)m_bot, false);
        m_bot->CleanupsBeforeDelete();
        m_bot->AddObjectToRemoveList();

        if (final)//on logout or by command
        {
            ClearBotMustBeCreated(guid);
            if (eraseFromDB)//by command
                SetBotActive(m_bot->GetEntry(), 0);

            if (eraseFromDB)//删除数据库
            {
                m_botInfo.erase(m_bot->GetEntry());
                CharacterDatabase.DirectPExecute("DELETE FROM character_npcbot WHERE owner = %u and entry = %u", GetGUIDLow(), m_bot->GetEntry());
            }
        }
        else
        {
            ModifyMoney(int64(GetNpcBotCost())); //temp restore money before retake

            for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
            {
                if (m_botmap[pos]->m_guid == guid)
                {
                    m_botmap[pos]->m_guid = 0;//reset guid so it can be set during recreation
                    m_botmap[pos]->m_creature = NULL;
                }
            }
        }
        m_bot = NULL;
    }
}

void Player::CreateBot(uint32 botentry, uint8 botrace, uint8 botclass, bool revive)
{

    if (IsBeingTeleported() || IsInFlight()) return; //don't create bot yet
    if (isDead() && !revive) return; //not to revive by command so abort
    if (IsInCombat()) return;

    if (m_bot != NULL && revive)
    {
        m_bot->SetHealth(m_bot->GetCreateHealth() / 6);//~15% of base health
        if (m_bot->getPowerType() == POWER_MANA)
            m_bot->SetPower(POWER_MANA, m_bot->GetCreateMana());
        SetUInt32Value(UNIT_NPC_FLAGS, m_bot->GetCreatureTemplate()->npcflag);
        ClearUnitState(uint32(UNIT_STATE_ALL_STATE));
        m_bot->setDeathState(ALIVE);
        m_bot->SetBotCommandState(COMMAND_FOLLOW, true);
        return;
    }
    if (m_enableNpcBots == false && revive == false)
    {
        ChatHandler ch(GetSession());
        ch.SendSysMessage("NpcBot system currently disabled. Please contact administration.");
        for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
            if (m_botmap[pos]->m_entry == botentry)
                ClearBotMustBeCreated(pos, false);
        return;
    }
    if (!botentry || !botrace || !botclass)
    {
        ////TC_LOG_ERROR("entities.player", "ERROR! CreateBot(): player %s (%u) trying to create bot with entry = %u, race = %u, class = %u, ignored", GetName().c_str(), GetGUIDLow(), botentry, botrace, botclass);
        for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
            if (m_botmap[pos]->m_entry == botentry)
                ClearBotMustBeCreated(pos, false);
        return;
    }

    if (GetNpcBotsCount() > GetMaxNpcBots())
    {
        ChatHandler ch(GetSession());
        for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
            if (m_botmap[pos]->m_entry == botentry)
                ClearBotMustBeCreated(pos, false);
        ch.PSendSysMessage("Youre exceed max npcbots");
        ch.SetSentErrorMessage(true);
        return;
    }
    //instance limit check
    if ((m_limitNpcBotsDungeons && GetMap()->IsNonRaidDungeon()) || (m_limitNpcBotsRaids && GetMap()->IsRaid()))
    {
        InstanceMap* map = (InstanceMap*)GetMap();
        uint32 count = 0;
        Map::PlayerList const& plMap = map->GetPlayers();
        for (Map::PlayerList::const_iterator itr = plMap.begin(); itr != plMap.end(); ++itr)
            if (Player* player = itr->GetSource())
                count += (1 + player->GetNpcBotsCount());

        //check "more" cuz current bot is queued and we are to choose to remove it or not
        if (count > map->GetMaxPlayers())
        {
            ChatHandler ch(GetSession());
            for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
                if (m_botmap[pos]->m_entry == botentry)
                    ClearBotMustBeCreated(pos, false);
            ch.PSendSysMessage("Instance players limit exceed");
            ch.SetSentErrorMessage(true);
            return;
        }
    }
    if (GetGroup() && GetGroup()->isRaidGroup() && GetGroup()->IsFull())
    {
        ChatHandler ch(GetSession());
        ch.PSendSysMessage("Your group is Full!");
        ch.SetSentErrorMessage(true);
        return;
    }
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
        if (m_botmap[pos]->m_entry == botentry)
            if (m_botmap[pos]->m_reviveTimer != 0)
                return;

    m_bot = SummonCreature(botentry, *this);

    //check if we have free slot
    bool _set = false;
    uint8 slot = 0;
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
    {
        if (m_botmap[pos]->m_entry == botentry && m_botmap[pos]->m_guid == 0)
        {
            m_botmap[pos]->m_guid = m_bot->GetGUID();
            m_botmap[pos]->m_creature = m_bot;//this will save some time but we need guid as well
            slot = pos;
            _set = true;
            break;
        }
    }
    if (!_set)
    {
        ////TC_LOG_ERROR("entities.player", "character %s (%u) is failed to create npcbot! Removing all bots", GetName().c_str(), GetGUIDLow());

        m_bot->CombatStop();
        m_bot->CleanupsBeforeDelete();
        m_bot->AddObjectToRemoveList();
        for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
            RemoveBot(m_botmap[pos]->m_guid, true);
        ClearBotMustBeCreated(0, false, true);
        return;
    }

    m_bot->SetBotOwner(this);

    m_bot->SetUInt64Value(UNIT_FIELD_CREATEDBY, GetGUID());
    SetMinion((Minion*)m_bot, true);
    m_bot->CombatStop();
    m_bot->DeleteThreatList();
    m_bot->AddUnitTypeMask(UNIT_MASK_MINION);

    m_bot->SetByteValue(UNIT_FIELD_BYTES_0, 0, botrace);
    m_bot->setFaction(getFaction());
    m_bot->SetLevel(getLevel());
    m_bot->SetBotClass(botclass);
    m_bot->AIM_Initialize();
    m_bot->InitBotAI();
    m_bot->SetBotCommandState(COMMAND_FOLLOW, true);

    InitBotEquips(m_bot);

    SetBotInfo(botentry, botrace, botclass, m_bot->GetBotRoles(), m_botmap[slot]->equips, uint8(1));

    //If we have a group, just add bot
    if (Group* gr = GetGroup())
    {
        if (!gr->IsFull())
        {
            if (!gr->AddMember((Player*)m_bot))
                RemoveBot(m_bot->GetGUID(), true);
        }
        else if (!gr->isRaidGroup()) //non-raid group is full
        {
            gr->ConvertToRaid();
            if (!gr->AddMember((Player*)m_bot))
                RemoveBot(m_bot->GetGUID(), true);
        }
        else //raid group is full
            RemoveBot(m_bot->GetGUID(), true);
    }
    else
    {
        gr = new Group;
        if (!gr->Create(this))
        {
            delete gr;
            return;
        }
        sGroupMgr->AddGroup(gr);
        if (!gr->AddMember((Player*)m_bot))
            RemoveBot(m_bot->GetGUID(), true);
    }

    if (uint32 cost = GetNpcBotCost())
        ModifyMoney(-(int32(cost)));

    UpdateBotModelid(m_bot);

    if (Group* gr = GetGroup())
    {
        Group::MemberSlotList const a = gr->GetMemberSlots();
        //try to remove 'absent' bots
        for (Group::member_citerator itr = a.begin(); itr != a.end(); ++itr)
        {
            if (itr->guid == 0)
                continue;
            if (IS_PLAYER_GUID(itr->guid))
                continue;
            if (!sObjectAccessor->FindUnit(itr->guid))
                gr->RemoveMember(itr->guid);
        }
    }

    // NPCBOT
    m_bot->SetBotShouldUpdateStats();
    // NPCBOT
} //end Player::CreateBot

uint8 Player::GetNpcBotsCount() const
{
    uint8 bots = 0;
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
        if (m_botmap[pos]->m_entry != 0)
            ++bots;
    return bots;
}

uint8 Player::GetMaxNpcBots() const
{
    uint8 botcount = (GetSession()->GetSecurity() == SEC_PLAYER) ? m_maxNpcBots : MAX_NPCBOTS;
    if (botcount > 20)
        return 20;
    return botcount;
}

bool Player::HaveBot() const
{
    for (uint8 i = 0; i != GetMaxNpcBots(); ++i)
        if (m_botmap[i]->m_entry != 0)
            return true;
    return false;
}

void Player::SendBotCommandState(Creature* cre, CommandStates state)
{
    if (!cre) return;
    for (uint8 i = 0; i != GetMaxNpcBots(); ++i)
        if (m_botmap[i]->m_creature == cre)
            cre->SetBotCommandState(state, true);
}
//finds bot's slot into master's botmap
int8 Player::GetNpcBotSlot(uint64 guid) const
{
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
        if (m_botmap[pos]->m_guid == guid)
            return pos;

    return -1;
}

void Player::SetNpcBotDied(uint64 guid)
{
    if (!guid) return;
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
        if (m_botmap[pos]->m_guid == guid)
        {
            m_botmap[pos]->m_reviveTimer = 15000;
            break;
        }
}

bool Player::RestrictBots() const
{
    return
        (!m_enableNpcBotsBGs && GetMap()->IsBattleground()) ||
        (!m_enableNpcBotsArenas && GetMap()->IsBattleArena()) ||
        (!m_enableNpcBotsDungeons && GetMap()->IsNonRaidDungeon()) ||
        (!m_enableNpcBotsRaids && GetMap()->IsRaid());
}

uint32 Player::GetNpcBotCost() const
{
    return m_NpcBotsCost ? uint32((m_NpcBotsCost / 80.f) * getLevel()) : 0;
}

std::string Player::GetNpcBotCostStr() const
{
    std::ostringstream money;

    if (uint32 cost = GetNpcBotCost())
    {
        uint32 gold = uint32(cost / 10000);
        cost -= (gold * 10000);
        uint32 silver = uint32(cost / 100);
        cost -= (silver * 100);

        if (gold != 0)
            money << gold << " |TInterface\\Icons\\INV_Misc_Coin_01:8|t";
        if (silver != 0)
            money << silver << " |TInterface\\Icons\\INV_Misc_Coin_03:8|t";
        if (cost)
            money << cost << " |TInterface\\Icons\\INV_Misc_Coin_05:8|t";
    }
    return money.str();
}

void Player::CreateEntryBot(uint32 BotId)
{
    CreatureTemplate const* Info = sObjectMgr->GetCreatureTemplate(BotId);
    if (!Info)
        return;

    if (BotId < 80001 || BotId > 80248)
        return;

    //check if we have too many bots of that class
    if (HaveBot())
    {
        uint8 count = 0;
        for (uint8 i = 0; i != GetMaxNpcBots(); ++i)
            if (m_botmap[i]->m_class == Info->trainer_class)
                ++count;

        if (count >= m_maxClassNpcBots)
        {
            //SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
            ChatHandler ch(GetSession());
            ch.PSendSysMessage("You cannot have more bots of that class! %u of %u", count, m_maxClassNpcBots);
            ch.SetSentErrorMessage(true);
            return;
        }
    }

    //check if player cannot afford a bot
    if (GetMoney() < GetNpcBotCost())
    {
        ChatHandler ch(GetSession());
        ch.SetSentErrorMessage(true);
        return;
    }

    uint32 equips[18];
    for (uint8 i = 0; i != 18; ++i)
        equips[i] = 0;

    BotInfo const* m_equips = GetBotInfo(BotId);
    if (m_equips)
    {
        for (uint8 i = 0; i != 18; ++i)
            equips[i] = m_equips->equips[i];
    }

    SetBotMustBeCreated(BotId, Info->trainer_race, Info->trainer_class, equips);
}

//NPCbot base setup
void Player::CreateNPCBot(uint8 bot_class)
{
    //check if we have too many bots of that class
    if (HaveBot())
    {
        uint8 count = 0;
        for (uint8 i = 0; i != GetMaxNpcBots(); ++i)
            if (m_botmap[i]->m_class == bot_class)
                ++count;
        if (count >= m_maxClassNpcBots)
        {
            //SendBuyError(BUY_ERR_NOT_ENOUGHT_MONEY, 0, 0, 0);
            ChatHandler ch(GetSession());
            ch.PSendSysMessage("You cannot have more bots of that class! %u of %u", count, m_maxClassNpcBots);
            ch.SetSentErrorMessage(true);
            return;
        }
    }

    //check if player cannot afford a bot
    if (GetMoney() < GetNpcBotCost())
    {
        ChatHandler ch(GetSession());
        std::string str = "You don't have enough money (";
        str += GetNpcBotCostStr();
        str += ")!";
        ch.SendSysMessage(str.c_str());
        ch.SetSentErrorMessage(true);
        return;
    }


    NpcBotsDataTemplate npcBotsData;
    uint32 npcBotsDataCount;
    std::ostringstream classStr;
    switch (bot_class)
    {
    case CLASS_ROGUE:
        classStr << "rogue_bot";        break;
    case CLASS_PRIEST:
        classStr << "priest_bot";       break;
    case CLASS_DRUID:
        classStr << "druid_bot";        break;
    case CLASS_SHAMAN:
        classStr << "shaman_bot";       break;
    case CLASS_MAGE:
        classStr << "mage_bot";         break;
    case CLASS_WARLOCK:
        classStr << "warlock_bot";      break;
    case CLASS_WARRIOR:
        classStr << "warrior_bot";      break;
    case CLASS_PALADIN:
        classStr << "paladin_bot";      break;
    case CLASS_HUNTER:
        classStr << "hunter_bot";       break;
    case CLASS_DEATH_KNIGHT:
        classStr << "death_knight_bot"; break;
    default:
        ChatHandler ch(GetSession());
        ch.PSendSysMessage("ERROR! unknown bot_class %u", bot_class);
        ch.SetSentErrorMessage(true);
        //TC_LOG_ERROR(LOG_FILTER_PLAYER, "Player::CreateNPCBot() player %u(%s) tried to create bot of unknown/unsupported class %u!", GetGUIDLow(), GetName().c_str(), bot_class);
        return;
    }

    switch (getRace())
    {
    case RACE_NONE:
    case RACE_HUMAN:
    case RACE_DWARF:
    case RACE_NIGHTELF:
    case RACE_GNOME:
    case RACE_DRAENEI:
        npcBotsData = sBotTemplate->GetNPCBotTemplate(bot_class, classStr.str(), 1, 3, 4, 7, 11);
        npcBotsDataCount = sBotTemplate->GetNPCBotTemplateCount(bot_class, classStr.str(), 1, 3, 4, 7, 11);
        break;

    case RACE_ORC:
    case RACE_UNDEAD_PLAYER:
    case RACE_TAUREN:
    case RACE_TROLL:
    case RACE_BLOODELF:
        npcBotsData = sBotTemplate->GetNPCBotTemplate(bot_class, classStr.str(), 2, 5, 6, 8, 10);
        npcBotsDataCount = sBotTemplate->GetNPCBotTemplateCount(bot_class, classStr.str(), 2, 5, 6, 8, 10);
        break;
    }

    if (npcBotsDataCount < 1)
    {
        sLog->outString("Player::CreateNPCBot() CANNOT create bot of class %u, not found in DB!", bot_class);
        return;
    }

    uint32 entry = 0;
    uint32 bot_race = 0;

    uint32 m_rand = urand(1, npcBotsDataCount);
    uint32 tmp_rand = 1;
    std::list< std::pair<uint32, uint8> >::const_iterator itr = npcBotsData.begin();
    bool haveSameBot = false;
    bool moveback = false;
    bool forcedCheck = false;
    bool secondCheck = false;
    while (true)
    {
        if (itr == npcBotsData.end()) //end of list is reached (selected bot is checked)
        {
            moveback = true;
            --itr; //tmp_rand is not needed anymore
            continue;
        }
        if (moveback && itr == npcBotsData.begin()) //search is finished, nothing found
            break;
        if (tmp_rand == m_rand || haveSameBot)
        {
            bool canAdd = true;
            for (uint8 i = 0; i != GetMaxNpcBots(); ++i)
            {
                if (m_botmap[i]->m_entry == itr->first)
                {
                    haveSameBot = true;
                    canAdd = false;
                    if (!secondCheck)
                        forcedCheck = true;
                    secondCheck = true;
                    break;
                }
            }
            if (canAdd)
            {
                entry = itr->first;
                bot_race = itr->second;
                break;
            }
            if (forcedCheck)
            {
                itr = npcBotsData.begin(); //reset searcher pos
                forcedCheck = false;
                continue;
            }
        }
        //move through
        if (moveback)
            --itr;
        else
        {
            ++itr;
            ++tmp_rand;
        }
    }

    if (!entry || !bot_race)
    {
        ChatHandler ch(GetSession());
        ch.SendSysMessage("No more bots of this class available");
        ch.SetSentErrorMessage(true);
        return;
    }

    uint32 equips[18];
    for (uint8 i = 0; i != 18; ++i)
        equips[i] = 0;

    BotInfo const* m_equips = GetBotInfo(entry);
    if (m_equips)
    {
        for (uint8 i = 0; i != 18; ++i)
            equips[i] = m_equips->equips[i];
    }

    SetBotMustBeCreated(entry, bot_race, bot_class, equips);
}

void Player::InitBotEquips(Creature* bot)
{
    int8 id = 1;
    EquipmentInfo const* einfo = sObjectMgr->GetEquipmentInfo(bot->GetEntry(), id);

    uint8 slot = 0;
    //Load stored equipment if any
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
    {
        if (m_botmap[pos]->m_creature == bot)
        {
            slot = pos;

            bot->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0, m_botmap[pos]->equips[0]);
            bot->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1, m_botmap[pos]->equips[1]);
            bot->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2, m_botmap[pos]->equips[2]);

            if (uint32 mh = bot->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0))
            {
                if (!einfo || einfo->ItemEntry[0] != mh)
                {
                    if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(mh))
                    {
                        bot->SetAttackTime(BASE_ATTACK, proto->Delay);
                        bot->ApplyBotItemBonuses(0);
                    }
                }
            }
            else
                bot->SetAttackTime(BASE_ATTACK, bot->GetCreatureTemplate()->BaseAttackTime);

            if (uint32 oh = bot->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1))
            {
                if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(oh))
                {
                    if (!einfo || einfo->ItemEntry[1] != oh)
                        bot->ApplyBotItemBonuses(1);

                    if (proto->Class == ITEM_CLASS_WEAPON)
                    {
                        bot->SetAttackTime(OFF_ATTACK, proto->Delay);
                        bot->SetCanDualWield(true);
                    }
                    else if (proto->Class == ITEM_CLASS_ARMOR && proto->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD)
                    {
                        if (bot->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_BLOCK)
                            const_cast<CreatureTemplate*>(bot->GetCreatureTemplate())->flags_extra &= ~CREATURE_FLAG_EXTRA_NO_BLOCK;
                    }
                }
            }
            else
                bot->SetAttackTime(OFF_ATTACK, bot->GetCreatureTemplate()->BaseAttackTime);

            if (uint32 rh = bot->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2))
            {
                if (!einfo || einfo->ItemEntry[2] != rh)
                {
                    if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(rh))
                    {
                        bot->SetAttackTime(RANGED_ATTACK, proto->Delay);
                        bot->ApplyBotItemBonuses(2);
                    }
                }
            }
            else
                bot->SetAttackTime(OFF_ATTACK, bot->GetCreatureTemplate()->RangeAttackTime);

            for (uint8 i = 3; i != 18; ++i)
                bot->ApplyBotItemBonuses(i);

            break;
        }
    }


    //Load remaining items as defaults
    if (einfo)
    {
        for (uint8 i = 0; i != MAX_EQUIPMENT_ITEMS; ++i)
        {
            if (bot->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + i) == 0 && einfo->ItemEntry[i] != 0)
            {
                bot->SetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + i, einfo->ItemEntry[i]);
                if (i == 1) //off-hand
                {
                    if (einfo->ItemEntry[i] != 0 && bot->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + i) == einfo->ItemEntry[i])
                    {
                        if (ItemTemplate const* proto = sObjectMgr->GetItemTemplate(einfo->ItemEntry[i]))
                        {
                            if (proto->Class == ITEM_CLASS_WEAPON)
                            {
                                bot->SetAttackTime(OFF_ATTACK, bot->GetBotClass() == CLASS_ROGUE ? 1400 : 1800);
                                bot->SetCanDualWield(true);
                            }
                            else if (proto->Class == ITEM_CLASS_ARMOR && proto->SubClass == ITEM_SUBCLASS_ARMOR_SHIELD)
                            {
                                if (bot->GetCreatureTemplate()->flags_extra & CREATURE_FLAG_EXTRA_NO_BLOCK)
                                    const_cast<CreatureTemplate*>(bot->GetCreatureTemplate())->flags_extra &= ~CREATURE_FLAG_EXTRA_NO_BLOCK;
                            }
                        }
                    }
                }
            }
        }

        //while creating new bot he has no equips but equip template so write these to bot map
        m_botmap[slot]->equips[0] = bot->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 0);
        m_botmap[slot]->equips[1] = bot->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 1);
        m_botmap[slot]->equips[2] = bot->GetUInt32Value(UNIT_VIRTUAL_ITEM_SLOT_ID + 2);
    }
}

uint32 Player::GetBotEquip(Creature* bot, uint8 slot) const
{
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
        if (m_botmap[pos]->m_creature == bot)
            return m_botmap[pos]->equips[slot];

    return 0;
}

void Player::UpdateBotEquips(Creature* bot, uint8 slot, uint32 itemId)
{
    for (uint8 pos = 0; pos != GetMaxNpcBots(); ++pos)
    {
        if (m_botmap[pos]->m_creature == bot)
        {
            m_botmap[pos]->equips[slot] = itemId;

            SetBotQquips(m_botmap[pos]->m_entry, m_botmap[pos]->equips);
            break;
        }
    }
}

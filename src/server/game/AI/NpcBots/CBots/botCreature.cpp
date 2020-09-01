#include "Creature.h"
#include "Player.h"
#include "../game/AI/NpcBots/bot_ai.h"

uint8 Creature::GetBotClass() const
{
    switch (m_bot_class)
    {
    case DRUID_BEAR_FORM:
    case DRUID_CAT_FORM:
        //case TRAVEL:
        //case FLY:
        return CLASS_DRUID;
    default:
        return m_bot_class;
    }
}

void Creature::SetIAmABot(bool bot)
{
    if (!bot)
    {
        bot_AI->UnsummonAll();
        IsAIEnabled = false;
        bot_AI = NULL;
        SetUInt64Value(UNIT_FIELD_CREATEDBY, 0);
    }
}

void Creature::SetBotsPetDied()
{
    if (!m_bots_pet)
        return;

    m_bots_pet->SetCharmerGUID(0);
    m_bots_pet->SetCreatureOwner(NULL);
    //m_bots_pet->GetBotPetAI()->SetCreatureOwner(NULL);
    m_bots_pet->SetIAmABot(false);
    m_bot_owner->SetMinion((Minion*)m_bots_pet, false);
    m_bots_pet->CleanupsBeforeDelete();
    m_bots_pet->AddObjectToRemoveList();
    m_bots_pet = NULL;
}

uint8 Creature::GetBotRoles() const
{
    return bot_AI ? bot_AI->GetBotRoles() : 0;
}

void Creature::SetBotCommandState(CommandStates st, bool force)
{
    if (bot_AI && IsAIEnabled)
        bot_AI->SetBotCommandState(st, force);
}
CommandStates Creature::GetBotCommandState() const
{
    return bot_AI ? bot_AI->GetBotCommandState() : COMMAND_ABANDON;
}
//Bot damage mods
void Creature::ApplyBotDamageMultiplierMelee(uint32& damage, CalcDamageInfo& damageinfo) const
{
    if (bot_AI)
        bot_AI->ApplyBotDamageMultiplierMelee(damage, damageinfo);
}
void Creature::ApplyBotDamageMultiplierMelee(int32& damage, SpellNonMeleeDamage& damageinfo, SpellInfo const* spellInfo, WeaponAttackType attackType, bool& crit) const
{
    if (bot_AI)
        bot_AI->ApplyBotDamageMultiplierMelee(damage, damageinfo, spellInfo, attackType, crit);
}
void Creature::ApplyBotDamageMultiplierSpell(int32& damage, SpellNonMeleeDamage& damageinfo, SpellInfo const* spellInfo, WeaponAttackType attackType, bool& crit) const
{
    if (bot_AI)
        bot_AI->ApplyBotDamageMultiplierSpell(damage, damageinfo, spellInfo, attackType, crit);
}

void Creature::ApplyBotDamageMultiplierEffect(SpellInfo const* spellInfo, uint8 effect_index, float &value) const
{
    if (bot_AI)
        bot_AI->ApplyBotDamageMultiplierEffect(spellInfo, effect_index, value);
}

bool Creature::GetIAmABot() const
{
    return bot_AI && bot_AI->IsMinionAI();
}

bool Creature::GetIAmABotsPet() const
{
    return bot_AI && bot_AI->IsPetAI();
}

bot_minion_ai* Creature::GetBotMinionAI() const
{
    return IsAIEnabled && bot_AI && bot_AI->IsMinionAI() ? const_cast<bot_minion_ai*>(bot_AI->GetMinionAI()) : NULL;
}

bot_pet_ai* Creature::GetBotPetAI() const
{
    return IsAIEnabled && bot_AI && bot_AI->IsPetAI() ? const_cast<bot_pet_ai*>(bot_AI->GetPetAI()) : NULL;
}

void Creature::InitBotAI(bool asPet)
{
    ASSERT(!bot_AI);

    if (asPet)
        bot_AI = (bot_pet_ai*)AI();
    else
        bot_AI = (bot_minion_ai*)AI();
}

void Creature::SetBotShouldUpdateStats()
{
    if (bot_AI)
        bot_AI->SetShouldUpdateStats();
}

void Creature::OnBotSummon(Creature* summon)
{
    if (bot_AI)
        bot_AI->OnBotSummon(summon);
}

void Creature::OnBotDespawn(Creature* summon)
{
    if (bot_AI)
        bot_AI->OnBotDespawn(summon);
}

void Creature::RemoveBotItemBonuses(uint8 slot)
{
    if (bot_AI)
        bot_AI->RemoveItemBonuses(slot);
}
void Creature::ApplyBotItemBonuses(uint8 slot)
{
    if (bot_AI)
        bot_AI->ApplyItemBonuses(slot);
}
bool Creature::CanUseOffHand() const
{
    return bot_AI && bot_AI->CanUseOffHand();
}
bool Creature::CanUseRanged() const
{
    return bot_AI && bot_AI->CanUseRanged();
}
bool Creature::CanEquip(ItemTemplate const* item, uint8 slot) const
{
    return bot_AI && bot_AI->CanEquip(item, slot);
}
bool Creature::Unequip(uint8 slot) const
{
    return bot_AI && bot_AI->Unequip(slot);
}
bool Creature::Equip(uint32 itemId, uint8 slot) const
{
    return bot_AI && bot_AI->Equip(itemId, slot);
}
bool Creature::ResetEquipment(uint8 slot) const
{
    return bot_AI && bot_AI->ResetEquipment(slot);
}

bool Creature::IsQuestBot() const
{
    return
        m_creatureInfo->Entry >= 71000 && m_creatureInfo->Entry < 72000 &&
        (m_creatureInfo->unit_flags2 & UNIT_FLAG2_ALLOW_ENEMY_INTERACT);
}


uint32 Creature::GetShieldBlockValue() const
{
    if (bot_AI)
        return bot_AI->GetShieldBlockValue();

    return (getLevel() / 2 + uint32(GetStat(STAT_STRENGTH) / 20));
}

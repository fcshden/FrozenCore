#ifndef __Spell_H
#define __Spell_H
#include "Common.h"
#include "Timer.h"
#include <ace/Singleton.h>
#include <ace/Atomic_Op.h>
#include "SharedDefines.h"
#include "Util.h"
#include <atomic>
#include <map>
#include <set>
#include <list>
#pragma execution_character_set("utf-8")

//所有需求表

struct SpellCustomMod
{
    uint32 spellid;
    uint32 reqId;
    uint32 rewId;
    float dmgMod;
    float healMod;
    bool accountBind;
    bool disable;
    int32 castingtime;
    int32 duration;
    uint32 cooldown;
    uint32 procChance;
    uint32 Periodic[MAX_SPELL_EFFECTS];
};

struct SpellCustomLearnMod
{
    uint32 spellid;
    uint32 reqId;
    uint32 rewId;
};

struct SpellCustomUnLearnMod
{
    uint32 spellid;
    uint32 reqId;
    uint32 rewId;
};

enum LeechTypes
{
    LEECH_TYPE_NONE,
    LEECH_TYPE_SELF_CUR_PCT,			//0 自身当前生命值百分比
    LEECH_TYPE_SELF_MAX_PCT,			//1 自身最大生命值百分比
    LEECH_TYPE_TARGET_CUR_PCT,			//2 目标当前生命值百分比
    LEECH_TYPE_TARGET_MAX_PCT,			//3 目标最大生命值百分比
    LEECH_TYPE_STATIC,					//4 固定数值
    LEECH_TYPE_DAMGE_PCT,				//5 技能伤害百分比
};

struct SpellCustomXxSpellMod
{
    uint32 spellid;
    float chance;
    LeechTypes type;
    float basepoints;
    float addDmg;
    uint32 meetAura;
};

struct SpellCustomXxAuraMod
{
    uint32 spellid;
    float chance;
    LeechTypes type;
    float basepoints;
    float addDmg;
    flag96 SpellClassMask;
    uint32 SpellFamily;
};

struct SpellCustomFsAuraMod
{
    uint32 spellid;
    uint32 dmgmask;
    float chance;
    LeechTypes type;
    float basepoints;
};

struct AuraStackTriggerMod
{
    uint32 auraid;
    uint32 Stacks;
    std::vector<uint32> TriggerSpellVec;
    uint32 RemoveStacks;
};

struct AuraOnDeathMod
{
    uint32 auraid;
    uint32 CoolDown;
    uint32 AttackerTriggerSpell;
    float AttackerTriggerChance;
    uint32 SelfTriggerSpell;
    float SelfTriggerChance;
    bool PreventLastDamage;
};

class SpellModMgr
{
public:
    SpellModMgr();
    ~SpellModMgr();

    SpellCustomMod const * FindSpellCustomMod(uint32 spellid)
    {
        for (auto i = m_spellmods.begin(); i != m_spellmods.end(); i++)
        {
            if ((*i)->spellid == spellid)
                return (*i);
        }
        return nullptr;
    }

    typedef std::vector<SpellCustomMod*>  CSpellModMap;
    CSpellModMap m_spellmods; //技能基础属性


    SpellCustomLearnMod const * FindSpellLearnMod(uint32 spellid)
    {
        for (auto i = m_spellmodlearns.begin(); i != m_spellmodlearns.end(); i++)
        {
            if ((*i)->spellid == spellid)
                return (*i);
        }
        return nullptr;
    }

    typedef std::vector<SpellCustomLearnMod*>  CSpellModLearnMap;
    CSpellModLearnMap m_spellmodlearns; //技能学习时


    SpellCustomUnLearnMod const * FindSpellUnLearnMod(uint32 spellid)
    {
        for (auto i = m_spellmodunlearns.begin(); i != m_spellmodunlearns.end(); i++)
        {
            if ((*i)->spellid == spellid)
                return (*i);
        }
        return nullptr;
    }

    typedef std::vector<SpellCustomUnLearnMod*>  CSpellModUnLearnMap;
    CSpellModUnLearnMap m_spellmodunlearns; //技能遗忘时


    SpellCustomXxSpellMod const * FindSpellXxSpellMod(uint32 spellid)
    {
        for (auto i = m_spellmodxxspell.begin(); i != m_spellmodxxspell.end(); i++)
        {
            if ((*i)->spellid == spellid)
                return (*i);
        }
        return nullptr;
    }

    typedef std::vector<SpellCustomXxSpellMod*>  CSpellModXxSpellMap;
    CSpellModXxSpellMap m_spellmodxxspell; //吸血技能

    SpellCustomXxAuraMod const * FindSpellXxAuraMod(uint32 spellid)
    {
        for (auto i = m_spellmodxxaura.begin(); i != m_spellmodxxaura.end(); i++)
        {
            if ((*i)->spellid == spellid)
                return (*i);
        }
        return nullptr;
    }

    typedef std::vector<SpellCustomXxAuraMod*>  CSpellModXxAuraMap;
    CSpellModXxAuraMap m_spellmodxxaura; //吸血光环


    SpellCustomFsAuraMod const * FindSpellFsAuraMod(uint32 spellid)
    {
        for (auto i = m_spellmodfsaura.begin(); i != m_spellmodfsaura.end(); i++)
        {
            if ((*i)->spellid == spellid)
                return (*i);
        }
        return nullptr;
    }

    typedef std::vector<SpellCustomFsAuraMod*>  CSpellModFsAuraMap;
    CSpellModFsAuraMap m_spellmodfsaura; //反伤光环


    AuraStackTriggerMod const * FindAuraStackMod(uint32 spellid)
    {
        for (auto i = m_AuraStackTriggers.begin(); i != m_AuraStackTriggers.end(); i++)
        {
            if ((*i)->auraid == spellid)
                return (*i);
        }
        return nullptr;
    }

    typedef std::vector<AuraStackTriggerMod*>  CAuraStackTriggerMap;
    CAuraStackTriggerMap m_AuraStackTriggers; //光环叠加


    AuraOnDeathMod const * FindAuraOnDeathMod(uint32 spellid)
    {
        for (auto i = m_AuraOnDeath.begin(); i != m_AuraOnDeath.end(); i++)
        {
            if ((*i)->auraid == spellid)
                return (*i);
        }
        return nullptr;
    }

    typedef std::vector<AuraOnDeathMod*>  CAuraOnDeathMap;
    CAuraOnDeathMap m_AuraOnDeath; //光环死亡触发

    void Load();
    void HealOnDamage(Unit* caster, Unit* target, SpellInfo const* spellInfo, uint32 &damage);
    void ShieldOnDamage(Unit* caster, Unit* target, SpellInfo const* spellInfo, uint32 &damage);
protected:
private:
};

#define sSpellModMgr ACE_Singleton<SpellModMgr, ACE_Null_Mutex>::instance()
#endif


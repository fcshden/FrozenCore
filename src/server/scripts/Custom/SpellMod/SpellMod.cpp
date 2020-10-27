#pragma execution_character_set("utf-8")
#include "SpellMod.h"


SpellModMgr::SpellModMgr()
{
    m_spellmods.clear();
    m_spellmodlearns.clear();
    m_spellmodunlearns.clear();
    m_spellmodxxspell.clear();
    m_spellmodxxaura.clear();
    m_spellmodfsaura.clear();
    m_AuraStackTriggers.clear();
    m_AuraOnDeath.clear();
}

SpellModMgr::~SpellModMgr()
{

}

void SpellModMgr::Load()
{
    QueryResult result;
    m_spellmods.clear();

    int count = 0;
    if (result = WorldDatabase.PQuery("SELECT 技能ID,需求ID,奖励ID,伤害倍率,治疗倍率,账号绑定,禁用,施法时间,持续时间,冷却时间,触发机率,周期时间1,周期时间2,周期时间3 FROM _技能"))
    {
        do
        {
            SpellCustomMod *Temp = new SpellCustomMod;
            Field* fields = result->Fetch();

            Temp->spellid = fields[0].GetUInt32();
            Temp->reqId = fields[1].GetUInt32();
            Temp->rewId = fields[2].GetUInt32();
            Temp->dmgMod = fields[3].GetFloat();
            Temp->healMod = fields[4].GetFloat();
            Temp->accountBind = fields[5].GetBool();
            Temp->disable = fields[6].GetBool();
            Temp->castingtime = fields[7].GetInt32();
            Temp->duration = fields[8].GetInt32();
            Temp->cooldown = fields[9].GetUInt32();
            Temp->procChance = fields[10].GetUInt32();

            for (size_t i = 0; i < MAX_SPELL_EFFECTS; i++)
                Temp->Periodic[i] = fields[11 + i].GetUInt32();

            m_spellmods.push_back(Temp);
            count++;

        } while (result->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_技能],共%u条数据读取加载...", count);


    m_spellmodlearns.clear();
    int count1 = 0;
    if (result = WorldDatabase.PQuery("SELECT 技能ID, 需求ID,奖励ID FROM _技能_当学习时"))
    {
        do
        {
            SpellCustomLearnMod *Temp = new SpellCustomLearnMod;
            Field* fields = result->Fetch();

            Temp->spellid = fields[0].GetUInt32();
            Temp->reqId = fields[1].GetUInt32();
            Temp->rewId = fields[2].GetUInt32();

            m_spellmodlearns.push_back(Temp);
            count1++;

        } while (result->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_技能_当学习时],共%u条数据读取加载...", count1);

    //技能遗忘时候
    m_spellmodunlearns.clear();
    int count2 = 0;
    if (result = WorldDatabase.PQuery("SELECT 技能ID, 需求ID,奖励ID FROM _技能_当遗忘时"))
    {
        do
        {
            SpellCustomUnLearnMod *Temp = new SpellCustomUnLearnMod;
            Field* fields = result->Fetch();

            Temp->spellid = fields[0].GetUInt32();
            Temp->reqId = fields[1].GetUInt32();
            Temp->rewId = fields[2].GetUInt32();

            m_spellmodunlearns.push_back(Temp);
            count2++;

        } while (result->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_技能_当遗忘时],共%u条数据读取加载...", count2);

    //吸血技能
    m_spellmodxxspell.clear();
    int count3 = 0;
    if (result = WorldDatabase.PQuery("SELECT 吸血技能ID,吸血几率,吸血类型,基础值,吸血转伤害,需要光环ID FROM _技能_吸血技能"))
    {
        do
        {
            SpellCustomXxSpellMod *Temp = new SpellCustomXxSpellMod;
            Field* fields = result->Fetch();
            
            Temp->spellid = fields[0].GetUInt32();
            Temp->chance = fields[1].GetFloat();

            const char* str = fields[2].GetCString();

            if (strcmp(str, "自身当前生命值百分比") == 0)
                Temp->type = LEECH_TYPE_SELF_CUR_PCT;
            else if (strcmp(str, "自身最大生命值百分比") == 0)
                Temp->type = LEECH_TYPE_SELF_MAX_PCT;
            else if (strcmp(str, "目标当前生命值百分比") == 0)
                Temp->type = LEECH_TYPE_TARGET_CUR_PCT;
            else if (strcmp(str, "目标最大生命值百分比") == 0)
                Temp->type = LEECH_TYPE_TARGET_MAX_PCT;
            else if (strcmp(str, "固定数值") == 0)
                Temp->type = LEECH_TYPE_STATIC;
            else if (strcmp(str, "技能伤害百分比") == 0)
                Temp->type = LEECH_TYPE_DAMGE_PCT;
            else
                Temp->type = LEECH_TYPE_NONE;

            Temp->basepoints = fields[3].GetFloat();
            Temp->addDmg = fields[4].GetFloat();
            Temp->meetAura = fields[5].GetUInt32();
            m_spellmodxxspell.push_back(Temp);
            count3++;
        } while (result->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_技能_吸血技能],共%u条数据读取加载...", count3);


    //吸血光环
    m_spellmodxxaura.clear();
    int count4 = 0;
    if (result = WorldDatabase.PQuery("SELECT 吸血光环技能ID,法术大类,法术小类1,法术小类2,法术小类3,吸血几率,吸血类型,基础值,吸血转伤害 FROM _技能_吸血光环"))
    {
        do
        {
            SpellCustomXxAuraMod *Temp = new SpellCustomXxAuraMod;
            Field* fields = result->Fetch();

            Temp->spellid = fields[0].GetUInt32();
            Temp->SpellFamily = fields[1].GetUInt32();
            Temp->SpellClassMask = flag96(fields[2].GetUInt32(), fields[3].GetUInt32(), fields[4].GetUInt32());
            Temp->chance = fields[5].GetFloat();


            const char* str = fields[6].GetCString();
            if (strcmp(str, "自身当前生命值百分比") == 0)
                Temp->type = LEECH_TYPE_SELF_CUR_PCT;
            else if (strcmp(str, "自身最大生命值百分比") == 0)
                Temp->type = LEECH_TYPE_SELF_MAX_PCT;
            else if (strcmp(str, "目标当前生命值百分比") == 0)
                Temp->type = LEECH_TYPE_TARGET_CUR_PCT;
            else if (strcmp(str, "目标最大生命值百分比") == 0)
                Temp->type = LEECH_TYPE_TARGET_MAX_PCT;
            else if (strcmp(str, "固定数值") == 0)
                Temp->type = LEECH_TYPE_STATIC;
            else if (strcmp(str, "技能伤害百分比") == 0)
                Temp->type = LEECH_TYPE_DAMGE_PCT;
            else
                Temp->type = LEECH_TYPE_NONE;

            Temp->basepoints = fields[7].GetFloat();
            Temp->addDmg = fields[8].GetFloat();
            m_spellmodxxaura.push_back(Temp);
            count4++;
        } while (result->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_技能_吸血光环],共%u条数据读取加载...", count4);

    //反伤光环
    m_spellmodfsaura.clear();
    int count5 = 0;
    if (result = WorldDatabase.PQuery("SELECT 反伤光环技能ID,受到伤害类型,反伤几率,反伤类型,基础值 FROM _技能_反伤光环"))
    {
        do
        {
            SpellCustomFsAuraMod *Temp = new SpellCustomFsAuraMod;
            Field* fields = result->Fetch();

            Temp->spellid = fields[0].GetUInt32();
            Temp->dmgmask = fields[1].GetUInt32();
            Temp->chance = fields[2].GetFloat();

            const char* str = fields[3].GetCString();
            if (strcmp(str, "自身当前生命值百分比") == 0)
                Temp->type = LEECH_TYPE_SELF_CUR_PCT;
            else if (strcmp(str, "自身最大生命值百分比") == 0)
                Temp->type = LEECH_TYPE_SELF_MAX_PCT;
            else if (strcmp(str, "目标当前生命值百分比") == 0)
                Temp->type = LEECH_TYPE_TARGET_CUR_PCT;
            else if (strcmp(str, "目标最大生命值百分比") == 0)
                Temp->type = LEECH_TYPE_TARGET_MAX_PCT;
            else if (strcmp(str, "固定数值") == 0)
                Temp->type = LEECH_TYPE_STATIC;
            else if (strcmp(str, "受到伤害百分比") == 0)
                Temp->type = LEECH_TYPE_DAMGE_PCT;
            else
                Temp->type = LEECH_TYPE_NONE;

            Temp->basepoints = fields[4].GetFloat();

            m_spellmodfsaura.push_back(Temp);
            count5++;
        } while (result->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_技能_反伤光环],共%u条数据读取加载...", count5);


    m_AuraStackTriggers.clear();
    int count6 = 0;
    result = WorldDatabase.PQuery("SELECT 光环技能ID, 叠加层数, 触发技能组, 触发技能后移除层数  FROM _技能_光环叠加");
    if (result)
    {
        do
        {
            AuraStackTriggerMod *Temp = new AuraStackTriggerMod;
            Field* fields = result->Fetch();
            Temp->auraid = fields[0].GetUInt32();
            Temp->Stacks = fields[1].GetUInt32();
            Tokenizer tokens(fields[2].GetString(), '#');
            for (Tokenizer::const_iterator itr = tokens.begin(); itr != tokens.end(); ++itr)
                Temp->TriggerSpellVec.push_back(uint32(atol(*itr)));
            Temp->RemoveStacks = fields[3].GetUInt32();
            m_AuraStackTriggers.push_back(Temp);
            count6++;
        } while (result->NextRow());
    }

    sLog->outString(">> 读取功能数据表[_技能_光环叠加],共%u条数据读取加载...", count6);

    m_AuraOnDeath.clear();
    int count7 = 0;
    if (result = WorldDatabase.PQuery("SELECT 光环技能ID,冷却时间,对攻击者施放技能ID,对攻击者施放技能几率,对自身施放技能ID,对自身施放技能几率,是否阻止死亡 FROM _技能_光环死亡触发"))
    {
        do
        {
            AuraOnDeathMod * Temp = new AuraOnDeathMod;
            Field* fields = result->Fetch();
            Temp->auraid = fields[0].GetUInt32();
            Temp->CoolDown = fields[1].GetUInt32();
            Temp->AttackerTriggerSpell = fields[2].GetUInt32();
            Temp->AttackerTriggerChance = fields[3].GetFloat();
            Temp->SelfTriggerSpell = fields[4].GetUInt32();
            Temp->SelfTriggerChance = fields[5].GetFloat();
            Temp->PreventLastDamage = fields[6].GetBool();
            m_AuraOnDeath.push_back(Temp);
            count7++;
        } while (result->NextRow());
    }
    sLog->outString(">> 读取功能数据表[_技能_光环死亡触发],共%u条数据读取加载...", count7);
}


void SpellModMgr::HealOnDamage(Unit* caster, Unit* target, SpellInfo const* spellInfo, uint32 &damage)
{
    //吸血技能
    if (SpellCustomXxSpellMod const *xxspell = FindSpellXxSpellMod(spellInfo->Id))
    {
        if (roll_chance_f(xxspell->chance))
        {
            if (xxspell->meetAura == 0 || caster->HasAura(xxspell->meetAura))
            {
                uint32 amount = 0;

                switch (xxspell->type)
                {
                case LEECH_TYPE_SELF_CUR_PCT:
                    amount = caster->GetHealth() * xxspell->basepoints / 100;
                    break;
                case LEECH_TYPE_SELF_MAX_PCT:
                    amount = caster->GetMaxHealth() * xxspell->basepoints / 100;
                    break;
                case LEECH_TYPE_TARGET_CUR_PCT:
                    amount = target->GetMaxHealth() * xxspell->basepoints / 100;
                    break;
                case LEECH_TYPE_TARGET_MAX_PCT:
                    amount = target->GetMaxHealth() * xxspell->basepoints / 100;
                    break;
                case LEECH_TYPE_STATIC:
                    amount = xxspell->basepoints;
                    break;
                case LEECH_TYPE_DAMGE_PCT:
                    amount = damage * xxspell->basepoints / 100;
                    break;
                }

                if (amount != 0)
                {
                    if (xxspell->addDmg != 0)
                        damage = damage + xxspell->addDmg * amount;
                    if (caster->IsAlive())
                        caster->HealBySpell(caster, spellInfo, amount);
                }
            }
        }
    }

    //吸血光环
    for (auto i = m_spellmodxxaura.begin(); i != m_spellmodxxaura.end(); i++)
    {
        if (!caster->HasAura((*i)->spellid))
            continue;

        if ((*i)->SpellFamily)
            if ((*i)->SpellFamily != spellInfo->SpellFamilyName)
                continue;
        
        if ((*i)->SpellClassMask != flag96(0, 0, 0))
            if (!((*i)->SpellClassMask & spellInfo->SpellFamilyFlags))
                continue;

        if (!roll_chance_f((*i)->chance))
            continue;

        uint32 amount = 0;
        switch ((*i)->type)
        {
        case LEECH_TYPE_SELF_CUR_PCT:
            amount = caster->GetHealth() * (*i)->basepoints / 100;
            break;
        case LEECH_TYPE_SELF_MAX_PCT:
            amount = caster->GetHealth() * (*i)->basepoints / 100;
            break;
        case LEECH_TYPE_TARGET_CUR_PCT:
            amount = target->GetMaxHealth() * (*i)->basepoints / 100;
            break;
        case LEECH_TYPE_TARGET_MAX_PCT:
            amount = target->GetMaxHealth() * (*i)->basepoints / 100;
            break;
        case LEECH_TYPE_STATIC:
            amount = (*i)->basepoints;
            break;
        case LEECH_TYPE_DAMGE_PCT:
            amount = damage * (*i)->basepoints / 100;
            break;
        }

        if (amount != 0)
        {
            if ((*i)->addDmg != 0)
                damage = damage + (*i)->addDmg * amount;
            if (caster->IsAlive())
                caster->HealBySpell(caster, spellInfo, amount);
        }
    }
}


void SpellModMgr::ShieldOnDamage(Unit* caster, Unit* target, SpellInfo const* spellInfo, uint32 & handdamage)
{
    
    for (auto dmgShieldItr = m_spellmodfsaura.begin(); dmgShieldItr != m_spellmodfsaura.end(); ++dmgShieldItr)
    {
        if (!caster->HasAura((*dmgShieldItr)->spellid))
            continue;

        SpellInfo const* i_spellProto = sSpellMgr->GetSpellInfo((*dmgShieldItr)->spellid);

        if (!i_spellProto)
            continue;

        if (!roll_chance_f((*dmgShieldItr)->chance))
            continue;

        if ((*dmgShieldItr)->dmgmask)
        {
            if (spellInfo)
            {
                if (!((*dmgShieldItr)->dmgmask & spellInfo->SchoolMask))
                    continue;
            }
            else
            {
                if (!((*dmgShieldItr)->dmgmask & 1))
                    continue;
            }
        }


        if (SpellMissInfo missInfo = caster->SpellHitResult(target, i_spellProto, false))
        {
            caster->SendSpellMiss(target, i_spellProto->Id, missInfo);
            continue;
        }


        if (target->IsImmunedToDamageOrSchool(i_spellProto))
        {
            caster->SendSpellDamageImmune(target, i_spellProto->Id);
            continue;
        }

        uint32 damage = 0;

        switch ((*dmgShieldItr)->type)
        {
        case LEECH_TYPE_SELF_CUR_PCT:
            damage = caster->GetHealth() * (*dmgShieldItr)->basepoints / 100;
            break;
        case LEECH_TYPE_SELF_MAX_PCT:
            damage = caster->GetHealth() * (*dmgShieldItr)->basepoints / 100;
            break;
        case LEECH_TYPE_TARGET_CUR_PCT:
            damage = target->GetMaxHealth() * (*dmgShieldItr)->basepoints / 100;
            break;
        case LEECH_TYPE_TARGET_MAX_PCT:
            damage = target->GetMaxHealth() * (*dmgShieldItr)->basepoints / 100;
            break;
        case LEECH_TYPE_STATIC:
            damage = (*dmgShieldItr)->basepoints;
            break;
        case LEECH_TYPE_DAMGE_PCT:
            damage = handdamage * (*dmgShieldItr)->basepoints / 100;
            break;
        }


        Unit::DealDamageMods(caster, damage, nullptr);
        WorldPacket data(SMSG_SPELLDAMAGESHIELD, (8 + 8 + 4 + 4 + 4 + 4));
        data << uint64(caster->GetGUID());
        data << uint64(target->GetGUID());
        data << uint32(i_spellProto->Id);
        data << uint32(damage);
        int32 overkill = int32(damage) - int32(target->GetHealth());
        data << uint32(overkill > 0 ? overkill : 0);
        data << uint32(i_spellProto->SchoolMask);
        caster->SendMessageToSet(&data, true);

        Unit::DealDamage(caster, target, damage, 0, SPELL_DIRECT_DAMAGE, i_spellProto->GetSchoolMask(), i_spellProto, true);
    }
}


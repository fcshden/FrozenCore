#pragma execution_character_set("utf-8")
#include "RandomEnchant.h"
#include "../ItemMod/ItemMod.h"
#include "../Switch/Switch.h"
#include "DisableMgr.h"
#include "../ItemMod/NoPatchItem.h"

std::vector<RandomEnchantTemplate> RandomEnchantVec;
std::unordered_map<uint32, ExtractEnchantTemplate> ExtractEnchantMap;

void RandomEnchant::Load()
{
	RandomEnchantVec.clear();

	/*QueryResult result = WorldDatabase.PQuery("SELECT entry, enchantId from _random_enchant");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			RandomEnchantTemplate Temp;
			Temp.entry = fields[0].GetUInt32();
			Temp.enchantId = fields[1].GetUInt32();
			RandomEnchantVec.push_back(Temp);
		} while (result->NextRow());
	}*/

	ExtractEnchantMap.clear();

	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT 物品ID, 是否可以提取附魔,是否可以覆盖附魔 from _物品_附魔提取与覆盖" :
		"SELECT entry, CanExtract,CanEnchant from _itemmod_extract_enchant");

	if (result)
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 entry = fields[0].GetUInt32();
			ExtractEnchantTemplate Temp;
			Temp.CanExtract = fields[1].GetBool();
			Temp.CanEnchant = fields[2].GetBool();
			ExtractEnchantMap.insert(std::make_pair(entry, Temp));
		} while (result->NextRow());
	}

}

void RandomEnchant::OnCreate(Item* item)
{
	if (item->GetTemplate()->Class != ITEM_CLASS_WEAPON && item->GetTemplate()->Class != ITEM_CLASS_ARMOR)
		return;

	if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(ON_CREATE_ENCHANT_ID))
		item->SetEnchantment(PROP_ENCHANTMENT_SLOT_4, ON_CREATE_ENCHANT_ID, 0, 0);
}

bool RandomEnchant::Enchant(Player* owner, Item* castItem, Item* targetItem)
{
	if (targetItem->GetOwner() != owner)
		return false;

	if (ON_CREATE_ENCHANT_ID != targetItem->GetEnchantmentId(PROP_ENCHANTMENT_SLOT_4))
		return false;

	if (RandomEnchantVec.empty())
		return false;

	std::vector<uint32> vec;

	for (size_t i = 0; i < RandomEnchantVec.size(); i++)
		if (castItem->GetEntry() == RandomEnchantVec[i].entry)
			if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(RandomEnchantVec[i].enchantId))
				vec.push_back(RandomEnchantVec[i].enchantId);

	if (vec.empty())
		return false;

	owner->ApplyEnchantment(targetItem, PROP_ENCHANTMENT_SLOT_4, false);
	targetItem->SetEnchantment(PROP_ENCHANTMENT_SLOT_4, vec[urand(0, vec.size() - 1)], 0, 0);
	owner->ApplyEnchantment(targetItem, PROP_ENCHANTMENT_SLOT_4, true);

	return true;
}

class spell_random_enchant : public SpellScriptLoader
{
public:
	spell_random_enchant() : SpellScriptLoader("spell_random_enchant") { }

	class spell_random_enchant_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_random_enchant_SpellScript);

		SpellCastResult CheckCast()
		{
			Player* owner = GetCaster()->ToPlayer();
			Item* targetItem = GetExplTargetItem();
			Item* castItem = GetCastItem();

			if (!owner || !targetItem || !castItem)
				return SPELL_FAILED_BAD_TARGETS;

			if (!sRandomEnchant->Enchant(owner, castItem, targetItem))
				return SPELL_FAILED_BAD_TARGETS;

			return SPELL_CAST_OK;
		}

		void Register() override
		{
			OnCheckCast += SpellCheckCastFn(spell_random_enchant_SpellScript::CheckCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_random_enchant_SpellScript();
	}
};

class spell_extract_enchant : public SpellScriptLoader
{
public:
	spell_extract_enchant() : SpellScriptLoader("spell_extract_enchant") { }

	class spell_extract_enchant_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_extract_enchant_SpellScript);

		SpellCastResult CheckCast()
		{
			Player* owner = GetCaster()->ToPlayer();
			Item* targetItem = GetExplTargetItem();
			Item* castItem = GetCastItem();

			if (!owner || !targetItem || !castItem)
				return SPELL_FAILED_BAD_TARGETS;

			if (targetItem->GetEntry() == atoi(sSwitch->GetFlagByIndex(ST_ENCHANT_SCROLL, 1).c_str()))
				return SPELL_FAILED_BAD_TARGETS;

			std::unordered_map<uint32, ExtractEnchantTemplate>::iterator itr = ExtractEnchantMap.find(targetItem->GetEntry());

			if (itr == ExtractEnchantMap.end())
				return SPELL_FAILED_BAD_TARGETS;

			if (!itr->second.CanExtract)
				return SPELL_FAILED_BAD_TARGETS;
				
			std::vector<uint32> enchantIdVec;

			for (uint8 i = PROP_ENCHANTMENT_SLOT_0; i < MAX_ENCHANTMENT_SLOT; i++)
			{
				uint32 enchantId = targetItem->GetEnchantmentId(EnchantmentSlot(i));
				if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId))
					enchantIdVec.push_back(enchantId);
			}
				
			if (enchantIdVec.empty())
				return SPELL_FAILED_BAD_TARGETS;

			uint32 enchantId = enchantIdVec[urand(0, enchantIdVec.size() - 1)];

			if (Item* item = sItemMod->AddItem(owner, atoi(sSwitch->GetFlagByIndex(ST_ENCHANT_SCROLL,1).c_str()), 1))
				item->SetEnchantment(PROP_ENCHANTMENT_SLOT_0, enchantId, 0, 0);
			else
				return SPELL_FAILED_ITEM_NOT_READY;
			
			owner->DestroyItem(targetItem->GetBagSlot(), targetItem->GetSlot(), true);

			return SPELL_CAST_OK;
		}

		void Register() override
		{
			OnCheckCast += SpellCheckCastFn(spell_extract_enchant_SpellScript::CheckCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_extract_enchant_SpellScript();
	}
};

class spell_extract_enchant_all : public SpellScriptLoader
{
public:
	spell_extract_enchant_all() : SpellScriptLoader("spell_extract_enchant_all") { }

	class spell_extract_enchant_all_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_extract_enchant_all_SpellScript);

		SpellCastResult CheckCast()
		{
			Player* owner = GetCaster()->ToPlayer();
			Item* targetItem = GetExplTargetItem();
			Item* castItem = GetCastItem();

			if (!owner || !targetItem || !castItem)
				return SPELL_FAILED_BAD_TARGETS;

			if (targetItem->GetEntry() == atoi(sSwitch->GetFlagByIndex(ST_ENCHANT_SCROLL, 2).c_str()))
				return SPELL_FAILED_BAD_TARGETS;

			std::unordered_map<uint32, ExtractEnchantTemplate>::iterator itr = ExtractEnchantMap.find(targetItem->GetEntry());

			if (itr == ExtractEnchantMap.end())
				return SPELL_FAILED_BAD_TARGETS;

			if (!itr->second.CanExtract)
				return SPELL_FAILED_BAD_TARGETS;

			std::vector<uint32> enchantIdVec;

			for (uint8 i = PROP_ENCHANTMENT_SLOT_0; i < MAX_ENCHANTMENT_SLOT; i++)
			{
				uint32 enchantId = targetItem->GetEnchantmentId(EnchantmentSlot(i));
				if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId))
					enchantIdVec.push_back(enchantId);
			}

			if (enchantIdVec.empty())
				return SPELL_FAILED_BAD_TARGETS;

			if (Item* item = sItemMod->AddItem(owner, atoi(sSwitch->GetFlagByIndex(ST_ENCHANT_SCROLL, 2).c_str()), 1))
			{
				for (uint32 i = 0; i < enchantIdVec.size(); i++)
				{
					item->SetEnchantment(EnchantmentSlot(PROP_ENCHANTMENT_SLOT_0 + i), enchantIdVec[i], 0, 0);
				}
			}
			else
				return SPELL_FAILED_ITEM_NOT_READY;

			owner->DestroyItem(targetItem->GetBagSlot(), targetItem->GetSlot(), true);

			return SPELL_CAST_OK;
		}

		void Register() override
		{
			OnCheckCast += SpellCheckCastFn(spell_extract_enchant_all_SpellScript::CheckCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_extract_enchant_all_SpellScript();
	}
};


class spell_override_enchant : public SpellScriptLoader
{
public:
	spell_override_enchant() : SpellScriptLoader("spell_override_enchant") { }

	class spell_override_enchant_SpellScript : public SpellScript
	{
		uint32 GetRandomEnchantId(std::vector<uint32> &enchantIdVec)
		{
			if (enchantIdVec.empty())
				return 0;

			uint32 i = urand(0, enchantIdVec.size() - 1);
			uint32 enchantId = enchantIdVec[i];
			enchantIdVec.erase(enchantIdVec.begin() + i);

			return enchantId;
		}


		PrepareSpellScript(spell_override_enchant_SpellScript);

		SpellCastResult CheckCast()
		{
			Player* owner = GetCaster()->ToPlayer();
			Item* targetItem = GetExplTargetItem();
			Item* castItem = GetCastItem();

			if (!owner || !targetItem || !castItem)
				return SPELL_FAILED_BAD_TARGETS;

			std::unordered_map<uint32, ExtractEnchantTemplate>::iterator itr = ExtractEnchantMap.find(targetItem->GetEntry());

			if (itr == ExtractEnchantMap.end())
				return SPELL_FAILED_BAD_TARGETS;

			if (!itr->second.CanEnchant)
				return SPELL_FAILED_BAD_TARGETS;

			//castItem
			std::vector<uint32> enchantIdVec;

			for (uint8 i = PROP_ENCHANTMENT_SLOT_0; i < MAX_ENCHANTMENT_SLOT; i++)
			{
				uint32 enchantId = castItem->GetEnchantmentId(EnchantmentSlot(i));
				if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId))
					enchantIdVec.push_back(enchantId);
			}

			if (enchantIdVec.empty())
				return SPELL_FAILED_ITEM_NOT_READY;

			std::vector<uint8> slotVec;

			for (uint8 i = PROP_ENCHANTMENT_SLOT_0; i < MAX_ENCHANTMENT_SLOT; i++)
			{
				uint32 enchantId = targetItem->GetEnchantmentId(EnchantmentSlot(i));

				if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchantId))
					slotVec.push_back(i);
			}

			if (slotVec.empty())
				return SPELL_FAILED_ITEM_NOT_READY;

			owner->ApplyEnchantment(targetItem, false);

			//一条属性 随机覆盖
			if (castItem->GetEntry() == atoi(sSwitch->GetFlagByIndex(ST_ENCHANT_SCROLL, 1).c_str()))
			{
				
				uint8 slot = slotVec[urand(0, slotVec.size() - 1)];
				targetItem->SetEnchantment(EnchantmentSlot(slot), enchantIdVec[0], 0, 0);
			}

			//全部属性 全部覆盖
			if (castItem->GetEntry() == atoi(sSwitch->GetFlagByIndex(ST_ENCHANT_SCROLL, 2).c_str()))
			{
				for (uint8 i = PROP_ENCHANTMENT_SLOT_0; i < MAX_ENCHANTMENT_SLOT; i++)
					targetItem->SetEnchantment(EnchantmentSlot(i), 0, 0, 0);

				for (uint32 i = 0; i < enchantIdVec.size(); i++)
					targetItem->SetEnchantment(EnchantmentSlot(i + PROP_ENCHANTMENT_SLOT_0), enchantIdVec[i], 0, 0);	
			}

			owner->ApplyEnchantment(targetItem, true);
			return SPELL_CAST_OK;
		}

		void Register() override
		{
			OnCheckCast += SpellCheckCastFn(spell_override_enchant_SpellScript::CheckCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_override_enchant_SpellScript();
	}
};


class spell_refresh_create_enchant : public SpellScriptLoader
{
public:
	spell_refresh_create_enchant() : SpellScriptLoader("spell_refresh_create_enchant") { }

	class spell_refresh_create_enchant_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_refresh_create_enchant_SpellScript);

		SpellCastResult CheckCast()
		{
			Player* owner = GetCaster()->ToPlayer();
			Item* targetItem = GetExplTargetItem();
			Item* castItem = GetCastItem();

			if (!owner || !targetItem || !castItem)
				return SPELL_FAILED_BAD_TARGETS;

			bool exsit = false;

			for (auto itr = CreateEnchantVec.begin(); itr != CreateEnchantVec.end(); itr++)
				if (itr->entry == targetItem->GetEntry())
					exsit = true;

			if (!exsit)
				return SPELL_FAILED_BAD_TARGETS;

			owner->ApplyEnchantment(targetItem, false);

			for (auto itr = CreateEnchantVec.begin(); itr != CreateEnchantVec.end(); itr++)
				if (itr->entry == targetItem->GetEntry())
					targetItem->SetEnchantment(EnchantmentSlot(itr->slot), 0, 0, 0);

			for (uint8 slot = PERM_ENCHANTMENT_SLOT; slot < MAX_ENCHANTMENT_SLOT; slot++)
			{
				uint32 enchant_id = sItemMod->GenerateEnchantId(targetItem->GetEntry(), slot);
				if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
					targetItem->SetEnchantment(EnchantmentSlot(slot), enchant_id, 0, 0);
			}

			owner->ApplyEnchantment(targetItem, true);

			return SPELL_CAST_OK;
		}

		void Register() override
		{
			OnCheckCast += SpellCheckCastFn(spell_refresh_create_enchant_SpellScript::CheckCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_refresh_create_enchant_SpellScript();
	}
};

class EnchantSpellDisable : PlayerScript
{
public:
	EnchantSpellDisable() : PlayerScript("EnchantSpellDisable") {}

	void OnUpdateZone(Player* player, uint32 /*newZone*/, uint32 /*newArea*/)
	{
		Unit::AuraApplicationMap &myAuras = player->GetAppliedAuras();
		for (Unit::AuraApplicationMap::iterator i = myAuras.begin(); i != myAuras.end();)
		{
			Aura const* aura = i->second->GetBase();

			if (aura && DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, aura->GetId(), player))
				player->RemoveAura(i);
			else
				++i;
		}

		for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; i++)
		{
			if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			{
				for (uint8 slot = PERM_ENCHANTMENT_SLOT; slot < MAX_ENCHANTMENT_SLOT; slot++)
				{
					uint32 enchant_id = pItem->GetEnchantmentId(EnchantmentSlot(slot));

					if (SpellItemEnchantmentEntry const* pEnchant = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
					{
						for (int s = 0; s < MAX_ITEM_ENCHANTMENT_EFFECTS; ++s)
						{
							uint32 enchant_display_type = pEnchant->type[s];
							
							if (enchant_display_type == ITEM_ENCHANTMENT_TYPE_EQUIP_SPELL)
							{
								uint32 enchant_amount = pEnchant->amount[s];
								uint32 enchant_spell_id = pEnchant->spellid[s];

								if (enchant_spell_id)
								{
									bool hasAura = player->HasAura(enchant_spell_id);
									bool disable = DisableMgr::IsDisabledFor(DISABLE_TYPE_SPELL, enchant_spell_id, player);

									if (hasAura && disable)
										player->RemoveAurasDueToItemSpell(enchant_spell_id, pItem->GetGUID());
									else if (!hasAura && !disable)
									{
										int32 basepoints = 0;
										// Random Property Exist - try found basepoints for spell (basepoints depends from item suffix factor)
										if (pItem->GetItemRandomPropertyId())
										{
											ItemRandomSuffixEntry const* item_rand = sItemRandomSuffixStore.LookupEntry(abs(pItem->GetItemRandomPropertyId()));
											if (item_rand)
											{
												// Search enchant_amount
												for (int k = 0; k < MAX_ITEM_ENCHANTMENT_EFFECTS; ++k)
												{
													if (item_rand->enchant_id[k] == enchant_id)
													{
														basepoints = int32((item_rand->prefix[k] * pItem->GetItemSuffixFactor()) / 10000);
														break;
													}
												}
											}
										}
										// Cast custom spell vs all equal basepoints got from enchant_amount
										if (basepoints)
											player->CastCustomSpell(player, enchant_spell_id, &basepoints, &basepoints, &basepoints, true, pItem);
										else
											player->CastSpell(player, enchant_spell_id, true, pItem);
									}
								}
							}
						}
					}
				}
			}
		}
	}
};

class spell_np_to_level : public SpellScriptLoader
{
public:
	spell_np_to_level() : SpellScriptLoader("spell_np_to_level") { }

	class spell_refresh_create_enchant_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_refresh_create_enchant_SpellScript);

		SpellCastResult CheckCast()
		{
			Player* owner = GetCaster()->ToPlayer();
			Item* targetItem = GetExplTargetItem();
			Item* casterItem = GetCastItem();


			sNoPatchItem->Compound(owner, targetItem);

			//if (!owner || !targetItem || !casterItem)
			//	return SPELL_FAILED_BAD_TARGETS;
			//
			//if (!targetItem->IsNoPatch())
			//	return SPELL_FAILED_BAD_TARGETS;
			//
			//auto itr = NpToLevelMap.find(casterItem->GetEntry());
			//if (itr == NpToLevelMap.end())
			//	return SPELL_FAILED_BAD_TARGETS;
			//
			//float chance = itr->second.chance;
			//uint32 level = itr->second.level;
			//
			//if (!roll_chance_f(itr->second.chance))
			//{
			//	owner->GetSession()->SendNotification("升级失败");
			//	return SPELL_CAST_OK;
			//}
			//else
			//	sNoPatchItem->LevelUp(owner, targetItem, level);

			return SPELL_CAST_OK;
		}

		void Register() override
		{
			OnCheckCast += SpellCheckCastFn(spell_refresh_create_enchant_SpellScript::CheckCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_refresh_create_enchant_SpellScript();
	}
};

void AddSC_Random_Enchant()
{
	new spell_refresh_create_enchant();
	new spell_random_enchant();
	new spell_extract_enchant();
	new spell_extract_enchant_all();
	new spell_override_enchant();
	new EnchantSpellDisable();
	new spell_np_to_level();
}

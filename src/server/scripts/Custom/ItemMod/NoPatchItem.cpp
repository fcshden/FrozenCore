#pragma execution_character_set("utf-8")
#include "NoPatchItem.h"
#include "ItemMod.h"
#include "../GCAddon/GCAddon.h"
#include "../Requirement/Requirement.h"
#include "../Reward/Reward.h"

std::unordered_map<uint32, Item*> ItemQueryMap;
std::unordered_map < uint32, NpTemplate> NpMap;
std::unordered_map<uint32, std::string> NpPrefixMap;
std::vector<NpStatTemplate> NpStatVec;
std::vector<NpSpellTemplate> NpSpellVec;
std::vector<NpSrcTemplate> NpSrcVec;
std::vector<_NpItemLevel> NpLevelVec;

std::vector<MapNpTemplate> MapNpVector;
std::unordered_map<uint32, MapNpTempTemplate> MapNpTempMap;
std::vector<MapNpStatModTemplate> MapNpStatModVector;
std::vector<MapNpEnchantTemplate> MapNpEnchantVector;
std::vector<MapNpWeaponTemplate> MapNpWeaponVector;
std::vector<_NpItemLevelStatModTemplate> NpLevelStatModVec;
std::vector<NpAuraTemplate> NpAuraVector;
std::unordered_map<uint32, NpToLevelTemplate> NpToLevelMap;
std::vector<NpSameTemplate> NpSameTempIndexVector;

void NoPatchItem::Load()
{
	NpMap.clear();
	NpPrefixMap.clear();
	NpSrcVec.clear();
	NpStatVec.clear();
	NpSpellVec.clear();
	NpLevelVec.clear();
	NpLevelStatModVec.clear();
	NpAuraVector.clear();
	NpSameTempIndexVector.clear();

	QueryResult result;
	
	if (result = WorldDatabase.PQuery("SELECT SrcIndex, SrcEntry, SrcChance From _itemmod_nopatch_src"))
	{
		do
		{
			Field* fields = result->Fetch();
			NpSrcTemplate Temp;
			Temp.SrcIndex = fields[0].GetUInt32();
			Temp.SrcEntry = fields[1].GetUInt32();
			Temp.SrcChance = fields[2].GetFloat();
			NpSrcVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT SpellIndex, SpellId, SpellTrigger, SpellCharges, SpellPPMRate, SpellCooldown, SpellCategory, SpellCategoryCooldown,SpellChance "
		"From _itemmod_nopatch_spell"))
	{
		do
		{
			Field* fields = result->Fetch();
			NpSpellTemplate Temp;
			Temp.SpellIndex						= fields[0].GetUInt32();
			Temp.Spell.SpellId					= fields[1].GetUInt32();
			Temp.Spell.SpellTrigger				= fields[2].GetUInt32();
			Temp.Spell.SpellCharges				= fields[3].GetUInt32();
			Temp.Spell.SpellPPMRate				= fields[4].GetFloat();
			Temp.Spell.SpellCooldown			= fields[5].GetInt32();
			Temp.Spell.SpellCategory			= fields[6].GetUInt32();
			Temp.Spell.SpellCategoryCooldown	= fields[7].GetInt32();
			Temp.SpellChance					= fields[8].GetFloat();
			NpSpellVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT StatIndex, ItemStatType, ItemStatMinValue, ItemStatMaxValue,StatChance From _itemmod_nopatch_stat"))
	{
		do
		{
			Field* fields = result->Fetch();
			NpStatTemplate Temp;
			Temp.StatIndex				= fields[0].GetUInt32();
			Temp.Stat.ItemStatType		= fields[1].GetUInt32();
			Temp.Stat.ItemStatMinValue	= fields[2].GetUInt32();
			Temp.Stat.ItemStatMaxValue	= fields[3].GetUInt32();
			Temp.StatChance				= fields[4].GetUInt32();
			NpStatVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT Entry, StatCount, SpellCount,"
		"MinDamageMin,MinDamageMax,MaxDamageMin,MaxDamageMax,MinDelay,MaxDelay,SrcIndex,SpellIndex,StatIndex,LevelIndex,Quality,ItemLevel,Suffix From _itemmod_nopatch"))
	{
		do
		{
			Field* fields				= result->Fetch();
			uint32 Entry				= fields[0].GetUInt32();
			NpTemplate Temp;
			Temp.StatCount				= fields[1].GetUInt32();
			if (Temp.StatCount > MAX_ITEM_PROTO_STATS)
				Temp.StatCount = MAX_ITEM_PROTO_STATS;
			Temp.SpellCount				= fields[2].GetUInt32();
			if (Temp.SpellCount > MAX_ITEM_PROTO_SPELLS)
				Temp.SpellCount = MAX_ITEM_PROTO_SPELLS;
			Temp.Damage.MinDamageMin	= fields[3].GetUInt32();
			Temp.Damage.MinDamageMax	= fields[4].GetUInt32();
			Temp.Damage.MaxDamageMin	= fields[5].GetUInt32();
			Temp.Damage.MaxDamageMax	= fields[6].GetUInt32();
			Temp.Delay.MinDelay			= fields[7].GetUInt32();
			Temp.Delay.MaxDelay			= fields[8].GetUInt32();
			Temp.SrcIndex				= fields[9].GetUInt32();
			Temp.SpellIndex				= fields[10].GetUInt32();
			Temp.StatIndex				= fields[11].GetUInt32();			
			Temp.LevelIndex				= fields[12].GetUInt32();
			Temp.Quality	= fields[13].GetInt32();
			Temp.ItemLevel	= fields[14].GetUInt32();
			Temp.Suffix		= fields[15].GetString();

			NpMap.insert(std::make_pair(Entry, Temp));

		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT ItemStatType, Prefix From _itemmod_nopatch_stat_prefix"))
	{
		do
		{
			Field* fields = result->Fetch();
			NpPrefixMap.insert(std::make_pair(fields[0].GetUInt32(), fields[1].GetString()));
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT Level, ReqId, Chance, SpellIndex,AddSpellCount,StatIndex,AddStatCount,LevelStatModIndex,DamageLevelType,DamageValue,"
		"Suffix,ItemLevel,Quality,LevelIndex,EnchantIndex From _itemmod_nopatch_level"))
	{
		do
		{
			Field* f = result->Fetch();
			_NpItemLevel Temp;
			Temp.Level				= f[0].GetUInt8();
			Temp.ReqId				= f[1].GetUInt32();
			Temp.Chance				= f[2].GetFloat();
			Temp.SpellIndex			= f[3].GetUInt32();
			Temp.AddSpellCount		= f[4].GetUInt32();
			Temp.StatIndex			= f[5].GetUInt32();
			Temp.AddStatCount		= f[6].GetUInt32();
			Temp.LevelStatModIndex	= f[7].GetUInt8();

			if (strcmp("加值", f[8].GetCString()) == 0)
				Temp.DamageLevelType = TYPE_ADD_VALUE;
			else
				Temp.DamageLevelType = TYPE_MUI_VALUE;
			Temp.DamageValue		= f[9].GetFloat();

			Temp.Suffix				= f[10].GetString();
			Temp.ItemLevel			= f[11].GetUInt32();
			Temp.Quality			= f[12].GetUInt32();
			Temp.LevelIndex			= f[13].GetUInt32();
			Temp.EnchantIndex		= f[14].GetUInt32();
			NpLevelVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT LevelStatModIndex, StatType, ModType, StatValue From _itemmod_nopatch_level_statmod"))
	{
		do
		{
			Field* fields = result->Fetch();
			_NpItemLevelStatModTemplate Temp;
			Temp.LevelStatModIndex = fields[0].GetUInt32();
			Temp.StatType = fields[1].GetUInt32();


			if (strcmp("加值", fields[2].GetCString()) == 0)
				Temp.ModType = TYPE_ADD_VALUE;
			else
				Temp.ModType = TYPE_MUI_VALUE;

			Temp.StatValue = fields[3].GetFloat();
			NpLevelStatModVec.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT LevelIndex, Level, Count, RewId, Aura1, Aura2, Aura3, Aura4, Aura5 From _itemmod_nopatch_aura"))
	{
		do
		{
			Field* fields = result->Fetch();
			NpAuraTemplate Temp;
			Temp.LevelIndex = fields[0].GetUInt32();
			Temp.Level		= fields[1].GetUInt32();
			Temp.Count		= fields[2].GetUInt32();
			Temp.RewId		= fields[3].GetUInt32();
			for (size_t i = 0; i < 5; i++)
				Temp.Auras[i] = fields[4 + i].GetUInt32();
			NpAuraVector.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT Entry, Level, Chance From _itemmod_nopatch_tolevel"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 Entry = fields[0].GetUInt32();
			NpToLevelTemplate Temp;
			Temp.level = fields[1].GetUInt32();
			Temp.chance = fields[2].GetFloat();
			NpToLevelMap.insert(std::make_pair(Entry, Temp));
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT SrcTempIndex, TargetTempIndex, MeetLevel, RewId From _itemmod_nopatch_compound"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 SrcTempIndex = fields[0].GetUInt32();
			NpSameTemplate Temp;
			Temp.SrcTempIndex		= fields[0].GetUInt32();
			Temp.TargetTempIndex	= fields[1].GetUInt32();
			Temp.MeetLevel			= fields[2].GetUInt32();
			Temp.RewId				= fields[3].GetUInt32();
			NpSameTempIndexVector.push_back(Temp);
		} while (result->NextRow());
	}

	LoadMap();
}

void NoPatchItem::LoadMap()
{
	MapNpVector.clear();
	MapNpTempMap.clear();
	MapNpStatModVector.clear();
	MapNpEnchantVector.clear();
	MapNpWeaponVector.clear();

	QueryResult result;
	//											0		1		2		3			4
	if (result = WorldDatabase.PQuery("SELECT MapId, Diff, ChallengeLv,TempIndex,TempChance From _itemmod_nopatch_map"))
	{
		do
		{
			Field* fields = result->Fetch();
			MapNpTemplate Temp;
			Temp.MapId			= fields[0].GetUInt32();
			Temp.Diff			= Difficulty(fields[1].GetUInt8());
			Temp.ChallengeLv	= fields[2].GetUInt32();
			Temp.TempIndex		= fields[3].GetUInt32();
			Temp.Chance			= fields[4].GetFloat();
			MapNpVector.push_back(Temp);

		} while (result->NextRow());
	}

	//											0			1		2		3			4			5	
	if (result = WorldDatabase.PQuery("SELECT TempIndex,Quality,ItemLevel,EnchantIndex,Suffix,LeveIndex,"
		//		6		7				8		9		10			11			12	
		"StatModIndex,StatAddData,StatOverData,SpellIndex,SpellCount, WeaponIndex,SellRewId,Prefix From _itemmod_nopatch_map_temp"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 TempIndex = fields[0].GetUInt32();
			MapNpTempTemplate Temp;
			Temp.Quality		= fields[1].GetUInt32();
			Temp.ItemLevel		= fields[2].GetUInt32();
			Temp.EnchantIndex	= fields[3].GetUInt32();
			Temp.Suffix			= fields[4].GetString();
			Temp.LevelIndex		= fields[5].GetUInt32();
			Temp.StatModIndex	= fields[6].GetUInt32();

			Tokenizer StatAdd(fields[7].GetString(), '#');
			for (Tokenizer::const_iterator itr = StatAdd.begin(); itr != StatAdd.end(); ++itr)
			{
				Tokenizer data(*itr, ',');
				if (data.size() > 1)
					Temp.StatAddMap.insert(std::make_pair(atoi(data[0]), atoi(data[1])));
			}

			Tokenizer StatOver(fields[8].GetString(), '#');
			for (Tokenizer::const_iterator itr = StatOver.begin(); itr != StatOver.end(); ++itr)
			{
				Tokenizer data(*itr, ',');
				if (data.size() > 1)
					Temp.StatOverMap.insert(std::make_pair(atoi(data[0]), atoi(data[1])));
			}

			Temp.SpellIndex		= fields[9].GetInt32();
			Temp.SpellCount		= fields[10].GetUInt32();
			if (Temp.SpellCount > MAX_ITEM_PROTO_SPELLS)
				Temp.SpellCount = MAX_ITEM_PROTO_SPELLS;
			Temp.WeaponIndex	= fields[11].GetUInt32();
			Temp.SellRewId		= fields[12].GetUInt32();
			Temp.Prefix			= fields[13].GetString();
			MapNpTempMap.insert(std::make_pair(TempIndex, Temp));

		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT StatModIndex, StatType, ModStatMin, ModStatMax From _itemmod_nopatch_map_statmod"))
	{
		do
		{
			Field* fields = result->Fetch();
			MapNpStatModTemplate Temp;
			Temp.StatModIndex	= fields[0].GetUInt32();
			Temp.StatType		= fields[1].GetUInt32();
			Temp.ModStatMin		= fields[2].GetFloat();
			Temp.ModStatMax		= fields[3].GetFloat();
			MapNpStatModVector.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT EnchantIndex, Slot, EnchantGroupId From _itemmod_nopatch_map_enchant"))
	{
		do
		{
			Field* fields = result->Fetch();
			MapNpEnchantTemplate Temp;
			Temp.EnchantIndex	= fields[0].GetUInt32();
			Temp.Slot			= fields[1].GetUInt8();
			Temp.EnchantGroupId = fields[2].GetUInt32();
			MapNpEnchantVector.push_back(Temp);
		} while (result->NextRow());
	}

	if (result = WorldDatabase.PQuery("SELECT WeaponIndex, SubClass, MinDamageMod, MaxDamageMod, MinDamageMin,MinDamageMax,MaxDamageMin,MaxDamageMax,MinDelay,MaxDelay From _itemmod_nopatch_map_weapon"))
	{
		do
		{
			Field* fields = result->Fetch();
			MapNpWeaponTemplate Temp;
			Temp.WeaponIndex			= fields[0].GetUInt32();
			Temp.SubClass				= fields[1].GetUInt32();
			Temp.MinDamageMod			= fields[2].GetFloat();
			Temp.MaxDamageMod			= fields[3].GetFloat();
			if (Temp.MaxDamageMod < Temp.MinDamageMod)
				Temp.MaxDamageMod = Temp.MinDamageMod;
			Temp.Damage.MinDamageMin	= fields[4].GetUInt32();
			Temp.Damage.MinDamageMax	= fields[5].GetUInt32();
			if (Temp.Damage.MinDamageMax < Temp.Damage.MinDamageMin)
				Temp.Damage.MinDamageMax = Temp.Damage.MinDamageMin;
			Temp.Damage.MaxDamageMin	= fields[6].GetUInt32();
			Temp.Damage.MaxDamageMax	= fields[7].GetUInt32();
			if (Temp.Damage.MaxDamageMax < Temp.Damage.MaxDamageMin)
				Temp.Damage.MaxDamageMax = Temp.Damage.MaxDamageMin;
			Temp.Delay.MinDelay			= fields[8].GetUInt32();
			Temp.Delay.MaxDelay			= fields[9].GetUInt32();
			if (Temp.Delay.MaxDelay	 < Temp.Delay.MinDelay)
				Temp.Delay.MaxDelay = Temp.Delay.MinDelay;
			MapNpWeaponVector.push_back(Temp);
		} while (result->NextRow());
	}
}

bool NoPatchItem::CalBase(uint32 entry, uint32 &SrcIndex, uint32 &SpellIndex, uint32 &SpellCount, uint32 &StatIndex, uint32 &StatCount, 
	uint32 &minDamage, uint32 &maxDamage, uint32 &delay, std::string &Suffix, uint32 &ItemLvel, int32 &Quality)

{
	std::unordered_map < uint32, NpTemplate>::iterator itr = NpMap.find(entry);

	if (itr != NpMap.end())
	{
		SrcIndex = itr->second.SrcIndex;
		SpellIndex = itr->second.SpellIndex;
		SpellCount = itr->second.SpellCount;
		StatIndex = itr->second.StatIndex;
		StatCount = itr->second.StatCount;
		minDamage = urand(itr->second.Damage.MinDamageMin, itr->second.Damage.MinDamageMax);
		maxDamage = urand(itr->second.Damage.MaxDamageMin, itr->second.Damage.MaxDamageMax);
		delay = urand(itr->second.Delay.MinDelay, itr->second.Delay.MaxDelay);
		Suffix = itr->second.Suffix;
		Quality = itr->second.Quality;
		ItemLvel = itr->second.ItemLevel;
		return true;
	}
	return false;
}

bool NoPatchItem::CalLevel(Item* item, uint32 &L_ReqId, float &L_Chance,
	uint32 &L_SpellIndex, uint32 &L_AddSpellCount, uint32 &L_StatIndex, uint32 &L_AddStatCount,
	uint8 &L_LevelStatModIndex, NpLevelTypes &L_DamageLevelType, float &L_DamageValue,
	std::string &L_Suffix, uint32 &L_ItemLvel, uint32 &L_Quality, uint32 &L_EnchantIndex)

{
	if (item->MapData > 0)
	{
		auto itr = MapNpTempMap.find(item->TempIndex);

		if (itr != MapNpTempMap.end())
		{
			for (auto i = NpLevelVec.begin(); i != NpLevelVec.end(); i++)
			{
				if (itr->second.LevelIndex == i->LevelIndex && GetMapLevelNow(item->MapData) == i->Level)
				{
					L_ReqId = i->ReqId;
					L_Chance = i->Chance;
					L_SpellIndex = i->SpellIndex;
					L_AddSpellCount = i->AddSpellCount;
					L_StatIndex = i->StatIndex;
					L_AddStatCount = i->AddStatCount;
					L_LevelStatModIndex = i->LevelStatModIndex;
					L_DamageLevelType = i->DamageLevelType;
					L_DamageValue = i->DamageValue;
					L_Suffix = i->Suffix;
					L_ItemLvel = i->ItemLevel;
					L_Quality = i->Quality;
					L_EnchantIndex = i->EnchantIndex;
					return true;
				}
			}
		}
	}

	if (item->LevelData > 0 && item->MapData == 0)
	{
		uint64 LevelData = item->LevelData;
		uint32 LevelEntry = GetLevelEntry(LevelData);
		uint8 LevelNow = GetLevelNow(LevelData);

		auto iter = NpMap.find(LevelEntry);

		if (iter != NpMap.end())
		{
			for (auto i = NpLevelVec.begin(); i != NpLevelVec.end(); i++)
			{
				if (LevelNow == i->Level && iter->second.LevelIndex == i->LevelIndex)
				{
					L_ReqId = i->ReqId;
					L_Chance = i->Chance;
					L_SpellIndex = i->SpellIndex;
					L_AddSpellCount = i->AddSpellCount;
					L_StatIndex = i->StatIndex;
					L_AddStatCount = i->AddStatCount;
					L_LevelStatModIndex = i->LevelStatModIndex;
					L_DamageLevelType = i->DamageLevelType;
					L_DamageValue = i->DamageValue;
					L_Suffix = i->Suffix;
					L_ItemLvel = i->ItemLevel;
					L_Quality = i->Quality;
					L_EnchantIndex = i->EnchantIndex;
					return true;
				}
			}
		}
	}

	return false;
}

bool NoPatchItem::GetExchange(Item* item, uint32 &L_ReqId, float &L_Chance)
{
	if (item->MapData > 0)
	{
		auto itr = MapNpTempMap.find(item->TempIndex);

		if (itr != MapNpTempMap.end())
		{
			for (auto i = NpLevelVec.begin(); i != NpLevelVec.end(); i++)
			{
				if (itr->second.LevelIndex == i->LevelIndex && GetMapLevelNow(item->MapData) == i->Level)
				{
					L_ReqId = i->ReqId;
					L_Chance = i->Chance;
					return true;
				}
			}
		}
	}
	
	if (item->LevelData > 0 && item->MapData == 0)
	{
		uint64 LevelData = item->LevelData;
		uint32 LevelEntry = GetLevelEntry(LevelData);
		uint8 LevelNow = GetLevelNow(LevelData);

		auto iter = NpMap.find(LevelEntry);

		if (iter != NpMap.end())
		{
			for (auto i = NpLevelVec.begin(); i != NpLevelVec.end(); i++)
			{
				if (LevelNow == i->Level && iter->second.LevelIndex == i->LevelIndex)
				{
					L_ReqId = i->ReqId;
					L_Chance = i->Chance;
					return true;
				}
			}
		}
	}
	
	return false;
}

uint32 NoPatchItem::GetSellRewId(Item* item)
{
	auto iter = MapNpTempMap.find(item->TempIndex);
	if (iter != MapNpTempMap.end())
		return iter->second.SellRewId;

	return 0;
}

std::string NoPatchItem::GetPrefix(Item* item)
{
	uint32 ItemStatType = MAX_ITEM_MOD;
	uint32 max = 0;

	for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		if (item->Stats[i].ItemStatValue > max)
			max = item->Stats[i].ItemStatValue;

	for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		if (item->Stats[i].ItemStatValue == max && item->Stats[i].ItemStatValue != 0)
			ItemStatType = item->Stats[i].ItemStatType;

	std::unordered_map<uint32, std::string>::iterator itr = NpPrefixMap.find(ItemStatType);

	if (itr != NpPrefixMap.end())
		return itr->second;

	return "";
}

bool SrcSort(const NpSrcTemplate &a, const NpSrcTemplate &b){
	return a.SrcChance < b.SrcChance;
}

bool StatSort(const NpStatTemplate &a, const NpStatTemplate &b){
	return a.StatChance < b.StatChance;
}

bool SpellSort(const NpSpellTemplate &a, const NpSpellTemplate &b){
	return a.SpellChance <  b.SpellChance;
	;
}

uint32 NoPatchItem::GetRandomSrcEntry(uint32 SrcIndex)
{
	if (NpSrcVec.empty())
		return 0;

	uint32 srcEntry = 0;

	std::vector<NpSrcTemplate> vtMap;

	for (size_t i = 0; i < NpSrcVec.size(); i++)
		if (SrcIndex == NpSrcVec[i].SrcIndex)
			vtMap.push_back(NpSrcVec[i]);

	if (vtMap.empty())
		return srcEntry;


	sort(vtMap.begin(), vtMap.end(), SrcSort);


	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->SrcChance;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->SrcChance;
		if (rand < sum)
		{
			srcEntry = it->SrcEntry;
			break;
		}
	}

	return srcEntry;
}

uint32 NoPatchItem::GetRandomTempIndex(uint32 MapId, Difficulty Diff, uint32 ChallengeLv)
{
	if (MapNpVector.empty())
		return 0;


	uint32 TempIndex = 0;

	std::vector<std::pair<int, float>> vtMap;

	for (auto it = MapNpVector.begin(); it != MapNpVector.end(); it++)
		if (it->MapId == MapId && it->Diff == Diff && it->ChallengeLv == ChallengeLv)
			vtMap.push_back(std::make_pair(it->TempIndex, it->Chance));

	sort(vtMap.begin(), vtMap.end(),
		[](const std::pair<int, float> &x, const std::pair<int, float> &y) -> int {
		return x.second < y.second;
	});

	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->second;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->second;
		if (rand <= sum)
		{
			TempIndex = it->first;
			break;
		}
	}

	return TempIndex;
}

_Spell NoPatchItem::GetRandomSpell(_Spell Spells[MAX_ITEM_PROTO_SPELLS], uint32 SpellIndex)
{
	_Spell Spell;
	Spell.SpellCategory = 0;
	Spell.SpellCategoryCooldown = -1;
	Spell.SpellCharges = 0;
	Spell.SpellCooldown = -1;
	Spell.SpellId = 0;
	Spell.SpellPPMRate = 0;
	Spell.SpellTrigger = 0;

	if (NpSpellVec.empty())
		return Spell;

	
	std::vector<NpSpellTemplate> vtMap;

	for (size_t i = 0; i < NpSpellVec.size(); i++)
		if (SpellIndex == NpSpellVec[i].SpellIndex)
		{
			bool exsit = false;
			for (size_t j = 0; j < MAX_ITEM_PROTO_SPELLS; j++)
				if (NpSpellVec[i].Spell.SpellId == Spells[j].SpellId)
					exsit = true;

			if (!exsit)
				vtMap.push_back(NpSpellVec[i]);
		}
			

	if (vtMap.empty())
		return Spell;

	sort(vtMap.begin(), vtMap.end(), SpellSort);

	
	float total = 0;
	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		total += it->SpellChance;

	float rand = frand(0, total);

	float sum = 0;
	
	for (auto it = vtMap.begin(); it != vtMap.end();++it)
	{
		sum += it->SpellChance;
	
		if (rand < sum)
			return it->Spell;
	}
	
	return Spell;
}

_ItemStat NoPatchItem::GetRandomStat(uint32 StatIndex)
{
	_ItemStat Stat;
	Stat.ItemStatType = 0;
	Stat.ItemStatValue = 0;

	if (NpStatVec.empty())
		return Stat;

	std::vector<NpStatTemplate> vtMap;

	for (size_t i = 0; i < NpStatVec.size(); i++)
		if (StatIndex == NpStatVec[i].StatIndex)
			vtMap.push_back(NpStatVec[i]);

	if (vtMap.empty())
		return Stat;


	sort(vtMap.begin(), vtMap.end(), StatSort);


	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->StatChance;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->StatChance;
		if (rand < sum)
		{
			Stat.ItemStatType = it->Stat.ItemStatType;
			Stat.ItemStatValue = urand(it->Stat.ItemStatMinValue, it->Stat.ItemStatMaxValue);
			break;
		}
	}

	return Stat;
}

void NoPatchItem::MergeStat(_ItemStat(&Stats)[MAX_ITEM_PROTO_STATS])
{
	for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
	{
		for (size_t j = i + 1; j < MAX_ITEM_PROTO_STATS; j++)
		{
			if (Stats[i].ItemStatType == Stats[j].ItemStatType)
			{
				Stats[i].ItemStatValue += Stats[j].ItemStatValue;
				Stats[j].ItemStatType = 0;
				Stats[j].ItemStatValue = 0;
			}
		}
	}
}

bool NoPatchItem::Create(Player* owner, Item* item)
{
	if (item->IsNoPatch())
		return false;

	uint32 entry = item->GetEntry();

	uint32 SrcIndex = 0;
	uint32 SpellIndex = 0;
	uint32 SpellCount = 0;
	uint32 StatIndex = 0;
	uint32 StatCount = 0;
	uint32 MinDamage = 0;
	uint32 MaxDamage = 0;
	uint32 Delay = 0;
	std::string Suffix = "";
	int32 Quality = 0;
	uint32 ItemLevel = 300;

	if (!CalBase(entry, SrcIndex, SpellIndex, SpellCount, StatIndex, StatCount, MinDamage, MaxDamage, Delay, Suffix, ItemLevel, Quality))
		return false;

	uint32 srcEntry = SrcIndex == 0 ? entry : GetRandomSrcEntry(SrcIndex);

	const ItemTemplate* srcTemp = sObjectMgr->GetItemTemplate(srcEntry);

	if (!srcTemp)
		return false;

	Item* pItem;

	if (Quality < 0)
		pItem = owner->AddItemById(entry, 1);
	else
		pItem = owner->AddItemById(srcEntry, 1);

	if (!pItem)
		return false;

	if (Quality < 0)
	{
		//设置新物品
		pItem->Name = srcTemp->Name1 + Suffix;
		pItem->LevelData = GetLevelData(entry, 1, srcTemp->Quality, ItemLevel);

		//产生技能
		_Spell NewSpells[MAX_ITEM_PROTO_SPELLS];

		for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
			NewSpells[i] = srcTemp->Spells[i];

		uint32 count = 0;

		for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
		{
			if (count >= SpellCount)
				continue;

			if (NewSpells[i].SpellId == 0)
			{
				NewSpells[i] = GetRandomSpell(NewSpells, SpellIndex);
				count++;
			}
		}

		for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
			pItem->Spells[i] = NewSpells[i];

		//产生属性
		for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		{
			pItem->Stats[i] = srcTemp->ItemStat[i];
			pItem->Stats[i].ItemStatValue = srcTemp->ItemStat[i].ItemStatValue * abs(Quality);
		}
		

		//产生属性
		_ItemStat NewStats[MAX_ITEM_PROTO_STATS];

		for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		{
			NewStats[i] = srcTemp->ItemStat[i];
			NewStats[i].ItemStatValue = srcTemp->ItemStat[i].ItemStatValue * abs(Quality);
		}

		count = 0;

		for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		{
			if (count >= StatCount)
				continue;

			if (NewStats[i].ItemStatType == 0 && NewStats[i].ItemStatValue == 0)
			{
				NewStats[i] = GetRandomStat(StatIndex);
				count++;
			}
		}

		MergeStat(NewStats);

		for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
			pItem->Stats[i] = NewStats[i];

		pItem->Damages[0].DamageMin = srcTemp->Damage[0].DamageMin;// *abs(Quality);
		pItem->Damages[0].DamageMax = srcTemp->Damage[0].DamageMax;// *abs(Quality);
		pItem->NpDelay = srcTemp->Delay;
	}
	else
	{
		//设置新物品
		pItem->Name = srcTemp->Name1 + Suffix;
		pItem->LevelData = GetLevelData(entry, 1, Quality, ItemLevel);

		//产生技能
		for (size_t i = 0; i < SpellCount; i++)
			pItem->Spells[i] = GetRandomSpell(pItem->Spells,SpellIndex);

		//产生属性
		for (size_t i = 0; i < StatCount; i++)
			pItem->Stats[i] = GetRandomStat(StatIndex);

		//合并属性
		MergeStat(pItem->Stats);

		//产生前缀
		pItem->Name = GetPrefix(pItem) + pItem->Name;

		pItem->Damages[0].DamageMin = MinDamage;
		pItem->Damages[0].DamageMax = MaxDamage;
		pItem->NpDelay = Delay;
	}
	
	//设置flag query data
	pItem->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, GetQueryId(pItem));
	//SetItemFlag(pItem);
	pItem->SetState(ITEM_CHANGED, owner);
	ItemQueryMap[pItem->GetGUIDLow()] = pItem;

	//摧毁原物品
	owner->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);

	return true;
}

Item* NoPatchItem::GetSameItem(Player* owner, Item* item)
{
	uint8 level = GetLevelNow(item->LevelData);

	for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
		if (Item* pItem = owner->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
			if (pItem->IsNoPatch() && pItem->GetGUID() != item->GetGUID() && pItem->GetEntry() == item->GetEntry() && GetLevelNow(pItem->LevelData) == level)
				return pItem;

	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* pItem = owner->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
			if (pItem->IsNoPatch() && pItem->GetGUID() != item->GetGUID() && pItem->GetEntry() == item->GetEntry() && GetLevelNow(pItem->LevelData) == level)
				return pItem;

	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = owner->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* pItem = owner->GetItemByPos(i, j))
					if (pItem->IsNoPatch() && pItem->GetGUID() != item->GetGUID() && pItem->GetEntry() == item->GetEntry() && GetLevelNow(pItem->LevelData) == level)
						return pItem;

	return NULL;
}

bool NoPatchItem::CanCompound(Item* item)
{
	if (!item->IsNoPatch())
		return false;

	uint32 level = GetLevelNow(item->LevelData) - 1;

	for (auto itr = NpSameTempIndexVector.begin(); itr != NpSameTempIndexVector.end(); itr++)
		if (itr->MeetLevel == level && itr->SrcTempIndex == item->TempIndex)
			return true;
	
	return false;
}

void NoPatchItem::GetTargetTemp(Item* item, uint32 &TargetTempIndex, uint32 &RewId)
{
	if (!item->IsNoPatch())
		return;

	uint32 level = GetLevelNow(item->LevelData) - 1;

	for (auto itr = NpSameTempIndexVector.begin(); itr != NpSameTempIndexVector.end(); itr++)
		if (itr->MeetLevel == level && itr->SrcTempIndex == item->TempIndex)
		{
			TargetTempIndex = itr->TargetTempIndex;
			RewId = itr->RewId;
		}
}

bool NoPatchItem::Compound(Player* owner, Item* item1)
{
	if (!item1->IsNoPatch())
	{
		owner->GetSession()->SendNotification("不是NoPathItem");
		return false;
	}
	
	const ItemTemplate* proto = item1->GetTemplate();
	if (!proto)
		return false;

	uint32 TempIndex = 0;
	uint32 RewId = 0;

	GetTargetTemp(item1, TempIndex, RewId);

	if (TempIndex == 0)
		return false;
	
	Item* item2 = GetSameItem(owner, item1);

	if (!item2)
	{
		owner->GetSession()->SendNotification("未找到完全相同的两件物品");
		return false;
	}

	auto itr = MapNpTempMap.find(TempIndex);

	if (itr != MapNpTempMap.end())
	{
		Item* item = owner->AddItemById(item1->GetEntry(), item1->GetCount());

		if (!item)
			return false;

		item->Name = proto->Name1 + itr->second.Suffix;

		if (itr->second.Quality > 0)
			item->LevelData = GetLevelData(item->GetEntry(), 1, itr->second.Quality, itr->second.ItemLevel);
		else
			item->LevelData = GetLevelData(item->GetEntry(), 1, proto->Quality, itr->second.ItemLevel);

		item->MapData = GetMapData(1, 1, 0, 0);

		//产生技能
		//附加额外技能
		if (itr->second.SpellIndex > 0)
		{
			_Spell NewSpells[MAX_ITEM_PROTO_SPELLS];

			for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
				NewSpells[i] = proto->Spells[i];

			uint32 count = 0;

			for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
			{
				if (count >= itr->second.SpellCount)
					continue;

				if (NewSpells[i].SpellId == 0)
				{
					NewSpells[i] = GetRandomSpell(NewSpells, itr->second.SpellIndex);
					count++;
				}
			}

			for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
				item->Spells[i] = NewSpells[i];
		}
		//额外技能
		else if (itr->second.SpellIndex < 0)
		{
			//产生技能
			for (size_t i = 0; i < itr->second.SpellCount; i++)
				item->Spells[i] = GetRandomSpell(item->Spells, abs(itr->second.SpellIndex));
		}
		else
		{
			for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
				item->Spells[i] = proto->Spells[i];
		}

		//原属性翻倍
		for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
			item->Stats[i] = proto->ItemStat[i];

		for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		{
			uint32 StatType = item->Stats[i].ItemStatType;

			for (auto it = MapNpStatModVector.begin(); it != MapNpStatModVector.end(); it++)
			{
				if (it->StatModIndex == itr->second.StatModIndex && it->StatType == StatType && item->Stats[i].ItemStatValue > 0)
				{
					float mod = frand(it->ModStatMin, it->ModStatMax);
					item->Stats[i].ItemStatValue *= mod;
					break;
				}
			}
		}

		//附加额外属性
		if (itr->second.StatOverMap.empty())
		{
			_ItemStat NewStats[MAX_ITEM_PROTO_STATS];
			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
				NewStats[i] = item->Stats[i];

			uint32 total = 0;

			for (auto it = itr->second.StatAddMap.begin(); it != itr->second.StatAddMap.end(); it++)
			{
				if (total >= MAX_ITEM_PROTO_STATS)
					continue;

				uint32 count = 0;

				for (size_t i = total; i < MAX_ITEM_PROTO_STATS; i++)
				{
					if (count >= it->second)
						continue;

					if (NewStats[i].ItemStatType == 0 && NewStats[i].ItemStatValue == 0)
					{
						NewStats[i] = GetRandomStat(it->first);
						count++;
						total++;
					}
				}
			}

			MergeStat(NewStats);

			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
				item->Stats[i] = NewStats[i];
		}
		else
		{
			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
			{
				item->Stats[i].ItemStatType = 0;
				item->Stats[i].ItemStatValue = 0;
			}

			_ItemStat NewStats[MAX_ITEM_PROTO_STATS];

			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
			{
				NewStats[i].ItemStatType = 0;
				NewStats[i].ItemStatValue = 0;
			}

			uint32 total = 0;

			for (auto it = itr->second.StatOverMap.begin(); it != itr->second.StatOverMap.end(); it++)
			{
				if (total >= MAX_ITEM_PROTO_STATS)
					continue;

				uint32 count = 0;

				for (size_t i = total; i < MAX_ITEM_PROTO_STATS; i++)
				{
					if (count >= it->second)
						continue;

					if (NewStats[i].ItemStatType == 0 && NewStats[i].ItemStatValue == 0)
					{
						NewStats[i] = GetRandomStat(it->first);
						count++;
						total++;
					}
				}
			}

			MergeStat(NewStats);

			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
				item->Stats[i] = NewStats[i];
		}

		//产生随机FM
		for (uint8 slot = PROP_ENCHANTMENT_SLOT_0; slot < MAX_ENCHANTMENT_SLOT; slot++)
		{
			uint32 enchant_id = GetCreateEnchantId(itr->second.EnchantIndex, slot - 6);

			if (enchant_id == 0)
				continue;

			if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
				item->SetEnchantment(EnchantmentSlot(slot), enchant_id, 0, 0);
		}

		item->Damages[0].DamageMin = proto->Damage[0].DamageMin;
		item->Damages[0].DamageMax = proto->Damage[0].DamageMax;
		item->NpDelay = proto->Delay;

		if (proto->Class == ITEM_CLASS_WEAPON)
		{
			for (auto i = MapNpWeaponVector.begin(); i != MapNpWeaponVector.end(); i++)
			{
				if (itr->second.WeaponIndex == i->WeaponIndex && proto->SubClass == i->SubClass)
				{
					item->Damages[0].DamageMin *= i->MinDamageMod;
					item->Damages[0].DamageMax *= i->MaxDamageMod;

					uint32 min = urand(i->Damage.MinDamageMin, i->Damage.MinDamageMax);
					uint32 max = urand(i->Damage.MaxDamageMin, i->Damage.MaxDamageMax);

					if (min != 0 && max != 0)
					{
						if (max < min)
							max = min;

						item->Damages[0].DamageMin = min;
						item->Damages[0].DamageMax = max;
					}

					uint32 delay = urand(i->Delay.MinDelay, i->Delay.MaxDelay);

					if (delay != 0)
						item->NpDelay = delay;
					else
						item->NpDelay = proto->Delay;

					break;
				}
			}
		}

		item->TempIndex = TempIndex;
		//设置flag query data
		item->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, GetQueryId(item));
		//SetItemFlag(item);
		ItemQueryMap[item->GetGUIDLow()] = item;
		if (Player* owner = item->GetOwner())
			item->SetState(ITEM_CHANGED, owner);

		owner->DestroyItem(item1->GetBagSlot(), item1->GetSlot(), true);
		owner->DestroyItem(item2->GetBagSlot(), item2->GetSlot(), true);
		sRew->Rew(owner, RewId);

		return true;
	}

	return false;
}

bool NoPatchItem::Create(Map* map, Item* item)
{
	if (!map || !item)
		return false;

	if (item->IsNoPatch())
		return false;

	const ItemTemplate* proto = item->GetTemplate();
	if (!proto)
		return false;

	if (proto->Class != ITEM_CLASS_WEAPON && proto->Class != ITEM_CLASS_ARMOR && proto->Class != ITEM_CLASS_CONTAINER)
		return false;

	uint32 TempIndex = GetRandomTempIndex(map->GetId(), map->GetDifficulty(), map->challengeLv);

	auto itr = MapNpTempMap.find(TempIndex);

	if (itr != MapNpTempMap.end())
	{
		item->Name = proto->Name1 + itr->second.Suffix;

		if (itr->second.Quality > 0)
			item->LevelData = GetLevelData(item->GetEntry(), 1, itr->second.Quality, itr->second.ItemLevel);
		else
			item->LevelData = GetLevelData(item->GetEntry(), 1, proto->Quality, itr->second.ItemLevel);

		item->MapData = GetMapData(map->GetId(), 1, map->GetDifficulty(), map->challengeLv);

		//产生技能
		//附加额外技能
		if (itr->second.SpellIndex > 0)
		{
			_Spell NewSpells[MAX_ITEM_PROTO_SPELLS];

			for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
				NewSpells[i] = proto->Spells[i];

			uint32 count = 0;

			for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
			{
				if (count >= itr->second.SpellCount)
					continue;

				if (NewSpells[i].SpellId == 0)
				{
					NewSpells[i] = GetRandomSpell(NewSpells, itr->second.SpellIndex);
					count++;
				}
			}

			for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
				item->Spells[i] = NewSpells[i];
		}
		//额外技能
		else if (itr->second.SpellIndex < 0)
		{
			//产生技能
			for (size_t i = 0; i < itr->second.SpellCount; i++)
				item->Spells[i] = GetRandomSpell(item->Spells, abs(itr->second.SpellIndex));
		}
		else
		{
			for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
				item->Spells[i] = proto->Spells[i];
		}

		//原属性翻倍
		for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
			item->Stats[i] = proto->ItemStat[i];

		for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		{
			uint32 StatType = item->Stats[i].ItemStatType;

			for (auto it = MapNpStatModVector.begin(); it != MapNpStatModVector.end(); it++)
			{
				if (it->StatModIndex == itr->second.StatModIndex && it->StatType == StatType && item->Stats[i].ItemStatValue > 0)
				{
					float mod = frand(it->ModStatMin, it->ModStatMax);
					item->Stats[i].ItemStatValue *= mod;
					break;
				}
			}
		}

		//附加额外属性
		if (itr->second.StatOverMap.empty())
		{
			_ItemStat NewStats[MAX_ITEM_PROTO_STATS];
			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
				NewStats[i] = item->Stats[i];

			uint32 total = 0;

			for (auto it = itr->second.StatAddMap.begin(); it != itr->second.StatAddMap.end(); it++)
			{
				if (total >= MAX_ITEM_PROTO_STATS)
					continue;

				uint32 count = 0;

				for (size_t i = total; i < MAX_ITEM_PROTO_STATS; i++)
				{
					if (count >= it->second)
						continue;

					if (NewStats[i].ItemStatType == 0 && NewStats[i].ItemStatValue == 0)
					{
						NewStats[i] = GetRandomStat(it->first);
						count++;
						total++;
					}
				}
			}

			MergeStat(NewStats);

			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
				item->Stats[i] = NewStats[i];
		}
		else
		{
			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
			{
				item->Stats[i].ItemStatType = 0;
				item->Stats[i].ItemStatValue = 0;
			}

			_ItemStat NewStats[MAX_ITEM_PROTO_STATS];

			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
			{
				NewStats[i].ItemStatType = 0;
				NewStats[i].ItemStatValue = 0;
			}

			uint32 total = 0;

			for (auto it = itr->second.StatOverMap.begin(); it != itr->second.StatOverMap.end(); it++)
			{
				if (total >= MAX_ITEM_PROTO_STATS)
					continue;

				uint32 count = 0;

				for (size_t i = total; i < MAX_ITEM_PROTO_STATS; i++)
				{
					if (count >= it->second)
						continue;

					if (NewStats[i].ItemStatType == 0 && NewStats[i].ItemStatValue == 0)
					{
						NewStats[i] = GetRandomStat(it->first);
						count++;
						total++;
					}
				}
			}

			MergeStat(NewStats);

			for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
				item->Stats[i] = NewStats[i];				
		}

		//产生随机FM
		for (uint8 slot = PROP_ENCHANTMENT_SLOT_0; slot < MAX_ENCHANTMENT_SLOT; slot++)
		{
			uint32 enchant_id = GetCreateEnchantId(itr->second.EnchantIndex, slot - 6);

			if (enchant_id == 0)
				continue;

			if (SpellItemEnchantmentEntry const* enchantEntry = sSpellItemEnchantmentStore.LookupEntry(enchant_id))
				item->SetEnchantment(EnchantmentSlot(slot), enchant_id, 0, 0);
		}

		item->Damages[0].DamageMin = proto->Damage[0].DamageMin;
		item->Damages[0].DamageMax = proto->Damage[0].DamageMax;
		item->NpDelay = proto->Delay;

		if (proto->Class == ITEM_CLASS_WEAPON)
		{
			for (auto i = MapNpWeaponVector.begin(); i != MapNpWeaponVector.end(); i++)
			{
				if (itr->second.WeaponIndex == i->WeaponIndex && proto->SubClass == i->SubClass)
				{
					item->Damages[0].DamageMin *= i->MinDamageMod;
					item->Damages[0].DamageMax *= i->MaxDamageMod;

					uint32 min = urand(i->Damage.MinDamageMin, i->Damage.MinDamageMax);
					uint32 max = urand(i->Damage.MaxDamageMin, i->Damage.MaxDamageMax);

					if (min != 0 && max != 0)
					{
						if (max < min)
							max = min;

						item->Damages[0].DamageMin = min;
						item->Damages[0].DamageMax = max;
					}

					uint32 delay = urand(i->Delay.MinDelay, i->Delay.MaxDelay);

					if (delay != 0)
						item->NpDelay = delay;
					else
						item->NpDelay = proto->Delay;

					break;
				}
			}
		}

		item->TempIndex = TempIndex;
		//设置flag query data
		item->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, GetQueryId(item));
		//SetItemFlag(item);
		ItemQueryMap[item->GetGUIDLow()] = item;
		if (Player* owner = item->GetOwner())
			item->SetState(ITEM_CHANGED, owner);

		return true;
	}

	return false;
}

uint32 NoPatchItem::GetCreateEnchantId(uint32 EnchantIndex, uint8 Slot)
{
	uint32 EnchantGroupId = 0;

	for (auto itr = MapNpEnchantVector.begin(); itr != MapNpEnchantVector.end(); itr++)
		if (itr->EnchantIndex == EnchantIndex && itr->Slot == Slot)
			EnchantGroupId = itr->EnchantGroupId;

	if (EnchantGroupId == 0)
		return 0;

	uint32 len_group = EnchantGroupVec.size();

	std::unordered_map<uint32, float> EnchantIdMap;

	for (size_t i = 0; i < len_group; i++)
		if (EnchantGroupId == EnchantGroupVec[i].groupId)
			EnchantIdMap.insert(std::make_pair(EnchantGroupVec[i].enchantId, EnchantGroupVec[i].chance));

	std::vector<std::pair<int, float>> vtMap;
	for (auto it = EnchantIdMap.begin(); it != EnchantIdMap.end(); it++)
		vtMap.push_back(std::make_pair(it->first, it->second));

	sort(vtMap.begin(), vtMap.end(),
		[](const std::pair<int, float> &x, const std::pair<int, float> &y) -> int {
		return x.second < y.second;
	});


	float sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
		sum += it->second;

	float rand = frand(0, sum);

	sum = 0;

	for (auto it = vtMap.begin(); it != vtMap.end(); it++)
	{
		sum += it->second;
		if (rand <= sum)
			return it->first;
	}

	return 0;
}

void NoPatchItem::GetMapPrefixSuffix(Item* item, std::string &prefix, std::string &suffix)
{
	auto iter = MapNpTempMap.find(item->TempIndex);
	if (iter != MapNpTempMap.end())
	{
		prefix = iter->second.Prefix;
		suffix = iter->second.Suffix;
	}
}

int32 NoPatchItem::GetLevelStat(uint32 LevelStatModIndex, uint32 type, int32 value)
{
	for (auto itr = NpLevelStatModVec.begin(); itr != NpLevelStatModVec.end(); itr++)
	{
		if (LevelStatModIndex == itr->LevelStatModIndex && type == itr->StatType)
		{
			if (itr->ModType == TYPE_MUI_VALUE)
				return value * itr->StatValue;
			else
				return value + itr->StatValue;
		}
	}

	return value;
}

bool NoPatchItem::LevelUp(Player* owner, Item* item)
{
	uint32 L_ReqId = 0;
	float L_Chance = 0;
	uint32 L_SpellIndex = 0;
	uint32 L_AddSpellCount = 0;
	uint32 L_StatIndex = 0;
	uint32 L_AddStatCount = 0;
	uint8 L_LevelStatModIndex = 0;
	NpLevelTypes L_DamageLevelType = TYPE_ADD_VALUE;
	float L_DamageValue = 0;
	std::string L_Suffix = "";
	uint32 L_ItemLvel = 0;
	uint32 L_Quality = 0;
	uint32 L_EnchantIndex = 0;

	if (!CalLevel(item, L_ReqId, L_Chance, L_SpellIndex, L_AddSpellCount, L_StatIndex, L_AddStatCount, L_LevelStatModIndex, L_DamageLevelType, L_DamageValue, L_Suffix, L_ItemLvel, L_Quality, L_EnchantIndex))
		return false;

	if (!sReq->Check(owner, L_ReqId))
		return false;

	if (frand(0, 100.0f) > L_Chance)
	{
		sReq->Des(owner, L_ReqId);
		owner->GetSession()->SendNotification("升级失败");
		return false;
	}

	Item* pItem = item->CloneItem(item->GetCount(), owner);

	if (!pItem)
		return false;

	//设置新物品
	std::string prefix = "";
	std::string suffix = "";
	GetMapPrefixSuffix(item, prefix, suffix);
	pItem->Name = prefix + item->GetTemplate()->Name1 + suffix + L_Suffix;
	pItem->TempIndex = item->TempIndex;


	if (L_Quality > 0)
		pItem->LevelData = GetLevelData(GetLevelEntry(item->LevelData), GetLevelNow(item->LevelData) + 1, L_Quality, L_ItemLvel);
	else
		pItem->LevelData = GetLevelData(GetLevelEntry(item->LevelData), GetLevelNow(item->LevelData) + 1, GetQuality(item->LevelData), L_ItemLvel);

	
	if (item->MapData > 0)
		pItem->MapData = GetMapData(GetMapId(item->MapData), GetMapLevelNow(item->MapData) + 1, GetDifficulty(item->MapData), GetChallegeLv(item->MapData));

	//产生技能
	_Spell NewSpells[MAX_ITEM_PROTO_SPELLS];

	for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
		NewSpells[i] = item->Spells[i];

	uint32 count = 0;

	for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
	{
		if (count >= L_AddSpellCount)
			continue;

		if (NewSpells[i].SpellId == 0)
		{
			NewSpells[i] = GetRandomSpell(NewSpells, L_SpellIndex);
			count++;
		}
	}

	for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
		pItem->Spells[i] = NewSpells[i];

	//产生属性
	_ItemStat NewStats[MAX_ITEM_PROTO_STATS];

	for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		NewStats[i] = item->Stats[i];

	for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
	{
		if (NewStats[i].ItemStatType == 0 && NewStats[i].ItemStatValue == 0)
			continue;

		NewStats[i].ItemStatValue = GetLevelStat(L_LevelStatModIndex, NewStats[i].ItemStatType, NewStats[i].ItemStatValue);

		//NewStats[i].ItemStatValue = L_StatLevelType == TYPE_ADD_VALUE ? NewStats[i].ItemStatValue + int32(L_StatValue) : NewStats[i].ItemStatValue * L_StatValue;
	}
	
	count = 0;

	for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
	{
		if (count >= L_AddStatCount)
			continue;

		if (NewStats[i].ItemStatType == 0 && NewStats[i].ItemStatValue == 0)
		{
			NewStats[i] = GetRandomStat(L_StatIndex);
			count++;
		}
	}

	MergeStat(NewStats);

	for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
		pItem->Stats[i] = NewStats[i];

	//产生前缀
	pItem->Name = GetPrefix(pItem) + pItem->Name;

	pItem->Damages[0].DamageMin = L_DamageLevelType == TYPE_ADD_VALUE ? item->Damages[0].DamageMin + L_DamageValue : item->Damages[0].DamageMin * L_DamageValue;
	pItem->Damages[0].DamageMax = L_DamageLevelType == TYPE_ADD_VALUE ? item->Damages[0].DamageMax + L_DamageValue : item->Damages[0].DamageMax * L_DamageValue;;

	pItem->NpDelay = item->NpDelay;

	//保留FM效果
	for (uint8 slot = PERM_ENCHANTMENT_SLOT; slot < MAX_ENCHANTMENT_SLOT; slot++)
	{
		uint32 x = GetCreateEnchantId(L_EnchantIndex, slot - 6);

		uint32 enchantId = x > 0 ? x : item->GetEnchantmentId(EnchantmentSlot(slot));

		if (SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(enchantId))
		{
			owner->ApplyEnchantment(pItem, EnchantmentSlot(slot), false);
			pItem->SetEnchantment(EnchantmentSlot(slot), enchantId, 0, 0);
			owner->ApplyEnchantment(pItem, EnchantmentSlot(slot), true);
		}
	}

	//设置flag query data
	pItem->SetUInt32Value(ITEM_FIELD_PROPERTY_SEED, GetQueryId(pItem));
	//SetItemFlag(pItem);
	pItem->SetState(ITEM_CHANGED, owner);
	ItemQueryMap[pItem->GetGUIDLow()] = pItem;

	//扣减消耗摧毁原物品
	sReq->Des(owner, L_ReqId);
	owner->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);

	ItemPosCountVec sDest;
	InventoryResult msg = owner->CanStoreItem(NULL_BAG, NULL_SLOT, sDest, pItem, false);
	if (msg == EQUIP_ERR_OK)
		owner->StoreItem(sDest, pItem, true);
	else
		owner->SendEquipError(msg, pItem, NULL);

	owner->CLOSE_GOSSIP_MENU();


	UpdateAuras(owner);

	return true;
}

uint8 NoPatchItem::GetInsert(ItemTemplate temp)
{
	uint8 insert = 0;
	//名字
	insert++;

	sLog->outString("-->%d->>>>名字", insert);

	//英雄模式
	if ((temp.Flags & ITEM_FLAG_HEROIC_TOOLTIP) != 0)
	{
		insert++;
		sLog->outString("-->%d->>>>英雄模式", insert);
	}

	//绑定 账号绑定 装备绑定...
	if (temp.Bonding != NO_BIND || (temp.Flags & ITEM_FLAG_IS_BOUND_TO_ACCOUNT) != 0 || temp.Quality == 7)
	{
		insert++;
		sLog->outString("-->%d->>>>绑定", insert);
	}

	//魔法制作的物品
	if ((temp.Flags & ITEM_FLAG_CONJURED) != 0)
		insert++;

	//唯一 最大数量 装备唯一...
	if (temp.MaxCount != 0 || (temp.Flags & ITEM_FLAG_UNIQUE_EQUIPPABLE) != 0)
	{
		insert++;
		sLog->outString("-->%d->>>>最大数量", insert);
	}

	//抗性
	if (temp.HolyRes != 0)
		insert++;
	if (temp.FireRes != 0)
		insert++;
	if (temp.NatureRes != 0)
		insert++;
	if (temp.FrostRes != 0)
		insert++;
	if (temp.ShadowRes != 0)
		insert++;
	if (temp.ArcaneRes != 0)
		insert++;

	//护甲
	if (temp.Armor > 0)
	{
		insert++;
		sLog->outString("-->%d->>>>护甲", insert);
	}

	//双手 护甲...
	if (temp.InventoryType != 0 && (temp.Class == ITEM_CLASS_WEAPON || temp.Class == ITEM_CLASS_ARMOR))
	{
		insert++;
		sLog->outString("-->%d->>>>位置", insert);
	}

	//伤害
	if (temp.Damage[0].DamageMin > 0 || temp.Damage[0].DamageMax > 0)
	{
		insert++;
		sLog->outString("-->%d->>>>伤害", insert);
	}
	if (temp.Damage[1].DamageMin > 0 || temp.Damage[1].DamageMax > 0)
	{
		insert++;
		sLog->outString("-->%d->>>>伤害", insert);
	}


	//秒伤
	if (temp.Damage[0].DamageMin > 0 && temp.Damage[0].DamageMax > 0 && temp.Delay > 0)
	{
		insert++;
		sLog->outString("-->%d->>>>秒伤", insert);
	}

	//属性
	for (uint8 i = 0; i < temp.StatsCount; ++i)
		if ((temp.ItemStat[i].ItemStatType == 0
			|| temp.ItemStat[i].ItemStatType == 1
			|| temp.ItemStat[i].ItemStatType == 3
			|| temp.ItemStat[i].ItemStatType == 4
			|| temp.ItemStat[i].ItemStatType == 5
			|| temp.ItemStat[i].ItemStatType == 6
			|| temp.ItemStat[i].ItemStatType == 7
			) && temp.ItemStat[i].ItemStatValue != 0)
		{
			insert++;
			sLog->outString("-->%d->>>>ItemStatType:%d", insert, temp.ItemStat[i].ItemStatType);
		}


	//宝石
	for (size_t i = 0; i < MAX_ITEM_PROTO_SOCKETS; i++)
		if (temp.Socket[i].Color != 0)
		{
			insert++;
			sLog->outString("-->%d->>>>宝石", insert);
		}

	//使用区域
	if (temp.Area != 0)
		insert++;

	if (temp.Map != 0)
		insert++;

	return insert;
}

uint32 NoPatchItem::GetItemFlag(Item* item)
{
	//uint32 insert = GetInsert(*(item->GetTemplate()));
	uint32 ReqId = 0;
	float Chance = 0;
	uint32 SellRewId = GetSellRewId(item);
	GetExchange(item, ReqId, Chance);


	std::ostringstream oss;
	oss << item->IsSoulBound() ? 1 : 0;
	char buffer[128];
	sprintf(buffer, "%03u", SellRewId);
	oss << buffer;
	sprintf(buffer, "%04u", ReqId);
	oss << buffer;

	sprintf(buffer, "%01u", CanCompound(item) ? 1 : 0);
	oss << buffer;

	//sprintf(buffer, "%02u", insert);
	//oss << buffer;

	//sLog->outString("SellRewId:%u", SellRewId);
	//sLog->outString("ReqId:%u", ReqId);
	//sLog->outString("insert:%u", insert);
	uint32 flag = atoi(oss.str().c_str());

	//sLog->outString("flag:%u", flag);

	//ChatHandler(item->GetOwner()->GetSession()).PSendSysMessage("PROPERTIES_ID %u", flag);

	return flag + 2000;
}

void NoPatchItem::SetItemFlag(Item* item)
{
	if (!item->IsNoPatch())
		return;

	item->SetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID, GetItemFlag(item));

	if (Player* owner = item->GetOwner())
		sGCAddon->SendItemEnchantData(item->GetOwner(), item);

	//ChatHandler(item->GetOwner()->GetSession()).PSendSysMessage("ITEM_FIELD_RANDOM_PROPERTIES_ID %u", item->GetUInt32Value(ITEM_FIELD_RANDOM_PROPERTIES_ID));
}

bool NoPatchItem::ItemQuery(Player* player, uint32 item)
{
	if (uint32 guid = GetGUID(item))
	{
		auto itr = ItemQueryMap.find(guid);

		if (itr != ItemQueryMap.end())
		{
			Item * pItem = itr->second;

			if (!pItem || !pItem->IsNoPatch())
				return false;

			if (const ItemTemplate* srcTemp = pItem->GetTemplate())
			{
				ItemTemplate sendTemp;

				sendTemp = *srcTemp;
				sendTemp.ItemId = GetQueryId(pItem);
				sendTemp.Name1 = pItem->Name;
				sendTemp.ItemLevel = GetItemLevel(pItem->LevelData);
				sendTemp.Quality = GetQuality(pItem->LevelData);
				sendTemp.StatsCount = MAX_ITEM_PROTO_STATS;
				for (size_t i = 0; i < MAX_ITEM_PROTO_SPELLS; i++)
					sendTemp.Spells[i] = pItem->Spells[i];
				for (size_t i = 0; i < MAX_ITEM_PROTO_STATS; i++)
					sendTemp.ItemStat[i] = pItem->Stats[i];
				for (size_t i = 0; i < MAX_ITEM_PROTO_DAMAGES; i++)
					sendTemp.Damage[i] = pItem->Damages[i];
				sendTemp.Delay = pItem->NpDelay;
				sendTemp.InitializeQueryData();

				player->GetSession()->SendPacket(&sendTemp.queryData);
				return true;
			}
		}
	}

	return false;
}

bool NoPatchItem::CanOpenItem(Player* player)
{
	if (!player)
		return true;

	Map* map = player->GetMap();

	if (!map)
		return true;

	for (auto itr = MapNpVector.begin(); itr != MapNpVector.end(); itr++)
		if (map->GetId() == itr->MapId && map->GetDifficulty() == itr->Diff && map->challengeLv == itr->ChallengeLv)
			return false;

	return true;
}

uint32 NoPatchItem::GetLevelIndex(Item* item)
{
	if (item->MapData > 0)
	{
		auto itr = MapNpTempMap.find(item->TempIndex);
		if (itr != MapNpTempMap.end())
			return itr->second.LevelIndex;
	}

	if (item->LevelData > 0 && item->MapData == 0)
	{
		uint32 LevelEntry = GetLevelEntry(item->LevelData);
		auto iter = NpMap.find(LevelEntry);
		if (iter != NpMap.end())
			return iter->second.LevelIndex;
	}

	return 0;
}

void NoPatchItem::UpdateAuras(Player* pl)
{
	for (auto itr = NpAuraVector.begin(); itr != NpAuraVector.end(); itr++)
		for (size_t i = 0; i < 5; i++)
			pl->RemoveAura(itr->Auras[i]);

	std::vector<NpAuraData>v;

	for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; slot++)
	{
		if (Item* item = pl->GetItemByPos(INVENTORY_SLOT_BAG_0, slot))
		{
			if (!item->IsNoPatch())
				continue;

			NpAuraData data;
			data.LevelIndex = GetLevelIndex(item);
			data.Level = GetLevelNow(item->LevelData) - 1;
			v.push_back(data);
		}
	}

	for (auto itr = NpAuraVector.begin(); itr != NpAuraVector.end(); itr++)
	{
		uint32 count = 0;
		uint32 LevelIndex = itr->LevelIndex;
		uint32 Level = itr->Level;

		for (auto i = v.begin(); i != v.end(); i++)
			if (LevelIndex == i->LevelIndex && Level == i->Level)
				count++;

		if (count >= itr->Count)
		{
			for (size_t i = 0; i < 5; i++)
				if (itr->Auras[i])
					pl->AddAura(itr->Auras[i], pl);

			sRew->Rew(pl, itr->RewId);
		}		
	}
}

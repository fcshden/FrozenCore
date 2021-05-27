#pragma execution_character_set("utf-8")
#include "DBCCreate.h"
#include <fstream>
#include <Windows.h>
#include "shellapi.h"
#include "../Switch/Switch.h"
#include "../ItemMod/NoPatchItem.h"
#include "../CommonFunc/CommonFunc.h"

void DBCCreate::RepStr(std::string &s1, const std::string &s2, const std::string &s3)
{
	std::string::size_type pos = 0;
	std::string::size_type a = s2.size();
	std::string::size_type b = s3.size();
	while ((pos = s1.find(s2, pos)) != std::string::npos)
	{
		s1.replace(pos, a, s3);
		pos += b;
	}
}

void DBCCreate::RepStr(std::string &s)
{
	RepStr(s, "\r", "\\r");
	RepStr(s, "\n", "\\n");
	RepStr(s, "\"", "\"\"");
}

char DBCCreate::GetFieldType(std::string fmt, uint32 index)
{
	switch (fmt.at(index))
	{
	case 's':
		return 's';
	case 'f':
		return 'f';
	case 'i':
	case 'u':
	case 'n':
	case 'x':
	default:
		return 'i';
	}
}

void DBCCreate::OutDBC(std::string file, std::vector<std::vector<std::string>> dbcData, std::string fmt, uint8 type)
{
	sLog->outString(">>%s - 正在写入数据...", file.c_str());

	std::unordered_map<std::string, uint32> stringMap;

	std::vector<char> stringBytes;
	stringBytes.push_back('\0');

	uint32 wdbc = 0x43424457;
	uint32 recordCount = dbcData.size();
	uint32 fieldCount = dbcData.at(0).size();
	uint32 recordSize = fieldCount * 4;

	ofstream f(file.c_str(), ios::binary);

	f.write((char*)&wdbc, sizeof(uint32));
	f.write((char*)&recordCount, sizeof(uint32));
	f.write((char*)&fieldCount, sizeof(uint32));
	f.write((char*)&recordSize, sizeof(uint32));


	for (uint32 i = 0; i < recordCount; i++)
	{
		std::vector<std::string> dataList = dbcData.at(i);

		for (uint32 j = 0; j < fieldCount; j++)
		{
			switch (GetFieldType(fmt, j))
			{
			case 's':
			{
				if (dataList[j].empty())
					continue;

				auto itr = stringMap.find(dataList[j]);

				if (itr == stringMap.end())
				{
					stringMap.insert(std::make_pair(dataList[j], stringBytes.size()));

					for (size_t k = 0; k < dataList[j].size(); k++)
						stringBytes.push_back(dataList[j][k]);

					stringBytes.push_back('\0');
				}
			}
			break;
			default:
				break;
			}
		}
	}


	uint32 size = stringBytes.size();
	f.write((char*)&size, sizeof(uint32));

	for (uint32 i = 0; i < recordCount; i++)
	{
		std::vector<std::string> dataList = dbcData.at(i);

		for (uint32 j = 0; j < fieldCount; j++)
		{
			switch (GetFieldType(fmt, j))
			{
			case 'i':
			{
				__int64 value = atoll(dataList[j].c_str());
				f.write((char*)&value, sizeof(uint32));
			}
			break;
			case 'f':
			{
				float value = atof(dataList[j].c_str());
				f.write((char*)&value, sizeof(uint32));
			}
			break;
			case 's':
			{
				uint32 value = 0;

				if (!dataList[j].empty())
				{
					auto itr = stringMap.find(dataList[j]);
					value = itr->second;
				}
				f.write((char*)&value, sizeof(uint32));
			}
			break;
			default:
			{
				uint32 value = atoi(dataList[j].c_str());
				f.write((char*)&value, sizeof(uint32));
			}
			break;
			}
		}

		uint32 x = (i + 1) * 100 / recordCount;
		uint32 y = (i + 2) * 100 / recordCount;

		if (x != 0 && (x != y || x == 100))
			sLog->outString(">>%s - write data %d%% ", file.c_str(), x);
	}

	for (uint32 i = 0; i < stringBytes.size(); i++)
		f.write((char*)&stringBytes[0 + i], sizeof(char));

	f.close();

	std::string serverPath = sSwitch->GetFlagByIndex(ST_DBC_PATH, 1) + file;
	std::string clientPath = sSwitch->GetFlagByIndex(ST_DBC_PATH, 2) + file;

	switch (type)
	{
	case 0:
		CopyFile(file.c_str(), serverPath.c_str(), FALSE);
		CopyFile(file.c_str(), clientPath.c_str(), FALSE);
		sLog->outString(">>%s - 服务端路径[%s]", file.c_str(), serverPath.c_str());
		sLog->outString(">>%s - 客户端路径[%s]", file.c_str(), clientPath.c_str());
		break;
	case 1:
		CopyFile(file.c_str(), serverPath.c_str(), FALSE);
		sLog->outString(">>%s - 服务端路径[%s]", file.c_str(), serverPath.c_str());
		break;
	case 2:
		CopyFile(file.c_str(), clientPath.c_str(), FALSE);
		sLog->outString(">>%s - 客户端路径[%s]", file.c_str(), clientPath.c_str());
		break;
	default:
		break;
	}

	remove(file.c_str());

	//Encrypt(file, recordCount, fieldCount);
}

void DBCCreate::Encrypt(std::string file, uint32 recordCount, uint32 fieldCount)
{
	std::string serverPath = sSwitch->GetFlagByIndex(ST_DBC_PATH, 1) + file;
	std::string clientPath = sSwitch->GetFlagByIndex(ST_DBC_PATH, 2) + file;

	if (strcmp(serverPath.c_str(), clientPath.c_str()) == 0)
	{
		sLog->outString(">>%s - 服务端与客户端路径相同，将无法使用客户端DBC加密功能...", file.c_str());
		return;
	}

	bool encrypt = true;

	std::vector<uint32> fields;

	if (strcmp(file.c_str(), "Spell.dbc") == 0)
	{
		//mydbceditor 列号
		fields.push_back(72);
		fields.push_back(73);
		fields.push_back(74);
		//fields.push_back(96);
		//fields.push_back(97);
		//fields.push_back(98);
	}
	else
		encrypt = false;

	if (!encrypt || fields.empty())
		return;

	FILE *fp;
	fp = fopen(clientPath.c_str(), "rb+");

	if (NULL != fp)
	{
		for (uint32 i = 0; i < recordCount; i++)
		{
			for (uint32 j = 0; j < fieldCount; j++)
			{
				uint32 offset = 5 * sizeof(uint32) + (i * 234 + j) * sizeof(uint32);

				if (std::find(fields.begin(), fields.end(), j + 1) != fields.end())
				{
					uint32 zero = 0;

					if (fseek(fp, offset, SEEK_SET) == 0)
						fwrite(&zero, sizeof(uint32), 1, fp);
				}
			}

			uint32 x = (i + 1) * 100 / recordCount;
			uint32 y = (i + 2) * 100 / recordCount;

			if (x != 0 && (x != y || x == 100))
				sLog->outString(">>%s - encrypt client dbc %d%% ", file.c_str(), x);
		}

		fclose(fp);

		sLog->outString(">>%s - 客户端部分加密完成...", file.c_str());
	}
	else
	{
		sLog->outString(">>%s - 无法打开[%s]...", file.c_str(), clientPath.c_str());
		sLog->outString(">>%s - 客户端部分加密失败...", file.c_str());
	}
}

void DBCCreate::GenerateTalentSql()
{
	//WorldDatabase.DirectPExecute("TRUNCATE TABLE _dbc_talent;");
	//
	//for (uint32 id = 0; id < sTalentStore.GetNumRows(); id++)
	//{
	//
	//	TalentEntry const* info = sTalentStore.LookupEntry(id);
	//
	//	if (!info)
	//		continue;
	//
	//	WorldDatabase.DirectPExecute("INSERT INTO _dbc_talent("
	//		"TalentID, TalentTab,"
	//		"Row, Col, RankID1, RankID2, RankID3, RankID4, RankID5, "
	//		"DependsOn, DependsOnRank, addToSpellBook ,petMask, petMask1)"
	//		"VALUES ('%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');",
	//		info->TalentID, info->TalentTab, info->Row,info->Col,
	//		info->RankID[0], info->RankID[1], info->RankID[2], info->RankID[3], info->RankID[4],
	//		info->DependsOn, info->DependsOnRank, info->addToSpellBook, info->petMask, info->petMask1);
	//}
	//
	//sLog->outString("_dbc_talent更新完成...");
}

void DBCCreate::GenerateSpellItemEnchantmentSql()
{
	WorldDatabase.DirectPExecute("TRUNCATE TABLE _dbc_spellitemenchantment;");

	for (uint32 id = 0; id < sSpellItemEnchantmentStore.GetNumRows(); id++)
	{

		SpellItemEnchantmentEntry const* info = sSpellItemEnchantmentStore.LookupEntry(id);

		if (!info)
			continue;

		WorldDatabase.DirectPExecute("INSERT INTO _dbc_spellitemenchantment("
			"ID, charges,"
			"type1, type2, type3, amount1, amount2, amount3, spellid1, spellid2, spellid3, "
			"description, aura_id, slot ,GemID, EnchantmentCondition, requiredSkill, requiredSkillValue, requiredLevel)"
			"VALUES ('%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%s','%d','%d','%d','%d','%d','%d','%d');",
			info->ID, info->charges,
			info->type[0], info->type[1], info->type[2], info->amount[0], info->amount[1], info->amount[2], info->spellid[0], info->spellid[1], info->spellid[2],
			info->description[4], info->aura_id, info->slot, info->GemID, info->EnchantmentCondition, info->requiredSkill, info->requiredSkillValue, info->requiredLevel);
	}

	sLog->outString("_dbc_spellitemenchantment更新完成...");
}

void DBCCreate::GenerateCharStartOutfitSql()
{
	WorldDatabase.DirectPExecute("TRUNCATE TABLE _dbc_charstartoutfit;");

	for (uint32 id = 0; id < sCharStartOutfitStore.GetNumRows(); id++)
	{

		CharStartOutfitEntry const* info = sCharStartOutfitStore.LookupEntry(id);

		if (!info)
			continue;

		WorldDatabase.DirectPExecute("INSERT INTO _dbc_charstartoutfit("
			"Race, Class, Gender,"
			"Item1, Item2, Item3, Item4, Item5, Item6, Item7, Item8, Item9,Item10, Item11, Item12, Item13, Item14, Item15, Item16, Item17, Item18, Item19,Item20, Item21, Item22, Item23, Item24)"
			"VALUES ('%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d');",
			info->Race, info->Class, info->Gender, 
			info->ItemId[0], info->ItemId[1], info->ItemId[2], info->ItemId[3], info->ItemId[4], info->ItemId[5], info->ItemId[6], info->ItemId[7], info->ItemId[8], info->ItemId[9],
			info->ItemId[10], info->ItemId[11], info->ItemId[12], info->ItemId[13], info->ItemId[14], info->ItemId[15], info->ItemId[16], info->ItemId[17], info->ItemId[18], info->ItemId[19],
			info->ItemId[20], info->ItemId[21], info->ItemId[22], info->ItemId[23]);
	}

	sLog->outString("_dbc_charstartoutfit更新完成...");
}

void DBCCreate::GenerateItemDisplayInfoSql()
{
	WorldDatabase.DirectPExecute("TRUNCATE TABLE _dbc_itemdisplayinfo;");

	for (uint32 id = 0; id < sItemDisplayInfoStore.GetNumRows(); id++)
	{

		ItemDisplayInfoEntry const* info = sItemDisplayInfoStore.LookupEntry(id);

		if (!info)
			continue;

		WorldDatabase.DirectPExecute("INSERT INTO _dbc_itemdisplayinfo(ID, LeftModel, RightModel, LeftModelTexture, RightModelTexture, Icon, Field7, Field8, Field9, Field10, Field11, Field12, groupSoundIndex, Field14, Field15, Field16, Field17, Field18, Field19, Field20, Field21, Field22, Field23, Field24, Field25) "
			"VALUES ('%u','%s','%s','%s','%s','%s','%s','%u','%u','%u','%u','%u','%u','%u','%u','%s','%s','%s','%s','%s','%s','%s','%s','%u','%u');",
			info->ID, info->Field2, info->Field3, info->Field4, info->Field5, info->inventoryIcon, info->Field7, info->Field8, info->Field9, info->Field10,
			info->Field11, info->Field12, info->Field13, info->Field14, info->Field15, info->Field16, info->Field17, info->Field18, info->Field19, info->Field20,
			info->Field21, info->Field22, info->Field23, info->Field24, info->Field25);
	}

	sLog->outString("_dbc_itemdisplayinfo更新完成...");
}

void DBCCreate::GenerateSpellIconSql()
{
	WorldDatabase.DirectPExecute("TRUNCATE TABLE _dbc_spellicon;");

	for (uint32 id = 0; id < sSpellIconStore.GetNumRows(); id++)
	{

		SpellIconEntry const* info = sSpellIconStore.LookupEntry(id);

		if (!info)
			continue;

		PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_SPELL_ICON_DBC);
		stmt->setUInt32(0, info->ID);
		stmt->setString(1, info->Icon);
		WorldDatabase.Execute(stmt);
	}

	sLog->outString("_dbc_spellicon更新完成...");
}

void DBCCreate::GenerateItemExtendCostSql()
{
	WorldDatabase.DirectPExecute("TRUNCATE TABLE _dbc_itemextendedcost;");

	for (uint32 id = 0; id < sItemExtendedCostStore.GetNumRows(); id++)
	{

		ItemExtendedCostEntry const* info = sItemExtendedCostStore.LookupEntry(id);

		if (!info)
			continue;

		WorldDatabase.DirectPExecute("INSERT INTO _dbc_itemextendedcost(ID,honorpoints,arenapoints,arenaslot,item1,item2,item3,item4,item5,reqitemcount1,reqitemcount2,reqitemcount3,reqitemcount4,reqitemcount5,arenarating,uk)"
			"VALUES ('%u','%u','%u','%u','%u','%u','%u','%u','%u','%u','%u','%u','%u','%u','%u','%u');",
			info->ID, info->reqhonorpoints, info->reqarenapoints, info->reqarenaslot, info->reqitem[0], info->reqitem[1], info->reqitem[2], info->reqitem[3], info->reqitem[4],
			info->reqitemcount[0], info->reqitemcount[1], info->reqitemcount[2], info->reqitemcount[3], info->reqitemcount[4], info->reqpersonalarenarating, 0);
	}

	sLog->outString("_dbc_itemextendedcost更新完成...");
}

void DBCCreate::GenerateSpellSql()
{
	WorldDatabase.DirectPExecute("TRUNCATE TABLE _dbc_spell;");

	for (uint32 id = 0; id < _sSpellStore.GetNumRows(); id++)
	{

		_SpellEntry const* info = _sSpellStore.LookupEntry(id);

		if (!info)
			continue;

		PreparedStatement* stmt = WorldDatabase.GetPreparedStatement(WORLD_INS_SPELL_DBC);
		uint8 i = 0;
		stmt->setUInt32(i++, info->Id);
		stmt->setUInt32(i++, info->Category);
		stmt->setUInt32(i++, info->Dispel);
		stmt->setUInt32(i++, info->Mechanic);
		stmt->setUInt64(i++, info->Attributes);
		stmt->setUInt64(i++, info->AttributesEx);
		stmt->setUInt64(i++, info->AttributesEx2);
		stmt->setUInt64(i++, info->AttributesEx3);
		stmt->setUInt64(i++, info->AttributesEx4);
		stmt->setUInt64(i++, info->AttributesEx5);
		stmt->setUInt64(i++, info->AttributesEx6);
		stmt->setUInt64(i++, info->AttributesEx7);
		stmt->setUInt32(i++, info->Stances);
		stmt->setUInt32(i++, info->unk_320_2);
		stmt->setUInt32(i++, info->StancesNot);
		stmt->setUInt32(i++, info->unk_320_3);
		stmt->setUInt32(i++, info->Targets);
		stmt->setUInt32(i++, info->TargetCreatureType);
		stmt->setUInt32(i++, info->RequiresSpellFocus);
		stmt->setUInt32(i++, info->FacingCasterFlags);
		stmt->setUInt32(i++, info->CasterAuraState);
		stmt->setUInt32(i++, info->TargetAuraState);
		stmt->setUInt32(i++, info->CasterAuraStateNot);
		stmt->setUInt32(i++, info->TargetAuraStateNot);
		stmt->setUInt32(i++, info->casterAuraSpell);
		stmt->setUInt32(i++, info->targetAuraSpell);
		stmt->setUInt32(i++, info->excludeCasterAuraSpell);
		stmt->setUInt32(i++, info->excludeTargetAuraSpell);
		stmt->setUInt32(i++, info->CastingTimeIndex);
		stmt->setUInt32(i++, info->RecoveryTime);
		stmt->setUInt32(i++, info->CategoryRecoveryTime);
		stmt->setUInt32(i++, info->InterruptFlags);
		stmt->setUInt32(i++, info->AuraInterruptFlags);
		stmt->setUInt32(i++, info->ChannelInterruptFlags);
		stmt->setUInt32(i++, info->procFlags);
		stmt->setUInt32(i++, info->procChance);
		stmt->setUInt32(i++, info->procCharges);
		stmt->setUInt32(i++, info->maxLevel);
		stmt->setUInt32(i++, info->baseLevel);
		stmt->setUInt32(i++, info->spellLevel);
		stmt->setUInt32(i++, info->DurationIndex);
		if (info->powerType > 127)
			stmt->setInt32(i++, -2);
		else
			stmt->setInt32(i++, info->powerType);
		stmt->setUInt32(i++, info->manaCost);
		stmt->setUInt32(i++, info->manaCostPerlevel);
		stmt->setUInt32(i++, info->manaPerSecond);
		stmt->setUInt32(i++, info->manaPerSecondPerLevel);
		stmt->setUInt32(i++, info->rangeIndex);
		stmt->setFloat(i++, info->speed);
		stmt->setUInt32(i++, info->modalNextSpell);
		stmt->setUInt32(i++, info->StackAmount);
		stmt->setUInt32(i++, info->Totem[0]);
		stmt->setUInt32(i++, info->Totem[1]);
		stmt->setInt32(i++, info->Reagent[0]);
		stmt->setInt32(i++, info->Reagent[1]);
		stmt->setInt32(i++, info->Reagent[2]);
		stmt->setInt32(i++, info->Reagent[3]);
		stmt->setInt32(i++, info->Reagent[4]);
		stmt->setInt32(i++, info->Reagent[5]);
		stmt->setInt32(i++, info->Reagent[6]);
		stmt->setInt32(i++, info->Reagent[7]);
		stmt->setInt32(i++, info->ReagentCount[0]);
		stmt->setInt32(i++, info->ReagentCount[1]);
		stmt->setInt32(i++, info->ReagentCount[2]);
		stmt->setInt32(i++, info->ReagentCount[3]);
		stmt->setInt32(i++, info->ReagentCount[4]);
		stmt->setInt32(i++, info->ReagentCount[5]);
		stmt->setInt32(i++, info->ReagentCount[6]);
		stmt->setInt32(i++, info->ReagentCount[7]);
		stmt->setInt32(i++, info->EquippedItemClass);
		stmt->setInt32(i++, info->EquippedItemSubClassMask);
		stmt->setInt32(i++, info->EquippedItemInventoryTypeMask);
		stmt->setUInt32(i++, info->Effect[0]);
		stmt->setUInt32(i++, info->Effect[1]);
		stmt->setUInt32(i++, info->Effect[2]);
		stmt->setInt32(i++, info->EffectDieSides[0]);
		stmt->setInt32(i++, info->EffectDieSides[1]);
		stmt->setInt32(i++, info->EffectDieSides[2]);
		stmt->setFloat(i++, info->EffectRealPointsPerLevel[0]);
		stmt->setFloat(i++, info->EffectRealPointsPerLevel[1]);
		stmt->setFloat(i++, info->EffectRealPointsPerLevel[2]);
		stmt->setInt32(i++, info->EffectBasePoints[0]);
		stmt->setInt32(i++, info->EffectBasePoints[1]);
		stmt->setInt32(i++, info->EffectBasePoints[2]);
		stmt->setUInt32(i++, info->EffectMechanic[0]);
		stmt->setUInt32(i++, info->EffectMechanic[1]);
		stmt->setUInt32(i++, info->EffectMechanic[2]);
		stmt->setUInt32(i++, info->EffectImplicitTargetA[0]);
		stmt->setUInt32(i++, info->EffectImplicitTargetA[1]);
		stmt->setUInt32(i++, info->EffectImplicitTargetA[2]);
		stmt->setUInt32(i++, info->EffectImplicitTargetB[0]);
		stmt->setUInt32(i++, info->EffectImplicitTargetB[1]);
		stmt->setUInt32(i++, info->EffectImplicitTargetB[2]);
		stmt->setUInt32(i++, info->EffectRadiusIndex[0]);
		stmt->setUInt32(i++, info->EffectRadiusIndex[1]);
		stmt->setUInt32(i++, info->EffectRadiusIndex[2]);
		stmt->setUInt32(i++, info->EffectApplyAuraName[0]);
		stmt->setUInt32(i++, info->EffectApplyAuraName[1]);
		stmt->setUInt32(i++, info->EffectApplyAuraName[2]);
		stmt->setUInt32(i++, info->EffectAmplitude[0]);
		stmt->setUInt32(i++, info->EffectAmplitude[1]);
		stmt->setUInt32(i++, info->EffectAmplitude[2]);
		stmt->setFloat(i++, info->EffectValueMultiplier[0]);
		stmt->setFloat(i++, info->EffectValueMultiplier[1]);
		stmt->setFloat(i++, info->EffectValueMultiplier[2]);
		stmt->setUInt32(i++, info->EffectChainTarget[0]);
		stmt->setUInt32(i++, info->EffectChainTarget[1]);
		stmt->setUInt32(i++, info->EffectChainTarget[2]);
		stmt->setUInt32(i++, info->EffectItemType[0]);
		stmt->setUInt32(i++, info->EffectItemType[1]);
		stmt->setUInt32(i++, info->EffectItemType[2]);
		stmt->setInt32(i++, info->EffectMiscValue[0]);
		stmt->setInt32(i++, info->EffectMiscValue[1]);
		stmt->setInt32(i++, info->EffectMiscValue[2]);
		stmt->setInt32(i++, info->EffectMiscValueB[0]);
		stmt->setInt32(i++, info->EffectMiscValueB[1]);
		stmt->setInt32(i++, info->EffectMiscValueB[2]);
		stmt->setInt32(i++, info->EffectTriggerSpell[0]);
		stmt->setInt32(i++, info->EffectTriggerSpell[1]);
		stmt->setInt32(i++, info->EffectTriggerSpell[2]);
		stmt->setFloat(i++, info->EffectPointsPerComboPoint[0]);
		stmt->setFloat(i++, info->EffectPointsPerComboPoint[1]);
		stmt->setFloat(i++, info->EffectPointsPerComboPoint[2]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[0][0]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[0][1]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[0][2]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[1][0]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[1][1]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[1][2]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[2][0]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[2][1]);
		stmt->setUInt64(i++, info->EffectSpellClassMask[2][2]);
		stmt->setUInt32(i++, info->SpellVisual[0]);
		stmt->setUInt32(i++, info->SpellVisual[1]);
		stmt->setUInt32(i++, info->SpellIconID);
		stmt->setUInt32(i++, info->activeIconID);
		stmt->setUInt32(i++, info->spellPriority);
		stmt->setString(i++, info->SpellName[4]);
		stmt->setUInt32(i++, info->SpellNameFlag);
		stmt->setString(i++, info->Rank[4]);
		stmt->setUInt32(i++, info->RankFlags);
		stmt->setString(i++, info->Description[4]);
		stmt->setUInt32(i++, info->DescriptionFlags);
		stmt->setString(i++, info->ToolTip[4]);
		stmt->setUInt32(i++, info->ToolTipFlags);
		stmt->setUInt32(i++, info->ManaCostPercentage);
		stmt->setUInt32(i++, info->StartRecoveryCategory);
		stmt->setUInt32(i++, info->StartRecoveryTime);
		stmt->setUInt32(i++, info->MaxTargetLevel);
		stmt->setUInt32(i++, info->SpellFamilyName);
		stmt->setUInt64(i++, info->SpellFamilyFlags[0]);
		stmt->setUInt64(i++, info->SpellFamilyFlags[1]);
		stmt->setUInt64(i++, info->SpellFamilyFlags[2]);
		stmt->setUInt32(i++, info->MaxAffectedTargets);
		stmt->setUInt32(i++, info->DmgClass);
		stmt->setUInt32(i++, info->PreventionType);
		stmt->setInt32(i++, info->StanceBarOrder);
		stmt->setFloat(i++, info->EffectDamageMultiplier[0]);
		stmt->setFloat(i++, info->EffectDamageMultiplier[1]);
		stmt->setFloat(i++, info->EffectDamageMultiplier[2]);
		stmt->setUInt32(i++, info->MinFactionId);
		stmt->setUInt32(i++, info->MinReputation);
		stmt->setUInt32(i++, info->RequiredAuraVision);
		stmt->setUInt32(i++, info->TotemCategory[0]);
		stmt->setUInt32(i++, info->TotemCategory[1]);
		stmt->setInt32(i++, info->AreaGroupId);
		stmt->setUInt32(i++, info->SchoolMask);
		stmt->setUInt32(i++, info->runeCostID);
		stmt->setUInt32(i++, info->spellMissileID);
		stmt->setInt32(i++, info->PowerDisplayId);
		stmt->setFloat(i++, info->EffectBonusMultiplier[0]);
		stmt->setFloat(i++, info->EffectBonusMultiplier[1]);
		stmt->setFloat(i++, info->EffectBonusMultiplier[2]);
		stmt->setInt32(i++, info->spellDescriptionVariableID);
		stmt->setUInt32(i++, info->SpellDifficultyId);
		WorldDatabase.Execute(stmt);
	}

	sLog->outString("_dbc_spell更新完成...");
}

void DBCCreate::GenerateItemDBC()
{
	std::vector<std::vector<std::string>> dbcData;
	std::string file = "Item.dbc";
	std::string fmt = "niiiiiii";
	uint32 index = 0;
	std::vector<uint32> EntryVec;

	if (QueryResult result = WorldDatabase.PQuery("SELECT entry,class,subclass,SoundOverrideSubclass,Material,displayid,InventoryType,sheath FROM item_template ORDER BY entry ASC"))
	{
		do
		{
			std::vector<std::string> v;
			Field* fields = result->Fetch();

			EntryVec.push_back(fields[0].GetUInt32());

			for (uint8 i = 0; i < 8; i++)
				v.push_back(fields[i].GetString());

			dbcData.push_back(v);

			index++;

			if (index % 100 == 0)
				sLog->outString(">>%s - packet data %d...", file.c_str(), index);

		} while (result->NextRow());
	}

	for (uint32 id = 0; id < sItemStore.GetNumRows(); id++)
	{
		ItemEntry const* info = sItemStore.LookupEntry(id);

		if (!info || info->ID > 56806)
			continue;

		if (std::find(EntryVec.begin(), EntryVec.end(), info->ID) != EntryVec.end())
			continue;

		std::vector<std::string> v;
		v.push_back(std::to_string(info->ID));
		v.push_back(std::to_string(info->Class));
		v.push_back(std::to_string(info->SubClass));
		v.push_back(std::to_string(info->SoundOverrideSubclass));
		v.push_back(std::to_string(info->Material));
		v.push_back(std::to_string(info->DisplayId));
		v.push_back(std::to_string(info->InventoryType));
		v.push_back(std::to_string(info->Sheath));
		dbcData.push_back(v);

		index++;

		if (index % 100 == 0)
			sLog->outString(">>%s - packet data %d...", file.c_str(), index);
	}

	OutDBC(file, dbcData, fmt);
}

void DBCCreate::GenerateItemDisplayInfoDBC()
{
	std::vector<std::vector<std::string>> dbcData;
	std::string file = "ItemDisplayInfo.dbc";
	std::string fmt = "nssssssiiiiiiiissssssssii";
	uint32 index = 0;
	//1		//2			//3			//4				//5				//6			//7	//8		//9			//10	//11	//12		//13			//14	//15	//16	//17	//18		//19	//20	//21	//22		//23	//24	//25
	if (QueryResult result = WorldDatabase.PQuery("SELECT ID, LeftModel, RightModel, LeftModelTexture, RightModelTexture, Icon, Field7, Field8, Field9, Field10, Field11, Field12, groupSoundIndex, Field14, Field15, Field16, Field17, Field18, Field19, Field20, Field21, Field22, Field23, Field24, Field25 FROM _dbc_itemdisplayinfo  ORDER BY ID ASC"))
	{
		do
		{
			std::vector<std::string> v;
			Field* fields = result->Fetch();
			for (uint8 i = 0; i < 25; i++)
				v.push_back(fields[i].GetString());
		
			dbcData.push_back(v);

			index++;
			sLog->outString(">>%s - packet data %d...", file.c_str(), index);

		} while (result->NextRow());
	}

	OutDBC(file, dbcData, fmt);
}

void DBCCreate::GenerateItemExtendCostDBC()
{
	std::vector<std::vector<std::string>> dbcData;
	std::string file = "ItemExtendedCost.dbc";
	std::string fmt = "niiiiiiiiiiiiiix";
	uint32 index = 0;									//1		//2			//3			//4		//5		//6	//7		//8	//9		//10		//11			//12		//13			//14		/15			//16
	if (QueryResult result = WorldDatabase.PQuery("SELECT ID,honorpoints,arenapoints,arenaslot,item1,item2,item3,item4,item5,reqitemcount1,reqitemcount2,reqitemcount3,reqitemcount4,reqitemcount5,arenarating,uk FROM _dbc_itemextendedcost ORDER BY ID ASC"))
	{
		do
		{
			std::vector<std::string> v;
			Field* fields = result->Fetch();

			for (size_t i = 0; i < 16; i++)
				v.push_back(fields[i].GetString());

			dbcData.push_back(v);

			index++;

			if (index % 200 == 0)
				sLog->outString(">>%s - packet data %d...", file.c_str(), index);

		} while (result->NextRow());
	}

	OutDBC(file, dbcData, fmt);
}

void DBCCreate::GenerateSpellDBC()
{
	std::vector<std::vector<std::string>> dbcData;
	std::string file = "Spell.dbc";
	std::string fmt = "niiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiifiiiiiiiiiiiiiiiiiiiiiiiiiiiiifffiiiiiiiiiiiiiiiiiiiiifffiiiiiiiiiiiiiiifffiiiiiiiiiiiiiissssssssssssssssissssssssssssssssissssssssssssssssissssssssssssssssiiiiiiiiiiiiifffiiiiiiiiiifffii";
	uint32 index = 0;
	if (QueryResult result = WorldDatabase.PQuery(
				//1	//2			//3		//4		//5			//6				//7				//8			//9				//10			//11			//12
		"SELECT Id, Category, Dispel, Mechanic, Attributes, AttributesEx, AttributesEx2, AttributesEx3, AttributesEx4, AttributesEx5, AttributesEx6, AttributesEx7, "
		//13		//14		//15		//16		//17		//18			//19				//20				//21
		"Stances, unk_320_2, StancesNot, unk_320_3, Targets, TargetCreatureType, RequiresSpellFocus, FacingCasterFlags, CasterAuraState, "
		//22				//23				//24				//25			//26				//27					//28
		"TargetAuraState, CasterAuraStateNot, TargetAuraStateNot, casterAuraSpell, targetAuraSpell, excludeCasterAuraSpell, excludeTargetAuraSpell, "
		//29				//30			//31					//32			//33				//34				//35
		"CastingTimeIndex, RecoveryTime, CategoryRecoveryTime, InterruptFlags, AuraInterruptFlags, ChannelInterruptFlags, ProcFlags, "
		//36			//37		//38		//39		//40		//41		//42		//43		//44			//45			//46
		"ProcChance, ProcCharges, MaxLevel, BaseLevel, SpellLevel, DurationIndex, powerType, manaCost, manaCostPerlevel, manaPerSecond, manaPerSecondPerLevel, "
		//47			//48		//49		//50		//51	//52	//53	//54		//55	//56		//57		//58	//59	//60
		"rangeIndex, speed, modalNextSpell, StackAmount, Totem1, Totem2, Reagent1, Reagent2, Reagent3, Reagent4, Reagent5, Reagent6, Reagent7, Reagent8, "
		//61				//62		//63			//64			//65			//66		//67			//68
		"ReagentCount1, ReagentCount2, ReagentCount3, ReagentCount4, ReagentCount5, ReagentCount6, ReagentCount7, ReagentCount8, "
		//69					//70					//71							//72	//73	//74		/75				//76			//77
		"EquippedItemClass, EquippedItemSubClassMask, EquippedItemInventoryTypeMask, Effect1, Effect2, Effect3, EffectDieSides1, EffectDieSides2, EffectDieSides3, "
		//78							//79					//80						//81				//82				//83
		"EffectRealPointsPerLevel1, EffectRealPointsPerLevel2, EffectRealPointsPerLevel3, EffectBasePoints1, EffectBasePoints2, EffectBasePoints3, "
		//84				//85			//86				//87					//88					//89
		"EffectMechanic1, EffectMechanic2, EffectMechanic3, EffectImplicitTargetA1, EffectImplicitTargetA2, EffectImplicitTargetA3, "
		//90						//91					//92					//93				//94				//95
		"EffectImplicitTargetB1, EffectImplicitTargetB2, EffectImplicitTargetB3, EffectRadiusIndex1, EffectRadiusIndex2, EffectRadiusIndex3, "
		//96						//97				//98				//99				//100			//101
		"EffectApplyAuraName1, EffectApplyAuraName2, EffectApplyAuraName3, EffectAmplitude1, EffectAmplitude2, EffectAmplitude3, "
		//102						//103					//104					//105				//106				//107
		"EffectValueMultiplier1, EffectValueMultiplier2, EffectValueMultiplier3, EffectChainTarget1, EffectChainTarget2, EffectChainTarget3, "
		//108				//109			//110				//111			//112				//113
		"EffectItemType1, EffectItemType2, EffectItemType3, EffectMiscValueA1, EffectMiscValueA2, EffectMiscValueA3, "
		//114					//115			//116				//117				//118					//119
		"EffectMiscValueB1, EffectMiscValueB2, EffectMiscValueB3, EffectTriggerSpell1, EffectTriggerSpell2, EffectTriggerSpell3, "
		//120							//121						//122
		"EffectPointsPerComboPoint1, EffectPointsPerComboPoint2, EffectPointsPerComboPoint3, "
		//123						//124					//125					//126					//127					//128					//129					//130					//131
		"EffectSpellClassMaskA1, EffectSpellClassMaskA2, EffectSpellClassMaskA3, EffectSpellClassMaskB1, EffectSpellClassMaskB2, EffectSpellClassMaskB3, EffectSpellClassMaskC1, EffectSpellClassMaskC2, EffectSpellClassMaskC3, "
		//132				//133		//134		//135			//136		//137		//138		//139	//140		//141		//142				//143		//144	
		"SpellVisual1, SpellVisual2, SpellIconID, activeIconID, spellPriority, SpellName, SpellNameFlag, Rank, RankFlags, Description, DescriptionFlags, ToolTip, ToolTipFlags, "
		//145					//146				//147				//148				//149			//150			//151				//152
		"ManaCostPercentage, StartRecoveryCategory, StartRecoveryTime, MaxTargetLevel, SpellFamilyName, SpellFamilyFlags1, SpellFamilyFlags2, SpellFamilyFlags3, "
		//153					//154		//155		//156			//157						//158					//159
		"MaxAffectedTargets, DmgClass, PreventionType, StanceBarOrder, EffectDamageMultiplier1, EffectDamageMultiplier2, EffectDamageMultiplier3, "
		//160			//161			//162				//163			//164			//165		//166		//167			//168		//169
		"MinFactionId, MinReputation, RequiredAuraVision, TotemCategory1, TotemCategory2, AreaGroupId, SchoolMask, runeCostID, spellMissileID, PowerDisplayId, "
		//170						//171					//172					//173						//174		
		"EffectBonusMultiplier1, EffectBonusMultiplier2, EffectBonusMultiplier3, spellDescriptionVariableID, SpellDifficultyId FROM _dbc_spell  ORDER BY Id ASC"))
	{
		do
		{
			std::vector<std::string> v;
			Field* fields = result->Fetch();

			for (size_t i = 0; i < 136; i++)
				v.push_back(fields[i].GetString());
			
			std::string SpellName			= fields[136].GetString();
			std::string SpellNameFlag		= fields[137].GetString();
			std::string Rank				= fields[138].GetString();
			std::string RankFlags			= fields[139].GetString();
			std::string Description			= fields[140].GetString();
			std::string DescriptionFlags	= fields[141].GetString();
			std::string ToolTip				= fields[142].GetString();
			std::string ToolTipFlags		= fields[143].GetString();

			//SpellName
			for (size_t i = 0; i < 16; i++)
			{
				if (i == 4)
					v.push_back(fields[136].GetString());
				else
					v.push_back("");
			}
			//SpellNameFlag
			v.push_back(fields[137].GetString());
			//Rank
			for (size_t i = 0; i < 16; i++)
			{
				if (i == 4)
					v.push_back(fields[138].GetString());
				else
					v.push_back("");
			}
			//RankFlags
			v.push_back(fields[139].GetString());
			//Description
			for (size_t i = 0; i < 16; i++)
			{
				if (i == 4)
					v.push_back(fields[140].GetString());
				else
					v.push_back("");
			}
			//DescriptionFlags
			v.push_back(fields[141].GetString());
			//ToolTip
			for (size_t i = 0; i < 16; i++)
			{
				if (i == 4)
					v.push_back(fields[142].GetString());
				else
					v.push_back("");
			}
			//ToolTipFlags
			v.push_back(fields[143].GetString());

			for (size_t i = 144; i < 174; i++)
				v.push_back(fields[i].GetString());

			dbcData.push_back(v);

			index++;

			if (index % 200 == 0)
				sLog->outString(">>%s - packet data %d...", file.c_str(), index);

		} while (result->NextRow());
	}


	//OutDBC(file, dbcData, fmt);

	std::string serverPath = sSwitch->GetFlagByIndex(ST_DBC_PATH, 1) + file;
	std::string clientPath = sSwitch->GetFlagByIndex(ST_DBC_PATH, 2) + file;

	if (strcmp(serverPath.c_str(), clientPath.c_str()) == 0)
	{
		OutDBC(file, dbcData, fmt);
		sLog->outString(">>%s - 服务端与客户端路径相同，将无法使用客户端DBC加密功能...", file.c_str());
		return;
	}

	OutDBC(file, dbcData, fmt, 1);

	for (size_t i = 0; i < dbcData.size(); i++)
	{
		if (atoi(dbcData[i][0].c_str()) <= 90000)
			continue;

		bool flag = false;

		for (size_t j = 0; j < dbcData[i].size(); j++)
		{
			uint32 col = j + 1;

			if (col == 117 || col == 118 || col == 119 || col == 230 || col == 231 || col == 232)
			{
				dbcData[i][j] = "0";
				flag = true;
			}
				
			if (col == 72 || col == 73 || col == 74)
			{
				if (strcmp(dbcData[i][j].c_str(), "6") == 0)// && strcmp(dbcData[i][13].c_str(), "1") != 0)
				{
					dbcData[i][j + 24] = "0";
					flag = true;
				}			
			}			
		}

		if (flag)
			sLog->outString(">>Spell.dbc - Encrypt data %s...", dbcData[i][0].c_str());

	}

	OutDBC(file, dbcData, fmt, 2);
}

void DBCCreate::GenerateCharStartOutfitDBC()
{
	sLog->outString("_dbc_charstartoutfit未完成开发");
	return;
	std::vector<std::vector<std::string>> dbcData;
	std::string file = "CharStartOutfit.dbc";
	std::string fmt = "dbbbXiiiiiiiiiiiiiiiiiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
	uint32 index = 0;
	if (QueryResult result = WorldDatabase.PQuery("SELECT "
		//1		//2		//3
		"Race, Class, Gender, "
		//4	- 28
		"Item1, Item2, Item3, Item4, Item5, Item6, Item7, Item8, Item9,Item10, Item11, Item12, Item13, Item14, Item15, Item16, Item17, Item18, Item19,Item20, Item21, Item22, Item23, Item24 "
		"FROM _dbc_charstartoutfit ORDER BY Race ASC"))
	{
		do
		{
			std::vector<std::string> v;
			Field* fields = result->Fetch();
			v.push_back(std::to_string(index + 1));	//ID
			v.push_back(fields[0].GetString());		//race
			v.push_back(fields[1].GetString());		//class
			v.push_back(fields[2].GetString());		//gender
			v.push_back("0");						//unused

			//itemid
			for (size_t i = 0; i < 24; i++)
				v.push_back(fields[3 + i].GetString());

			//displayid
			for (size_t i = 0; i < 24; i++)
			{
				uint32 entry = fields[3 + i].GetUInt32();
				const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);

				if (!temp)
					v.push_back("-1");
				else
					v.push_back(std::to_string(temp->DisplayInfoID));
			}

			//inventory
			for (size_t i = 0; i < 24; i++)
			{
				uint32 entry = fields[3 + i].GetUInt32();
				const ItemTemplate* temp = sObjectMgr->GetItemTemplate(entry);

				if (!temp)
					v.push_back("-1");
				else
					v.push_back(std::to_string(temp->InventoryType));
			}

			dbcData.push_back(v);

			index++;

			if (index % 200 == 0)
				sLog->outString(">>%s - packet data %d...", file.c_str(), index);


		} while (result->NextRow());
	}

	OutDBC(file, dbcData, fmt);
}

void DBCCreate::GenerateSpellItemEnchantmentDBC()
{
	std::vector<std::vector<std::string>> dbcData;
	std::string file = "SpellItemEnchantment.dbc";
	std::string fmt = "niiiiiiixxxiiissssssssssssssssxiiiiiii";
	uint32 index = 0;									
	if (QueryResult result = WorldDatabase.PQuery(
				//1	//2			//3		//4	//5		//6		//7			//8		//9		//10		//11
		"SELECT ID, charges, type1, type2, type3, amount1, amount2, amount3, spellid1, spellid2, spellid3, "
			//12		//13	//14	//15	//16				//17			//18				//19
		"description, aura_id, slot ,GemID, EnchantmentCondition, requiredSkill, requiredSkillValue, requiredLevel FROM _dbc_spellitemenchantment ORDER BY ID ASC"))
	{
		do
		{
			std::vector<std::string> v;
			Field* fields = result->Fetch();

			v.push_back(fields[0].GetString());
			v.push_back(fields[1].GetString());
			v.push_back(fields[2].GetString());
			v.push_back(fields[3].GetString());
			v.push_back(fields[4].GetString());
			v.push_back(fields[5].GetString());
			v.push_back(fields[6].GetString());
			v.push_back(fields[7].GetString());
			v.push_back("0");
			v.push_back("0");
			v.push_back("0");
			v.push_back(fields[8].GetString());
			v.push_back(fields[9].GetString());
			v.push_back(fields[10].GetString());

			for (size_t i = 0; i < 16; i++)
			{
				if (i != 4)
					v.push_back("");
				else
					v.push_back(fields[11].GetString());
			}

			v.push_back("16712190");
			v.push_back(fields[12].GetString());
			v.push_back(fields[13].GetString());
			v.push_back(fields[14].GetString());
			v.push_back(fields[15].GetString());
			v.push_back(fields[16].GetString());
			v.push_back(fields[17].GetString());
			v.push_back(fields[18].GetString());

			dbcData.push_back(v);

			index++;
			
			if (index % 200 == 0)
				sLog->outString(">>%s - packet data %d...", file.c_str(), index);

		} while (result->NextRow());
	}

	OutDBC(file, dbcData, fmt);
}

void DBCCreate::GenerateTalentDBC()
{
	std::vector<std::vector<std::string>> dbcData;
	std::string file = "Talent.dbc";
	std::string fmt = "niiiiiiiixxxxixxixxixxx";
	uint32 index = 0;
	if (QueryResult result = WorldDatabase.PQuery(
		//1			//2		//3		//4
		"SELECT TalentID, TalentTab, Row, Col, "
		//5			//6		//7		//8			//9
		"RankID1, RankID2, RankID3, RankID4, RankID5, "
		//10			//11		//12			//13		//14
		"DependsOn, DependsOnRank, addToSpellBook ,petMask, petMask1 FROM _dbc_talent ORDER BY TalentID ASC"))
	{
		do
		{
			std::vector<std::string> v;
			Field* fields = result->Fetch();

			v.push_back(fields[0].GetString());	//0
			v.push_back(fields[1].GetString());	//1
			v.push_back(fields[2].GetString());	//2
			v.push_back(fields[3].GetString());	//3
			v.push_back(fields[4].GetString());	//4
			v.push_back(fields[5].GetString());	//5
			v.push_back(fields[6].GetString());	//6
			v.push_back(fields[7].GetString());	//7
			v.push_back(fields[8].GetString());	//8
			v.push_back("0");					//9
			v.push_back("0");					//10
			v.push_back("0");					//11
			v.push_back("0");					//12
			v.push_back(fields[9].GetString());	//13
			v.push_back("0");					//14
			v.push_back("0");					//15
			v.push_back(fields[10].GetString());//16
			v.push_back("0");					//17
			v.push_back("0");					//18
			v.push_back(fields[11].GetString());//19
			v.push_back("0");					//20
			v.push_back(fields[12].GetString());//21
			v.push_back(fields[13].GetString());//22

			dbcData.push_back(v);

			index++;

			if (index % 200 == 0)
				sLog->outString(">>%s - packet data %d...", file.c_str(), index);

		} while (result->NextRow());
	}

	OutDBC(file, dbcData, fmt);
}

void DBCCreate::GenerateSpellIconDBC()
{
	std::vector<std::vector<std::string>> dbcData;
	std::string file = "SpellIcon.dbc";
	std::string fmt = "ns";
	uint32 index = 0;
	if (QueryResult result = WorldDatabase.PQuery("SELECT ID, Icon FROM _dbc_spellicon  ORDER BY ID ASC"))
	{
		do
		{
			std::vector<std::string> v;
			Field* fields = result->Fetch();

			v.push_back(fields[0].GetString());
			v.push_back(fields[1].GetString());
			
			dbcData.push_back(v);

			index++;

			if (index % 200 == 0)
				sLog->outString(">>%s - packet data %d...", file.c_str(), index);


		} while (result->NextRow());
	}

	OutDBC(file, dbcData, fmt);
}

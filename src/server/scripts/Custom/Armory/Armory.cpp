#pragma execution_character_set("utf-8")
#include "Armory.h"
#include "../GCAddon/GCAddon.h"
#include "GuildMgr.h"
#include "../CharNameMod/CharNameMod.h"

std::vector<ArmoryTokenTemplate> ArmoryTokenVec;
std::vector<ArmorySigilTemplate> ArmorySigilVec;
std::vector<ArmoryPvpTemplate> ArmoryPvpVec;

bool TokenSort(ArmoryTokenTemplate a, ArmoryTokenTemplate b) { return (a.amount < b.amount); }
bool TokenSortDown(ArmoryTokenTemplate a, ArmoryTokenTemplate b) { return (a.amount > b.amount); }
bool SigilSort(ArmorySigilTemplate a, ArmorySigilTemplate b) { return (a.amount < b.amount); }
bool SigilSortDown(ArmorySigilTemplate a, ArmorySigilTemplate b) { return (a.amount > b.amount); }
bool PvpSort(ArmoryPvpTemplate a, ArmoryPvpTemplate b) { return (a.amount < b.amount); }
bool PvpSortDown(ArmoryPvpTemplate a, ArmoryPvpTemplate b) { return (a.amount > b.amount); }

const int SigilArray[44] = 
{ 
	125000, 125001, 125002, 125003, 125004, 125005, 125006, 125007, 125008, 125009, 125010,
	126000, 126001, 126002, 126003, 126004, 126005, 126006, 126007, 126008, 126009, 126010,
	127000, 127001, 127002, 127003, 127004, 127005, 127006, 127007, 127008, 127009, 127010,
	128000, 128001, 128002, 128003, 128004, 128005, 128006, 128007, 128008, 128009, 128010
};

void Armory::Load()
{
	ArmoryTokenVec.clear();
	QueryResult result1 = CharacterDatabase.PQuery("SELECT guid,account,amount from armory_token");
	if (result1)
	{
		do
		{
			Field* fields = result1->Fetch();
			ArmoryTokenTemplate Temp;
			Temp.guid		= fields[0].GetUInt32();
			Temp.account	= fields[1].GetUInt32();
			Temp.amount		= fields[2].GetUInt32();
			ArmoryTokenVec.push_back(Temp);
		} while (result1->NextRow());
	}

	ArmorySigilVec.clear();
	QueryResult result2 = CharacterDatabase.PQuery("SELECT guid,account,amount from armory_sigil");
	if (result2)
	{
		do
		{
			Field* fields = result2->Fetch();
			ArmorySigilTemplate Temp;
			Temp.guid = fields[0].GetUInt32();
			Temp.account = fields[1].GetUInt32();
			Temp.amount = fields[2].GetUInt32();
			ArmorySigilVec.push_back(Temp);
		} while (result2->NextRow());
	}

	ArmoryPvpVec.clear();
	QueryResult result3 = CharacterDatabase.PQuery("SELECT guid,account,amount from armory_pvp");
	if (result3)
	{
		do
		{
			Field* fields = result3->Fetch();
			ArmoryPvpTemplate Temp;
			Temp.guid = fields[0].GetUInt32();
			Temp.account = fields[1].GetUInt32();
			Temp.amount = fields[2].GetUInt32();
			ArmoryPvpVec.push_back(Temp);
		} while (result3->NextRow());
	}
}

uint32 Armory::QueryToken(Player* player)
{
	return player->totalTokenAmount;
}

uint32 Armory::QuerySigil(Player* player)
{
	uint32 count = 0;

	//主背包
	for (uint8 i = INVENTORY_SLOT_ITEM_START; i < INVENTORY_SLOT_ITEM_END; i++)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			for (size_t k = 0; k < 44; k++)
				if (entry == SigilArray[k])
				{
					count++;
					break;
				}
		}

	//额外三个背包
	for (uint8 i = INVENTORY_SLOT_BAG_START; i < INVENTORY_SLOT_BAG_END; i++)
		if (Bag* pBag = player->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = player->GetItemByPos(i, j))
				{
					uint32 entry = item->GetEntry();
					for (size_t k = 0; k < 44; k++)
						if (entry == SigilArray[k])
						{
							count++;
							break;
						}
				}

	//银行
	for (uint8 i = BANK_SLOT_ITEM_START; i < BANK_SLOT_BAG_END; ++i)
		if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
		{
			uint32 entry = item->GetEntry();
			for (size_t k = 0; k < 44; k++)
				if (entry == SigilArray[k])
				{
					count++;
					break;
				}
		}
	//银行背包
	for (uint8 i = BANK_SLOT_BAG_START; i < BANK_SLOT_BAG_END; ++i)
		if (Bag* pBag = player->GetBagByPos(i))
			for (uint32 j = 0; j < pBag->GetBagSize(); j++)
				if (Item* item = player->GetItemByPos(i, j))
				{
					uint32 entry = item->GetEntry();
					for (size_t k = 0; k < 44; k++)
						if (entry == SigilArray[k])
						{
							count++;
							break;
						}
				}
	return count;
}

uint32 Armory::QueryPvp(Player* player)
{
	return player->GetUInt32Value(PLAYER_FIELD_LIFETIME_HONORABLE_KILLS);
}


void Armory::UpdateTokenArmory(Player* player)
{
	uint32 account = player->GetSession()->GetAccountId();
	uint32 guid = player->GetGUIDLow();
	uint32 amount = QueryToken(player); 

	std::vector<ArmoryTokenTemplate>::iterator itr;
	for (itr = ArmoryTokenVec.begin(); itr != ArmoryTokenVec.end(); ++itr)
		if (account == itr->account)
		{
			SQLTransaction trans = CharacterDatabase.BeginTransaction();
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ARMORY_TOKEN);
			stmt->setUInt32(0, guid);
			stmt->setUInt32(1, amount);
			stmt->setUInt32(2, account);
			trans->Append(stmt);
			CharacterDatabase.CommitTransaction(trans);

			itr->guid = guid;
			itr->amount = amount;
			return;
		}


	if (ArmoryTokenVec.size() < 5)
	{
		SQLTransaction trans = CharacterDatabase.BeginTransaction();
		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARMORY_TOKEN);
		stmt->setUInt32(0, guid);
		stmt->setUInt32(1, account);
		stmt->setUInt32(2, amount);
		trans->Append(stmt);
		CharacterDatabase.CommitTransaction(trans);

		ArmoryTokenTemplate Temp;
		Temp.guid = guid;
		Temp.account = account;
		Temp.amount = amount;
		ArmoryTokenVec.push_back(Temp);
		return;
	}

	std::sort(ArmoryTokenVec.begin(), ArmoryTokenVec.end(), TokenSort);

	for (itr = ArmoryTokenVec.begin(); itr != ArmoryTokenVec.end(); ++itr)
	{
		if (amount > itr->amount)
		{
			SQLTransaction trans = CharacterDatabase.BeginTransaction();
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ARMORY_TOKEN);
			stmt->setUInt32(0, itr->account);
			trans->Append(stmt);

			stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARMORY_TOKEN);
			stmt->setUInt32(0, guid);
			stmt->setUInt32(1, account);
			stmt->setUInt32(2, amount);
			trans->Append(stmt);
			CharacterDatabase.CommitTransaction(trans);

			itr->guid = guid;
			itr->account = account;
			itr->amount = amount;
			return;
		}
	}

	std::sort(ArmoryTokenVec.begin(), ArmoryTokenVec.end(), TokenSort);
}

void Armory::UpdateSigilArmory(Player* player)
{
	uint32 account = player->GetSession()->GetAccountId();
	uint32 guid = player->GetGUIDLow();
	uint32 amount = QuerySigil(player);

	std::vector<ArmorySigilTemplate>::iterator itr;
	for (itr = ArmorySigilVec.begin(); itr != ArmorySigilVec.end(); ++itr)
		if (account == itr->account)
		{
			//如果当前玩家圣物数量小于榜中同一账号下的玩家圣物数量，则返回 否则 替换榜中同一账号下的玩家数据
			if (itr->amount > amount)
				return;

			SQLTransaction trans = CharacterDatabase.BeginTransaction();
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ARMORY_SIGIL);
			stmt->setUInt32(0, guid);
			stmt->setUInt32(1, amount);
			stmt->setUInt32(2, account);
			trans->Append(stmt);
			CharacterDatabase.CommitTransaction(trans);

			itr->guid = guid;
			itr->amount = amount;
			return;
		}


	if (ArmorySigilVec.size() < 5)
	{
		SQLTransaction trans = CharacterDatabase.BeginTransaction();
		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARMORY_SIGIL);
		stmt->setUInt32(0, guid);
		stmt->setUInt32(1, account);
		stmt->setUInt32(2, amount);
		trans->Append(stmt);
		CharacterDatabase.CommitTransaction(trans);

		ArmorySigilTemplate Temp;
		Temp.guid = guid;
		Temp.account = account;
		Temp.amount = amount;
		ArmorySigilVec.push_back(Temp);
		return;
	}

	std::sort(ArmorySigilVec.begin(), ArmorySigilVec.end(), SigilSort);

	for (itr = ArmorySigilVec.begin(); itr != ArmorySigilVec.end(); ++itr)
	{
		if (amount > itr->amount)
		{
			SQLTransaction trans = CharacterDatabase.BeginTransaction();
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ARMORY_SIGIL);
			stmt->setUInt32(0, itr->account);
			trans->Append(stmt);

			stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARMORY_SIGIL);
			stmt->setUInt32(0, guid);
			stmt->setUInt32(1, account);
			stmt->setUInt32(2, amount);
			trans->Append(stmt);
			CharacterDatabase.CommitTransaction(trans);

			itr->guid = guid;
			itr->account = account;
			itr->amount = amount;
			return;
		}
	}

	std::sort(ArmorySigilVec.begin(), ArmorySigilVec.end(), SigilSort);
}

void Armory::UpdatePvpArmory(Player* player)
{
	uint32 account = player->GetSession()->GetAccountId();
	uint32 guid = player->GetGUIDLow();
	uint32 amount = QueryPvp(player);

	std::vector<ArmoryPvpTemplate>::iterator itr;
	//for (itr = ArmoryPvpVec.begin(); itr != ArmoryPvpVec.end(); ++itr)
	//	if (account == itr->account)
	//	{
	//		//如果当前玩家击杀数量小于榜中同一账号下的玩家击杀数量，则返回 否则 替换榜中同一账号下的玩家数据
	//		if (itr->amount > amount)
	//			return;
	//
	//		SQLTransaction trans = CharacterDatabase.BeginTransaction();
	//		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ARMORY_PVP);
	//		stmt->setUInt32(0, guid);
	//		stmt->setUInt32(1, amount);
	//		stmt->setUInt32(2, account);
	//		trans->Append(stmt);
	//		CharacterDatabase.CommitTransaction(trans);
	//
	//		itr->guid = guid;
	//		itr->amount = amount;
	//		return;
	//	}
	for (itr = ArmoryPvpVec.begin(); itr != ArmoryPvpVec.end(); ++itr)
	if (guid == itr->guid)
	{
		SQLTransaction trans = CharacterDatabase.BeginTransaction();
		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_ARMORY_PVP);
		stmt->setUInt32(0, amount);
		stmt->setUInt32(1, guid);
		trans->Append(stmt);
		CharacterDatabase.CommitTransaction(trans);
		
		itr->amount = amount;
		return;
	}

	if (ArmoryPvpVec.size() < 5)
	{
		SQLTransaction trans = CharacterDatabase.BeginTransaction();
		PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARMORY_PVP);
		stmt->setUInt32(0, guid);
		stmt->setUInt32(1, account);
		stmt->setUInt32(2, amount);
		trans->Append(stmt);
		CharacterDatabase.CommitTransaction(trans);

		ArmoryPvpTemplate Temp;
		Temp.guid = guid;
		Temp.account = account;
		Temp.amount = amount;
		ArmoryPvpVec.push_back(Temp);
		return;
	}

	std::sort(ArmoryPvpVec.begin(), ArmoryPvpVec.end(), PvpSort);

	for (itr = ArmoryPvpVec.begin(); itr != ArmoryPvpVec.end(); ++itr)
	{
		if (amount > itr->amount)
		{
			SQLTransaction trans = CharacterDatabase.BeginTransaction();
			PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_ARMORY_PVP);
			stmt->setUInt32(0, itr->account);
			trans->Append(stmt);

			stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_ARMORY_PVP);
			stmt->setUInt32(0, guid);
			stmt->setUInt32(1, account);
			stmt->setUInt32(2, amount);
			trans->Append(stmt);
			CharacterDatabase.CommitTransaction(trans);

			itr->guid = guid;
			itr->account = account;
			itr->amount = amount;
			return;
		}
	}

	std::sort(ArmoryPvpVec.begin(), ArmoryPvpVec.end(), PvpSort);
}

void Armory::SendArmoryTokenMsg(Player* player)
{
	std::sort(ArmoryTokenVec.begin(), ArmoryTokenVec.end(), TokenSortDown);

	std::ostringstream oss;
	oss << "GC_SMSG_OPC_ARMORY_TOKEN@";
	std::vector<ArmoryTokenTemplate>::iterator itr;
	for (itr = ArmoryTokenVec.begin(); itr != ArmoryTokenVec.end(); ++itr)
	{
		GlobalPlayerData const* playerData = sWorld->GetGlobalPlayerData(itr->guid);
		if (!playerData)
			continue;

		std::string guildName = sGuildMgr->GetGuildNameById(playerData->guildId);

		if (guildName.empty())
			guildName = "无";

		oss << sCharNameMod->GetPureName(playerData->name) << "-" << playerData->name << "-" << GetGender(playerData->gender) << "-" << GetRace(playerData->race) << "-" << GetClass(playerData->playerClass) << "-" << guildName << "-" << itr->amount << ":";
	}
	//sGCAddon->SendPacket(player, oss.str());
}

void Armory::SendArmorySigilMsg(Player* player)
{
	std::sort(ArmorySigilVec.begin(), ArmorySigilVec.end(), SigilSortDown);

	std::ostringstream oss;
	oss << "GC_SMSG_OPC_ARMORY_SIGIL@";
	std::vector<ArmorySigilTemplate>::iterator itr;
	for (itr = ArmorySigilVec.begin(); itr != ArmorySigilVec.end(); ++itr)
	{
		GlobalPlayerData const* playerData = sWorld->GetGlobalPlayerData(itr->guid);
		if (!playerData)
			continue;

		std::string guildName = sGuildMgr->GetGuildNameById(playerData->guildId);

		if (guildName.empty())
			guildName = "无";

		oss << sCharNameMod->GetPureName(playerData->name) << "-" << playerData->name << "-" << GetGender(playerData->gender) << "-" << GetRace(playerData->race) << "-" << GetClass(playerData->playerClass) << "-" << guildName << "-" << itr->amount << ":";
	}
	//sGCAddon->SendPacket(player, oss.str());
}

void Armory::SendArmoryPvpMsg(Player* player)
{
	std::sort(ArmoryPvpVec.begin(), ArmoryPvpVec.end(), PvpSortDown);

	std::ostringstream oss;
	oss << "GC_SMSG_OPC_ARMORY_PVP@";
	std::vector<ArmoryPvpTemplate>::iterator itr;
	for (itr = ArmoryPvpVec.begin(); itr != ArmoryPvpVec.end(); ++itr)
	{
		GlobalPlayerData const* playerData = sWorld->GetGlobalPlayerData(itr->guid);
		if (!playerData)
			continue;

		std::string guildName = sGuildMgr->GetGuildNameById(playerData->guildId);

		if (guildName.empty())
			guildName = "无";

		oss << sCharNameMod->GetPureName(playerData->name) << "-" << playerData->name << "-" << GetGender(playerData->gender) << "-" << GetRace(playerData->race) << "-" << GetClass(playerData->playerClass) << "-" << guildName << "-" << itr->amount << ":";
	}
	//sGCAddon->SendPacket(player, oss.str());
}

class ArmoryPlayerScript : PlayerScript
{
public:
	ArmoryPlayerScript() : PlayerScript("ArmoryPlayerScript") {}
	void OnLogin(Player* player)
	{
		sArmory->UpdateTokenArmory(player);
		sArmory->UpdateSigilArmory(player);
		sArmory->UpdatePvpArmory(player);
	}
};

void AddSC_Armory()
{
	//new ArmoryPlayerScript();
}

std::string Armory::GetRace(uint8 race)
{
	switch (race)
	{
	case 1:
		return "Human";
	case 2:
		return "Orc";
	case 3:
		return "Dwarf";
	case 4:
		return "NightElf";
	case 5:
		return "Scourge";
	case 6:
		return "Tauren";
	case 7:
		return "Gnome";
	case 8:
		return "Troll";
	case 10:
		return "BloodElf";
	case 11:
		return "Draenei";
	default:
		break;
	}

	return "";
}

std::string Armory::GetGender(uint8 gender)
{
	if (gender == 0)
		return "Male";

	return "Female";
}

std::string Armory::GetClass(uint8 playerClass)
{
	switch (playerClass)
	{
	case 1:
		return "战士";
	case 2:
		return "圣骑士";
	case 3:
		return "猎人";
	case 4:
		return "盗贼";
	case 5:
		return "牧师";
	case 6:
		return "死亡骑士";
	case 7:
		return "萨满祭司";
	case 8:
		return "法师";
	case 9:
		return "术士";
	case 11:
		return "德鲁伊";
	default:
		break;
	}

	return "";
}

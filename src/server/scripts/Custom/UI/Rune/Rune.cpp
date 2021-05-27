#include "Rune.h"
#include "../../GCAddon/GCAddon.h"
#include "../../Requirement/Requirement.h"

std::vector<RuneTemplate> RuneVec;
std::unordered_map<uint32/*page*/, RuneCategoryTemplate> RuneCategoryMap;

void Rune::Load()
{
	RuneVec.clear();

	if (QueryResult result = WorldDatabase.PQuery("SELECT page,id,spellid,reqId,classIndex FROM _ui_rune"))
	{
		do
		{
			Field* fields = result->Fetch();
			
			RuneTemplate Temp;
			Temp.page		= fields[0].GetUInt32();
			Temp.id			= fields[1].GetUInt32();
			Temp.spellid	= fields[2].GetUInt32();
			Temp.reqId		= fields[3].GetUInt32();
			Temp.classIndex = fields[4].GetUInt32();
			RuneVec.push_back(Temp);
		} while (result->NextRow());
	}

	RuneCategoryMap.clear();

	if (QueryResult result = WorldDatabase.PQuery("SELECT page, title,tip,icon FROM _ui_rune_category"))
	{
		do
		{
			Field* fields = result->Fetch();
			uint32 page = fields[0].GetUInt32();
			RuneCategoryTemplate Temp;
			Temp.title	= fields[1].GetString();
			Temp.tip	= fields[2].GetString();
			Temp.icon	= fields[3].GetString();
			RuneCategoryMap.insert(std::make_pair(page, Temp));
		} while (result->NextRow());
	}
}

void Rune::SendData(Player* player)
{
	for (auto iter = RuneVec.begin(); iter != RuneVec.end(); iter++)
	{
		if (iter->classIndex == 0)
		{
			std::ostringstream oss;
			oss << iter->page << " ";
			oss << iter->id << " ";
			oss << iter->spellid << " ";
			oss << iter->reqId << " ";

			if (player->HasSpell(iter->spellid))
				oss << 1;
			else
				oss << 0;

			sGCAddon->SendPacketTo(player, "GC_S_RUNE", oss.str());
		}
	}

	for (auto iter = RuneVec.begin(); iter != RuneVec.end(); iter++)
	{
		if (iter->classIndex == player->getClass())
		{
			std::ostringstream oss;
			oss << iter->page << " ";
			oss << iter->id << " ";
			oss << iter->spellid << " ";
			oss << iter->reqId << " ";

			if (player->HasSpell(iter->spellid))
				oss << 1;
			else
				oss << 0;

			sGCAddon->SendPacketTo(player, "GC_S_RUNE", oss.str());
		}
	}

	for (auto iter = RuneCategoryMap.begin(); iter != RuneCategoryMap.end(); iter++)
	{
		std::ostringstream oss;
		oss << iter->first << " ";
		oss << iter->second.title << " ";
		oss << iter->second.tip << " ";
		oss << iter->second.icon;
		sGCAddon->SendPacketTo(player, "GC_S_RUNE_CATEGORY", oss.str());
	}
}

void Rune::Update(Player* player, uint32 page, uint32 id)
{
	uint32 reqid = 0;
	uint32 spell = 0;

	for (auto iter = RuneVec.begin(); iter != RuneVec.end(); iter++)
	{
		if (iter->page == page && iter->id == id && iter->classIndex == 0)
		{
			reqid = iter->reqId;
			spell = iter->spellid;
			break;
		}
	}

	for (auto iter = RuneVec.begin(); iter != RuneVec.end(); iter++)
	{
		if (iter->page == page && iter->id == id && iter->classIndex == player->getClass())
		{
			reqid = iter->reqId;
			spell = iter->spellid;
			break;
		}
	}

	if (spell == 0)
		return;

	if(sReq->Check(player, reqid))
	{
		std::ostringstream oss;
		oss << page << " ";
		oss << id << " ";
		oss << spell << " ";
		oss << reqid;
		sGCAddon->SendPacketTo(player, "GC_S_RUNE_UPDATE", oss.str());
		player->learnSpell(spell);
		sReq->Des(player, reqid);
	}	
}

void Rune::Add(Player* player, uint32 spellId)
{
	for (auto iter = RuneVec.begin(); iter != RuneVec.end(); iter++)
	{
		if (iter->spellid == spellId)
		{
			std::ostringstream oss;
			oss << iter->page << " ";
			oss << iter->id << " ";
			oss << iter->spellid << " ";
			oss << iter->reqId;
			sGCAddon->SendPacketTo(player, "GC_S_RUNE_UPDATE", oss.str());

			if (!player->HasSpell(spellId))
				player->learnSpell(spellId);
		}
	}
}
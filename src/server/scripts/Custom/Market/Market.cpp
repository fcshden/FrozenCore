#pragma execution_character_set("utf-8")
#include "Market.h"
#include "../CommonFunc/CommonFunc.h"
#include "../GCAddon/GCAddon.h"
#include "../String/myString.h"

void Market::SetSelection(Player* buyer, uint64 guid)
{
	Player* seller = ObjectAccessor::GetPlayer(*buyer, guid);

	if (seller && seller->OnSale)
		UpdateMaket(buyer, seller);
}

void Market::InitMarket(Player* player)
{
	player->MarketVec.clear();
	player->OnSale = true;
}

void Market::ClickItem(Player* player, Item* item)
{
	if (player->MarketVec.size() < 6)
	{
		MarketTemplate Temp;
		Temp.item = item;
		Temp.token = 0;
		player->MarketVec.push_back(Temp);
	}
	else
		ChatHandler(player->GetSession()).PSendSysMessage("û�ж������λ");

	UpdateMaket(player);
}

void Market::UpdateMaket(Player* player)
{
	std::ostringstream oss;
	oss << "GC_SMSG_OPC_MARKET_UPDATE@";

	for (uint32 slot = 0; slot < player->MarketVec.size(); slot++)
	{
		Item* item = player->MarketVec[slot].item;
		uint32 token = player->MarketVec[slot].token;

		if (!item)
			continue;

		oss << slot + 1 << "-" << item->GetEntry() << "-" << item->GetCount() << "-" << token << ":";
	}

	player->OnSale = true;
	//sGCAddon->SendPacket(player, oss.str());
}

void Market::UpdateMaket(Player* buyer, Player* seller)
{
	std::ostringstream oss;
	oss << "GC_SMSG_OPC_MARKET_BUYER_UPDATE@";

	for (uint32 slot = 0; slot < seller->MarketVec.size(); slot++)
	{
		Item* item = seller->MarketVec[slot].item;
		uint32 token = seller->MarketVec[slot].token;

		if (!item)
			continue;

		ItemDisplayInfoEntry const* info = sItemDisplayInfoStore.LookupEntry(item->GetTemplate()->DisplayInfoID);

		if (!info)
			continue;

		oss << slot + 1 << "-" << item->GetEntry() << "-" << item->GetCount() << "-" << info->inventoryIcon << "-" << token << ":";
	}

	//sGCAddon->SendPacket(buyer, oss.str());
}

void Market::SetPrice(Player* player, uint32 slot, uint32 token)
{
	for (uint32 slot1 = 0; slot1 < player->MarketVec.size(); slot1++)
	{
		if (slot == slot1)
		{
			player->MarketVec[slot1].token = token;
			break;
		}
	}
}

void Market::OffShelve(Player* player, uint32 slot)
{
	std::vector<MarketTemplate>::iterator it = player->MarketVec.begin() + slot;
	player->MarketVec.erase(it);

	UpdateMaket(player);
}

void Market::OnSell(Player* seller, Player* buyer, uint32 slot)
{
	Item* item = NULL;
	uint32 token = 0;

	for (uint32 i = 0; i < seller->MarketVec.size(); i++)
		if (i == slot)
		{
			item	= seller->MarketVec[i].item;
			token	= seller->MarketVec[i].token;
			break;
		}

	if (!item)
	{
		ChatHandler(buyer->GetSession()).PSendSysMessage("����Ʒ������");
		UpdateMaket(buyer, seller);
		return;
	}
		
	if (buyer->totalTokenAmount < token)
	{
		ChatHandler(buyer->GetSession()).PSendSysMessage("%s�����Թ������Ʒ", sString->GetText(CORE_STR_TYPES(STR_TOKEN)));
		return;
	}
		
	OffShelve(seller, slot);
	UpdateMaket(buyer, seller);


	buyer->AddItem(item->GetBagSlot(), item->GetCount());
	ChatHandler(buyer->GetSession()).PSendSysMessage("������Ʒ���");
	sCF->UpdateTokenAmount(buyer, token, false, "[��̯]������Ʒ");

	sCF->UpdateTokenAmount(seller, token, true, "[��̯]������Ʒ");
	seller->DestroyItem(item->GetBagSlot(), item->GetSlot(), true);
	ChatHandler(seller->GetSession()).PSendSysMessage("������Ʒ���");
}


class spell_market : public SpellScriptLoader
{
public:
	spell_market() : SpellScriptLoader("spell_market") { }

	class spell_market_SpellScript : public SpellScript
	{
		PrepareSpellScript(spell_market_SpellScript);
		void HandleAfterCast()
		{
			Player* player = GetCaster()->ToPlayer();
			if (!player)
				return;

			Item* item = GetExplTargetItem();
			if (!item)
				return;

			if (item->GetOwner() != player)
			{
				ChatHandler(player->GetSession()).PSendSysMessage("�㲻ӵ�������Ʒ��");
				return;
			}
			
			sMarket->ClickItem(player, item);
		}

		void Register() override
		{
			AfterCast += SpellCastFn(spell_market_SpellScript::HandleAfterCast);
		}
	};

	SpellScript* GetSpellScript() const override
	{
		return new spell_market_SpellScript();
	}
};

void AddSC_Market()
{
	//new spell_market();
}

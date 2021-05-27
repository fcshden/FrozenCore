#include "ItemMod.h"
#include "../GCAddon/GCAddon.h"

#define AT_ID_START 100001
#define AT_ID_END	110000
#define SP_ID_START	110001
#define SP_ID_END	120000

#define AT_ITEM_ENTRY 3
#define SP_ITEM_ENTRY 38662


void ItemMod::NeckUp(Player* player, bool isPVP)
{
	if (!isPVP && urand(0, 100) > 30)
		return;

	Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, EQUIPMENT_SLOT_NECK);

	if (!item)
		return;
	uint32 itemEntry = item->GetEntry();

	if (itemEntry != AT_ITEM_ENTRY && itemEntry != SP_ITEM_ENTRY)
		return;

	uint32 enchantId = item->GetEnchantmentId(PERM_ENCHANTMENT_SLOT);

	uint32 id_start = 0;
	uint32 id_end = 0;

	if (itemEntry == AT_ITEM_ENTRY)
	{
		id_start = AT_ID_START;
		id_end = AT_ID_END;
	}
	else
	{
		id_start = SP_ID_START;
		id_end = SP_ID_END;
	}

	if (enchantId < id_start)
		enchantId = id_start;
	else
		enchantId++;

	if (enchantId > id_end)
		return;

	player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, false);
	item->SetEnchantment(PERM_ENCHANTMENT_SLOT, enchantId, 0, 0);
	player->ApplyEnchantment(item, PERM_ENCHANTMENT_SLOT, true);

	//save to db 需要测试一下
	//player->SaveToDB(false, false);
}

class GCAddon
{
public:
	static GCAddon* instance()
	{
		static GCAddon instance;
		return &instance;
	}
	
	std::string SplitStr(std::string msg, uint32 index);

	bool OnRecv(Player* player, std::string msg);

	void SendPacketTo(const Player* player, std::string opcode, std::string msg)
	{
		msg = opcode + "\t" + msg;
		WorldPacket data(SMSG_MESSAGECHAT, 100);
		data << uint8(CHAT_MSG_WHISPER);
		data << int32(LANG_ADDON);
		data << uint64(player->GetGUID());
		data << uint32(0);
		data << uint64(player->GetGUID());
		data << uint32(msg.length() + 1);
		data << msg;
		data << uint8(0);
		player->GetSession()->SendPacket(&data);
	}
	
	std::string GetStrByIndex(uint32 index, std::string buff);

	void Load();

	uint32 GetItemEntry(std::string itemLink);
	std::string GetItemLink(Item* item, WorldSession* session);

	//»ñÈ¡ÏûºÄ¡¢½±Àø×Ö·û´®
	std::string GetReqString(Player* player, uint32 reqId);
	std::string GetRewString(Player* player, uint32 rewId);

	//»Ã»¯
	uint32 GetTransReqId(uint32 itemId);
	std::string GetItemIcon(uint32 entry);

	void SendAllData(Player* player);
	void SendDBData(Player* player);
	void SendReqData(Player* player);
	void SendRewData(Player* player);
	void SendVIPData(Player* player);
	void SendHRData(Player* player);
	void SendFactionData(Player* player);
	void SendReincarnationData(Player* player);
	void SendRankData(Player* player);
	void SendEnchantData(Player* player);
	void SendItemEnchantData(Player* player, Item* item);
	void SendItemEntryData(Player* player, uint32 entry);
	void SendItemDayLimitData(Player* player);


	//char data
	void SendCharData(Player* player);
	void SendTokenData(Player* player);
	void SendTokenUpdateData(Player* player, int64 count, bool add);

	void SendTransMogData(Player* player);

	//check map achieve spell quest
	void SendReqCheck(Player* player, uint32 req, bool pop);
private:

};
#define sGCAddon GCAddon::instance()

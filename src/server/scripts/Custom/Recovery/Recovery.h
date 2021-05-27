struct RecoveryTemplate
{
	uint32 categoryId;
	float rewToken;
    uint32 rewitem;
    uint32 rewcunt;
    uint32 rewid;
};

extern std::unordered_map<uint32/*entry*/, RecoveryTemplate> RecoveryMap;
extern std::unordered_map<uint32/*categoryId*/, std::string/*categoryName*/> RecoveryCategoryMap;

class Recovery
{
public:
	static Recovery* instance()
	{
		static Recovery instance;
		return &instance;
	}
	void Load();
	std::string Recovery::GetDes(uint32 entry);
	bool HasCategoryItem(Player* player, uint32 categoryId);
	void OpenPanel(Player* player);
	void GetItemInfo(uint32 entry, uint32 count, uint32 &categoryId, float &tokenAmount);
	uint32 GetCategoryId(uint32 entry);
	uint32 GetTokenAmount(Player* player, uint32 categoryId);
	void SendCategoryMsg(Player* player, uint32 categoryId);
	void Action(Player* player, uint32 categoryId);
private:

};
#define sRecovery Recovery::instance()

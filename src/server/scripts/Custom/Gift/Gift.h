struct GiftTemplate
{
	uint32 time;
	uint32 rewId;
	std::string notice;
};

extern std::vector<GiftTemplate> GiftVec;


class Gift
{
public:
	static Gift* instance()
	{
		static Gift instance;
		return &instance;
	}
	void Load();
	void Check(Player* player);
private:

};
#define sGift Gift::instance()
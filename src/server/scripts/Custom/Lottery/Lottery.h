enum LotteryType
{
	LOTTERY_TYPE_AAA = 0,
	LOTTERY_TYPE_MAX = 1,
	LOTTERY_TYPE_MIN = 2
};

struct LotteryTemplate
{
	uint32 guidLow;
	LotteryType lotteryType;
	uint32 lotteryAmount;
};

struct LotterySetTemplate
{
	uint32 tokenLimit;
	uint32 interval;
	uint32 aaa_muil;
	uint32 max_muil;
	uint32 min_muil;

	uint32 aaa_chance;
	uint32 max_chance;
	uint32 min_chance;
};
extern std::vector<LotterySetTemplate> LotteryVec;

class Lottery
{
public:
	static Lottery* instance()
	{
		static Lottery instance;
		return &instance;
	}
	void Load();

	LotteryType GetLotteryType(){ return m_type; }
	void SetLotteryType(LotteryType type){ m_type = type; }

	uint32 GetRewardTokenAmountByType(LotteryType type);
	void GenerateLotteryType();
	uint32 GetMuilByType(LotteryType type);
	std::string GetLotteryName(LotteryType type);

	void LotteryRunNotice();
	void LotteryEnd();
	void LoadLotterData();

	bool PlayerBuyLottery(Player*, LotteryType type, uint32 lotteryAmount);
	uint32 GetPlayerLotteryAmount(Player* player);
	bool PlayerCanBuy(Player* player, uint32 lotteryAmount);
	void RewardPlayer(LotteryType type);

	void SetLotteryAmountLimit(uint32 lotteryAmountLimit){ m_lotteryAmountLimit = lotteryAmountLimit; }
	uint32 GetLotterAmountLimit(){ return m_lotteryAmountLimit; }

	void AddLotteryGossip(Player* player, Object* obj);
private:
	LotteryType m_type;
	uint32 m_lotteryAmountLimit;

};
#define sLottery Lottery::instance()
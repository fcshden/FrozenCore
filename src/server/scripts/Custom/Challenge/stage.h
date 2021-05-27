struct StageTemplate
{
	uint32 stageRank;
	uint32 summonsGroupId;
	uint32 rewardGameobjectId;
	std::string description;
};
extern std::vector<StageTemplate> StageVec;

class Stage
{
public:
	static Stage* instance()
	{
		static Stage instance;
		return &instance;
	}

	void Load();

	uint32 GetSumId(uint32 stage);
	uint32 GetGobId(uint32 stage);
	std::string GetDescription(uint32 stage);
};

#define sStage Stage::instance()
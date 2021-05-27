struct CustomSkillTemplate
{
	uint32 skillId;
	std::string skillName;
	uint32 categoryId;
	uint32 reqId;
	uint32 rewId;
	std::string skillIcon;
};

extern std::vector<CustomSkillTemplate> CustomSkillVec;

struct CustomSkillCategoryTemplate
{
	uint32 categoryId;
	std::string categoryName;
	std::string icategoryIcon;
};

extern std::vector<CustomSkillCategoryTemplate> CustomSkillCategoryVec;

class CustomSkill
{
public:
	static CustomSkill* instance()
	{
		static CustomSkill instance;
		return &instance;
	}

	void Load();
	void LearnSkill(Player* player, uint32 skillId, bool learn = true);

	std::string GetSkillName(uint32 skillId);

	uint32 GetReqId(uint32 skillId);
	uint32 GetRewId(uint32 skillId);
	uint32 GetCategoryId(uint32 skillId);
	std::string GetCategoryName(uint32 skillId);
	std::string GetSkillIcon(uint32 skillId);

	void CharLoadSkill(Player* player);

	void AddGossip(Player* player, Object* obj);
	void Action(Player* player, uint32 action, Object* obj);

private:

};
#define sCustomSkill CustomSkill::instance()
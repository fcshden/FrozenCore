struct RuneTemplate
{
	uint32 page;
	uint32 id;
	uint32 spellid;
	uint32 reqId;
	uint32 classIndex;
};

extern std::vector<RuneTemplate> RuneVec;

struct RuneCategoryTemplate
{
	std::string title;
	std::string tip;
	std::string icon;
};

extern std::unordered_map<uint32/*page*/, RuneCategoryTemplate> RuneCategoryMap;

class Rune
{
public:
	static Rune* instance()
	{
		static Rune instance;
		return &instance;
	}

	void Load();
	void SendData(Player* player);
	void Update(Player* player, uint32 page, uint32 id);
	void Add(Player* player, uint32 spellId);
private:

};
#define sRune Rune::instance()
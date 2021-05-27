enum StoryAction
{
	STORY_ACTION_NONE,
	STORY_ACTION_START,
	STORY_ACTION_MOVE,
	STORY_ACTION_SAY,
	STORY_ACTION_YELL,
	STORY_ACTION_EMOTE,
	STORY_ACTION_SPELL,
	STORY_ACTION_END,
};

struct StoryTemplate
{
	StoryAction action;
	std::string param1;
	std::string param2;
};

extern std::unordered_map<uint32/*entry*/, std::unordered_map<uint32/*index*/, StoryTemplate> > StoryMap;

class Story
{
public:
	static Story* instance()
	{
		static Story instance;
		return &instance;
	}

	void Load();

	void GetParams(uint32 entry, uint32 index, StoryAction &action, std::string &param1, std::string &param2);
};
#define sStory Story::instance()
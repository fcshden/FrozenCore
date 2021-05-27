struct CommandTemplate
{
	uint32 ID;
	std::string command;
};

extern std::vector<CommandTemplate> CommandVec;

class CustomCommand
{
public:
	static CustomCommand* instance()
	{
		static CustomCommand instance;
		return &instance;
	}

	void Load();
	void DoCommandByID(Player* player, uint32 ID);
	void DoCommand(Player* player, std::string command);
private:

};
#define sCustomCommand CustomCommand::instance()
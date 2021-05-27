class CharNameMod
{
public:
	static CharNameMod* instance()
	{
		static CharNameMod instance;
		return &instance;
	}
	void UpdatePrefix(Player* player, std::string namePrefix);
	void UpdateSuffix(Player* player, std::string nameSuffix);
	std::string GetPureName(std::string name);
private:

};
#define sCharNameMod CharNameMod::instance()
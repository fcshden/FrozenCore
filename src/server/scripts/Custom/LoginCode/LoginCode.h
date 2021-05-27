class LoginCode
{
public:
	static LoginCode* instance()
	{
		static LoginCode instance;
		return &instance;
	}
	void AddGossip(Player* player, Object* obj);
	void Action(Player*player, uint32 action, Object*obj);
	void GenerateCode(Player* player);
private:

};
#define sLoginCode LoginCode::instance()
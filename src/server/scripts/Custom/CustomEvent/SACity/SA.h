#define EVENT_ID_SA 80

#define CorpseCreature_ENTRY 50008

class SACity
{
public:
	static SACity* instance()
	{
		static SACity instance;
		return &instance;
	}

private:

};
#define sSACity SACity::instance()

#define MountTryTime 5

struct MountVendorTemplate
{
	uint32 spellId;
	bool onSale;
	uint32 reqId;
};

extern std::vector<MountVendorTemplate> MountVendorVec;

class MountVendor
{
public:
	static MountVendor* instance()
	{
		static MountVendor instance;
		return &instance;
	}

	void Load();
	

private:

};
#define sMountVendor MountVendor::instance()
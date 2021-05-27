class DBCCreate
{
public:
	static DBCCreate* instance()
	{
		static DBCCreate instance;
		return &instance;
	}

	void RepStr(std::string &s1, const std::string &s2, const std::string &s3);
	void RepStr(std::string &s);

	char GetFieldType(std::string fmt, uint32 index);
	void OutDBC(std::string file, std::vector<std::vector<std::string>> dbcData, std::string fmt, uint8 type = 0);
	void Encrypt(std::string file, uint32 recordCount, uint32 fieldCount);

	void GenerateItemDisplayInfoSql();	
	void GenerateItemDBC();
	void GenerateItemDisplayInfoDBC();

	void GenerateItemExtendCostSql();
	void GenerateItemExtendCostDBC();

	void GenerateSpellSql();
	void GenerateSpellDBC();

	void GenerateCharStartOutfitSql();
	void GenerateCharStartOutfitDBC();

	void GenerateSpellItemEnchantmentSql();
	void GenerateSpellItemEnchantmentDBC();

	void GenerateTalentSql();
	void GenerateTalentDBC();

	void GenerateSpellIconSql();
	void GenerateSpellIconDBC();

private:

};
#define sDBCCreate DBCCreate::instance()
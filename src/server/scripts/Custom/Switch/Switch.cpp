#pragma execution_character_set("utf-8")
#include "Switch.h"
#include "../CommonFunc/CommonFunc.h"
#include "BattlegroundMgr.h"

uint32 PlayerTimeRewType;
uint32 PlayerTimeRewIntervals;
uint32 AntiCheatType;

std::vector<SwitchTemplate> SwitchVec;

void Switch::Load()
{
	SwitchVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ? 
		"SELECT ID, 参数 FROM __通用配置" :
		"SELECT ID, param FROM _common_setting");

	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		SwitchTemplate Temp;
		Temp.Type	= SwithTypes(fields[0].GetUInt32());
		Temp.param = fields[1].GetString();
		SwitchVec.push_back(Temp);
	} while (result->NextRow());

	PlayerTimeRewType = GetValue(ST_TIME_REW);
	PlayerTimeRewIntervals = atoi(GetFlagByIndex(ST_TIME_REW, 2).c_str()) * MINUTE * IN_MILLISECONDS;
	AntiCheatType = GetValue(ST_ANTICHEAT);

	uint32 bgquequeintervals = GetValue(ST_BG_QUEQUE_INTERVALS);
	bool bgquequeenable = bgquequeintervals > 0 ? true : false;
	sBattlegroundMgr->SetAnnounceData(bgquequeenable, bgquequeintervals);
}

std::string Switch::GetParam(SwithTypes type)
{
	for (std::vector<SwitchTemplate>::iterator itr = SwitchVec.begin(); itr != SwitchVec.end(); ++itr)
		if (itr->Type == type)
			return itr->param;
	
	return "";
}
			
std::string Switch::GetFlag(SwithTypes type)
{
	std::string param = GetParam(type);
	if (param.empty())
		return "";

	std::string::size_type idx = param.find("#");

	if (idx != std::string::npos)
		return sCF->SplitStr(param, "#")[0];
	else
		return param;
}

std::string Switch::GetFlagByIndex(SwithTypes type, uint32 index)
{
	std::string flag = GetFlag(type);
	if (flag.empty())
		return "";

	std::string::size_type idx = flag.find("|");

	if (idx != std::string::npos)
	{
		std::vector<std::string> vec = sCF->SplitStr(flag, "|");

		if (index > vec.size())
			return vec[0];
		else
			return vec[index - 1];
	}		
	else
		return flag;
}

int32 Switch::GetValueByIndex(SwithTypes type, uint32 index)
{
	return atoi(GetFlagByIndex(type, index).c_str());
}

bool Switch::GetOnOff(SwithTypes type)
{
	std::string param = GetParam(type);

	if (param.empty())
		return false;

	std::string::size_type idx = param.find("#");

	if (idx != std::string::npos)
		return atoi(sCF->SplitStr(param, "#")[1].c_str()) > 0;
	else
		return atoi(param.c_str()) > 0;
}

int32 Switch::GetValue(SwithTypes type)
{
	std::string param = GetParam(type);

	if (param.empty())
		return 0;

	std::string::size_type idx = param.find("#");

	if (idx != std::string::npos)
		return atoi(sCF->SplitStr(param, "#")[1].c_str());
	else
		return atoi(param.c_str());
}

void Switch::GetVec(SwithTypes type, std::vector<int32> &vec)
{
	std::string args = GetParam(type);
	char* s = strtok(const_cast<char*>(args.c_str()), "#");

	Tokenizer data(s, '|');
	for (Tokenizer::const_iterator itr = data.begin(); itr != data.end(); ++itr)
			vec.push_back(atoi(*itr));
}

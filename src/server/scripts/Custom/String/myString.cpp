#pragma execution_character_set("utf-8")
#include "myString.h"

void CoreString::Load()
{
	StringVec.clear();
	QueryResult result = WorldDatabase.PQuery(sWorld->getBoolConfig(CONFIG_ZHCN_DB) ?
		"SELECT ID,文本 FROM __提示文本" :
		"SELECT ID,TEXT FROM _string");
	if (!result) return;
	do
	{
		Field* fields = result->Fetch();
		StringTemplate Temp;
		Temp.types = fields[0].GetUInt32();
		Temp.text = fields[1].GetString();
		StringVec.push_back(Temp);
	} while (result->NextRow());
}

const char* CoreString::Format(const char *format, ...)
{
	va_list ap;
	char str[2048];
	va_start(ap, format);
	vsnprintf(str, 2048, format, ap);
	va_end(ap);
	const char* c_str = str;
	return c_str;
}

const char* CoreString::GetText(CORE_STR_TYPES type)
{
	uint32 len = StringVec.size();
	
	for (size_t i = 0; i < len; i++)
		if (type == StringVec[i].types)
			return StringVec[i].text.c_str();

	return "";
}


void CoreString::Replace(std::string &s1, const std::string &s2, const std::string &s3)
{
	std::string::size_type pos = 0;
	std::string::size_type a = s2.size();
	std::string::size_type b = s3.size();
	while ((pos = s1.find(s2, pos)) != std::string::npos)
	{
		s1.replace(pos, a, s3);
		pos += b;
	}
}

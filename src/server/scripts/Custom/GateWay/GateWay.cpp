#include "GateWay.h"
#include <Winsock2.h>
#include "AccountMgr.h"
#include "../CommonFunc/CommonFunc.h"

std::string string_To_UTF8(const std::string & str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
//////////////////////////////////////////////////////////////////////////
std::string UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//一定要加1，不然会出现尾巴
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}
//////////////////////////////////////////////////////////////////////////

std::string GetStrByIndex(uint32 index,std::string buff)
{
	std::string::size_type idx = buff.find("|");

	if (idx != std::string::npos)
	{
		std::vector<std::string> vec = sCF->SplitStr(buff, "|");

		if (index > vec.size())
			return vec[0];
		else
			return vec[index - 1];
	}
	else
		return buff;
}

char* CreateAccount(std::string accountName, std::string password, std::string macAdress)
{
	if (sGateWay->IsMacBaned(macAdress))
		return "封机器码";

	AccountOpResult result = AccountMgr::CreateAccount(accountName, password, macAdress);

	if (result == AOR_OK)
	{
		PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_PWD);
		stmt->setString(0, password);
		stmt->setString(1, accountName);
		LoginDatabase.Execute(stmt);

		return "注册成功";
	}
		

	return "账号存在";
}

char* ModPassword(std::string accountName, std::string oldPassword, std::string newPassword, std::string macAdress)
{
	if (sGateWay->IsMacBaned(macAdress))
		return "封机器码";

	uint32 accountId = AccountMgr::GetId(accountName);

	if (!accountId)
		return "账号不存在";

	if (!AccountMgr::CheckPassword(accountId, oldPassword))
		return "原密码不正确";

	AccountOpResult result = AccountMgr::ChangePassword(accountId, newPassword);
	if (result == AOR_OK)
		return "改密成功";

	return "改密失败";
}

char* ResetChar(std::string accountName, std::string password, std::string charName, std::string macAdress)
{
	if (sGateWay->IsMacBaned(macAdress))
		return "封机器码";

	uint32 accountId = AccountMgr::GetId(accountName);

	if (!accountId)
		return "账号不存在";

	if (!AccountMgr::CheckPassword(accountId, password))
		return "密码错误";

	uint32 guid = sWorld->GetGlobalPlayerGUID(string_To_UTF8(charName));
	
	if (!guid)
		return "角色不存在";

	if (guid)
	{

		Player* player = ObjectAccessor::FindPlayerInOrOutOfWorld(MAKE_NEW_GUID(guid, 0, HIGHGUID_PLAYER));

		if (player && player->GetSession())
			player->TeleportTo(571, 5804.15f, 624.771f, 647.767f, 0);
		else
			CharacterDatabase.DirectPExecute("UPDATE characters SET position_x = 5804.15,position_y = 624.771,position_z = 647.767,map = 571 WHERE guid = %d", guid);
		return "解卡成功";
	}

	return "解卡失败";
}

DWORD WINAPI GateWayRun(PVOID pvParam)	
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(1, 1);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return 0;
	}

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1) {
		WSACleanup();
		return 0;
	}
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(6000);

	bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

	listen(sockSrv, SOMAXCONN);

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);
	while (1)
	{
		SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
		
		char recvBuf[256];
		recv(sockConn, recvBuf, 256, 0);

		std::string buff = recvBuf;

		char *sendBuff;

		std::string opcode = GetStrByIndex(1, buff);

		if (std::strcmp(opcode.c_str(), "opcode_check") == 0)
		{
			std::string macAdress = GetStrByIndex(2, buff);
			if (sGateWay->IsMacBaned(macAdress))
				sendBuff = "封机器码";
			else
				sendBuff = "验证通过";
		}else if (std::strcmp(opcode.c_str(), "opcode_acc") == 0)
		{
			std::string accountName = GetStrByIndex(2, buff);
			std::string password = GetStrByIndex(3, buff);
			std::string macAdress = GetStrByIndex(4, buff);
			sendBuff = CreateAccount(accountName, password, macAdress);
		}
		else if (std::strcmp(opcode.c_str(), "opcode_pwd") == 0)
		{
			std::string accountName = GetStrByIndex(2, buff);
			std::string oldPassword = GetStrByIndex(3, buff);
			std::string newPassword = GetStrByIndex(4, buff);
			std::string macAdress = GetStrByIndex(5, buff);
			sendBuff = ModPassword(accountName, oldPassword, newPassword, macAdress);
		}
		else if (std::strcmp(opcode.c_str(), "opcode_char") == 0)
		{
			std::string accountName = GetStrByIndex(2, buff);
			std::string password = GetStrByIndex(3, buff);
			std::string charName = GetStrByIndex(4, buff);
			std::string macAdress = GetStrByIndex(5, buff);
			sendBuff = ResetChar(accountName, password, charName, macAdress);
		}
	
		send(sockConn, sendBuff, strlen(sendBuff) + 1, 0);
		closesocket(sockConn);
	}

	return 0;
}


std::vector<std::string/*ban mac*/> MacBanedVec;
void GateWay::Load()
{
	MacBanedVec.clear();
	QueryResult result = LoginDatabase.PQuery("SELECT pcIDCode FROM pc_ban");
	if (!result) 
		return;
	do
	{
		Field* fields = result->Fetch();
		std::vector<std::string> vec = sCF->SplitStr(fields[0].GetString(), "#");

		for (size_t i = 0; i < vec.size(); i++)
		{
			MacBanedVec.push_back(vec[i]);
		}
	
	} while (result->NextRow());
}

bool GateWay::IsMacBaned(std::string macAdress)
{
	for (size_t i = 0; i < MacBanedVec.size(); i++)
	{
		if (macAdress.find(MacBanedVec[i]) != std::string::npos)
			return true;
	}

	return false;
}

void GateWay::BanPc(std::string charName)
{
	QueryResult result = CharacterDatabase.PQuery("SELECT account FROM characters where name = '%s'", charName);

	if (!result)
		return;

	uint32 account = result->Fetch()[0].GetUInt32();

	QueryResult result1 = LoginDatabase.PQuery("SELECT pcIDCode FROM account where id = '%d'", account);

	std::string pcIDCode = result1->Fetch()[0].GetString();

	//LoginDatabase.DirectPExecute("INSERT INTO pc_ban VALUES ('%u', '%s')", account, pcIDCode.c_str());


	PreparedStatement *stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_PCCODE);
	stmt->setUInt32(0, account);
	stmt->setString(1, pcIDCode);
	LoginDatabase.Query(stmt);

	Load();
}

void GateWay::UnBanPc(std::string charName)
{
	QueryResult result = CharacterDatabase.PQuery("SELECT account FROM characters where name = '%s'", charName);

	if (!result)
		return;

	uint32 account = result->Fetch()[0].GetUInt32();

	QueryResult result1 = LoginDatabase.PQuery("SELECT pcIDCode FROM account where id = '%d'", account);

	std::string pcIDCode = result1->Fetch()[0].GetString();

	//LoginDatabase.DirectPExecute("DELETE FROM pc_ban WHERE pcIDCode = '%s'", pcIDCode.c_str());


	PreparedStatement *stmt = LoginDatabase.GetPreparedStatement(LOGIN_DEL_PCCODE);
	stmt->setString(0, pcIDCode);
	LoginDatabase.Query(stmt);

	Load();
}

class GateWayScript : public WorldScript
{
public:
	GateWayScript() : WorldScript("GateWayScript") {}
	void OnStartup() override
	{
		HANDLE hThread1 = CreateThread(NULL, 0, GateWayRun, NULL, 0, NULL);
	}
};

void AddSC_GateWayScript()
{
	//new GateWayScript();
}

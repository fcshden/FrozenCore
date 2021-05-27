#pragma execution_character_set("utf-8")
#include "LoginCode.h"
#include <cstring>
#include "openssl/hmac.h"
#include "openssl/evp.h"
int base32_decode(const char* encoded, char* result, int bufSize)
{
    // Base32 implementation
    // Copyright 2010 Google Inc.
    // Author: Markus Gutschke
    // Licensed under the Apache License, Version 2.0
    int buffer = 0;
    int bitsLeft = 0;
    int count = 0;
    for (const char* ptr = encoded; count < bufSize && *ptr; ++ptr)
    {
        char ch = *ptr;
        if (ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n' || ch == '-')
            continue;
        buffer <<= 5;
        // Deal with commonly mistyped characters
        if (ch == '0')
            ch = 'O';
        else if (ch == '1')
            ch = 'L';
        else if (ch == '8')
            ch = 'B';
        // Look up one base32 digit
        if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z'))
            ch = (ch & 0x1F) - 1;
        else if (ch >= '2' && ch <= '7')
            ch -= '2' - 26;
        else
            return -1;
        buffer |= ch;
        bitsLeft += 5;
        if (bitsLeft >= 8)
        {
            result[count++] = buffer >> (bitsLeft - 8);
            bitsLeft -= 8;
        }
    }
    if (count < bufSize)
        result[count] = '\000';
    return count;
}

#define HMAC_RES_SIZE 20

int GenerateToken(const char* b32key)
{
    size_t keySize = strlen(b32key);
    int bufsize = (keySize + 7) / 8 * 5;
    char* encoded = new char[bufsize];
    memset(encoded, 0, bufsize);
    unsigned int hmacResSize = HMAC_RES_SIZE;
    unsigned char hmacRes[HMAC_RES_SIZE];
    unsigned long timestamp = time(nullptr) / 30;
    unsigned char challenge[8];
    for (int i = 8; i--; timestamp >>= 8)
        challenge[i] = timestamp;
    base32_decode(b32key, encoded, bufsize);
    HMAC(EVP_sha1(), encoded, bufsize, challenge, 8, hmacRes, &hmacResSize);
    unsigned int offset = hmacRes[19] & 0xF;
    unsigned int truncHash = (hmacRes[offset] << 24) | (hmacRes[offset + 1] << 16) | (hmacRes[offset + 2] << 8) | (hmacRes[offset + 3]);
    truncHash &= 0x7FFFFFFF;
    delete[] encoded;
    return truncHash % 1000000;
}

#define LETTER_SIZE 24

const std::string letter[LETTER_SIZE] = { "A", "B", "C", "D", "E", "F", "G", "H", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

void LoginCode::GenerateCode(Player* player)
{
	std::string code = "";

	for (size_t i = 0; i < 16; i++)
		code += letter[urand(0, LETTER_SIZE - 1)];

	player->temp_token_key = code;
}

void LoginCode::AddGossip(Player* player, Object* obj)
{
	player->PlayerTalkClass->ClearMenus();

	if (player->token_key.empty())
		player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "使用动态口令登录可以让账号更加安全\n\n操作步骤\n1.下载谷歌令牌APP\n2.点击产生一个16位的密钥\n3.打开APP选择输入提供的密钥，将账号和密钥输入，将密钥与APP绑定\n\n「点击产生密钥」", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF);
	else
		player->ADD_GOSSIP_ITEM_EXTENDED(GOSSIP_ICON_CHAT, "解除动态口令登录", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF, "", 0, true);
	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
}

void LoginCode::Action(Player*player, uint32 action, Object*obj)
{
	player->PlayerTalkClass->ClearMenus();

	switch (action)
	{
	case GOSSIP_ACTION_INFO_DEF:
		GenerateCode(player);
		player->ADD_GOSSIP_ITEM_EXTENDED(0, "将下列密钥绑定在谷歌令牌APP\n\n|cFFFF0000「|r" + player->temp_token_key + "|cFFFF0000」|r\n\n「点击添加动态口令登录」", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1, "\n点击确认后服务器将保存你的密钥\n\n|cFFFF0000「|r" + player->temp_token_key + "|cFFFF0000」|r\n\n请确认你已经下载谷歌令牌APP且已经绑定该密钥\n", 0, false);
		player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, obj->GetGUID());
		break;
	case GOSSIP_ACTION_INFO_DEF + 1:	
		player->token_key = player->temp_token_key;
		LoginDatabase.DirectPExecute("UPDATE account SET token_key = '%s' WHERE id = '%u'", player->token_key.c_str(), player->GetSession()->GetAccountId());
		ChatHandler(player->GetSession()).PSendSysMessage("|cFFFF0000「|r%s|cFFFF0000」|r已经存储，请及时绑定谷歌令牌APP", player->token_key.c_str());
		player->CLOSE_GOSSIP_MENU();
		break;
	}
}


class LoginCodePlayerScript : PlayerScript
{
public:
	LoginCodePlayerScript() : PlayerScript("LoginCodePlayerScript") {}

	void OnLogin(Player* player)
	{
		QueryResult result = LoginDatabase.PQuery("SELECT token_key FROM account WHERE id = '%u'", player->GetSession()->GetAccountId());

		if (!result) 
			return;

		player->token_key = result->Fetch()[0].GetString();
	}
};

class LoginCodeCreatureScript : public CreatureScript
{
public:
	LoginCodeCreatureScript() : CreatureScript("LoginCodeCreatureScript") { }


	bool OnGossipHello(Player* player, Creature* creature) override
	{
		sLoginCode->AddGossip(player, creature);
		return true;
	}

	bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 action) override
	{
		sLoginCode->Action(player, action, creature);
		return true;
	}

	bool OnGossipSelectCode(Player* player, Creature* creature, uint32 sender, uint32 action, const char* code) override
	{
		if (!*code)
			return false;

		int inputToken = atoi(code);
		int validToken = GenerateToken(player->token_key.c_str());

		if (validToken != inputToken)
		{
			player->GetSession()->SendNotification("你输入的动态口令密码无法通过验证！");
			player->CLOSE_GOSSIP_MENU();
			return false;
		}

		LoginDatabase.DirectPExecute("UPDATE account SET token_key = '' WHERE id = '%u'", player->GetSession()->GetAccountId());
		player->token_key = "";
		ChatHandler(player->GetSession()).PSendSysMessage("已经解除动态口令登录！");
		player->CLOSE_GOSSIP_MENU();
		return true;
	}
};

void AddSC_LoginCodeScript()
{
	new LoginCodePlayerScript();
	new LoginCodeCreatureScript();
}

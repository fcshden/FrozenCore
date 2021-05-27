#include <iostream>
#include <cstring>
#include "RC4.h"
#include "Base64.h"
#include "RC4_Base64.h"

using namespace std;

char key[128] = {"C2D11B476BCADAA9B9C422D99A912265"};

void Rc4Base64EnCode(std::string key, std::string data, std::string &EnCodeData)
{
    struct rc4_state *s;
    s = (struct rc4_state *)malloc(sizeof(struct rc4_state));

    rc4_setup(s, (unsigned char *)key.c_str(), key.length());
    rc4_crypt(s, (unsigned char *)data.c_str(), data.length());

    std::string strMyPwd = data;
    EnCodeData = base64_encode((unsigned char const *)strMyPwd.c_str(), strMyPwd.length());
    return;
}

void Rc4Base64DeCode(std::string key, std::string data, std::string &DeCodeData)
{
    struct rc4_state *s;
    s = (struct rc4_state *)malloc(sizeof(struct rc4_state));
    std::string Strbase64DeCode = base64_decode(data);
    char Chardata[512];
    strcpy(Chardata, Strbase64DeCode.data());

    rc4_setup(s, (unsigned char *)key.c_str(), key.length());
    rc4_crypt(s, (unsigned char *)Chardata, strlen(Chardata));

    DeCodeData = Chardata;
    return;
}

string EncryptionRC4(const string &strSrc)
{
    std::string str;
    Rc4Base64EnCode(key, strSrc, str);
    return str;
}

string DecryptionRC4(const string &strSrc)
{
    std::string str;
    Rc4Base64DeCode(key, strSrc, str);
    return str;
}
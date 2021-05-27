#pragma once

void Rc4Base64EnCode(std::string key, std::string data, std::string &EnCodeData);
void Rc4Base64DeCode(std::string key, std::string data, std::string &DeCodeData);
std::string EncryptionRC4(const std::string &strSrc);
std::string DecryptionRC4(const std::string &strSrc);
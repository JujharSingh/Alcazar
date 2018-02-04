#pragma once
// this is just to clean up code i guess?
extern "C" {
	#include "scrypt/crypto_scrypt-hexconvert.h"
	#include "scrypt\libscrypt.h"
}
std::string scrypt_hex(std::string password, std::string salt, int N, int r, int p) {
	char buf[SCRYPT_HASH_LEN];
	char hex_buf[SCRYPT_HASH_LEN * 2 + 1];
	libscrypt_scrypt((uint8_t*)password.c_str(), password.length(), (uint8_t*)salt.c_str(), salt.length(), N, r, p, (uint8_t*)buf, sizeof(buf));
	libscrypt_hexconvert((uint8_t*)buf, sizeof(buf), hex_buf, sizeof(hex_buf));
	return std::string(hex_buf);
}
std::string scrypt(std::string password, std::string salt, int N, int r, int p) {
	char buf[SCRYPT_HASH_LEN];
	libscrypt_scrypt((uint8_t*)password.c_str(), password.length(), (uint8_t*)salt.c_str(), salt.length(), N, r, p, (uint8_t*)buf, sizeof(buf));
	return std::string(buf);
}
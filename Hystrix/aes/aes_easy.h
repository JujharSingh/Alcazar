#pragma once
#include "aes/aes256.hpp"
ByteArray stov(std::string str) {
	ByteArray vec;
	for (char x : str) {
		vec.push_back(x);
	}
	return vec;
}
std::string aes256(std::string str, std::string key) {
	ByteArray buf;
	Aes256::encrypt(stov(key), stov(str), buf);
	return std::string(buf.data());
}
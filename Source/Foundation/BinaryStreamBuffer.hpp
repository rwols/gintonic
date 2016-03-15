#pragma once

#include <iostream>
#include <vector>
#include <arpa/inet.h>

namespace gintonic {

void serializeUInt8(std::ostream& os, const uint8_t value)
{
	os.write((const char*)&value, sizeof(uint8_t));
}

uint8_t deserializeUInt8(std::istream& is)
{
	uint8_t value;
	is.read((char*)&value, sizeof(uint8_t));
	return value;
}

void serializeInt8(std::ostream& os, const int8_t value)
{
	os.write((const char*)&value, sizeof(int8_t));
}

int8_t deserializeInt8(std::istream& is)
{
	int8_t value;
	is.read((char*)&value, sizeof(int8_t));
	return value;
}

void serializeUInt16(std::ostream& os, uint16_t value)
{
	value = htons(value);
	os.write((const char*)&value, sizeof(int16_t));
}

uint16_t deserializeUInt16(std::istream& is)
{
	uint16_t value;
	is.read((char*)&value, sizeof(uint16_t));
	return ntohs(value);
}

void serializeInt16(std::ostream& os, int16_t value)
{
	value = htons(value);
	os.write((const char*)&value, sizeof(int16_t));
}

int16_t deserializeInt16(std::istream& is)
{
	int16_t value;
	is.read((char*)&value, sizeof(int16_t));
	return ntohs(value);
}

void serializeUInt32(std::ostream& os, uint32_t value)
{
	value = htonl(value);
	os.write((const char*)&value, sizeof(uint32_t));
}

uint32_t deserializeUInt32(std::istream& is)
{
	uint32_t value;
	is.read((char*)&value, sizeof(uint32_t));
	return ntohl(value);
}

void serializeInt32(std::ostream& os, int32_t value)
{
	value = htonl(value);
	os.write((const char*)&value, sizeof(int32_t));
}

int32_t deserializeInt32(std::istream& is)
{
	int32_t value;
	is.read((char*)&value, sizeof(int32_t));
	return ntohl(value);
}

void serializeFloat(std::ostream& os, float value)
{
	static_assert(sizeof(uint32_t) == sizeof(float), 
		"This code only works when a float is 32 bits.");

	serializeUInt32(os, *reinterpret_cast<uint32_t*>(&value));
}

float deserializeFloat(std::istream& is)
{
	static_assert(sizeof(uint32_t) == sizeof(float), 
		"This code only works when a float is 32 bits.");

	uint32_t value = deserializeUInt32(is);
	return *reinterpret_cast<float*>(&value);
}

void serializeChar(std::ostream& os, const char value)
{
	static_assert(sizeof(char) == sizeof(int8_t), 
		"This code only works when a char is one byte.");

	serializeInt8(os, static_cast<int8_t>(value));
}

char deserializeChar(std::istream& is)
{
	static_assert(sizeof(char) == sizeof(int8_t), 
		"This code only works when a char is one byte.");

	return static_cast<char>(deserializeInt8(is));
}

void serializeUChar(std::ostream& os, const unsigned char value)
{
	static_assert(sizeof(unsigned char) == sizeof(uint8_t), 
		"This code only works when an unsigned char is one byte.");

	serializeUInt8(os, static_cast<uint8_t>(value));
}

unsigned char deserializeUChar(std::istream& is)
{
	static_assert(sizeof(unsigned char) == sizeof(uint8_t), 
		"This code only works when an unsigned char is one byte.");

	return static_cast<unsigned char>(deserializeUInt8(is));
}

void serializeShort(std::ostream& os, const short value)
{
	static_assert(sizeof(short) == sizeof(uint16_t),
		"This code only works when a short is two bytes.");

	serializeInt16(os, static_cast<int16_t>(value));
}

short deserializeShort(std::istream& is)
{
	static_assert(sizeof(unsigned short) == sizeof(int16_t),
		"This code only works when an unsigned short is two bytes.");

	return static_cast<unsigned short>(deserializeUInt16(is));
}

void serializeInt(std::ostream& os, const int value)
{
	static_assert(sizeof(int) == sizeof(int32_t),
		"This code only works when an int is four bytes.");

	serializeInt32(os, static_cast<int32_t>(value));
}

int deserializeInt(std::istream& is)
{
	static_assert(sizeof(int) == sizeof(int32_t),
		"This code only works when an int is four bytes.");

	return static_cast<int>(deserializeInt32(is));
}

void serializeUInt(std::ostream& os, const unsigned int value)
{
	static_assert(sizeof(unsigned int) == sizeof(uint32_t),
		"This code only works when an unsigned int is four bytes.");

	serializeUInt32(os, static_cast<uint32_t>(value));
}

unsigned int deserializeUInt(std::istream& is)
{
	static_assert(sizeof(unsigned int) == sizeof(uint32_t),
		"This code only works when an unsigned int is four bytes.");

	return static_cast<unsigned int>(deserializeUInt32(is));
}

template <class ElementType>
void serializeStdVector(std::ostream& os, const std::vector<ElementType>& container)
{
	const unsigned int size = static_cast<unsigned int>(container.size());
	serializeUInt(os, size);
	for (const auto& element : container)
	{
		element.serialize(os);
	}
}

template <class ElementType>
std::vector<ElementType> deserializeStdVector(std::istream& is)
{
	std::vector<ElementType> result;
	const unsigned int size = deserializeUInt(is);
	for (unsigned int i = 0; i < size; ++i) result.emplace_back(ElementType(is));
	return result;
}

} // namespace gintonic

#pragma once

#include <vector>
#include "emath/vector.h"
#include "emath/quaternion.h"
#include "../utilities/macro.h"

class ENIGMA_API Frame
{
public:
	uint8_t* GetData();
	uint8_t* GetFirstData();
	size_t GetSize() const;
	void LoadFromPacket(uint8_t* packet, size_t packetSize);

	void WriteUInt8_t(uint8_t data);
	void WriteInt(int data);
	void WriteFloat(float data);
	void WriteVector3(float x, float y, float z);
	void WriteQuaternion(float x, float y, float z, float w);

	uint8_t ReadUInt8_t();
	int ReadInt();
	float ReadFloat();
	Math::Vector3D ReadVector3();
	Math::Quaternion ReadQuaternion();
	bool IsTraveled();
	bool IsEmpty();


private:
	std::vector<uint8_t> buffer;
	uint8_t cursor = 0;
};
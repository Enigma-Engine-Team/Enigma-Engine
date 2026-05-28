#include "network/frame.h"

uint8_t* Frame::GetData()
{
	return buffer.data();
}

uint8_t* Frame::GetFirstData()
{
	return &buffer[cursor];
}

size_t Frame::GetSize() const
{
	return buffer.size();
}

void Frame::LoadFromPacket(uint8_t* packet, size_t packetSize)
{
	buffer.insert(buffer.end(), packet, packet + packetSize);
}

void Frame::WriteUInt8_t(uint8_t data)
{
	buffer.push_back(data);
}

void Frame::WriteInt(int data)
{
	auto* start = (uint8_t*)&data;
	buffer.insert(buffer.end(), start, start + 4);
}

void Frame::WriteFloat(float data)
{
	auto* start = (uint8_t*)&data;
	buffer.insert(buffer.end(), start, start + 4);
}

void Frame::WriteVector3(float x, float y, float z)
{
	WriteFloat(x);
	WriteFloat(y);
	WriteFloat(z);
}

void Frame::WriteQuaternion(float x, float y, float z, float w)
{
	WriteFloat(x);
	WriteFloat(y);
	WriteFloat(z);
	WriteFloat(w);
}

uint8_t Frame::ReadUInt8_t()
{
	uint8_t data = buffer[cursor];
	++cursor;
	return data;
}

bool Frame::IsTraveled()
{
	return cursor >= buffer.size();
}

int Frame::ReadInt()
{
	int i = 0;
	memcpy(&i, GetFirstData(), 4);
	cursor += 4;
	return i;
}

float Frame::ReadFloat()
{
	float f = 0.f;
	memcpy(&f, GetFirstData(), 4);
	cursor += 4;
	return f;
}

Math::Vector3D Frame::ReadVector3()
{
	float vec3[3] = { 0 };
	memcpy(vec3, GetFirstData(), 12);
	cursor += 12;
	return Math::Vector3D(vec3[0], vec3[1], vec3[2]);
}

Math::Quaternion Frame::ReadQuaternion()
{
	float quat[4] = { 0 };
	memcpy(quat, GetFirstData(), 16);
	cursor += 16;
	return Math::Quaternion(quat[0], quat[1], quat[2], quat[3]);
}

bool Frame::IsEmpty()
{
	return buffer.empty();
}
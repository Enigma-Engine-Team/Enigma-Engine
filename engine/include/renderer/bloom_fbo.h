#pragma once

#include "emath/emath.h"

struct BloomMip
{
	Math::Vector2D size;
	int intSizeX;
	int intSizeY;
	unsigned int texture;
};

class BloomFBO
{
public:
	BloomFBO();
	~BloomFBO();
	bool Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength);
	void Destroy();
	void BindForWriting();
	const std::vector<BloomMip>& MipChain() const;

private:

	bool init;
	unsigned int mFBO;
	std::vector<BloomMip> mipChain;
};
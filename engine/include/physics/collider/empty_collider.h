#pragma once

#include "collider.h"
#include "../../utilities/macro.h"

class ENIGMA_API EmptyCollider : public Collider
{
public:
	EmptyCollider();
	void Create() override;
};
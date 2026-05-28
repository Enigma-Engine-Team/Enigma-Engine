#pragma once

#include "emath/emath.h"
#include <iostream>

struct Color 
{
	static const Math::Vector4D WHITE;
	static const Math::Vector4D BLACK;
	static const Math::Vector4D RED;
	static const Math::Vector4D GREEN;
	static const Math::Vector4D BLUE;
	static const Math::Vector4D YELLOW;
	static const Math::Vector4D CYAN;
	static const Math::Vector4D MAGENTA;
	static const Math::Vector4D GRAY;
	static const Math::Vector4D ORANGE;
	static const Math::Vector4D PURPLE;
	static const Math::Vector4D BROWN;
};

const Math::Vector4D Color::WHITE = Math::Vector4D(1.f, 1.f, 1.f, 1.f);
const Math::Vector4D Color::BLACK = Math::Vector4D(0.f, 0.f, 0.f, 1.f);
const Math::Vector4D Color::RED = Math::Vector4D(1.f, 0.f, 0.f, 1.f);
const Math::Vector4D Color::GREEN = Math::Vector4D(0.f, 1.f, 0.f, 1.f);
const Math::Vector4D Color::BLUE = Math::Vector4D(0.f, 0.f, 1.f, 1.f);
const Math::Vector4D Color::YELLOW = Math::Vector4D(1.f, 1.f, 0.f, 1.f);
const Math::Vector4D Color::CYAN = Math::Vector4D(0.f, 1.f, 1.f, 1.f);
const Math::Vector4D Color::MAGENTA = Math::Vector4D(1.f, 0.f, 1.f, 1.f);
const Math::Vector4D Color::GRAY = Math::Vector4D(0.5f, 0.5f, 0.5f, 1.f);
const Math::Vector4D Color::ORANGE = Math::Vector4D(1.f, 0.5f, 0.f, 1.f);
const Math::Vector4D Color::PURPLE = Math::Vector4D(0.5f, 0.f, 0.5f, 1.f);
const Math::Vector4D Color::BROWN = Math::Vector4D(0.6f, 0.3f, 0.f, 1.f);
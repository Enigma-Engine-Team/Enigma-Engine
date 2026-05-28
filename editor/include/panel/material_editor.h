#pragma once

#include "context/panel.h"

namespace UI
{
	class MaterialEditor : public IUIPanel
	{
	public:

		MaterialEditor();

		void Draw() override;

	private:

		void DrawTextureSlot(bool& useTexture, Texture& textureObj, unsigned int placeholderImg, const char* payloadType);
	};
}


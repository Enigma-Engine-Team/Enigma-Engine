#include "IVertexInput.h"

namespace EnigmaRHI
{
	class GLVertexInput : public IVertexInput
	{
	public:

		GLVertexInput() = default;

		void Create() override;
		void Destroy() override;
		void AddVertexAttribute(unsigned int index, int size, EDataType type, bool normalized, unsigned int offset) override;
		void Bind() const override;
		void Unbind() const override;

		GLVertexInput& API_GL() override { return (*this); }

	public:

		unsigned int GetVAO() const { return vaoID; }

	private :

		unsigned int vaoID;
	};
}
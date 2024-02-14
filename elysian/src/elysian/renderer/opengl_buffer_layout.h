#pragma once

#include <GL/glew.h>

namespace ely
{
	struct BufferElement
	{
		uint32_t type;
		uint32_t count;
		bool normalised;

		uint32_t NumBytes() const
		{
			return count * getSizeOfType(type);
		}

		static uint32_t getSizeOfType(uint32_t type)
		{
			switch (type)
			{
			case GL_FLOAT:			 return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE: return 1;
			}
			return 0;
		}
	};

	class BufferLayout
	{

	public:
		BufferLayout() :
			m_stride(0)
		{
		}
		uint32_t getStride() const { return m_stride; }
		const std::vector<BufferElement>& getElements() const { return m_elements; }

		template<typename T>
		void push(uint32_t count)
		{
			static_assert(false);
		}

		template<>
		void push<float>(uint32_t count)
		{
			m_elements.push_back({ GL_FLOAT, count, false });
			m_stride += count * BufferElement::getSizeOfType(GL_FLOAT);
		}

		template<>
		void push<uint32_t>(uint32_t count)
		{
			m_elements.push_back({ GL_UNSIGNED_INT, count, false });
			m_stride += count * BufferElement::getSizeOfType(GL_UNSIGNED_INT);
		}

		/*template<>
		void push<uint8_t>(uint8_t count)
		{
			m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
			m_stride += count * BufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
		}*/

	private:
		std::vector<BufferElement> m_elements;
		unsigned int m_stride;

	};

}
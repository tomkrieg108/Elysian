#pragma once

#include "elysian/renderer/opengl_shader.h"
#include "elysian/renderer/opengl_shader_utils.h"

namespace ely
{
	/*
	TODO - better to call these AttributeElement, AttributeLayout?
	*/

	/*
TODO use consistent naming for GLSL shader names (attributes, uniforms, the name of the shader itself) and
buffer layouts, materials etc
*/
	
	struct BufferElement
	{
		BufferElement(const std::string& attribute_name, ShaderDataType data_type, bool normalized = false) :
			attribute_name{ attribute_name }, data_type{ data_type }, normalized{ normalized }
		{
			size = ShaderUtils::ShaderDataTypeSize(data_type);
		}

		std::string attribute_name{ "" };
		ShaderDataType data_type{ ShaderDataType ::None};
		uint32_t size{ 0 };	//bytes
		bool normalized{ false };
		size_t offset{ 0 };
	};

	class BufferLayout
	{
		
	public:

		//constexpr uint32_t MaxElements = 32;

		BufferLayout() = default;
		~BufferLayout() = default;

		BufferLayout(std::initializer_list<BufferElement> element_list) :
			m_element_list{ element_list }
		{
			for (BufferElement& element : m_element_list)
				UpdateStrideAndOffset(element);
		}

		BufferLayout& Add(BufferElement&& element)
		{
			m_element_list.push_back(element);
			UpdateStrideAndOffset(element);
			return *this;
		}

		uint32_t GetStride() const { return m_stride; }

		const std::vector<BufferElement>& GetElements() const { return m_element_list; }

		void Reset() { m_element_list.clear(); }

	private:
		//TODO - this could probably be std::array
		std::vector<BufferElement> m_element_list;
		uint32_t m_stride = 0;

	private:
		void UpdateStrideAndOffset(BufferElement& element)
		{
			element.offset = m_stride;
			m_stride += element.size;
		}
	};

}
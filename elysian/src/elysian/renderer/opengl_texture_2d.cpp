#include "pch.h"
#include "elysian/kernal/base.h"
#include "opengl_texture_2d.h"
#include "elysian/kernal/log.h"
#include <stb_image/stb_image.h>

namespace ely
{
	std::string const OpenGLTexture2D::s_texture_path = std::string{ "../../assets/textures/" };
	std::array<uint32_t, 32> OpenGLTexture2D::m_slots = { 0 };

	OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height, const Params& params) :
		m_width{width}, m_height{height}, m_params {params}
	{
		m_internal_formal = GL_RGBA8;
		m_data_format = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_id); //ogl 4.5
		glTextureStorage2D(m_id, 1, m_internal_formal, m_width, m_height); //ogl 4.5

		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, params.MIN_FILTER);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, params.MAG_FILTER);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, params.WRAP_S);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, params.WRAP_T);
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& filename, const Params& params) :
		m_filepath{ s_texture_path + filename}, m_params{params}
	{
		m_is_loaded = false;
		if(params.flip_img_vertically)
			stbi_set_flip_vertically_on_load(1);

		int32_t width = 0, height = 0, channels = 0;
		//Anton pg180 - can set last param to 4 to force RGBA format
		uint8_t* data = stbi_load(m_filepath.c_str(), &width, &height, &channels, 0);
		
		if (!data)
		{
			CORE_ERROR("Failed to load texture: {} ", m_filepath);
			return;
		}

		//Anton pg 180
		/*if ((width & (width - 1)) != 0)
			CORE_WARN("Texture: {} width of {} not power-of-2 ", m_filepath, width);
		if ((height & (height - 1)) != 0)
			CORE_WARN("Texture: {} height of {} not power-of-2 ", m_filepath, height);*/

		m_width = width;
		m_height = height;
		
		if (channels == 4)
		{
			m_internal_formal = params.enable_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
			m_data_format = GL_RGBA;
		}
		else if (channels == 3)
		{
			m_internal_formal = params.enable_srgb ? GL_SRGB8 : GL_RGB8;
			m_data_format = GL_RGB;
		}
		//todo - verify formats (channel 3 or 4)
		glGenTextures(1, &m_id); //ogl 2.0+
		glBindTexture(GL_TEXTURE_2D, m_id); //ogl 2.0+
		glTexImage2D(GL_TEXTURE_2D, 0, m_internal_formal, width, height, 0, m_data_format, GL_UNSIGNED_BYTE, data);
		glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, params.MIN_FILTER);
		glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, params.MAG_FILTER);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, params.WRAP_S);
		glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, params.WRAP_T);

		if(params.enable_mipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);
		m_is_loaded = true;

		//Alternatively (V4.5)
		//glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		//glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);
	}
	

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		if (m_is_loaded)
			glDeleteTextures(1, &m_id);
	}

	void OpenGLTexture2D::SetData(void* data, uint32_t size)
	{
		uint32_t bpp = m_data_format == GL_RGBA ? 4 : 3;
		//HZ_CORE_ASSERT(size == m_width * m_height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, m_data_format, GL_UNSIGNED_BYTE, data); //V4.5
	}

	void OpenGLTexture2D::Bind(uint32_t slot)
	{
		glBindTextureUnit(slot, m_id); //V4.5
		m_slot = slot;
	}

	void OpenGLTexture2D::Bind()
	{
		if (m_slot != -1)
			return;
		bool ok = false;
		for(auto slot=0;slot<MAX_SLOTS; ++slot)
		{
			if (m_slots[slot] == 0)
			{
				glActiveTexture(GL_TEXTURE0 + slot); //V2.0
				glBindTexture(GL_TEXTURE_2D, m_id);  //V2.0
				m_slots[slot] = m_id;
				m_slot = slot;
				ok = true;
				break;
			}
		}
		if (!ok)
			CORE_ERROR("No free texture slots!");
	}

	void OpenGLTexture2D::Unbind()
	{
		if (m_slot == -1)
			return;
		for (auto slot = 0; slot < MAX_SLOTS; ++slot)
		{
			if (m_slots[slot] == m_id)
			{
				glActiveTexture(GL_TEXTURE0 + slot);
				glBindTexture(GL_TEXTURE_2D, 0);
				m_slots[slot] = 0;
				m_slot = -1;
				break;
			}
		}
	}

	//----------------------------------------------------------------------------------------------------------
	//Texture2D Repo

	std::unordered_map<std::string, Ref<OpenGLTexture2D>> Texture2DRepo::m_texture2d_repo{};
	std::string const Texture2DRepo::s_texture_asset_path = std::string{ "../../assets/textures/" };

	void Texture2DRepo::Init()
	{
		Texture2DRepo::LoadTextures();
	}

	Ref<OpenGLTexture2D> Texture2DRepo::Load(const std::string& name)
	{
		//TODO: assert that texture doesn't already exist
		m_texture2d_repo[name] = CreateRef<OpenGLTexture2D>(name);
		return m_texture2d_repo[name];
	}

	Ref<OpenGLTexture2D> Texture2DRepo::Get(const std::string& name)
	{
		//TODO: assert that texture exists
		return m_texture2d_repo[name];
	}

	bool Texture2DRepo::Exists(const std::string& name)
	{
		return m_texture2d_repo.find(name) != m_texture2d_repo.end();
	}

	void Texture2DRepo::LoadTextures()
	{
		Texture2DRepo::Load("container2.png");
		Texture2DRepo::Load("container2_specular.png");
	}

}
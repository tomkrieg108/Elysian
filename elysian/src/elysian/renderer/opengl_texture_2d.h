#pragma once
#include <glad/glad.h>

namespace ely
{
	//TODO: an-isotropy filtering (Anton page 200)
	/*
			// work out maximum an-isotropy
			GLfloat max_aniso = 0.0f;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
			// set the maximum!
			glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
			Can generate a screenshot PNG using stbi_write_png()
			 http://nothings.org/stb/stb_image_write.h
			 Anton pg 205
	*/

	class OpenGLTexture2D
	{
	public:

		struct Params
		{
			Params() = default;
			uint32_t MIN_FILTER = GL_LINEAR;
			uint32_t MAG_FILTER = GL_LINEAR;
			uint32_t WRAP_S = GL_REPEAT;
			uint32_t WRAP_T = GL_REPEAT;
			bool flip_img_vertically = true;
			bool enable_mipmaps = false;
			bool enable_srgb = false;
		};

		
	public:
		OpenGLTexture2D(uint32_t width, uint32_t height, const Params& params = Params());
		OpenGLTexture2D(const std::string& filename, const Params& params = Params());
		~OpenGLTexture2D();

		void Bind(uint32_t slot);
		void Bind();  //sets m_slot
		void Unbind();  //sets m_slot
		void SetData(void* data, uint32_t size);
		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }
		uint32_t IsLoaded() const { return m_is_loaded; }
		bool IsBound() const { return m_slot != -1; }
		uint32_t GetId() const { return m_id; }
		uint32_t GetSlot() const { return m_slot; }
		const Params& GetParams() const { return m_params; }

	private:
		Params m_params;
		uint32_t m_id = 0;
		std::string m_filepath = "";
		uint32_t m_width = 0, m_height = 0;	
		GLenum m_internal_formal = GL_RGBA8, m_data_format = GL_RGBA;
		int32_t m_slot = -1;
		bool m_is_loaded = false;

	private:
		static const std::string s_texture_path;
		enum {MAX_SLOTS = 32};
		static std::array<uint32_t, MAX_SLOTS> m_slots; //assume 32 slots for now
		
	};

}
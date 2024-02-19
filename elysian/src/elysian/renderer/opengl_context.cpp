#include "pch.h"
#include "elysian/kernal/base.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "elysian/kernal/log.h"
#include "opengl_context.h"

namespace ely
{
	void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
		GLsizei length, const char* message, const void* userParam);

	OpenGLContext::OpenGLContext(GLFWwindow* window_handle):
		m_window_handle{ window_handle }
	{
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_window_handle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!status)
		{
			CORE_ERROR("Glad initalisation failed");
			glfwDestroyWindow(m_window_handle);
			glfwTerminate();
		}

#ifdef DEBUG
		//Setup GL error callback.  Needs to come after m_context->Init() ( glewInit() )
		//https://learnopengl.com/In-Practice/Debugging
		int flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(glDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
#endif

		CORE_INFO("OpenGL Context Initialised (GLEW)");
		CORE_INFO("OpenGL Info:");
		//TODO
		//hazel just does this!  But gives me compile error.
		/*CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		CORE_INFO("  Version: {0}", glGetString(GL_VERSION));*/

		//https://stackoverflow.com/questions/1673445/how-to-convert-unsigned-char-to-stdstring-in-c
		//see also log.h in hazel
		CORE_TRACE("   Vendor: {}", std::string(reinterpret_cast<char const*>(glGetString(GL_VENDOR))));
		CORE_TRACE("   Renderer: {}", std::string(reinterpret_cast<char const*>(glGetString(GL_RENDERER))));
		CORE_TRACE("   Version: {}", std::string(reinterpret_cast<char const*>(glGetString(GL_VERSION))));

		#ifdef DEBUG
			DisplayGLParams();
			DisplayVideoModes();
		#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_window_handle);
	}


	//OpenGL parameters - 'Anton's openGL tutorials'
	GLenum params[] = { GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, GL_MAX_CUBE_MAP_TEXTURE_SIZE, GL_MAX_DRAW_BUFFERS, GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, GL_MAX_TEXTURE_IMAGE_UNITS, GL_MAX_TEXTURE_SIZE, GL_MAX_VARYING_FLOATS, GL_MAX_VERTEX_ATTRIBS, GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, GL_MAX_VERTEX_UNIFORM_COMPONENTS, GL_MAX_UNIFORM_BUFFER_BINDINGS, GL_MAX_VIEWPORT_DIMS, GL_STEREO, };

	const char* param_name[] = { "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", "GL_MAX_CUBE_MAP_TEXTURE_SIZE", "GL_MAX_DRAW_BUFFERS", "GL_MAX_FRAGMENT_UNIFORM_COMPONENTS",
	"GL_MAX_TEXTURE_IMAGE_UNITS", "GL_MAX_TEXTURE_SIZE", "GL_MAX_VARYING_FLOATS", "GL_MAX_VERTEX_ATTRIBS", "GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", "GL_MAX_VERTEX_UNIFORM_COMPONENTS", "GL_MAX_UNIFORM_BUFFER_BINDINGS", "GL_MAX_UNIFORM_BUFFERS" "GL_MAX_VIEWPORT_DIMS", "GL_STEREO" };

	
	void OpenGLContext::DisplayGLParams()
	{
		CORE_INFO("OPENGL PARAMETER VALUES");
		for (int i = 0; i <= 11; i++)
		{
			static int param_val;
			glGetIntegerv(params[i], &param_val);
			CORE_TRACE("   {} \t {}", param_name[i], param_val);
		}
		static GLboolean bool_val;
		glGetBooleanv(GL_STEREO, &bool_val);
		CORE_TRACE("   {} \t {}", param_name[11], bool_val);
	}

	void OpenGLContext::DisplayVideoModes()
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		int count;
		const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);
		//std::cout << "Video modes supported: " << count << "\n\n";
		CORE_INFO("VIDEO MODES SUPPORTED");
		for (int i = 0; i < count; i++)
		{
			CORE_TRACE("   {}: {}: {}: {}: {}: {}: {}:", i, modes[i].height, modes[i].width, modes[i].redBits, modes[i].blueBits, modes[i].greenBits, modes[i].refreshRate);
		}
	}

	//https://learnopengl.com/In-Practice/Debugging
	void APIENTRY glDebugOutput(GLenum source,
		GLenum type,
		unsigned int id,
		GLenum severity,
		GLsizei length,
		const char* message,
		const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) 
			return;

		CORE_WARN("-----------------------------------------------------");
		CORE_WARN("GL Debug message.  ID: {}.  Description: {}", id, message);

		char* source_msg = "";
		char* type_msg = "";
		char* severity_msg = "";

		switch (source)
		{
			case GL_DEBUG_SOURCE_API:             source_msg = "Source: API"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   source_msg = "Source: Window System"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: source_msg = "Source: Shader Compiler"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     source_msg = "Source: Third Party"; break;
			case GL_DEBUG_SOURCE_APPLICATION:     source_msg = "Source: Application"; break;
			case GL_DEBUG_SOURCE_OTHER:           source_msg = "Source: Other"; break;
			default:															source_msg = "Source: Unknown"; break;
		}

		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:               type_msg = "Type: Error"; break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: type_msg = "Type: Deprecated Behaviour"; break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  type_msg = "Type: Undefined Behaviour"; break;
			case GL_DEBUG_TYPE_PORTABILITY:         type_msg = "Type: Portability"; break;
			case GL_DEBUG_TYPE_PERFORMANCE:         type_msg = "Type: Performance"; break;
			case GL_DEBUG_TYPE_MARKER:              type_msg = "Type: Marker"; break;
			case GL_DEBUG_TYPE_PUSH_GROUP:          type_msg = "Type: Push Group"; break;
			case GL_DEBUG_TYPE_POP_GROUP:           type_msg = "Type: Pop Group"; break;
			case GL_DEBUG_TYPE_OTHER:               type_msg = "Type: Other"; break;
			default:																type_msg = "Type: Unknown"; break;
		}

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:					severity_msg = "Severity: high"; break;
			case GL_DEBUG_SEVERITY_MEDIUM:				severity_msg = "Severity: medium"; break;
			case GL_DEBUG_SEVERITY_LOW:						severity_msg = "Severity: low"; break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:	severity_msg = "Severity: notification"; break;
			default:															severity_msg = "Severity: Unknown"; break;
		}

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:						CORE_CRITICAL("{} {} {}", severity_msg, type_msg, source_msg); break;
			case GL_DEBUG_SEVERITY_MEDIUM:					CORE_ERROR("{} {} {}", severity_msg, type_msg, source_msg); break;
			case GL_DEBUG_SEVERITY_LOW:							CORE_WARN("{} {} {}", severity_msg, type_msg, source_msg); break; 
			case GL_DEBUG_SEVERITY_NOTIFICATION:		CORE_INFO("{} {} {}", severity_msg, type_msg, source_msg); break; 
			default:																CORE_TRACE("{} {} {}", severity_msg, type_msg, source_msg); break; 
		}
		CORE_WARN("-----------------------------------------------------");
	}

	void GLLogCall(char* file, int line)
	{
		while (GLenum error = glGetError())
			CORE_ERROR("Open GL error: {} Function: {} File: {} Line: ", error, file, line);
	}

}
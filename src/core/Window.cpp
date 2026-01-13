#include "Window.h"
#include <stdexcept>

Window::Window()
	: window(nullptr)
	, width(0)
	, height(0)
	, title()
{
}

Window::Window(int _width, int _height, const std::string& _title, bool throwOnFail)
	: window(nullptr), width(0), height(0), title()
{
	if (!Create(_width, _height, _title))
	{
		if (throwOnFail)
			throw std::runtime_error("Window::Window - failed to create window");
		else
			std::cerr << "Window::Window - failed to create window (non-throwing)\n";
	}
}

Window::~Window()
{
	Destroy();
}

bool Window::Create(int width, int height, const std::string& title)
{
	if (window)
		Destroy();

	// 🔹 stocker les valeurs
	this->width = width;
	this->height = height;
	this->title = title;

	static bool s_glfwInitialized = false;
	if (!s_glfwInitialized)
	{
		if (!glfwInit())
		{
			std::cerr << "Window::Create - failed to initialize GLFW\n";
			return false;
		}
		s_glfwInitialized = true;
	}

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Window::Create - failed to create GLFW window\n";
		return false;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Window::Create - failed to initialize GLAD\n";
		glfwDestroyWindow(window);
		window = nullptr;
		return false;
	}

	glViewport(0, 0, width, height);
	return true;
}


void Window::Destroy()
{
	if (window)
	{
		glfwDestroyWindow(window);
		window = nullptr;
	}
	// Intentionally do not call glfwTerminate() here:
	// let the application decide when to terminate GLFW.
}

bool Window::ShouldClose() const
{
	if (!window)
		return true;
	return glfwWindowShouldClose(window) != 0;
}

void Window::SwapBuffers()
{
	if (window)
		glfwSwapBuffers(window);
}

void Window::PollEvents()
{
	// Polling events is global to GLFW; safe to call even if window == nullptr
	glfwPollEvents();
}
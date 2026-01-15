#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>

class Window {
private:
	GLFWwindow* window;
	int width;
	int height;
	std::string title;

public:
	Window();
	// Constructeur pratique : si throwOnFail == true, une exception est lancée en cas d'échec
	Window(int _width, int _height, const std::string& _title, bool throwOnFail = false);
	~Window();

	bool Create(int width, int height, const std::string& title);
	void Destroy();
	bool ShouldClose() const;
	void SwapBuffers();
	void PollEvents();
	inline GLFWwindow* GetWindow() const { return window; }
	inline int GetWidth() const { return width; }
	inline int GetHeight() const { return height; }
	inline std::string GetTitle() const { return title; }
};
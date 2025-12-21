#pragma once

#include <GLFW/glfw3.h>
#include <vector>

class IMouseListener {
public:
    virtual ~IMouseListener() = default;
    virtual void OnMouseButton(GLFWwindow*, int button, int action, int mods) {}
    virtual void OnCursorPos(GLFWwindow*, double x, double y) {}
    virtual void OnScroll(GLFWwindow*, double xoff, double yoff) {}
};

class InputRouter {
public:
    InputRouter() = default;

    void AddListener(IMouseListener* l) { listeners.push_back(l); }

    void DispatchMouseButton(GLFWwindow* w, int b, int a, int m);
    void DispatchCursorPos(GLFWwindow* w, double x, double y);
    void DispatchScroll(GLFWwindow* w, double xoff, double yoff);

private:
    std::vector<IMouseListener*> listeners;
};

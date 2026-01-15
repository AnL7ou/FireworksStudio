#include <iostream>
#include "core/Application.h"

int main()
{
	Application app;
	if (!app.Initialize()) {
		std::cerr << "Échec de l'initialisation de l'application\n";
		return -1;
	}
	app.Run();
	return 0;
}
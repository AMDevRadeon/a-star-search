#include "gui_application.hpp"

Application::Application()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
}

Application::~Application()
{
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Application::run()
{

}

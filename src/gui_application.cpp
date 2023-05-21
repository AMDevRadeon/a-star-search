#include "gui_application.hpp"

void Application::update()
{
	SDL_Event sdlEvent{};
	SDL_WaitEvent(&sdlEvent);

	if (sdlEvent.type == SDL_QUIT)
		m_isRunning = false;
}

void Application::create_window(int width, int height)
{
	destroy_window();
	constexpr auto pos = SDL_WINDOWPOS_UNDEFINED;

	m_window = SDL_CreateWindow("A* Pathfinding", pos, pos, width, height, 0);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
}

void Application::destroy_window()
{
	if (m_renderer != nullptr)
	{
		SDL_DestroyRenderer(m_renderer);
		m_renderer = nullptr;
	}

	if (m_window != nullptr)
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}
}

Application::Application()
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
}

Application::~Application()
{
	destroy_window();

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Application::run()
{
	create_window(400, 400);
	while (m_isRunning) update();
}

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "gui_global.hpp"
#include "common.hpp"

class Application
{
	bool m_isRunning = true;

	int m_matrixWidth = 20;
	int m_matrixHeight = 20;

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	void draw();
	void update();
	void create_main_window();
	void destroy_window();

	public:
	Application();
	~Application();

	void run();
};

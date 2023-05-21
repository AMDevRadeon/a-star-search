#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "gui_global.hpp"
#include "gui_viewport.hpp"
#include "common.hpp"

#include <vector>

class Application
{
	bool m_isRunning = true;

	int m_matrixWidth = 20;
	int m_matrixHeight = 20;

	Viewport m_viewport;
	std::vector<Vertex> m_matrix;

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	void draw();
	void update();
	void create_matrix();
	void create_main_window();
	void destroy_window();

	public:
	Application();
	~Application();

	void run();
};

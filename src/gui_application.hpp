#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "gui_global.hpp"
#include "gui_viewport.hpp"
#include "common.hpp"

#include <cmath>
#include <vector>

class Application
{
	bool m_isRunning = true;
	float m_defaultScale = 1.0f;

	int m_matrixWidth = 21;
	int m_matrixHeight = 16;

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

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

class Application
{
	bool m_isRunning = true;

	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;

	void update();
	void create_window(int width, int height);
	void destroy_window();

	public:
	Application();
	~Application();

	void run();
};

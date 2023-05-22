#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "gui_global.hpp"
#include "gui_viewport.hpp"
#include "gui_widget.hpp"
#include "common.hpp"

#include <cmath>
#include <vector>

enum TextureID
{
	PLAY, STOP, REPEAT, CURSOR,
	FLAG_START, FLAG_STOP, ACTIVATE, DEACTIVATE,
	RESIZE, RANDOMIZE, SELECTED, HOVER,
	DISABLED
};

enum WidgetID
{
	B_PLAY, B_REPEAT, B_CURSOR, B_FLAG_START,
	B_FLAG_STOP, B_ACTIVATE, B_DEACTIVATE, B_RESIZE,
	B_RANDOMIZE
};

class Application
{
	bool m_isRunning = true;
	float m_defaultScale = 1.0f;

	int m_iconWidth = 0;
	int m_iconHeight = 0;
	int m_iconCount = 0;

	int m_matrixWidth = 21;
	int m_matrixHeight = 16;

	Viewport m_viewport;
	std::vector<Vertex> m_matrix;

	Widget* m_widgetList = nullptr;
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	SDL_Texture* m_icons = nullptr;

	void draw();
	void update();
	void load_icons();
	void unload_icons();
	void create_matrix();
	void create_main_window();
	void destroy_window();

	template <typename Type, typename... Args>
	Type& access_widget(int id, Args... args);

	public:
	Application();
	~Application();

	void run();

	friend class Button;
};

template <typename Type, typename... Args>
Type& Application::access_widget(int id, Args... args)
{
	for (Widget* crr = m_widgetList; crr != nullptr; crr = crr -> m_next)
	{
		if (crr -> m_id != id)
			continue;

		Type* const ptr = dynamic_cast<Type*>(crr);
		if (ptr != nullptr) return *ptr;
	}

	Widget::s_appPointer = this;
	Type* const ptr = new Type(args...);

	ptr -> m_id = id;
	ptr -> m_next = m_widgetList;
	m_widgetList = ptr;

	return *ptr;
}

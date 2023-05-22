#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "gui_global.hpp"
#include "gui_viewport.hpp"
#include "gui_widget.hpp"
#include "gui_callback.hpp"
#include "common.hpp"

#include <cmath>
#include <vector>

enum TextureID
{
	PLAY, STOP, RESET, CURSOR,
	FLAG_START, FLAG_STOP, ACTIVATE, DEACTIVATE,
	RESIZE, RANDOMIZE, LOAD, OK,
	FLAG_START_2, FLAG_STOP_2, SELECTED, HOVER,
	DISABLED
};

enum WidgetID
{
	B_PLAY, B_RESET, B_CURSOR, B_FLAG_START,
	B_FLAG_STOP, B_ACTIVATE, B_DEACTIVATE, B_RESIZE,
	B_RANDOMIZE, B_LOAD, B_OK
};

struct Application
{
	int m_mode = B_CURSOR;
	bool m_isRunning = true;
	float m_defaultScale = 1.0f;

	int m_iconWidth = 0;
	int m_iconHeight = 0;
	int m_iconCount = 0;

	int m_matrixWidth = 21;
	int m_matrixHeight = 16;

	Viewport m_viewport;
	std::vector<Vertex> m_matrix;

	Vertex* m_vertStart = nullptr;
	Vertex* m_vertStop = nullptr;

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
	void refresh_mode();

	template <typename Type, typename... Args>
	Type& access_widget(int id, Args... args);

	Application();
	~Application();

	void run();
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

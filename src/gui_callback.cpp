#include "gui_callback.hpp"
#include "gui_application.hpp"
#include "../lib/sfd/sfd.h"

Application* g_appPtr = nullptr;

void callback_play(Application& app)
{
	if (!app.m_isSolving)
	{
		g_appPtr = &app;
		app.m_isSolving = true;

		for (Vertex& vertex : app.m_matrix)
			vertex.SoftReset();

		SDL_Event sdlEvent{};
		SDL_PushEvent(&sdlEvent);

		app.refresh_buttons();
		app.update();
		app.draw();

		app.m_graph.start = app.m_vertStart;
		app.m_graph.stop = app.m_vertStop;

		if (app.m_isDijkstra)
			solve_dijkstra(app.m_graph, app.m_isDiagonal, &callback_for_solver);
		else
			solve_astar(app.m_graph, app.m_isDiagonal, &callback_for_solver);
	}

	app.m_isSolving = false;
	app.refresh_buttons();
	app.draw();
}

void callback_reset(Application& app)
{
	for (Vertex& vertex : app.m_matrix)
		vertex.Reset();

	app.m_vertStart = nullptr;
	app.m_vertStop = nullptr;
}

void callback_cursor(Application& app) { app.m_mode = B_CURSOR; }
void callback_flag_start(Application& app) { app.m_mode = B_FLAG_START; }
void callback_flag_stop(Application& app) { app.m_mode = B_FLAG_STOP; }
void callback_activate(Application& app) { app.m_mode = B_ACTIVATE; }
void callback_deactivate(Application& app) { app.m_mode = B_DEACTIVATE; }

void callback_resize(Application& app)
{
	// TODO: Write implementation
}

void callback_randomize(Application& app)
{
	// TODO: Write implementation
}

void callback_load(Application& app)
{
	SDL_HideWindow(app.m_window);

	sfd_Options options{};
	const char* path = sfd_open_dialog(&options);

	if (*path != '\0')
	{
		app.m_templatePath = path;
		app.reload();
	}

	SDL_Event sdlEvent{};
	sdlEvent.type = SDL_MOUSEBUTTONUP;

	SDL_ShowWindow(app.m_window);
	SDL_PushEvent(&sdlEvent);
	app.update();
}

void callback_diagonal(Application& app) { app.m_isDiagonal = !app.m_isDiagonal; }
void callback_sel_astar(Application& app) { app.m_isDijkstra = false; }
void callback_sel_dijkstra(Application& app) { app.m_isDijkstra = true; }

void callback_for_solver()
{
	if (g_appPtr == nullptr || !g_appPtr->m_isSolving)
		return;

	g_appPtr->update();
	g_appPtr->draw();

	SDL_Delay(g_stepDelay);
}

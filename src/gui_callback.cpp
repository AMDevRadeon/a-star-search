#include "gui_callback.hpp"
#include "gui_application.hpp"

Application* g_appPtr = nullptr;

void callback_play(Application& app)
{
	if (!app.m_isSolving)
	{
		g_appPtr = &app;
		app.m_isSolving = true;

		// TODO: Soft reset?
		for (Vertex& vertex : app.m_matrix)
			vertex.state = AVAILABLE;

		SDL_Event sdlEvent{};
		SDL_PushEvent(&sdlEvent);

		app.refresh_buttons();
		app.update();
		app.draw();

		// TODO: Replace with proper solve function call
		for (Vertex& vertex : app.m_matrix)
		{
			if ((vertex.xpos + vertex.ypos) & 1)
				vertex.state = OPEN;
			else
				vertex.is_active = !vertex.is_active;
			callback_for_solver();
		}
		// ---------------------------------------------
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

void callback_cursor(Application& app)
{
	app.m_mode = B_CURSOR;
	app.refresh_buttons();
}

void callback_flag_start(Application& app)
{
	app.m_mode = B_FLAG_START;
	app.refresh_buttons();
}

void callback_flag_stop(Application& app)
{
	app.m_mode = B_FLAG_STOP;
	app.refresh_buttons();
}

void callback_activate(Application& app)
{
	app.m_mode = B_ACTIVATE;
	app.refresh_buttons();
}

void callback_deactivate(Application& app)
{
	app.m_mode = B_DEACTIVATE;
	app.refresh_buttons();
}

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
	// TODO: Write implementation
}

void callback_diagonal(Application& app)
{
	app.m_isDiagonal = !app.m_isDiagonal;
	app.refresh_buttons();
}

void callback_sel_astar(Application& app)
{
	app.m_isDijkstra = false;
	app.refresh_buttons();
}

void callback_sel_dijkstra(Application& app)
{
	app.m_isDijkstra = true;
	app.refresh_buttons();
}

void callback_for_solver()
{
	if (g_appPtr == nullptr || !g_appPtr->m_isSolving)
		return;

	g_appPtr->update();
	g_appPtr->draw();

	SDL_Delay(g_stepDelay);
}

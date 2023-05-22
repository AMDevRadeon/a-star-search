#include "gui_callback.hpp"
#include "gui_application.hpp"

void callback_play(Application& app)
{

}

void callback_repeat(Application& app)
{

}

void callback_cursor(Application& app)
{
	app.m_mode = B_CURSOR;
	app.refresh_mode();
}

void callback_flag_start(Application& app)
{
	app.m_mode = B_FLAG_START;
	app.refresh_mode();
}

void callback_flag_stop(Application& app)
{
	app.m_mode = B_FLAG_STOP;
	app.refresh_mode();
}

void callback_activate(Application& app)
{
	app.m_mode = B_ACTIVATE;
	app.refresh_mode();
}

void callback_deactivate(Application& app)
{
	app.m_mode = B_DEACTIVATE;
	app.refresh_mode();
}

void callback_resize(Application& app)
{

}

void callback_randomize(Application& app)
{

}

void callback_load(Application& app)
{

}

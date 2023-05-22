#pragma once

struct Application;

void callback_play(Application& app);
void callback_repeat(Application& app);
void callback_cursor(Application& app);
void callback_flag_start(Application& app);
void callback_flag_stop(Application& app);
void callback_activate(Application& app);
void callback_deactivate(Application& app);
void callback_resize(Application& app);
void callback_randomize(Application& app);
void callback_load(Application& app);


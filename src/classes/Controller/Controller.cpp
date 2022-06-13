//
// Created by asd on 6.5.22.
//

#include "Controller.h"

Controller* Controller::_instance;

Controller::Controller() : m_scenes(_gui, *this)
{
    _instance = this;

    signal(SIGINT, Controller::static_signal_handler);

    change_scene(&m_scenes.start);
    listen_for_input();
}

// ---------------------------------------------------------------------------------------------------------------------

void Controller::listen_for_input()
{
    int ch;
    nodelay(stdscr, TRUE);
    while (!_terminated)
    {
        if ((ch = getch()) != ERR || _timeout > 0)
        {
            synchronize_timeout(ch);

            switch (ch)
            {
                case ctrl('c'):
                    quit();
                    break;
                case ctrl('d'):
                    quit();
                    break;
                // F2 is reserved for keymap's "less..."
                case KEY_F(2):
                    _gui.less_keymap();
                    break;
                // F3 is reserved for keymap's "more..."
                case KEY_F(3):
                    _gui.more_keymap();
                    break;
                case KEY_RESIZE:
                    _gui.rewin();
                    _active_scene->render();
                    break;
                    
                default:
                    _active_scene->input(ch);
            }
        }
    }
}

// ---------------------------------------------------------------------------------------------------------------------

void Controller::synchronize_timeout(int ch)
{
    if (_timeout <= 0) return;
    
    // if timeout occured
    if (ch == ERR)
    {
        // then set it to it's original value
        set_timeout(_timeout);
        return;
    }

    // now we need to actually recalculate the timeout, since the user resetted it...
    
    // how many milliseconds passed since the last timeout occured
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _last_timeout_timestamp).count();

    timeout(_last_timeout - ms);
    _last_timeout -= ms;
    _last_timeout_timestamp = std::chrono::steady_clock::now();
}

// ---------------------------------------------------------------------------------------------------------------------

void Controller::set_timeout(int ms)
{
    if (ms != _last_timeout)
    {
        timeout(ms);
        _last_timeout = ms;
    }
    
    _timeout = ms;
    _last_timeout_timestamp = std::chrono::steady_clock::now();
}

// ---------------------------------------------------------------------------------------------------------------------

void Controller::change_scene(Scene* new_scene)
{
    _active_scene = new_scene;
    _active_scene->render();
}

// ---------------------------------------------------------------------------------------------------------------------

void Controller::quit()
{
    _terminated = true;
    _gui.clear_gui(2, 6);
}

// ---------------------------------------------------------------------------------------------------------------------

void Controller::static_signal_handler(int sig)
{
    _instance->signal_handler(sig);
}

// ---------------------------------------------------------------------------------------------------------------------

void Controller::signal_handler(int sig)
{
    switch (sig)
    {
        case SIGINT:
            quit();
            break;
    }
}
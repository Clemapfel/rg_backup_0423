// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <iostream>

namespace mousetrap
{
    Widget::operator GtkWidget*()
    {
        return get_native();
    }

    void Widget::set_signal_blocked(const std::string& signal_id, bool b)
    {
        auto it = _signal_handlers.find(signal_id);

        if (it == _signal_handlers.end())
        {
            std::cerr << "[WARNING] In Widget::set_signal_blocked: no signal with id \"" << signal_id << "\" connected."
                      << std::endl;
            return;
        }

        if (b)
            g_signal_handler_block(get_native(), it->second);
        else
            g_signal_handler_unblock(get_native(), it->second);
    }

    void Widget::set_all_signals_blocked(bool b)
    {
        for (auto& pair : _signal_handlers)
        {
            if (b)
                g_signal_handler_block(get_native(), pair.second);
            else
                g_signal_handler_unblock(get_native(), pair.second);
        }
    }

    template<typename Function_t>
    void Widget::connect_signal(const std::string& signal_id, Function_t* function, void* data)
    {
        auto handler_id = g_signal_connect(get_native(), signal_id.c_str(), G_CALLBACK(function), data);
        _signal_handlers.insert_or_assign(signal_id, handler_id);
    }

    Vector2f Widget::get_size_request()
    {
        int w, h;
        gtk_widget_get_size_request(get_native(), &w, &h);
        return Vector2f(w, h);
    }
}

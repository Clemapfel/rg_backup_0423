// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/window.hpp>
#include <include/gtk_common.hpp>

namespace mousetrap
{
    Window initialize();
}

// ###

namespace mousetrap
{
    Window initialize()
    {
        gtk_init(nullptr, nullptr);

        auto out = Window(GTK_WINDOW_TOPLEVEL);
        out.connect_signal("destroy", gtk_main_quit, nullptr);

        gtk_widget_realize(out.get_native());
        gtk_initialize_opengl(GTK_WINDOW(out.get_native()));

        return out;
    }
}
// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace mousetrap
{
    class Button : public WidgetImplementation<GtkButton>
    {
        public:
            Button();

            void set_has_frame(bool b);
            void set_child(Widget*);
    };
}

#include <src/button.inl>

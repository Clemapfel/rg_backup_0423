// 
// Copyright 2022 Clemens Cords
// Created on 8/29/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/menu_model.hpp>
#include <include/popover.hpp>
#include <include/popover_menu.hpp>

namespace mousetrap
{
    class MenuButton : public WidgetImplementation<GtkMenuButton>
    {
        public:
            MenuButton();

            void set_child(Widget*);

            void set_popover_position(GtkPositionType);
            void set_popover(Popover*);
            void set_popover(PopoverMenu*);

            void set_always_show_arrow(bool);
            void set_has_frame(bool);

            void popup();
            void popdown();

        private:
            void set_model(MenuModel*);
    };
}

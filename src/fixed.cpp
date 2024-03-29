//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/18/23
//

#include <include/fixed.hpp>

namespace mousetrap
{
    Fixed::Fixed()
            : WidgetImplementation<GtkFixed>(GTK_FIXED(gtk_fixed_new()))
    {}

    void Fixed::add_child(Widget* widget, Vector2f pos)
    {
        gtk_fixed_put(get_native(), widget->operator GtkWidget *(), pos.x, pos.y);
    }

    void Fixed::remove_child(Widget* widget)
    {
        gtk_fixed_remove(get_native(), widget->operator GtkWidget*());
    }

    void Fixed::set_child_position(Widget* widget, Vector2f pos)
    {
        gtk_fixed_move(get_native(), widget->operator GtkWidget*(), pos.x, pos.y);
    }

    Vector2f Fixed::get_child_position(Widget* widget)
    {
        double x, y;
        gtk_fixed_get_child_position(get_native(), widget->operator GtkWidget*(), &x, &y);
        return Vector2f(x, y);
    }
}
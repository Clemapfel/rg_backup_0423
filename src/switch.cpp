//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/18/23
//

#include <include/switch.hpp>

namespace mousetrap
{
    Switch::Switch()
        : WidgetImplementation<GtkSwitch>(GTK_SWITCH(gtk_switch_new())),
          HasStateSetSignal<Switch>(this)
    {}

    bool Switch::get_active()
    {
        return gtk_switch_get_active(get_native());
    }

    void Switch::set_active(bool b)
    {
        gtk_switch_set_active(get_native(), b);
    }

    void Switch::set_state(State state)
    {
        gtk_switch_set_state(get_native(), (bool) state);
    }

    Switch::State Switch::get_state()
    {
        return (State) gtk_switch_get_state(get_native());
    }
}

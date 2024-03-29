// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

#include <vector>

#include <include/box.hpp>

namespace mousetrap
{
    Box::Box(GtkOrientation orientation, float spacing)
        : WidgetImplementation<GtkBox>(GTK_BOX(gtk_box_new(orientation, spacing)))
    {}

    Box::Box(GtkBox* instance)
        : WidgetImplementation<GtkBox>(instance)
    {}

    void Box::push_back(Widget* widget)
    {
        gtk_box_append(get_native(), widget->operator GtkWidget*());
    }

    void Box::push_front(Widget* widget)
    {
        gtk_box_prepend(get_native(), widget->operator GtkWidget*());
    }

    void Box::insert_after(Widget* to_append, Widget* after)
    {
        gtk_box_insert_child_after(
            get_native(),
            to_append->operator GtkWidget*(),
            after->operator GtkWidget*()
        );
    }

    void Box::reorder_after(Widget* to_reorder, Widget* after)
    {
        gtk_box_reorder_child_after(
                get_native(),
                to_reorder->operator GtkWidget*(),
                after->operator GtkWidget*()
        );
    }

    void Box::remove(Widget* widget)
    {
        gtk_box_remove(get_native(), widget->operator GtkWidget *());
    }

    void Box::clear()
    {
        std::vector<GtkWidget*> to_remove;
        GtkWidget* current = gtk_widget_get_first_child(GTK_WIDGET(get_native()));
        while (current != nullptr)
        {
            to_remove.push_back(current);
            current = gtk_widget_get_next_sibling(current);
        }

        for (auto* w : to_remove)
            gtk_box_remove(get_native(), w);
    }

    void Box::set_homogeneous(bool b)
    {
        gtk_box_set_homogeneous(get_native(), b);
    }

    GtkOrientation Box::get_orientation() const
    {
        return gtk_orientable_get_orientation(GTK_ORIENTABLE(get_native()));
    }

    size_t Box::get_n_items()
    {
        size_t i = 0;
        auto* child = gtk_widget_get_first_child(GTK_WIDGET(get_native()));
        while(child != nullptr)
        {
            child = gtk_widget_get_next_sibling(child);
            i++;
        }

        return i;
    }
}
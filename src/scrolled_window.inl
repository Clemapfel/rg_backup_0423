// 
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    ScrolledWindow::ScrolledWindow()
    {
        _native = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(nullptr, nullptr));
    }

    GtkWidget* ScrolledWindow::get_native()
    {
        return GTK_WIDGET(_native);
    }

    void ScrolledWindow::set_min_content_width(float v)
    {
        gtk_scrolled_window_set_min_content_width(_native, v);
    }

    void ScrolledWindow::set_min_content_height(float v)
    {
        gtk_scrolled_window_set_min_content_height(_native, v);
    }

    void ScrolledWindow::set_max_content_width(float v)
    {
        gtk_scrolled_window_set_min_content_width(_native, v);
    }

    void ScrolledWindow::set_max_content_height(float v)
    {
        gtk_scrolled_window_set_min_content_height(_native, v);
    }

    void ScrolledWindow::set_policy(GtkPolicyType h, GtkPolicyType v)
    {
        gtk_scrolled_window_set_policy(_native, h, v);
    }

    void ScrolledWindow::set_placement(GtkCornerType type)
    {
        gtk_scrolled_window_set_placement(_native, type);
    }

    void ScrolledWindow::set_shadow_type(GtkShadowType type)
    {
        gtk_scrolled_window_set_shadow_type(_native, type);
    }

    void ScrolledWindow::set_kinetic_scrolling_enabled(bool b)
    {
        gtk_scrolled_window_set_kinetic_scrolling(_native, b);
    }
}
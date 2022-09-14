// 
// Copyright 2022 Clemens Cords
// Created on 8/14/22 by clem (mail@clemens-cords.com)
//

#include <include/flow_box.hpp> // TODO

namespace mousetrap
{
    FlowBox::FlowBox(GtkOrientation orientation)
    {
        _native = GTK_FLOW_BOX(gtk_flow_box_new());
        set_selection_mode(GTK_SELECTION_NONE);
        set_orientation(orientation);
    }

    FlowBox::operator GtkWidget*()
    {
        return GTK_WIDGET(_native);
    }

    void FlowBox::set_orientation(GtkOrientation orientation)
    {
        gtk_orientable_set_orientation(GTK_ORIENTABLE(_native), orientation);
    }

    void FlowBox::set_activate_on_single_click(bool b)
    {
        gtk_flow_box_set_activate_on_single_click(_native, b);
    }

    void FlowBox::set_min_children_per_line(size_t n)
    {
        gtk_flow_box_set_min_children_per_line(_native, n);
    }

    void FlowBox::set_max_children_per_line(size_t n)
    {
        gtk_flow_box_set_max_children_per_line(_native, n);
    }

    void FlowBox::set_column_spacing(float v)
    {
        gtk_flow_box_set_column_spacing(_native, v);
    }

    void FlowBox::set_row_spacing(float v)
    {
        gtk_flow_box_set_row_spacing(_native, v);
    }

    void FlowBox::set_selection_mode(GtkSelectionMode mode)
    {
        gtk_flow_box_set_selection_mode(_native, mode);
    }

    void FlowBox::set_homogeneous(bool b)
    {
        gtk_flow_box_set_homogeneous(_native, b);
    }

    GtkFlowBoxChild* FlowBox::get_child_at_index(size_t i)
    {
        return gtk_flow_box_get_child_at_index(_native, i);
    }

    GtkFlowBoxChild* FlowBox::get_child_at_pos(int x, int y)
    {
        return gtk_flow_box_get_child_at_pos(_native, x, y);
    }

    void FlowBox::push_back(Widget* widget)
    {
        gtk_flow_box_append(_native, widget->operator GtkWidget*());
    }

    void FlowBox::push_front(Widget* widget)
    {
        gtk_flow_box_prepend(_native, widget->operator GtkWidget*());
    }

    void FlowBox::insert_at(Widget* widget, size_t i)
    {
        gtk_flow_box_insert(_native, widget->operator GtkWidget *(), i);
    }

    void FlowBox::remove(Widget* widget)
    {
        gtk_flow_box_remove(_native, widget->operator GtkWidget*());
    }

    void FlowBox::clear()
    {
        std::vector<GtkWidget*> to_remove;
        GtkWidget* current = gtk_widget_get_first_child(GTK_WIDGET(_native));
        while (current != nullptr)
        {
            to_remove.push_back(current);
            current = gtk_widget_get_next_sibling(current);
        }

        for (auto* w : to_remove)
            gtk_flow_box_remove(_native, w);
    }
}
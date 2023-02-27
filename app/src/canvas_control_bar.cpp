//
// Created by clem on 2/25/23.
//

#include <app/canvas.hpp>

#include <sstream>

namespace mousetrap
{
    Canvas::ControlBar::ControlBar(Canvas* owner)
        : _owner(owner)
    {
        _grid_visible_toggle_button.set_child(&_grid_visible_icon);
        _grid_visible_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_grid_visible(button->get_active());
        }, this);

        _background_visible_toggle_button.set_child(&_background_visible_icon);
        _background_visible_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_background_visible(button->get_active());
        }, this);

        _horizontal_symmetry_toggle_button.set_child(&_horizontal_symmetry_icon);
        _horizontal_symmetry_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_horizontal_symmetry_active(button->get_active());
        }, this);

        _vertical_symmetry_toggle_button.set_child(&_vertical_symmetry_icon);
        _vertical_symmetry_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_vertical_symmetry_active(button->get_active());
        }, this);

        _brush_outline_visible_toggle_button.set_child(&_brush_outline_visible_icon);
        _brush_outline_visible_toggle_button.connect_signal_toggled([](ToggleButton* button, ControlBar* instance){
            instance->_owner->set_brush_outline_visible(button->get_active());
        }, this);

        _scale_scale.connect_signal_value_changed([](SpinButton* scale, ControlBar* instance) {
            instance->_owner->set_scale(scale->get_value());
        }, this);

        for (auto* icon : {
            &_grid_visible_icon,
            &_background_visible_icon,
            &_horizontal_symmetry_icon,
            &_vertical_symmetry_icon,
            &_brush_outline_visible_icon
        })
        {
            icon->set_size_request(icon->get_size());
        }

        _scale_scale.set_expand(false);
        _position_label.set_expand(false);

        auto insert_spacer = [&](){
            auto sep = SeparatorLine();
            (&sep)->set_opacity(0);
            (&sep)->set_expand(true);
            _main.push_back(&sep);
        };

        _position_label.set_text("");

        _main.push_back(&_position_label);
        _main.push_back(&_scale_scale);
        _main.push_back(&_brush_outline_visible_toggle_button);
        _main.push_back(&_grid_visible_toggle_button);
        _main.push_back(&_background_visible_toggle_button);
        _main.push_back(&_horizontal_symmetry_toggle_button);
        _main.push_back(&_vertical_symmetry_toggle_button);

        set_scale(_owner->_scale);
        set_horizontal_symmetry_active(_owner->_horizontal_symmetry_active);
        set_vertical_symmetry_active(_owner->_vertical_symmetry_active);
        set_cursor_in_bounds(_position_visible);
        set_brush_outline_visible(_owner->_brush_outline_visible);

        auto tooltip = [](const std::string& value) {
            return state::tooltips_file->get_value("canvas", value);
        };

        _position_label.set_tooltip_text(tooltip("position_label"));
        _scale_scale.set_tooltip_text(tooltip("scale_scale"));
        _brush_outline_visible_toggle_button.set_tooltip_text(tooltip("toggle_brush_outline_visible"));
        _grid_visible_toggle_button.set_tooltip_text(tooltip("toggle_grid_visible"));
        _background_visible_toggle_button.set_tooltip_text(tooltip("toggle_background_visible"));
        _horizontal_symmetry_toggle_button.set_tooltip_text(tooltip("toggle_horizontal_symmetry_active"));
        _vertical_symmetry_toggle_button.set_tooltip_text(tooltip("toggle_vertical_symmetry_active"));
    }

    Canvas::ControlBar::operator Widget*()
    {
        return &_main;
    }

    void Canvas::ControlBar::set_grid_visible(bool b)
    {
        _grid_visible_toggle_button.set_signal_toggled_blocked(true);
        _grid_visible_toggle_button.set_active(b);
        _grid_visible_toggle_button.set_signal_toggled_blocked(false);
        state::actions::canvas_toggle_grid_visible.set_state(b);
    }

    void Canvas::ControlBar::set_background_visible(bool b)
    {
        _background_visible_toggle_button.set_signal_toggled_blocked(true);
        _background_visible_toggle_button.set_active(b);
        _background_visible_toggle_button.set_signal_toggled_blocked(false);
    }

    void Canvas::ControlBar::set_horizontal_symmetry_active(bool b)
    {
        _horizontal_symmetry_toggle_button.set_signal_toggled_blocked(true);
        _horizontal_symmetry_toggle_button.set_active(b);
        _horizontal_symmetry_toggle_button.set_signal_toggled_blocked(false);
    }

    void Canvas::ControlBar::set_vertical_symmetry_active(bool b)
    {
        _vertical_symmetry_toggle_button.set_signal_toggled_blocked(true);
        _vertical_symmetry_toggle_button.set_active(b);
        _vertical_symmetry_toggle_button.set_signal_toggled_blocked(false);
    }

    void Canvas::ControlBar::set_brush_outline_visible(bool b)
    {
        _brush_outline_visible_toggle_button.set_signal_toggled_blocked(true);
        _brush_outline_visible_toggle_button.set_active(b);
        _brush_outline_visible_toggle_button.set_signal_toggled_blocked(false);
    }

    void Canvas::ControlBar::set_cursor_position(Vector2i xy)
    {
        std::stringstream str;
        str << "<tt>[" << xy.x << " | " << xy.y << "]</tt>";
        //_position_label.set_text(str.str());
    }

    void Canvas::ControlBar::set_cursor_in_bounds(bool b)
    {
        _position_visible = b;
        _position_label.set_opacity(_position_visible ? 1 : 0);
    }

    void Canvas::ControlBar::set_scale(float scale)
    {
        _scale_scale.set_signal_value_changed_blocked(true);
        _scale_scale.set_value(scale);
        _scale_scale.set_signal_value_changed_blocked(false);
    }
}
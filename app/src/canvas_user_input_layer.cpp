
//
// Created by clem on 2/14/23.
//

#include <app/canvas.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    Canvas::UserInputLayer::UserInputLayer(Canvas* canvas)
        : _owner(canvas), _shortcut_controller(state::app)
    {
        _proxy.set_opacity(1 / 255.f);
        _proxy.connect_signal_resize(on_area_resize, this);

        _click_controller.connect_signal_click_pressed(on_click_pressed, this);
        _click_controller.connect_signal_click_released(on_click_released, this);

        _motion_controller.connect_signal_motion_enter(on_motion_enter, this);
        _motion_controller.connect_signal_motion(on_motion, this);
        _motion_controller.connect_signal_motion_leave(on_motion_leave, this);

        _key_controller.connect_signal_key_pressed(on_key_pressed, this);
        _key_controller.connect_signal_key_released(on_key_released, this);
        _key_controller.set_propagation_phase(GTK_PHASE_CAPTURE);

        _scroll_controller.connect_signal_scroll_begin(on_scroll_begin, this);
        _scroll_controller.connect_signal_scroll(on_scroll, this);
        _scroll_controller.connect_signal_scroll_end(on_scroll_end, this);

        _drag_controller.connect_signal_drag_begin(on_drag_begin, this);
        _drag_controller.connect_signal_drag_update(on_drag_update, this);
        _drag_controller.connect_signal_drag_end(on_drag_end, this);

        _zoom_controller.connect_signal_scale_changed(on_scale_changed, this);

        _proxy.add_controller(&_click_controller);
        _proxy.add_controller(&_motion_controller);
        state::main_window->add_controller(&_key_controller);
        _proxy.add_controller(&_scroll_controller);
        _proxy.add_controller(&_drag_controller);
        _proxy.add_controller(&_shortcut_controller);
        _proxy.add_controller(&_zoom_controller);

        static auto increase_scale_action = Action("canvas.increase_scale");
        increase_scale_action.set_function([](){
           state::canvas->set_scale(state::canvas->_scale + state::settings_file->get_value_as<float>("canvas", "scale_step"));
        });
        state::add_shortcut_action(increase_scale_action);
        _shortcut_controller.add_action(increase_scale_action.get_id());

        static auto decrease_scale_action = Action("canvas.decrease_scale");
        decrease_scale_action.set_function([](){
            state::canvas->set_scale(state::canvas->_scale - state::settings_file->get_value_as<float>("canvas", "scale_step"));
        });
        state::add_shortcut_action(decrease_scale_action);
        _shortcut_controller.add_action(decrease_scale_action.get_id());

        _proxy.set_focusable(true);
        _proxy.set_focus_on_click(true);
        _proxy.set_cursor(GtkCursorType::CELL);

        _proxy.add_tick_callback([](FrameClock, UserInputLayer* instance) -> bool
        {
            auto current_tool = active_state->get_current_tool();
            if (instance->_mouse_button_pressed and (current_tool == ToolID::BRUSH or current_tool == ToolID::ERASER)) {

                auto points = generate_line_points(instance->_previous_cursor_pos, active_state->get_cursor_position());
                auto brush = active_state->get_current_brush()->get_image();
                auto out = DrawData();

                for (auto p : points)
                {
                    for (int x = 0; x < brush.get_size().x; ++x)
                    {
                        for (int y = 0; y < brush.get_size().y; ++y)
                        {
                            auto pos = Vector2i(
                                p.x + x - 0.5 * brush.get_size().x + 1,
                                p.y + y - 0.5 * brush.get_size().y + 1
                            );

                            if (brush.get_pixel(x, y).a == 0 or pos.x < 0 or pos.x >= active_state->get_layer_resolution().x or pos.y < 0 or pos.y >= active_state->get_layer_resolution().y)
                                continue;

                            if (instance->_stroke_points.find(pos) != instance->_stroke_points.end())
                                continue;

                            instance->_stroke_points.insert(pos);

                            auto current = active_state->get_current_cell()->get_pixel(pos.x, pos.y);
                            if (current_tool == ToolID::BRUSH)
                            {
                                RGBA left = active_state->get_primary_color();
                                left.a = active_state->get_brush_opacity();
                                RGBA right = current;

                                auto mixed = glm::mix(Vector3f(left.r, left.g, left.b), Vector3f(right.r, right.g, right.b), 1 -  left.a);

                                out.insert(std::pair<Vector2i, HSVA>(pos, RGBA(mixed.r, mixed.g, mixed.b, right.a + left.a)));
                            }
                            else if (current_tool == ToolID::ERASER)
                            {
                                current.a -= active_state->get_brush_opacity();

                                if (current.a <= 0)
                                    current = RGBA(0, 0, 0, 0);

                                out.insert(std::pair<Vector2i, HSVA>(pos, current));
                            }
                        }
                    }
                }

                active_state->draw_to_cell(active_state->get_current_cell_position(), out);
            }

            instance->_previous_cursor_pos = active_state->get_cursor_position();
            return true;
        }, this);
    }

    Canvas::UserInputLayer::operator Widget*()
    {
        return &_proxy;
    }

    void Canvas::UserInputLayer::on_area_resize(GLArea* area, int x, int y, UserInputLayer* instance)
    {
        instance->_canvas_size = {x, y};

        if (not area->get_is_realized())
            return;

        instance->_owner->set_canvas_size({x, y});
        instance->update_cursor_pos();
    }

    void Canvas::UserInputLayer::set_scale(float scale)
    {
        if (_scale == scale)
            return;

        _scale = scale;
        update_cursor_pos();
    }

    void Canvas::UserInputLayer::set_offset(Vector2f offset)
    {
        if (_offset == offset)
            return;

        _offset = {offset.x, offset.y};
        update_cursor_pos();
    }

    void Canvas::UserInputLayer::update_cursor_pos()
    {
        _owner->set_widget_cursor_position(_absolute_widget_space_pos);

        auto x = _absolute_widget_space_pos.x;
        auto y = _absolute_widget_space_pos.y;

        auto layer_resolution = active_state->get_layer_resolution();

        float width = layer_resolution.x / _canvas_size.x;
        float height = layer_resolution.y / _canvas_size.y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        Vector2f top_left = center - Vector2f{0.5 * width, 0.5 * height};

        float pixel_w = width / layer_resolution.x;
        float pixel_h = height / layer_resolution.y;

        Vector2f cursor_pos = Vector2f(x / _canvas_size.x, y / _canvas_size.y);
        active_state->set_cursor_position(Vector2i(
            ((cursor_pos.x - top_left.x) / pixel_w),
            ((cursor_pos.y - top_left.y) / pixel_h)
        ));
    }

    void Canvas::UserInputLayer::on_click_pressed(ClickEventController*, size_t n, double x, double y, UserInputLayer* instance)
    {
        instance->_absolute_widget_space_pos = {x, y};
        instance->update_cursor_pos();
        instance->_mouse_button_pressed = true;


        switch (active_state->get_current_tool())
        {
            case ToolID::BRUSH:
                // handled in tick callback
                return;
            case ToolID::ERASER:
                // handled in tick callback
                return;
            case ToolID::BUCKET_FILL:
                state::actions::canvas_apply_bucket_fill.activate();
                return;
            case ToolID::COLOR_SELECT:
                state::actions::canvas_apply_color_select.activate();
                return;
            case ToolID::MARQUEE_NEIGHBORHODD_SELECT:
                state::actions::canvas_apply_marquee_neighborhood_select.activate();
                return;
            default:
                return;
        }

    }

    void Canvas::UserInputLayer::on_click_released(ClickEventController*, size_t n, double x, double y, UserInputLayer* instance)
    {
        instance->_absolute_widget_space_pos = {x, y};
        instance->update_cursor_pos();
        instance->_mouse_button_pressed = false;
        instance->_stroke_points.clear();
    }

    void Canvas::UserInputLayer::on_motion_enter(MotionEventController*, double x, double y, UserInputLayer* instance)
    {
        instance->_owner->set_cursor_in_bounds(true);
        instance->_absolute_widget_space_pos = {x, y};
        instance->update_cursor_pos();

        static bool once = true;
        if (once)
        {
            instance->_owner->update_adjustment_bounds();
            once = false;
        }
    }

    void Canvas::UserInputLayer::on_motion_leave(MotionEventController*, UserInputLayer* instance)
    {
        instance->_owner->set_cursor_in_bounds(false);
        //active_state->set_cursor_position(*active_state->get_selection().begin());
    }

    void Canvas::UserInputLayer::on_motion(MotionEventController*, double x, double y, UserInputLayer* instance)
    {
        instance->_absolute_widget_space_pos = {x, y};
        instance->_normalized_widget_space_pos = {x / instance->_canvas_size.x, y / instance->_canvas_size.y};
        instance->update_cursor_pos();
    }

    bool Canvas::UserInputLayer::should_trigger(const std::string& id, guint keyval)
    {
        auto* trigger = gtk_shortcut_trigger_parse_string(id.c_str());
        auto normalized = std::string(gtk_shortcut_trigger_to_string(trigger));

        g_object_unref(trigger);

        bool shift_pressed = false;
        bool alt_pressed = false;
        bool control_pressed = false;

        // order: <Shift><Control><Alt>

        shift_pressed =
            keyval == GDK_KEY_Shift_L or
            keyval == GDK_KEY_Shift_R or
            keyval == GDK_KEY_Shift_Lock;// or
            //((modifier & GdkModifierType::GDK_CONTROL_MASK) > 0);

        alt_pressed =
            keyval == GDK_KEY_Alt_L or
            keyval == GDK_KEY_Alt_R;// or
            //((modifier & GdkModifierType::GDK_ALT_MASK) > 0);

        control_pressed =
            keyval == GDK_KEY_Control_L or
            keyval == GDK_KEY_Control_R;// or
            //((modifier & GdkModifierType::GDK_CONTROL_MASK) > 0);

        if (normalized == "<Shift>")
            return shift_pressed;
        else if (normalized == "<Alt>")
            return alt_pressed;
        else if (normalized == "<Control>")
            return control_pressed;
        else
            return false;
    }

    bool Canvas::UserInputLayer::on_key_pressed(KeyEventController* controller, guint keyval, guint keycode, GdkModifierType state, UserInputLayer* instance)
    {
        if (instance->should_trigger(state::keybindings_file->get_value("canvas", "scroll_scale_active"), keyval))
            instance->_scroll_scale_active = true;

        if (instance->should_trigger(state::keybindings_file->get_value("canvas", "lock_axis_movement"), keyval))
            instance->_lock_axis_movement = true;

        auto was_pressed = [&](const std::string& value){
            auto* trigger = gtk_shortcut_trigger_parse_string(state::keybindings_file->get_value("canvas", value).c_str());
            bool out = gtk_shortcut_trigger_trigger(trigger, instance->_key_controller.get_current_event(), false);
            g_object_unref(trigger);
            return out;
        };

        if (was_pressed("move_float_up"))
           state::actions::canvas_move_float_up.activate();

        if (was_pressed("move_float_right"))
            state::actions::canvas_move_float_right.activate();

        if (was_pressed("move_float_down"))
            state::actions::canvas_move_float_down.activate();

        if (was_pressed("move_float_left"))
            state::actions::canvas_move_float_left.activate();

        return false;
    }

    bool Canvas::UserInputLayer::on_key_released(KeyEventController* controller, guint keyval, guint keycode, GdkModifierType state, UserInputLayer* instance)
    {
        if (instance->should_trigger(state::keybindings_file->get_value("canvas", "scroll_scale_active"), keyval))
            instance->_scroll_scale_active = false;

        if (instance->should_trigger(state::keybindings_file->get_value("canvas", "lock_axis_movement"), keyval))
            instance->_lock_axis_movement = false;

        return false;
    }

    void Canvas::UserInputLayer::on_scroll_begin(ScrollEventController*, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_scroll(ScrollEventController*, double x, double y, UserInputLayer* instance)
    {
        if (instance->_scroll_scale_active)
        {
            bool inverted = state::settings_file->get_value_as<bool>("canvas", "scroll_inverted");
            float sensitivity = state::settings_file->get_value_as<float>("canvas", "scroll_sensitivity");

            auto scale = instance->_owner->_scale;
            scale += (inverted ? -1.f : 1) * y * sensitivity;
            instance->_owner->set_scale(scale);
            instance->_scale_backup = scale;
            return;
        }

        bool x_inverted = state::settings_file->get_value_as<bool>("canvas", "offset_x_inverted");
        bool y_inverted = state::settings_file->get_value_as<bool>("canvas", "offset_y_inverted");
        bool x_speed = state::settings_file->get_value_as<float>("canvas", "offset_x_speed");
        bool y_speed = state::settings_file->get_value_as<float>("canvas", "offset_y_speed");

        instance->_owner->set_offset(
            ((instance->_owner->_offset.x * instance->_canvas_size.x) + (x_inverted ? -1 : 1) * x * x_speed) / instance->_canvas_size.x,
            ((instance->_owner->_offset.y * instance->_canvas_size.y) + (y_inverted ? -1 : 1) * y * y_speed) / instance->_canvas_size.y
        );
    }

    void Canvas::UserInputLayer::on_scroll_end(ScrollEventController*, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_drag_begin(DragEventController*, double x, double y, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_drag_update(DragEventController*, double x, double y, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_drag_end(DragEventController*, double x, double y, UserInputLayer* instance)
    {

    }

    void Canvas::UserInputLayer::on_scale_changed(ZoomEventController*, double distance, UserInputLayer* instance)
    {
        if (distance < 1)
            distance = -1 - (1 - distance);
        
        instance->_owner->set_scale(instance->_owner->_scale + distance * state::settings_file->get_value_as<float>("canvas", "pinch_scale_step"));
    }
}

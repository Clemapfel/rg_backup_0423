//
// Created by clem on 1/23/23.
//

#include <app/animation_preview.hpp>

namespace mousetrap
{
    AnimationPreview::AnimationPreview()
    {
        _area.connect_signal_realize(on_realize, this);
        _area.connect_signal_resize(on_resize, this);
        _area.set_size_request({0, 0});

        _menu_button.set_child(&_menu_button_label);

        // actions

        using namespace state::actions;

        for (auto* action : {&animation_preview_toggle_playback_active, &animation_preview_decrease_scale_factor, &animation_preview_increase_scale_factor, &animation_preview_toggle_background_visible})
            state::add_shortcut_action(*action);

        auto playback_section = MenuModel();
        playback_section.add_action("Start / Stop Playback", "animation_preview.toggle_playback_active");
        _menu.add_section("Playback", &playback_section);

        auto settings_section = MenuModel();

        _fps_spin_button.set_value(_fps);
        _fps_spin_button.connect_signal_value_changed([](SpinButton* scale, AnimationPreview* instance){
            state::animation_preview->set_fps(scale->get_value());
        }, this);
        _fps_box.push_back(&_fps_label);
        _fps_spacer.set_opacity(0);
        _fps_box.push_back(&_fps_spacer);
        _fps_box.push_back(&_fps_spin_button);
        _fps_box.set_tooltip_text(state::tooltips_file->get_value("animation_preview", "fps_setting"));

        auto fps_submenu = MenuModel();
        fps_submenu.add_widget(&_fps_box);
        settings_section.add_submenu("Fps...", &fps_submenu);

        _scale_factor_spin_button.set_value(_scale_factor);
        _scale_factor_spin_button.connect_signal_value_changed([](SpinButton* scale, AnimationPreview* instance){
            state::animation_preview->set_scale_factor(scale->get_value());
        }, this);
        _scale_factor_box.push_back(&_scale_factor_label);
        _scale_factor_spacer.set_opacity(0);
        _scale_factor_box.push_back(&_scale_factor_spacer);
        _scale_factor_box.push_back(&_scale_factor_spin_button);
        _scale_factor_box.set_tooltip_text(state::tooltips_file->get_value("animation_preview", "scale_factor_setting"));

        auto scale_factor_submenu = MenuModel();
        scale_factor_submenu.add_widget(&_scale_factor_box);
        settings_section.add_submenu("Scale Factor...", &scale_factor_submenu);

        _background_visible_switch.set_active(_background_visible);
        _background_visible_switch.connect_signal_state_set([](Switch* s, bool state, AnimationPreview* instance) {
            state::animation_preview->set_background_visible(state);
            s->set_state((Switch::State) state);
        }, this);
        _background_visible_box.push_back(&_background_visible_label);
        _background_visible_spacer.set_opacity(0);
        _background_visible_box.push_back(&_background_visible_spacer);
        _background_visible_box.push_back(&_background_visible_switch);
        _background_visible_box.set_tooltip_text(state::tooltips_file->get_value("animation_preview", "show_background_setting"));

        auto background_submenu = MenuModel();
        background_submenu.add_widget(&_background_visible_box);
        settings_section.add_submenu("Show / Hide Background...", &background_submenu);

        _menu.add_section("Settings", &settings_section);

        _popover_menu.refresh_widgets();
        _menu_button.set_popover(&_popover_menu);
        _menu_button.set_popover_position(GTK_POS_LEFT);

        for (auto* box : {&_fps_box, &_scale_factor_box, &_background_visible_box})
            box->set_margin_horizontal(state::margin_unit);

        _frame.set_child(&_area);
        _frame.set_expand(true);
        _frame.set_label_widget(nullptr);

        _box.push_back(&_menu_button);
        _box.push_back(&_frame);

        _tooltip.create_from("animation_preview", state::tooltips_file, state::keybindings_file);
        _box.set_tooltip_widget(_tooltip);

        _click_controller.connect_signal_click_pressed(on_click_pressed, this);
        _frame.add_controller(&_click_controller);

        _box.add_tick_callback([](FrameClock clock, AnimationPreview* instance) -> bool {
            instance->on_tick_callback(clock);
            return true;
        }, this);

        _main.push_back(&_box);
    }

    AnimationPreview::operator Widget*()
    {
        return &_main;
    }

    void AnimationPreview::set_scale_factor(size_t x)
    {
        _scale_factor = x;

        auto pixel_size = Vector2f{
                1 / float(_canvas_size.x),
                1 / float(_canvas_size.y)
        };

        auto centroid = Vector2f{0.5, 0.5};
        auto size = Vector2f(
                pixel_size.x * active_state->get_layer_resolution().x * _scale_factor,
                pixel_size.y * active_state->get_layer_resolution().y * _scale_factor
        );

        for (auto* shape : _layer_shapes)
        {
            shape->as_rectangle(
                    {centroid.x - size.x, centroid.y - size.y},
                    {centroid.x + size.x, centroid.y - size.y},
                    {centroid.x + size.x, centroid.y + size.y},
                    {centroid.x - size.x, centroid.y + size.y}
            );
        }

        _area.queue_render();
    }

    void AnimationPreview::set_fps(float value)
    {
        _fps = value;
    }

    void AnimationPreview::set_background_visible(bool b)
    {
        _background_visible = b;

        if (_transparency_tiling_shape != nullptr)
            _transparency_tiling_shape->set_visible(b);

        _area.queue_render();
    }

    void AnimationPreview::on_tick_callback(FrameClock& clock)
    {
        if (not _playback_active or _fps == 0)
        {
            if (_current_frame == active_state->get_current_frame_index())
                return;

            _current_frame = active_state->get_current_frame_index();
        }
        else
        {
            auto frame_duration = seconds(1.f / _fps);
            _elapsed += clock.get_time_since_last_frame();

            while (_elapsed > frame_duration)
            {
                _elapsed -= frame_duration;
                _current_frame = _current_frame + 1 >= active_state->get_n_frames() ? 0 : _current_frame + 1;
            }
        }

        for (size_t layer_i = 0; _layer_shapes.size() == active_state->get_n_layers() and layer_i < active_state->get_n_layers(); ++layer_i)
        {
            auto* layer = active_state->get_layer(layer_i);
            _layer_shapes.at(layer_i)->set_texture(layer->get_frame(_current_frame)->get_texture());
        }

        _area.queue_render();
    }

    void AnimationPreview::set_playback_active(bool b)
    {
        _playback_active = b;

        if (not _playback_active)
        {
            _current_frame = active_state->get_current_frame_index();
            for (size_t layer_i = 0; _layer_shapes.size() == active_state->get_n_layers() and layer_i < active_state->get_n_layers(); ++layer_i)
            {
                auto* layer = active_state->get_layer(layer_i);
                _layer_shapes.at(layer_i)->set_texture(layer->get_frame(_current_frame)->get_texture());
            }
            _area.queue_render();
        }
    }

    void AnimationPreview::on_click_pressed(ClickEventController*, gint n_press, double x, double y, AnimationPreview* instance)
    {
        for (size_t i = 0; i < n_press; ++i)
            state::animation_preview->set_playback_active(not state::animation_preview->_playback_active);
    }

    void AnimationPreview::queue_render_tasks()
    {
        _area.clear_render_tasks();

        auto transparency_task = RenderTask(_transparency_tiling_shape, _transparency_tiling_shader);
        transparency_task.register_vec2("_canvas_size", &_canvas_size);

        _area.add_render_task(transparency_task);

        for (size_t layer_i = 0; layer_i < active_state->get_n_layers(); ++layer_i)
        {
            auto task = RenderTask(_layer_shapes.at(layer_i), nullptr, nullptr, active_state->get_layer(layer_i)->get_blend_mode());
            _area.add_render_task(task);
        }

        _area.queue_render();
    }

    void AnimationPreview::on_realize(Widget* widget, AnimationPreview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (_transparency_tiling_shader == nullptr)
        {
            _transparency_tiling_shader = new Shader();
            _transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        instance->on_layer_count_changed();

        instance->set_scale_factor(instance->_scale_factor);
        instance->set_fps(instance->_fps);
        instance->set_background_visible(instance->_background_visible);

        area->queue_render();
    }

    void AnimationPreview::on_resize(GLArea*, int w, int h, AnimationPreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->set_scale_factor(instance->_scale_factor);
        instance->_area.queue_render();
    }

    void AnimationPreview::on_layer_count_changed()
    {
        _area.make_current();

        for (auto* shape : _layer_shapes)
            delete shape;

        _layer_shapes.clear();


        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
        {
            if (_layer_shapes.size() <= i)
                _layer_shapes.emplace_back(new Shape());

            auto* layer = _layer_shapes.at(i);
            layer->as_rectangle({0, 0}, {1, 1});
            layer->set_texture(active_state->get_frame(i, _current_frame)->get_texture());
        }

        queue_render_tasks();
    }

    void AnimationPreview::on_layer_properties_changed()
    {
        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
            _layer_shapes.at(i)->set_visible(active_state->get_layer(i)->get_is_visible());

        queue_render_tasks();
    }

    void AnimationPreview::on_layer_image_updated()
    {
        if (not _area.get_is_realized())
            return;

        for (size_t i = 0; i < active_state->get_n_layers(); ++i)
            _layer_shapes.at(i)->set_texture(active_state->get_frame(i, _current_frame)->get_texture());

        _area.queue_render();
    }

    void AnimationPreview::on_layer_frame_selection_changed()
    {
        on_layer_image_updated();
    }
}

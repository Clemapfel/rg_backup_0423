//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), 1/5/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/global_state.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(animation_preview, toggle_playback_active);
        DECLARE_GLOBAL_ACTION(animation_preview, increase_scale_factor);
        DECLARE_GLOBAL_ACTION(animation_preview, decrease_scale_factor);
        DECLARE_GLOBAL_ACTION(animation_preview, toggle_background_visible);
    }

    class AnimationPreview : public AppComponent
    {
        public:
            AnimationPreview();

            operator Widget*() override;
            void update() override;

            size_t get_scale_factor() const;
            void set_scale_factor(size_t);

            float get_fps() const;
            void set_fps(float);

            bool get_background_visible() const;
            void set_background_visible(bool);

        private:
            // render
            GLArea _area;

            std::vector<Shape*> _layer_shapes;

            static inline Shader* _transparency_tiling_shader = nullptr;
            Vector2f _canvas_size = Vector2f(1, 1);
            Shape* _transparency_tiling_shape = nullptr;

            void queue_render_tasks();
            static void on_realize(Widget*, AnimationPreview*);
            static void on_resize(GLArea*, int w, int h, AnimationPreview*);

            // scale factor

            size_t _scale_factor = state::settings_file->get_value_as<size_t>("animation_preview", "default_scale_factor");
            size_t _max_scale_factor = state::settings_file->get_value_as<size_t>("animation_preview", "max_scale_factor");

            SpinButton _scale_factor_spin_button = SpinButton(1, _max_scale_factor, 1);
            SeparatorLine _scale_factor_spacer;
            Label _scale_factor_label = Label("Scale Factor: ");
            Box _scale_factor_box = Box(GTK_ORIENTATION_HORIZONTAL);

            static void on_scale_factor_spin_button_value_changed(SpinButton* spin_button, AnimationPreview*);

            // fps

            float _fps = state::settings_file->get_value_as<float>("animation_preview", "default_fps");
            float max_fps = state::settings_file->get_value_as<float>("animation_preview", "max_fps");

            SpinButton _fps_spin_button = SpinButton(0.1, max_fps, 0.1);
            SeparatorLine _fps_spacer;
            Label _fps_label = Label("Scale Factor: ");
            Box _fps_box = Box(GTK_ORIENTATION_HORIZONTAL);

            static void on_fps_spin_button_value_changed(SpinButton* spin_button, AnimationPreview*);
            void on_tick_callback(FrameClock&);

            // background

            bool _background_visible = state::settings_file->get_value_as<bool>("animation_preview", "background_visible");

            Switch _background_visible_switch;
            Label _background_visible_label = Label("Show Background: ");
            SeparatorLine _background_visible_spacer;
            Box _background_visible_box = Box(GTK_ORIENTATION_HORIZONTAL);

            static void on_background_visible_switch_state_set(Switch*, bool, AnimationPreview*);

            // playback

            bool _playback_active = state::settings_file->get_value_as<bool>("animation_preview", "playback_active");
            size_t _current_frame = 0;
            Time _elapsed = seconds(0);

            // menu

            MenuButton _menu_button;
            Label _menu_button_label = Label("Preview");
            MenuModel _menu;
            PopoverMenu _popover_menu = PopoverMenu(&_menu);

            // layout

            Frame _frame;
            Box _box = Box(GTK_ORIENTATION_VERTICAL);
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            // actions

            void on_playback_toggled();

            Tooltip _tooltip;
            ClickEventController _click_controller;
            static void on_click_pressed(ClickEventController*, gint n_press, double x, double y, AnimationPreview*);
    };

    namespace state
    {
        static inline AnimationPreview* animation_preview = nullptr;
    }
}

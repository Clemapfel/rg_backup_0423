//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), 1/5/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/project_state.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/tooltip.hpp>
#include <app/app_signals.hpp>

#include "include/render_texture.hpp" // TODO

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(animation_preview, toggle_playback_active);
        DECLARE_GLOBAL_ACTION(animation_preview, increase_scale_factor);
        DECLARE_GLOBAL_ACTION(animation_preview, decrease_scale_factor);
        DECLARE_GLOBAL_ACTION(animation_preview, toggle_background_visible);
    }

    class AnimationPreview : public AppComponent,
        public signals::LayerFrameSelectionChanged,
        public signals::LayerImageUpdated,
        public signals::LayerCountChanged,
        public signals::LayerPropertiesChanged,
        public signals::LayerResolutionChanged,
        public signals::PlaybackToggled,
        public signals::PlaybackFpsChanged,
        public signals::ColorOffsetChanged,
        public signals::ImageFlipChanged
    {
        public:
            AnimationPreview();

            operator Widget*() override;

            void set_scale_factor(size_t);
            void set_fps(float);
            void set_background_visible(bool);
            void set_playback_active(bool);

        protected:
            void on_layer_frame_selection_changed() override;
            void on_layer_image_updated() override;
            void on_layer_count_changed() override;
            void on_layer_properties_changed() override;
            void on_playback_toggled() override;
            void on_playback_fps_changed() override;
            void on_layer_resolution_changed() override;
            void on_color_offset_changed() override;
            void on_image_flip_changed() override;

        private:
            // render: layers

            GLArea _layer_area;
            std::vector<Shape*> _layer_shapes;

            static void on_layer_area_realize(Widget*, AnimationPreview*);
            static void on_layer_area_resize(GLArea*, int w, int h, AnimationPreview*);
            static gboolean on_layer_area_render(GLArea*, GdkGLContext*, AnimationPreview*);

            Shader* _post_fx_shader = nullptr;
            // uniforms, c.f. resources/shaders/project_post_fx.frag

            float* _h_offset = new float(0);
            float* _s_offset = new float(0);
            float* _v_offset = new float(0);
            float* _r_offset = new float(0);
            float* _g_offset = new float(0);
            float* _b_offset = new float(0);
            float* _a_offset = new float(0);

            int* _flip_horizontally = new int(0);
            int* _flip_vertically = new int(0);

            static inline const int* yes = new int(1);
            static inline const int* no = new int(0);

            ApplyScope _color_offset_apply_scope = active_state->get_color_offset_apply_scope();
            ApplyScope _image_flip_apply_scope = active_state->get_image_flip_apply_scope();

            // render: transparency tiling

            GLArea _transparency_area;

            static inline Shader* _transparency_tiling_shader = nullptr;
            Vector2f _canvas_size = Vector2f(1, 1);
            Shape* _transparency_tiling_shape = nullptr;

            static void on_transparency_area_realize(Widget*, AnimationPreview*);
            static void on_transparency_area_resize(GLArea*, int w, int h, AnimationPreview*);

            void queue_render_tasks();

            // scale factor

            size_t _scale_factor = state::settings_file->get_value_as<size_t>("animation_preview", "default_scale_factor");
            size_t _max_scale_factor = state::settings_file->get_value_as<size_t>("animation_preview", "max_scale_factor");

            SpinButton _scale_factor_spin_button = SpinButton(1, _max_scale_factor, 1);
            SeparatorLine _scale_factor_spacer;
            Label _scale_factor_label = Label("Scale Factor: ");
            Box _scale_factor_box = Box(GTK_ORIENTATION_HORIZONTAL);

            // fps

            float _fps = state::settings_file->get_value_as<float>("animation_preview", "default_fps");
            float max_fps = state::settings_file->get_value_as<float>("animation_preview", "max_fps");

            SpinButton _fps_spin_button = SpinButton(0.1, max_fps, 0.1);
            SeparatorLine _fps_spacer;
            Label _fps_label = Label("Scale Factor: ");
            Box _fps_box = Box(GTK_ORIENTATION_HORIZONTAL);

            void on_tick_callback(FrameClock&);

            // background
            bool _background_visible = state::settings_file->get_value_as<bool>("animation_preview", "background_visible");

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
            Overlay _overlay;
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            Tooltip _tooltip;
            ClickEventController _click_controller;
            static void on_click_pressed(ClickEventController*, gint n_press, double x, double y, AnimationPreview*);
    };

    namespace state
    {
        inline AnimationPreview* animation_preview = nullptr;
    }
}

// 
// Copyright 2022 Clemens Cords
// Created on 12/21/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>
#include <app/project_state.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/tooltip.hpp>
#include <app/app_signals.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(frame_view, toggle_onionskin_visible);
        DECLARE_GLOBAL_ACTION(frame_view, increase_n_onionskin_layers);
        DECLARE_GLOBAL_ACTION(frame_view, decrease_n_onionskin_layers);

        DECLARE_GLOBAL_ACTION(frame_view, jump_to_start);
        DECLARE_GLOBAL_ACTION(frame_view, jump_to_end);
        DECLARE_GLOBAL_ACTION(frame_view, go_to_previous_frame);
        DECLARE_GLOBAL_ACTION(frame_view, go_to_next_frame);
        DECLARE_GLOBAL_ACTION(frame_view, toggle_playback_active);

        DECLARE_GLOBAL_ACTION(frame_view, frame_move_right);
        DECLARE_GLOBAL_ACTION(frame_view, frame_move_left);

        DECLARE_GLOBAL_ACTION(frame_view, frame_new_left_of_current);
        DECLARE_GLOBAL_ACTION(frame_view, frame_new_right_of_current);
        DECLARE_GLOBAL_ACTION(frame_view, frame_delete);
        DECLARE_GLOBAL_ACTION(frame_view, frame_duplicate);

        DECLARE_GLOBAL_ACTION(frame_view, copy_to_cell_above)
        DECLARE_GLOBAL_ACTION(frame_view, copy_to_cell_below)
        DECLARE_GLOBAL_ACTION(frame_view, copy_to_cell_before)
        DECLARE_GLOBAL_ACTION(frame_view, copy_to_cell_after)

        DECLARE_GLOBAL_ACTION(frame_view, swap_with_cell_above)
        DECLARE_GLOBAL_ACTION(frame_view, swap_with_cell_below)
        DECLARE_GLOBAL_ACTION(frame_view, swap_with_cell_before)
        DECLARE_GLOBAL_ACTION(frame_view, swap_with_cell_after)

        DECLARE_GLOBAL_ACTION(frame_view, toggle_current_frame_is_keyframe);
    }
    
    class FrameView : public AppComponent,
        public signals::LayerFrameSelectionChanged,
        public signals::OnionskinVisibilityToggled,
        public signals::OnionskinLayerCountChanged,
        public signals::LayerImageUpdated,
        public signals::LayerCountChanged,
        public signals::LayerPropertiesChanged,
        public signals::LayerResolutionChanged,
        public signals::PlaybackToggled,
        public signals::PlaybackFpsChanged
    {
        public:
            FrameView();
            operator Widget*() override;

            void set_preview_size(size_t);

        protected:
            void on_layer_frame_selection_changed() override;
            void on_onionskin_visibility_toggled() override;
            void on_onionskin_layer_count_changed() override;
            void on_layer_image_updated() override;
            void on_layer_count_changed() override;
            void on_layer_properties_changed() override;
            void on_playback_toggled() override;
            void on_layer_resolution_changed() override;
            void on_playback_fps_changed() override;

        private:
            class FramePreview
            {
                public:
                    FramePreview(FrameView* owner);
                    ~FramePreview();
                    operator Widget*();

                    void set_texture(const Texture*);
                    void set_is_inbetween(bool);
                    void set_preview_size(size_t);
                    void set_opacity(float);
                    void set_visible(bool);
                    void set_selected(bool);
                    void set_resolution(Vector2ui);

                private:
                    FrameView* _owner;
                    size_t _layer_i;
                    size_t _frame_i;

                    AspectFrame _aspect_frame;
                    GLArea _area;

                    static inline Shader* _transparency_tiling_shader = nullptr;
                    Vector2f _canvas_size = Vector2f(1, 1);

                    const Texture* _texture = nullptr;
                    Shape* _transparency_tiling_shape = nullptr;
                    Shape* _layer_shape = nullptr;
                    Shape* _negative_overlay = nullptr;

                    static void on_realize(Widget*, FramePreview*);
                    static void on_resize(GLArea*, int w, int h, FramePreview*);

                    bool _is_inbetween;
                    ImageDisplay _inbetween_indicator = ImageDisplay(get_resource_path() + "icons/inbetween_indicator.png");
                    Overlay _inbetween_label_overlay;
            };

            size_t _preview_size = state::settings_file->get_value_as<int>("frame_view", "frame_preview_size");

            class FrameColumn
            {
                public:
                    FrameColumn(FrameView*, size_t frame_i);

                    operator Widget*();

                    void select_layer(size_t i);
                    void set_layer_visible(size_t i, bool);
                    void set_layer_opacity(size_t i, float);
                    void set_is_inbetween(size_t i, bool);
                    void set_preview_size(size_t);

                    void set_n_layers(size_t);
                    void set_frame(size_t i);
                    void set_resolution(Vector2ui);

                private:
                    FrameView* _owner;

                    size_t _frame_i;
                    ListView _list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
                    static void on_selection_changed(SelectionModel*, size_t i, size_t n_items, FrameColumn* instance);

                    class PreviewElement
                    {
                        public:
                            PreviewElement(FrameView* owner);
                            operator Widget*();

                            void set_layer_frame_index(size_t layer_i, size_t frame_i);
                            void set_preview_size(size_t);
                            void set_opacity(float);
                            void set_visible(bool);
                            void set_is_inbetween(bool);
                            void set_selected(bool);
                            void set_resolution(Vector2ui);

                        private:
                            FramePreview _preview;
                            ListView _wrapper;
                            Frame _frame;
                            Label _frame_label;
                            Label _layer_label;
                            SeparatorLine _layer_label_spacer;
                            Box _wrapper_box;
                    };

                    std::deque<PreviewElement> _preview_elements;
            };

            class ControlBar
            {
                public:
                    ControlBar(FrameView* owner);
                    operator Widget*();

                    void set_onionskin_visible(bool);
                    void set_n_onionskin_layers(size_t);
                    void set_playback_active(bool);
                    void set_is_keyframe(bool);
                    void set_fps(float);

                private:
                    FrameView* _owner;

                    ToggleButton _toggle_onionskin_visible_button;
                    ImageDisplay _onionskin_visible_icon = ImageDisplay(get_resource_path() + "icons/onionskin_visible.png");
                    ImageDisplay _onionskin_not_visible_icon = ImageDisplay(get_resource_path() + "icons/onionskin_not_visible.png");

                    SpinButton _onionskin_n_layers_spin_button = SpinButton(1, 99, 1);

                    Button _jump_to_start_button;
                    ImageDisplay _jump_to_start_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_start.png");

                    Button _jump_to_end_button;
                    ImageDisplay _jump_to_end_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_jump_to_end.png");

                    Button _go_to_previous_frame_button;
                    ImageDisplay _go_to_previous_frame_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_go_to_previous_frame.png");

                    Button _go_to_next_frame_button;
                    ImageDisplay _go_to_next_frame_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_go_to_next_frame.png");

                    ToggleButton _play_pause_button;
                    ImageDisplay _play_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_play.png");
                    ImageDisplay _pause_icon = ImageDisplay(get_resource_path() + "icons/animation_playback_pause.png");

                    Button _frame_move_right_button;
                    ImageDisplay _frame_move_right_icon = ImageDisplay(get_resource_path() + "icons/frame_move_right.png");

                    Button _frame_move_left_button;
                    ImageDisplay _frame_move_left_icon = ImageDisplay(get_resource_path() + "icons/frame_move_left.png");

                    Button _frame_new_left_of_current_button;
                    ImageDisplay _frame_new_left_of_current_icon = ImageDisplay(get_resource_path() + "icons/frame_new_left_of_current.png");

                    Button _frame_new_right_of_current_button;
                    ImageDisplay _frame_new_right_of_current_icon = ImageDisplay(get_resource_path() + "icons/frame_new_right_of_current.png");

                    Button _frame_duplicate_button;
                    ImageDisplay _frame_duplicate_icon = ImageDisplay(get_resource_path() + "icons/frame_duplicate.png");

                    Button _frame_delete_button;
                    ImageDisplay _frame_delete_icon = ImageDisplay(get_resource_path() + "icons/frame_delete.png");

                    ToggleButton _frame_is_keyframe_toggle_button;
                    ImageDisplay _frame_is_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_keyframe.png");
                    ImageDisplay _frame_is_not_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_not_keyframe.png");

                    size_t _preview_size = state::settings_file->get_value_as<int>("frame_view", "frame_preview_size");
                    MenuModel _preview_size_submenu;

                    Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _preview_size_label = Label("Preview Size (px): ");
                    SpinButton _preview_size_spin_button = SpinButton(2, 256, 1);

                    MenuButton _menu_button;
                    Label _menu_button_label = Label("Frames");
                    MenuModel _menu;
                    PopoverMenu _popover_menu = PopoverMenu(&_menu);

                    ScrolledWindow _scrolled_window;
                    Box _button_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                    SpinButton _fps_spin_button = SpinButton(1, state::settings_file->get_value_as<float>("animation_preview", "max_fps"), 0.5);
                    Label _fps_label;
                    Overlay _fps_label_overlay;
                    SeparatorLine _fps_label_underlay;

                    Tooltip _tooltip;
            };

            ControlBar _control_bar = ControlBar(this);

            size_t _selected_layer_i = 0;
            size_t _selected_frame_i = 0;
            void set_selection(size_t layer_i, size_t frame_i);

            std::deque<FrameColumn> _frame_columns;

            ListView _frame_column_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);

            Box _frame_colum_list_view_window_box = Box(GTK_ORIENTATION_VERTICAL);
            SeparatorLine _frame_colum_list_view_window_spacer;

            ScrolledWindow _frame_column_list_view_window;

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };

    namespace state
    {
        inline FrameView* frame_view = nullptr;
    }
}

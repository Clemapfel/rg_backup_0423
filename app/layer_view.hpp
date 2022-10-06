// 
// Copyright 2022 Clemens Cords
// Created on 10/6/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/image_display.hpp>
#include <include/toggle_button.hpp>
#include <include/entry.hpp>
#include <include/scale.hpp>
#include <include/check_button.hpp>
#include <include/get_resource_path.hpp>
#include <include/box.hpp>
#include <include/list_view.hpp>
#include <include/reorderable_list.hpp>
#include <include/viewport.hpp>
#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/label.hpp>
#include <include/separator_line.hpp>
#include <include/scrolled_window.hpp>
#include <include/menu_button.hpp>
#include <include/popover.hpp>
#include <include/dropdown.hpp>
#include <include/overlay.hpp>
#include <include/center_box.hpp>
#include <include/frame.hpp>
#include <include/scrollbar.hpp>

#include <app/global_state.hpp>
#include <app/app_component.hpp>

namespace mousetrap
{
    class LayerView : public AppComponent
    {
        public:
            LayerView();
            operator Widget*() override;
            void update() override;

        private:
            static inline float thumbnail_height = 5 * state::margin_unit;
            static inline const float thumbnail_margin_top_bottom = 0.5 * state::margin_unit;
            static inline const float thumbnail_margin_left_right = 0.25 * state::margin_unit;
            static inline Shader* transparency_tiling_shader = nullptr;

            class LayerFrameDisplay
            {
                public:
                    LayerFrameDisplay(size_t layer, size_t frame, LayerView* owner);
                    ~LayerFrameDisplay();

                    operator Widget*();
                    void update();
                    void update_selection();

                private:
                    size_t _layer;
                    size_t _frame;
                    LayerView* _owner;

                    Vector2f _canvas_size;

                    static void on_gl_area_realize(Widget*, LayerFrameDisplay*);
                    static void on_gl_area_resize(GLArea*, int, int, LayerFrameDisplay*);

                    GLArea _gl_area;
                    Shape* _transparency_tiling_shape;
                    Shape* _layer_shape;

                    static inline Texture* selection_indicator_texture = nullptr;

                    AspectFrame _aspect_frame;

                    ImageDisplay _layer_frame_active_icon = ImageDisplay(get_resource_path() + "icons/layer_frame_active.png");
                    Overlay _overlay;
                    Label _frame_widget_label;
                    Frame _frame_widget = Frame();
                    ListView _main = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);
            };

            class LayerPropertyOptions
            {
                public:
                    LayerPropertyOptions(size_t layer_i, LayerView*);
                    operator Widget*();

                    void update();

                private:
                    size_t _layer;
                    LayerView* _owner;
                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                    ImageDisplay _locked_toggle_button_icon_locked = ImageDisplay(get_resource_path() + "icons/layer_locked.png", state::icon_scale);
                    ImageDisplay _locked_toggle_button_icon_not_locked = ImageDisplay(get_resource_path() + "icons/layer_not_locked.png", state::icon_scale);
                    ToggleButton _locked_toggle_button;
                    static void on_locked_toggle_button_toggled(ToggleButton*, LayerPropertyOptions*);

                    ImageDisplay _visible_toggle_button_icon_visible = ImageDisplay(get_resource_path() + "icons/layer_visible.png", state::icon_scale);
                    ImageDisplay _visible_toggle_button_icon_not_visible = ImageDisplay(get_resource_path() + "icons/layer_not_visible.png", state::icon_scale);
                    ToggleButton _visible_toggle_button;
                    static void on_visible_toggle_button_toggled(ToggleButton*, LayerPropertyOptions*);

                    Box _visible_locked_indicator_box = Box(GTK_ORIENTATION_HORIZONTAL, state::margin_unit * 0.5);

                    Label _menu_button_title_label;
                    ScrolledWindow _menu_button_title_label_viewport;
                    MenuButton _menu_button;
                    Popover _menu_button_popover;
                    Box _menu_button_popover_box = Box (GTK_ORIENTATION_VERTICAL);

                    Label _name_label = Label("Name");
                    SeparatorLine _name_separator;
                    Entry _name_entry;
                    Box _name_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    static void on_name_entry_activate(Entry*, LayerPropertyOptions* instance);

                    Label _visible_label = Label("Visible");
                    SeparatorLine _visible_separator;
                    CheckButton _visible_check_button;
                    Box _visible_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    static void on_visible_check_button_toggled(CheckButton*, LayerPropertyOptions* instance);

                    Label _locked_label = Label("Locked");
                    SeparatorLine _locked_separator;
                    CheckButton _locked_check_button;
                    Box _locked_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    static void on_locked_check_button_toggled(CheckButton*, LayerPropertyOptions* instance);

                    Label _opacity_label = Label("Opacity");
                    SeparatorLine _opacity_separator;
                    Scale _opacity_scale = Scale(0, 1, 0.01);
                    Box _opacity_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    static void on_opacity_scale_value_changed(Scale*, LayerPropertyOptions* instance);

                    Label _blend_mode_label = Label("Blend");
                    SeparatorLine _blend_mode_separator;
                    Box _blend_mode_box = Box(GTK_ORIENTATION_HORIZONTAL);

                    DropDown _blend_mode_dropdown;
                    static inline const std::vector<BlendMode> _blend_modes_in_order = {
                            BlendMode::NORMAL,
                            BlendMode::ADD,
                            BlendMode::SUBTRACT,
                            BlendMode::REVERSE_SUBTRACT,
                            BlendMode::MULTIPLY,
                            BlendMode::MIN,
                            BlendMode::MAX
                    };
                    std::vector<Label> _blend_mode_dropdown_label_items;
                    std::vector<Label> _blend_mode_dropdown_list_items;

                    using on_blend_mode_select_data = struct {BlendMode blend_mode; LayerPropertyOptions* instance;};
                    static void on_blend_mode_select(on_blend_mode_select_data*);
            };

            class LayerRow
            {
                public:
                    LayerRow(size_t layer, LayerView* owner);

                    operator Widget*();
                    void update();
                    void update_selection();

                private:
                    size_t _layer;
                    LayerView* _owner;

                    std::deque<LayerFrameDisplay> _layer_frame_displays;
                    ListView _layer_frame_display_list_view = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_SINGLE);
                    static void on_layer_frame_display_list_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerRow* instance);

                    LayerPropertyOptions _layer_property_options;
                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
            };

            class FrameControlBar
            {
                public:
                    FrameControlBar(LayerView*);
                    operator Widget*();
                    void update();

                private:
                    LayerView* _owner;

                    ImageDisplay _frame_move_left_icon = ImageDisplay(get_resource_path() + "icons/frame_move_left.png");
                    Button _frame_move_left_button;
                    static void on_frame_move_left_button_clicked(Button*, FrameControlBar* instance);

                    ImageDisplay _frame_create_left_of_this_icon = ImageDisplay(get_resource_path() + "icons/frame_create_left_of_this.png");
                    Button _frame_create_left_of_this_button;
                    static void on_frame_create_left_of_this_button_clicked(Button*, FrameControlBar* instance);

                    ImageDisplay _frame_is_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_keyframe.png");
                    ImageDisplay _frame_is_not_keyframe_icon = ImageDisplay(get_resource_path() + "icons/frame_is_not_keyframe.png");
                    ToggleButton _frame_keyframe_toggle_button;
                    static void on_frame_keyframe_toggle_button_toggled(ToggleButton*, FrameControlBar* instance);

                    ImageDisplay _frame_delete_icon = ImageDisplay(get_resource_path() + "icons/frame_delete.png");
                    Button _frame_delete_button;
                    static void on_frame_delete_button_clicked(Button*, FrameControlBar* instance);

                    ImageDisplay _frame_duplicate_icon = ImageDisplay(get_resource_path() + "icons/frame_duplicate.png");
                    Button _frame_duplicate_button;
                    static void on_frame_duplicate_button_clicked(Button*, FrameControlBar* instance);

                    ImageDisplay _frame_create_right_of_this_icon = ImageDisplay(get_resource_path() + "icons/frame_create_right_of_this.png");
                    Button _frame_create_right_of_this_button;
                    static void on_frame_create_right_of_this_button_clicked(Button*, FrameControlBar* instance);

                    ImageDisplay _frame_move_right_icon = ImageDisplay(get_resource_path() + "icons/frame_move_right.png");
                    Button _frame_move_right_button;
                    static void on_frame_move_right_button_clicked(Button*, FrameControlBar* instance);

                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
            };

            class LayerControlBar
            {
                public:
                    LayerControlBar(LayerView*);

                    void update();
                    operator Widget*();

                private:
                    LayerView* _owner;

                    ImageDisplay _layer_move_up_icon = ImageDisplay(get_resource_path() + "icons/layer_move_up.png");
                    Button _layer_move_up_button;
                    static void on_layer_move_up_button_clicked(Button*, LayerControlBar* instance);

                    ImageDisplay _layer_create_icon = ImageDisplay(get_resource_path() + "icons/layer_create.png");
                    Button _layer_create_button;
                    static void on_layer_create_button_clicked(Button*, LayerControlBar* instance);

                    ImageDisplay _layer_duplicate_icon = ImageDisplay(get_resource_path() + "icons/layer_duplicate.png");
                    Button _layer_duplicate_button;
                    static void on_layer_duplicate_button_clicked(Button*, LayerControlBar* instance);

                    ImageDisplay _layer_delete_icon = ImageDisplay(get_resource_path() + "icons/layer_delete.png");
                    Button _layer_delete_button;
                    static void on_layer_delete_button_clicked(Button*, LayerControlBar* instance);

                    ImageDisplay _layer_move_down_icon = ImageDisplay(get_resource_path() + "icons/layer_move_down.png");
                    Button _layer_move_down_button;
                    static void on_layer_move_down_button_clicked(Button*, LayerControlBar* instance);

                    ImageDisplay _layer_merge_down_icon = ImageDisplay(get_resource_path() + "icons/layer_merge_down.png");
                    Button _layer_merge_down_button;
                    static void on_layer_merge_down_button_clicked(Button*, LayerControlBar* instance);

                    ImageDisplay _layer_flatten_all_icon = ImageDisplay(get_resource_path() + "icons/layer_flatten_all.png");
                    Button _layer_flatten_all_button;
                    static void on_layer_flatten_all_button_clicked(Button*, LayerControlBar* instance);

                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);
            };

            size_t _selected_frame;
            size_t _selected_layer;
            void set_selection(size_t layer_i, size_t frame_i);

            std::deque<LayerRow> _layer_rows;
            FrameControlBar _frame_control_bar;
            LayerControlBar _layer_control_bar;

            ListView _layer_row_list_view = ListView(GTK_ORIENTATION_VERTICAL, GTK_SELECTION_SINGLE);
            static void on_layer_row_list_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerView*);
            KeyEventController _layer_row_list_view_key_event_controller;
            static bool on_layer_row_list_view_key_event_controller_key_pressed(KeyEventController*, guint keyval, guint keycode, GdkModifierType state, LayerView* instance);

            Box _main = Box(GTK_ORIENTATION_VERTICAL);
    };
}

#include <app/layer_view/layer_frame_display.inl>
#include <app/layer_view/layer_row.inl>
#include <app/layer_view/frame_control_bar.inl>
#include <app/layer_view/layer_control_bar.inl>
#include <app/layer_view/layer_property_options.inl>

namespace mousetrap
{
    LayerView::operator Widget*()
    {
        return &_main;
    }

    void LayerView::on_layer_row_list_view_selection_changed(SelectionModel*, size_t first_item_position, size_t n_items_changed, LayerView* instance)
    {
        instance->set_selection(first_item_position, instance->_selected_frame);
    }

    bool LayerView::on_layer_row_list_view_key_event_controller_key_pressed(KeyEventController*, guint keyval, guint keycode, GdkModifierType state, LayerView* instance)
    {
        std::cout << keyval << std::endl;

        if (keyval == GDK_KEY_Right)
            instance->set_selection(instance->_selected_layer, std::min(instance->_selected_frame+1, state::n_frames - 1));
        if (keyval == GDK_KEY_Left)
            instance->set_selection(instance->_selected_layer, instance->_selected_frame - (instance->_selected_frame != 0 ? 1 : 0));
        if (keyval == GDK_KEY_Down)
            instance->set_selection(std::min(instance->_selected_layer+1, state::layers.size()-1), instance->_selected_frame);
        if (keyval == GDK_KEY_Up)
            instance->set_selection(instance->_selected_layer - (instance->_selected_layer != 0 ? 1 : 0), instance->_selected_frame);

        return true;
    }

    void LayerView::set_selection(size_t layer_i, size_t frame_i)
    {
        _selected_frame = frame_i;
        _selected_layer = layer_i;

        for (auto& row : _layer_rows)
            row.update_selection();

        _layer_row_list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _layer_row_list_view.get_selection_model()->select(layer_i);
        _layer_row_list_view.get_selection_model()->set_signal_selection_changed_blocked(false);
    }

    LayerView::LayerView()
        : _frame_control_bar(this), _layer_control_bar(this)
    {
        for (size_t layer_i = 0; layer_i < state::layers.size(); ++layer_i)
        {
            _layer_rows.emplace_back(layer_i, this);
            _layer_rows.back().update();
        }

        for (auto& row : _layer_rows)
            _layer_row_list_view.push_back(row);

        _layer_row_list_view.get_selection_model()->connect_signal_selection_changed(
                on_layer_row_list_view_selection_changed, this);
        _layer_row_list_view_key_event_controller.connect_signal_key_pressed(
                on_layer_row_list_view_key_event_controller_key_pressed, this);
        _layer_row_list_view.add_controller(&_layer_row_list_view_key_event_controller);

        _main.push_back(&_layer_row_list_view);
        _main.push_back(_frame_control_bar);
        _main.push_back(_layer_control_bar);
    }

    void LayerView::update()
    {}
}
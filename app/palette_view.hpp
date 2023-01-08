//
// Copyright 2022 Clemens Cords
// Created on 10/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/app_component.hpp>
#include <app/palette.hpp>
#include <app/global_state.hpp>
#include <app/bubble_log_area.hpp>
#include <app/file_chooser_dialog.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    class PaletteView : public AppComponent
    {
        public:
            PaletteView();
            operator Widget*() override;
            void update() override;

            void select(size_t i);

            void set_preview_size(size_t);
            size_t get_preview_size() const;

            void set_palette_locked(bool);
            bool get_palette_locked() const;

            void reload();

        protected:
            void load_from_file(const std::string& path);
            void load_as_debug();

            void save_to_file(const std::string& path);

        private:
            bool _palette_locked = state::settings_file->get_value_as<bool>("palette_view", "palette_locked");
            size_t _preview_size = state::settings_file->get_value_as<size_t>("palette_view", "color_preview_size");

            void update_from_palette();

            class ColorTile
            {
                public:
                    ColorTile(PaletteView* owner, HSVA color);
                    operator Widget*();

                    void set_color(HSVA color);
                    void set_size(size_t);
                    HSVA get_color();
                    void set_selected(bool);

                private:
                    PaletteView* _owner;
                    HSVA _color;

                    GLArea _color_area;
                    GLArea _selection_frame_area;

                    Shape* _color_shape = nullptr;
                    Shape* _frame_shape = nullptr;

                    static inline Texture* is_selected_overlay_texture = nullptr;
                    Shape* _is_selected_shape = nullptr;

                    static void on_color_area_realize(Widget* widget, ColorTile* instance);
                    static void on_selection_frame_realize(Widget* widget, ColorTile* instance);

                    AspectFrame _aspect_frame = AspectFrame(1, 1);
                    Overlay _overlay;
            };

            class PaletteControlBar
            {
                public:
                    PaletteControlBar(PaletteView* owner);
                    operator Widget*();
                    void update();

                private:
                    PaletteView* _owner;

                    Box _main = Box(GTK_ORIENTATION_HORIZONTAL);

                    ImageDisplay _palette_locked_icon = ImageDisplay(get_resource_path() + "icons/palette_locked.png");
                    ImageDisplay _palette_not_locked_icon = ImageDisplay(get_resource_path() + "icons/palette_not_locked.png");
                    ToggleButton _palette_not_locked_button;
                    static void on_palette_not_locked_button_toggled(ToggleButton*, PaletteControlBar* instance);

                    Box _palette_locked_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _palette_locked_label = Label("Palette Editing Enabled: ");
                    SeparatorLine _palette_locked_spacer;
                    Switch _palette_locked_switch;
                    static void on_palette_locked_switch_state_set(Switch*, bool, PaletteControlBar* instance);

                    Box _preview_size_box = Box(GTK_ORIENTATION_HORIZONTAL);
                    Label _preview_size_label = Label("Preview Size (px): ");
                    SeparatorLine _preview_size_spacer;
                    SpinButton _preview_size_scale = SpinButton(2, 512, 1);
                    static void on_preview_size_scale_value_changed(SpinButton*, PaletteControlBar* instance);

                    void update_palette_locked();
                    void update_preview_size_changed();

                    Label _menu_button_label = Label("Palette");
                    MenuButton _menu_button;
                    MenuModel _menu;
            };

            PaletteControlBar _palette_control_bar = PaletteControlBar(this);

            size_t _selected_i = 0;
            static void on_color_tile_view_selection_model_selection_changed(SelectionModel*, size_t child_i, size_t n_items, PaletteView* instance);

            std::vector<ColorTile*> _color_tiles;
            GridView _color_tile_view = GridView(GTK_SELECTION_SINGLE);
            ScrolledWindow _scrolled_window;

            Box _palette_view_box = Box(GTK_ORIENTATION_HORIZONTAL);
            Box _main = Box(GTK_ORIENTATION_VERTICAL);

            // actions

            Action _on_load_action = Action("palette_view.load");
            OpenFileDialog _on_load_dialog = OpenFileDialog("Load Palette");
            void on_load_ok_pressed();

            Action _on_save_as_action = Action("palette_view.save_as");
            SaveAsFileDialog _on_save_as_dialog = SaveAsFileDialog("Save Palette As");
            void on_save_as_ok_pressed();

            std::string _save_to_path = "";

            Action _on_load_default_action = Action("palette_view.load_default");
            Action _on_save_action = Action("palette_view.save");
            Action _on_save_as_default_action = Action("palette_view.save_as_default");

            Action _on_sort_by_default_action = Action("palette_view.sort_by_default");
            Action _on_sort_by_hue_action = Action("palette_view.sort_by_hue");
            Action _on_sort_by_saturation_action = Action("palette_view.sort_by_saturation");
            Action _on_sort_by_value_action = Action("palette_view.sort_by_value");

            Action _select_color_0 = Action("palette_view.select_color_0");
            Action _select_color_1 = Action("palette_view.select_color_1");
            Action _select_color_2 = Action("palette_view.select_color_2");
            Action _select_color_3 = Action("palette_view.select_color_3");
            Action _select_color_4 = Action("palette_view.select_color_4");
            Action _select_color_5 = Action("palette_view.select_color_5");
            Action _select_color_6 = Action("palette_view.select_color_6");
            Action _select_color_7 = Action("palette_view.select_color_7");
            Action _select_color_8 = Action("palette_view.select_color_8");
            Action _select_color_9 = Action("palette_view.select_color_9");

            ShortcutController _shortcut_controller = ShortcutController(state::app);

            Tooltip _tooltip;
    };
}

//

namespace mousetrap
{
    // COLOR TILE

    PaletteView::ColorTile::ColorTile(PaletteView* owner, HSVA color)
            : _owner(owner), _color(color)
    {
        _color_area.connect_signal_realize(on_color_area_realize, this);
        _selection_frame_area.connect_signal_realize(on_selection_frame_realize, this);

        _color_area.set_size_request(Vector2f(owner->_preview_size));
        _selection_frame_area.set_size_request(Vector2f(owner->_preview_size));

        _overlay.set_child(&_color_area);
        _overlay.add_overlay(&_selection_frame_area);
        _aspect_frame.set_child(&_overlay);

        operator Widget*()->set_align(GTK_ALIGN_CENTER);
        operator Widget*()->set_cursor(GtkCursorType::POINTER);

        set_color(color);
        set_size(_owner->_preview_size);
    }

    void PaletteView::ColorTile::set_color(HSVA color)
    {
        _color = color;
        if (_color_shape != nullptr)
            _color_shape->set_color(color);

        auto round = [](float v) -> std::string {
            auto as_int = int(v * 100);
            if (v == 1)
                return "1.000";
            else
                return "0." + std::to_string(as_int);
        };

        std::stringstream tooltip;
        tooltip << "HSVA (" << round(color.h) << ", " << round(color.v) << ", " << round(color.a) << ")";
        _aspect_frame.set_tooltip_text(tooltip.str());
        _color_area.queue_render();
    }

    void PaletteView::ColorTile::set_size(size_t size)
    {
        _color_area.set_size_request({size, size});
        _selection_frame_area.set_size_request({size, size});
        _aspect_frame.set_size_request({size, size});
    }

    HSVA PaletteView::ColorTile::get_color()
    {
        return _color;
    }

    void PaletteView::ColorTile::set_selected(bool b)
    {
        if (_is_selected_shape != nullptr)
            _is_selected_shape->set_visible(b);

        _selection_frame_area.queue_render();
    }

    PaletteView::ColorTile::operator Widget*()
    {
        return &_aspect_frame;
    }

    void PaletteView::ColorTile::on_color_area_realize(Widget* widget, ColorTile* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->_color_shape = new Shape();
        instance->_color_shape->as_rectangle({0, 0}, {1, 1});
        instance->_color_shape->set_color(instance->_color);

        instance->_frame_shape = new Shape();
        float eps = 0.001;
        instance->_frame_shape->as_wireframe({
            {eps, eps},
            {eps + (1 - eps), eps},
            {(1 - eps), (1 - eps)},
            {eps, (1 - eps)}
        });
        instance->_frame_shape->set_color(RGBA(0, 0, 0, 1));

        area->add_render_task(instance->_color_shape);
        area->add_render_task(instance->_frame_shape);

        area->queue_render();
    }

    void PaletteView::ColorTile::on_selection_frame_realize(Widget* widget, ColorTile* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (is_selected_overlay_texture == nullptr)
        {
            is_selected_overlay_texture = new Texture();
            is_selected_overlay_texture->create_from_file(get_resource_path() + "icons/" + "selected_indicator" + ".png");
        }

        instance->_is_selected_shape = new Shape();
        instance->_is_selected_shape->as_rectangle({0, 0}, {1, 1});
        instance->_is_selected_shape->set_texture(is_selected_overlay_texture);
        instance->_is_selected_shape->set_visible(false);

        area->add_render_task(instance->_is_selected_shape);
        area->queue_render();
    }

    // CONTROL BAR

    void PaletteView::PaletteControlBar::update_palette_locked()
    {
        if (_owner->_palette_locked)
        {
            _palette_not_locked_button.set_child(&_palette_locked_icon);
            _palette_not_locked_button.set_tooltip_text(state::tooltips_file->get_value("palette_view.control_bar", "palette_editing_not_active"));
        }
        else
        {
            _palette_not_locked_button.set_child(&_palette_not_locked_icon);
            _palette_not_locked_button.set_tooltip_text(state::tooltips_file->get_value("palette_view.control_bar", "palette_editing_active"));
        }

        _palette_not_locked_button.set_signal_toggled_blocked(true);
        _palette_not_locked_button.set_active(not _owner->_palette_locked);
        _palette_not_locked_button.set_signal_toggled_blocked(false);

        _palette_locked_switch.set_signal_state_set_blocked(true);
        _palette_locked_switch.set_active(_owner->_palette_locked);
        _palette_locked_switch.set_state(_owner->_palette_locked ? Switch::State::ON : Switch::State::OFF);
        _palette_locked_switch.set_signal_state_set_blocked(false);
    }

    void PaletteView::PaletteControlBar::update_preview_size_changed()
    {
        _preview_size_scale.set_signal_value_changed_blocked(true);
        _preview_size_scale.set_value(_owner->_preview_size);
        _preview_size_scale.set_signal_value_changed_blocked(false);
    }

    void PaletteView::PaletteControlBar::on_palette_not_locked_button_toggled(ToggleButton* button, PaletteControlBar* instance)
    {
        instance->_owner->set_palette_locked(not button->get_active());
    }

    void PaletteView::PaletteControlBar::on_palette_locked_switch_state_set(Switch* button, bool value, PaletteControlBar* instance)
    {
        instance->_owner->set_palette_locked(button->get_active());
    }

    void PaletteView::PaletteControlBar::on_preview_size_scale_value_changed(SpinButton* scale, PaletteControlBar* instance)
    {
        instance->_owner->set_preview_size( scale->get_value());
    }

    PaletteView::PaletteControlBar::operator Widget*()
    {
        return &_main;
    }

    PaletteView::PaletteControlBar::PaletteControlBar(PaletteView* owner)
            : _owner(owner)
    {
        _palette_not_locked_button.set_active(not owner->_palette_locked);
        _palette_not_locked_button.set_child(owner->_palette_locked ? &_palette_locked_icon : &_palette_not_locked_icon);
        _palette_not_locked_button.connect_signal_toggled(on_palette_not_locked_button_toggled, this);

        _palette_locked_icon.set_size_request(_palette_locked_icon.get_size());
        _palette_not_locked_icon.set_size_request(_palette_not_locked_icon.get_size());

        _preview_size_label.set_hexpand(false);
        _preview_size_label.set_halign(GTK_ALIGN_START);
        _preview_size_scale.set_hexpand(true);
        _preview_size_scale.connect_signal_value_changed(on_preview_size_scale_value_changed, this);

        _preview_size_box.push_back(&_preview_size_label);
        _preview_size_box.push_back(&_preview_size_spacer);
        _preview_size_box.push_back(&_preview_size_scale);

        _palette_locked_label.set_hexpand(false);
        _palette_locked_label.set_halign(GTK_ALIGN_START);
        _palette_locked_spacer.set_hexpand(true);
        _palette_locked_switch.set_size_request({state::margin_unit, state::margin_unit});
        _palette_locked_switch.set_halign(GTK_ALIGN_END);
        _palette_locked_switch.connect_signal_state_set(on_palette_locked_switch_state_set, this);

        _palette_locked_box.push_back(&_palette_locked_label);
        _palette_locked_box.push_back(&_palette_locked_spacer);
        _palette_locked_box.push_back(&_palette_locked_switch);

        _palette_locked_box.set_tooltip_text(state::tooltips_file->get_value_as<std::string>("palette_view.control_bar", "palette_editing_menu_item"));
        _preview_size_box.set_tooltip_text(state::tooltips_file->get_value_as<std::string>("palette_view.control_bar", "preview_size_menu_item"));

        update_palette_locked();
        update_preview_size_changed();

        for (auto* box : {&_palette_locked_box, &_preview_size_box})
            box->set_margin(state::margin_unit);

        for (auto* spacer : {&_palette_locked_spacer, &_preview_size_spacer})
        {
            spacer->set_opacity(0);
            spacer->set_size_request({state::margin_unit, 0});
        }

        auto settings_section = MenuModel();

        auto preview_size_submenu = MenuModel();
        preview_size_submenu.add_widget(&_preview_size_box);

        auto palette_locked_submenu = MenuModel();
        palette_locked_submenu.add_widget(&_palette_locked_box);

        settings_section.add_submenu("Preview Size...", &preview_size_submenu);
        settings_section.add_submenu("Editing...", &palette_locked_submenu);
        _menu.add_section("Settings", &settings_section);

        auto load_save_submenu = MenuModel();
        load_save_submenu.add_action("Load...", "palette_view.load");
        load_save_submenu.add_action("Load Default", "palette_view.load_default");
        load_save_submenu.add_action("Save", "palette_view.save");
        load_save_submenu.add_action("Save As...", "palette_view.save_as");
        load_save_submenu.add_action("Save As Default", "palette_view.save_as_default");
        _menu.add_section("Load / Save", &load_save_submenu);

        auto sort_by_section = MenuModel();
        auto sort_by_submenu = MenuModel();
        sort_by_submenu.add_action("None", "palette_view.sort_by_default");
        sort_by_submenu.add_action("Hue", "palette_view.sort_by_hue");
        sort_by_submenu.add_action("Saturation", "palette_view.sort_by_saturation");
        sort_by_submenu.add_action("Value", "palette_view.sort_by_value");
        sort_by_section.add_submenu("Sort By...", &sort_by_submenu);
        _menu.add_section("Sorting", &sort_by_section);

        auto* popover_menu = new PopoverMenu(&_menu);
        _menu_button.set_popover(popover_menu);
        _menu_button.set_child(&_menu_button_label);

        _menu_button_label.set_hexpand(true);
        _menu_button_label.set_halign(GTK_ALIGN_START);
        _menu_button.set_hexpand(true);
        _palette_not_locked_button.set_hexpand(false);

        //_menu_button.set_cursor(GtkCursorType::POINTER);
        //_palette_editing_active_toggle_button.set_cursor(GtkCursorType::POINTER);

        _main.push_back(&_menu_button);
        _main.push_back(&_palette_not_locked_button);
    }

    void PaletteView::PaletteControlBar::update()
    {
        update_palette_locked();
        update_preview_size_changed();
    }

    // PALETTE VIEW

    PaletteView::operator Widget*()
    {
        return &_main;
    }

    void PaletteView::on_color_tile_view_selection_model_selection_changed(SelectionModel*, size_t child_i, size_t n_items, PaletteView* instance)
    {
        instance->_color_tiles.at(instance->_selected_i)->set_selected(false);
        instance->_color_tiles.at(child_i)->set_selected(true);
        instance->_selected_i = child_i;

        state::primary_color = instance->_color_tiles.at(child_i)->get_color();
        state::preview_color_current = state::primary_color;
        state::preview_color_previous = state::primary_color;

        state::update_color_picker();
        state::update_verbose_color_picker();
        state::update_color_swapper();
        state::update_color_preview();
        state::update_canvas();
    }

    void PaletteView::load_as_debug()
    {
        std::vector<HSVA> colors;

        const size_t n_steps = 8;
        for (size_t i = 0; i < n_steps; ++i)
        {
            float h = i / float(n_steps);

            for (float s : {0.33f, 0.66f, 1.f})
                for (float v : {0.33f, 0.66f, 1.f})
                    colors.push_back(HSVA(h, s, v, 1));
        }

        for (size_t i = 0; i < n_steps; ++i)
            colors.push_back(HSVA(0, 0, i / float(n_steps), 1));

        state::palette = Palette(colors);
        update_from_palette();
    }

    void PaletteView::load_from_file(const std::string& path)
    {
        auto& palette = state::palette;
        auto palette_backup = palette;
        if (not palette.load_from(path))
        {
            palette = palette_backup;
            ((BubbleLogArea*) state::bubble_log)->send_message("Unable to load palette at \"" + path + "\"");
            return;
        }
        update_from_palette();
    }

    void PaletteView::save_to_file(const std::string& path)
    {
        auto& palette = state::palette;
        if (palette.save_to(path))
            ((BubbleLogArea*) state::bubble_log)->send_message("Saved current palette as \"" + path + "\"");
        else
            ((BubbleLogArea*) state::bubble_log)->send_message("Unable to save palette as \"" + path + "\"", InfoMessageType::ERROR);
    }

    void PaletteView::on_load_ok_pressed()
    {
        auto path = _on_load_dialog.get_current_name();
        load_from_file(path);
        _on_load_dialog.close();
    }

    void PaletteView::on_save_as_ok_pressed()
    {
        auto path = _on_save_as_dialog.get_current_name();
        save_to_file(path);
        _on_save_as_dialog.close();
    }

    void PaletteView::update_from_palette()
    {
        _color_tile_view.clear();

        for (auto* tile : _color_tiles)
            tile->set_color(HSVA(0, 0, 0, 1));

        auto& palette = state::palette;
        auto sort_mode = state::palette_sort_mode;

        for (size_t i = 0; palette.get_n_colors() > _color_tiles.size() and i < palette.get_n_colors() - _color_tiles.size(); ++i)
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 1)));

        std::vector<HSVA> colors;
        if (sort_mode == PaletteSortMode::NONE)
            colors = palette.get_colors();
        else if (sort_mode == PaletteSortMode::BY_HUE)
            colors = palette.get_colors_by_hue();
        else if (sort_mode == PaletteSortMode::BY_VALUE)
            colors = palette.get_colors_by_value();
        else if (sort_mode == PaletteSortMode::BY_SATURATION)
            colors = palette.get_colors_by_saturation();

        for (size_t i = 0; i < colors.size(); ++i)
        {
            auto color = colors.at(i);
            color.a = 1;
            _color_tiles.at(i)->set_color(color);
            _color_tile_view.push_back(_color_tiles.at(i)->operator Widget*());
        }

        if (colors.size() == 0)
        {
            _color_tiles.at(0)->set_color(HSVA(0, 0, 0, 1));
            _color_tile_view.push_back(_color_tiles.at(0)->operator Widget*());
        }
    }

    PaletteView::PaletteView()
    {
        for (size_t i = 0; i < 256; ++i) // pre allocate to reduce load time
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 0)));

        for (auto* tile : _color_tiles)
            _color_tile_view.push_back(tile->operator Widget*());

        auto path = state::settings_file->get_value_as<std::string>("palette_view", "default_palette");
        if (path == "DEBUG")
            load_as_debug();
        else
            load_from_file(path);

        update_from_palette();

        _scrolled_window.set_child(&_color_tile_view);
        _scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
        _palette_view_box.push_back(&_scrolled_window);

        _scrolled_window.set_hexpand(true);
        _palette_view_box.set_hexpand(true);

        _color_tile_view.get_selection_model()->connect_signal_selection_changed(on_color_tile_view_selection_model_selection_changed, this);
        _color_tile_view.get_selection_model()->unselect_all();

        _color_tiles.at(_selected_i)->set_selected(true);

        _palette_control_bar.operator Widget *()->set_vexpand(false);
        _palette_view_box.set_vexpand(true);
        _palette_view_box.set_size_request({0, _color_tiles.at(0)->operator Widget *()->get_preferred_size().natural_size.y + 4});

        _main.push_back(_palette_control_bar);
        _main.push_back(&_palette_view_box);
        _main.set_vexpand(true);

        _palette_control_bar.update();

        FileFilter palette_filer = FileFilter("Mousetrap Palette File");
        palette_filer.add_allowed_suffix("palette");

        // ACTION: load
        _on_load_dialog.get_file_chooser().add_filter(palette_filer, true);

        _on_load_dialog.set_on_accept_pressed([](OpenFileDialog*, PaletteView* instance){
            instance->on_load_ok_pressed();
            instance->_on_load_dialog.close();
        }, this);

        _on_load_dialog.set_on_cancel_pressed([](OpenFileDialog*, PaletteView* instance){
            instance->_on_load_dialog.close();
        }, this);

        _on_load_action.set_do_function([](PaletteView* instance){
            instance->_on_load_dialog.show();
        }, this);

        state::app->add_action(_on_load_action);

        // ACTION: save as
        _on_save_as_dialog.get_file_chooser().add_filter(palette_filer, true);

        _on_save_as_dialog.set_on_accept_pressed([](SaveAsFileDialog*, PaletteView* instance){
            instance->on_save_as_ok_pressed();
            instance->_on_save_as_dialog.close();
        }, this);

        _on_save_as_dialog.set_on_cancel_pressed([](SaveAsFileDialog*, PaletteView* instance){
            instance->_on_save_as_dialog.close();
        }, this);

        _on_save_as_action.set_do_function([](PaletteView* instance){
            instance->_on_save_as_dialog.get_name_entry().set_text("Untitled.palette");
            instance->_on_save_as_dialog.show();
        }, this);

        state::app->add_action(_on_save_as_action);

        // Action:: Save

        _on_save_action.set_do_function([](PaletteView* instance){
            if (instance->_save_to_path == "")
            {
                instance->_on_save_as_dialog.get_name_entry().set_text("Untitled.palette");
                instance->_on_save_as_dialog.show();
            }
            else
                instance->save_to_file(instance->_save_to_path);
        }, this);

        state::app->add_action(_on_save_action);

        // Action: Load Default
        _on_load_default_action.set_do_function([](PaletteView* instance)
        {
            instance->load_from_file(get_resource_path() + "default.palette");
        }, this);

        state::app->add_action(_on_load_default_action);

        // Action: Save As Default

        _on_save_as_default_action.set_do_function([](PaletteView* instance)
        {
            if (state::palette.save_to(get_resource_path() + "default.palette"))
                ((BubbleLogArea*) state::bubble_log)->send_message("Current palette saved as default");
            else
                ((BubbleLogArea*) state::bubble_log)->send_message("Unable to save current palette as default");
        }, this);

        state::app->add_action(_on_save_as_default_action);

        // Action:: Sort

        _on_sort_by_default_action.set_do_function([](PaletteView* instance){
            state::palette_sort_mode = PaletteSortMode::NONE;
            instance->update_from_palette();
        }, this);

        _on_sort_by_hue_action.set_do_function([](PaletteView* instance){
            state::palette_sort_mode = PaletteSortMode::BY_HUE;
            instance->update_from_palette();
        }, this);

        _on_sort_by_value_action.set_do_function([](PaletteView* instance){
            state::palette_sort_mode = PaletteSortMode::BY_VALUE;
            instance->update_from_palette();
        }, this);

        _on_sort_by_saturation_action.set_do_function([](PaletteView* instance){
            state::palette_sort_mode = PaletteSortMode::BY_SATURATION;
            instance->update_from_palette();
        }, this);

        for (auto* action : {&_on_sort_by_default_action, &_on_sort_by_hue_action, &_on_sort_by_value_action, &_on_sort_by_saturation_action})
            state::app->add_action(*action);

        // on startup: make first color current color
        operator Widget*()->add_tick_callback([](FrameClock, PaletteView* instance) -> bool
        {
            instance->select(0);
            return false;
        }, this);

        // keybinding

        std::vector<Action*> select_color_actions = {
            &_select_color_0,
            &_select_color_1,
            &_select_color_2,
            &_select_color_3,
            &_select_color_4,
            &_select_color_5,
            &_select_color_6,
            &_select_color_7,
            &_select_color_8,
            &_select_color_9
        };

        for (size_t i = 0; i < select_color_actions.size(); ++i)
        {
            auto* action = select_color_actions.at(i);
            action->set_do_function([index = i](PaletteView* instance){
                instance->select(index);
            }, this);

            auto shortcut = state::keybindings_file->get_value_as<std::string>("palette_view", "select_color_" + std::to_string(i));
            action->add_shortcut(shortcut);
            state::app->add_action(*action);
            _shortcut_controller.add_action(action->get_id());
        }

        _shortcut_controller.set_scope(ShortcutScope::GLOBAL);
        _shortcut_controller.set_propagation_phase(GTK_PHASE_BUBBLE);

        operator Widget*()->add_controller(&_shortcut_controller);

        _tooltip.create_from("palette_view", state::tooltips_file, state::keybindings_file);
        operator Widget*()->set_tooltip_widget(_tooltip.operator Widget*());
    }

    void PaletteView::update()
    {
        auto color = state::primary_color;

        _palette_control_bar.update();

        if (_palette_locked)
        {
            _color_tile_view.get_selection_model()->unselect_all();

            for (size_t i = 0; i < _color_tiles.size(); ++i)
            {
                auto* tile = _color_tiles.at(i);
                bool should_select = tile->get_color() == color;
                tile->set_selected(should_select);

                if (should_select)
                    select(i);
            }

            return;
        }

        color.a = 1;
        _color_tiles.at(_selected_i)->set_color(color);
    }

    void PaletteView::reload()
    {
        update_from_palette();
    }

    void PaletteView::select(size_t i)
    {
        if (i > _color_tiles.size())
            i = _color_tiles.size() - 1;

        _color_tile_view.get_selection_model()->select(i);
        on_color_tile_view_selection_model_selection_changed(_color_tile_view.get_selection_model(), i, 1, this);
    }

    void PaletteView::set_preview_size(size_t x)
    {
        _preview_size = x;
        for (auto* tile : _color_tiles)
            tile->set_size(x);
    }

    size_t PaletteView::get_preview_size() const
    {
        return _preview_size;
    }

    void PaletteView::set_palette_locked(bool b)
    {
        _palette_locked = b;
        _palette_control_bar.update();
    }

    bool PaletteView::get_palette_locked() const
    {
        return _palette_locked;
    }
}
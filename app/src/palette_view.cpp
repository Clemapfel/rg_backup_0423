//
// Created by clem on 1/21/23.
//

#include <app/palette_view.hpp>
#include <app/add_shortcut_action.hpp>
#include <app/bubble_log_area.hpp>

namespace mousetrap
{
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

    PaletteView::ColorTile::~ColorTile()
    {
        delete _color_shape;
        delete _frame_shape;
        delete _is_selected_shape;
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
        _selected = b;

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

        instance->set_selected(instance->_selected);
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
        instance->_is_selected_shape->set_visible(instance->_selected);

        area->add_render_task(instance->_is_selected_shape);
        area->queue_render();
    }

    PaletteView::PaletteControlBar::PaletteControlBar(PaletteView* owner)
            : _owner(owner)
    {
        set_palette_editing_enabled(active_state->get_palette_editing_enabled());
        _palette_locked_button.connect_signal_toggled(on_palette_locked_button_toggled, this);

        _palette_locked_icon.set_size_request(_palette_locked_icon.get_size());
        _palette_not_locked_icon.set_size_request(_palette_not_locked_icon.get_size());

        _preview_size_label.set_hexpand(false);
        _preview_size_label.set_halign(GTK_ALIGN_START);
        _preview_size_scale.set_hexpand(true);
        _preview_size_scale.set_value(_owner->_preview_size);
        _preview_size_scale.connect_signal_value_changed(on_preview_size_scale_value_changed, this);

        _preview_size_box.push_back(&_preview_size_label);
        _preview_size_box.push_back(&_preview_size_spacer);
        _preview_size_box.push_back(&_preview_size_scale);
        _preview_size_box.set_tooltip_text(state::tooltips_file->get_value_as<std::string>("palette_view.control_bar", "preview_size_menu_item"));

        _preview_size_box.set_margin(state::margin_unit);
        _preview_size_spacer.set_opacity(0);
        _preview_size_spacer.set_size_request({state::margin_unit, 0});

        auto settings_section = MenuModel();

        auto preview_size_submenu = MenuModel();
        preview_size_submenu.add_widget(&_preview_size_box);

        using namespace state::actions;

        auto tooltip = [](const std::string& value) {
            return state::tooltips_file->get_value_as<std::string>("palette_view", value);
        };

        settings_section.add_stateful_action(tooltip("toggle_palette_locked"), palette_view_toggle_palette_locked.get_id(), active_state->get_palette_editing_enabled());
        settings_section.add_submenu("Preview Size...", &preview_size_submenu);
        _menu.add_section("Settings", &settings_section);

        auto load_save_submenu = MenuModel();
        load_save_submenu.add_action(tooltip("load"), palette_view_load.get_id());
        load_save_submenu.add_action(tooltip("load_default"), palette_view_load_default.get_id());
        load_save_submenu.add_action(tooltip("save"), palette_view_save.get_id());
        load_save_submenu.add_action(tooltip("save_as"), palette_view_save_as.get_id());
        load_save_submenu.add_action(tooltip("save_as_default"), palette_view_save_as_default.get_id());
        _menu.add_section("Load / Save", &load_save_submenu);

        auto sort_by_section = MenuModel();
        auto sort_by_submenu = MenuModel();
        sort_by_submenu.add_action(tooltip("sort_by_default"), palette_view_sort_by_default.get_id());
        sort_by_submenu.add_action(tooltip("sort_by_hue"), palette_view_sort_by_hue.get_id());
        sort_by_submenu.add_action(tooltip("sort_by_saturation"), palette_view_sort_by_saturation.get_id());
        sort_by_submenu.add_action(tooltip("sort_by_value"), palette_view_sort_by_value.get_id());
        sort_by_section.add_submenu("Sort By...", &sort_by_submenu);
        _menu.add_section("Sorting", &sort_by_section);

        auto* popover_menu = new PopoverMenu(&_menu);
        _menu_button.set_popover(popover_menu);
        _menu_button.set_child(&_menu_button_label);

        _menu_button_label.set_hexpand(true);
        _menu_button_label.set_halign(GTK_ALIGN_START);
        _menu_button.set_hexpand(true);
        _palette_locked_button.set_hexpand(false);

        _main.push_back(&_menu_button);
        _main.push_back(&_palette_locked_button);
    }

    void PaletteView::PaletteControlBar::set_palette_editing_enabled(bool is_editing_enabled)
    {
        auto is_locked = not is_editing_enabled;

        if (is_locked)
        {
            _palette_locked_button.set_child(&_palette_locked_icon);
            _palette_locked_button.set_tooltip_text(state::tooltips_file->get_value("palette_view.control_bar", "palette_editing_not_active"));
        }
        else
        {
            _palette_locked_button.set_child(&_palette_not_locked_icon);
            _palette_locked_button.set_tooltip_text(state::tooltips_file->get_value("palette_view.control_bar", "palette_editing_active"));
        }

        _palette_locked_button.set_signal_toggled_blocked(true);
        _palette_locked_button.set_active(is_locked);
        _palette_locked_button.set_signal_toggled_blocked(false);
    }

    void PaletteView::PaletteControlBar::on_palette_locked_button_toggled(ToggleButton* button, PaletteControlBar* instance)
    {
        active_state->set_palette_editing_enabled(not button->get_active());
    }

    void PaletteView::PaletteControlBar::on_preview_size_scale_value_changed(SpinButton* scale, PaletteControlBar* instance)
    {
        instance->_owner->set_preview_size(scale->get_value());
    }

    PaletteView::PaletteControlBar::operator Widget*()
    {
        return &_main;
    }

    PaletteView::PaletteFileSelectOpen::PaletteFileSelectOpen(PaletteView* owner)
        : _owner(owner), _dialog("Load Palette...")
    {
        _dialog.set_on_accept_pressed([](FileChooserDialog<FileChooserDialogMode::OPEN>* instance)
        {
            auto path = instance->get_current_name();
            auto palette = Palette({HSVA(0, 0, 0, 1)});

            if (palette.load_from(path))
            {
                auto& backup = state::actions::detail::palette_view_load_undo_backup.emplace_back();
                for (auto* tile : state::palette_view->_color_tiles)
                    backup.push_back(tile->get_color());

                active_state->set_palette(palette.get_colors());
            }
            else
                state::bubble_log->send_message("Unable to load palette at `" + path + "`: " + state::tooltips_file->get_value("palette_view", "on_palette_load_error"), InfoMessageType::ERROR);

            instance->close();
        });

        _dialog.set_on_cancel_pressed([](FileChooserDialog<FileChooserDialogMode::OPEN>* instance)
        {
            instance->close();
        });

        auto filter = FileFilter(".palette");
        filter.add_allowed_suffix("palette");
        _dialog.get_file_chooser().add_filter(filter);
    }

    void PaletteView::PaletteFileSelectOpen::show()
    {
        _dialog.show();
    }

    PaletteView::PaletteFileSelectSave::PaletteFileSelectSave(PaletteView* owner)
        : _owner(owner), _dialog("Save Palette...")
    {
        _dialog.set_on_accept_pressed([](SaveAsFileDialog* instance)
        {
            state::palette_view->save_palette_to_file(instance->get_current_name());
            instance->close();
        });

        _dialog.set_on_cancel_pressed([](SaveAsFileDialog* instance)
        {
            instance->close();
        });

        auto filter = FileFilter(".palette");
        filter.add_allowed_suffix("palette");
        _dialog.get_file_chooser().add_filter(filter, true);
    }

    void PaletteView::PaletteFileSelectSave::show()
    {
        _dialog.show();
    }

    void PaletteView::save_palette_to_file(const std::string& path)
    {
        if (active_state->get_palette().save_to(path))
        {
            state::bubble_log->send_message("Wrote current palette to `" + path + "`");
            _palette_file_save_path = path;
        }
        else
            state::bubble_log->send_message("Unable to write current palette to `" + path + "`", InfoMessageType::ERROR);
    }

    void PaletteView::on_palette_updated()
    {
        auto sort_mode = active_state->get_palette_sort_mode();

        std::vector<HSVA> colors = {};

        if (sort_mode == PaletteSortMode::NONE)
            colors = active_state->get_palette().get_colors();
        else if (sort_mode == PaletteSortMode::BY_HUE)
            colors = active_state->get_palette().get_colors_by_hue();
        else if (sort_mode == PaletteSortMode::BY_SATURATION)
            colors = active_state->get_palette().get_colors_by_saturation();
        else if (sort_mode == PaletteSortMode::BY_VALUE)
            colors = active_state->get_palette().get_colors_by_value();

        while (_color_tiles.size() < colors.size())
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 0)));

        _color_tile_view.clear();
        for (size_t i = 0; i < _color_tiles.size(); ++i)
        {
            auto* tile = _color_tiles.at(i);

            if (i >= colors.size())
            {
                tile->operator Widget*()->set_visible(false);
                continue;
            }

            tile->operator Widget*()->set_visible(true);
            tile->set_color(colors.at(i));
            tile->set_selected(colors_equal(colors.at(i), active_state->get_primary_color()));
            _color_tile_view.push_back(tile->operator Widget*());
        }
    }

    void PaletteView::on_color_selection_changed()
    {
        if (active_state->get_palette_editing_enabled())
        {
            _color_tile_view.get_selection_model()->select(_selected_i);

            for (auto* tile : _color_tiles)
                tile->set_selected(false);

            _color_tiles.at(_selected_i)->set_color(active_state->get_primary_color());
            _color_tiles.at(_selected_i)->set_selected(true);
            return;
        }

        bool matched_once = false;
        for (size_t i = 0; i < _color_tiles.size(); ++i)
        {
            auto* tile = _color_tiles.at(i);
            if (not tile->operator Widget*()->get_visible())
                break;

            auto matched = colors_equal(tile->get_color(), active_state->get_primary_color());
            tile->set_selected(matched);

            if (matched)
            {
                _selected_i = i;
                matched_once = true;
            }
        }

        if (matched_once)
            _color_tile_view.get_selection_model()->select(_selected_i);
        else
        {
            _color_tile_view.get_selection_model()->unselect_all();
        }
    }

    void PaletteView::on_palette_sort_mode_changed()
    {
        on_palette_updated();
    }

    void PaletteView::on_palette_editing_toggled()
    {
        auto state = active_state->get_palette_editing_enabled();
        _palette_control_bar.set_palette_editing_enabled(state);
        state::actions::palette_view_toggle_palette_locked.set_state(state);
    }

    PaletteView::PaletteView()
    {
        for (size_t i = 0; i < 256; ++i) // pre allocate to reduce load time
            _color_tiles.emplace_back(new ColorTile(this, HSVA(0, 0, 0, 0)));

        on_palette_updated();

        _scrolled_window.set_child(&_color_tile_view);
        _scrolled_window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_EXTERNAL);
        _palette_view_box.push_back(&_scrolled_window);

        _scrolled_window.set_hexpand(true);
        _palette_view_box.set_hexpand(true);

        _color_tile_view.get_selection_model()->connect_signal_selection_changed(on_color_tile_view_selection_model_selection_changed, this);
        _color_tile_view.get_selection_model()->unselect_all();

        _palette_control_bar.operator Widget *()->set_vexpand(false);
        _palette_view_box.set_vexpand(true);
        _palette_view_box.set_size_request({0, _color_tiles.at(0)->operator Widget *()->get_preferred_size().natural_size.y + 4});

        _main.push_back(_palette_control_bar);
        _main.push_back(&_palette_view_box);
        _main.set_vexpand(true);

        on_color_selection_changed();

        using namespace state::actions;

        palette_view_select_color_0.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(0)->get_color());
        });

        palette_view_select_color_1.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(1)->get_color());
        });

        palette_view_select_color_2.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(2)->get_color());
        });

        palette_view_select_color_3.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(3)->get_color());
        });

        palette_view_select_color_4.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(4)->get_color());
        });

        palette_view_select_color_5.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(5)->get_color());
        });

        palette_view_select_color_6.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(6)->get_color());
        });

        palette_view_select_color_7.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(7)->get_color());
        });

        palette_view_select_color_8.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(8)->get_color());
        });

        palette_view_select_color_9.set_function([](){
            active_state->set_primary_color(state::palette_view->_color_tiles.at(9)->get_color());
        });

        palette_view_sort_by_default.set_function([](){
           active_state->set_palette_sort_mode(PaletteSortMode::NONE);
        });

        palette_view_sort_by_hue.set_function([](){
            active_state->set_palette_sort_mode(PaletteSortMode::BY_HUE);
        });

        palette_view_sort_by_saturation.set_function([](){
            active_state->set_palette_sort_mode(PaletteSortMode::BY_SATURATION);
        });

        palette_view_sort_by_value.set_function([](){
            active_state->set_palette_sort_mode(PaletteSortMode::BY_VALUE);
        });

        palette_view_load.set_function( [](){
            state::palette_view->_palette_file_select_open.show();
        });

        palette_view_save.set_function([]()
        {
            auto path = state::palette_view->_palette_file_save_path;
            if (path.empty())
               palette_view_save_as.activate();
            else
               state::palette_view->save_palette_to_file(path);
        });

        palette_view_save_as.set_function([](){
            state::palette_view->_palette_file_select_save.show();
        });

        palette_view_load_default.set_function([]
        {
           auto palette = active_state->get_default_palette();
           active_state->set_palette(palette.get_colors());
        });

        palette_view_save_as_default.set_function([]
        {
            std::vector<HSVA> colors;
            for (auto* tile : state::palette_view->_color_tiles)
                if (tile->operator Widget*()->get_visible())
                    colors.push_back(tile->get_color());

            active_state->set_default_palette(colors);
        });

        palette_view_toggle_palette_locked.set_stateful_function([](bool) -> bool
        {
           auto next = not active_state->get_palette_editing_enabled();
           active_state->set_palette_editing_enabled(next);
           return next;
        });

        for (auto* action : {
                &palette_view_load_default,
                &palette_view_save,
                &palette_view_save_as_default,
                &palette_view_load,
                &palette_view_save_as,
                &palette_view_sort_by_default,
                &palette_view_sort_by_hue,
                &palette_view_sort_by_saturation,
                &palette_view_sort_by_value,
                &palette_view_select_color_0,
                &palette_view_select_color_1,
                &palette_view_select_color_2,
                &palette_view_select_color_3,
                &palette_view_select_color_4,
                &palette_view_select_color_5,
                &palette_view_select_color_6,
                &palette_view_select_color_7,
                &palette_view_select_color_8,
                &palette_view_select_color_9,
                &palette_view_toggle_palette_locked
        })
            state::add_shortcut_action(*action);

        on_palette_editing_toggled();
    }

    PaletteView::~PaletteView()
    {
        for (auto* tile : _color_tiles)
            delete tile;
    }

    PaletteView::operator Widget*()
    {
        return &_main;
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

    void PaletteView::on_color_tile_view_selection_model_selection_changed(SelectionModel*, size_t child_i, size_t n_items, PaletteView* instance)
    {
        instance->_selected_i = child_i;
        auto color = instance->_color_tiles.at(child_i)->get_color();
        color.a = active_state->get_primary_color().a;
        active_state->set_primary_color(color);
        active_state->set_preview_colors(color, color);
    }

    bool PaletteView::colors_equal(HSVA a, HSVA b)
    {
        return (int(a.h * 255) == int(b.h * 255)) and
            (int(a.s * 255) == int(b.s * 255)) and
            (int(a.v * 255) == int(b.v * 255));
    }
}
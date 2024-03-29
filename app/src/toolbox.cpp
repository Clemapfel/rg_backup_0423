//
// Created by clem on 1/24/23.
//

#include <app/toolbox.hpp>
#include <app/project_state.hpp>
#include <app/add_shortcut_action.hpp>

namespace mousetrap
{
    Toolbox::ToolIcon::ToolIcon(const std::string& image_id, Toolbox* owner)
        : _tool_icon(get_resource_path() + "icons/" + image_id + ".png"), _owner(owner)
    {
        for (auto* w : {&_has_popover_indicator_icon, &_child_selected_indicator_icon, &_selected_indicator_icon, &_tool_icon})
        {
            w->set_size_request(w->get_size());
            w->set_expand(false);
            w->set_align(GTK_ALIGN_CENTER);
        }

        _overlay.set_child(&_selected_indicator_icon);
        _overlay.add_overlay(&_tool_icon);
        _overlay.add_overlay(&_has_popover_indicator_icon);
        _overlay.add_overlay(&_child_selected_indicator_icon);

        set_selection_indicator_visible(false);
        set_child_selection_indicator_visible(false);
        set_has_popover_indicator_visible(false);

        _main.set_child(&_overlay);

        _click_event_controller.connect_signal_click_pressed([](ClickEventController*, size_t, double, double, ToolIcon* instance) {
            if (instance->_on_click)
                instance->_on_click();
        }, this);
        _main.add_controller(&_click_event_controller);
    }

    Toolbox::ToolIcon::operator Widget*()
    {
        return &_main;
    }

    void Toolbox::ToolIcon::set_has_popover_indicator_visible(bool b)
    {
        _has_popover_indicator_icon.set_opacity(b ? 1 : 0);
    }

    void Toolbox::ToolIcon::set_selection_indicator_visible(bool b)
    {
        _selected_indicator_icon.set_opacity(b ? 1 : 0);
    }

    void Toolbox::ToolIcon::set_child_selection_indicator_visible(bool b)
    {
        _child_selected_indicator_icon.set_opacity(b ? 1 : 0);
    }

    Toolbox::ClickForPopover::ClickForPopover()
    {
        _popover.attach_to(&_label_box);
        _popover.set_child(&_popover_box);

        _label_click_controller.connect_signal_click_pressed([](ClickEventController*, size_t, double, double, ClickForPopover* instance) {
            instance->_popover.popup();
        }, this);
        _label_box.add_controller(&_label_click_controller);

        _popover_click_controller.connect_signal_click_pressed([](ClickEventController*, size_t, double, double, ClickForPopover* instance) {
            //instance->_popover.popdown();
        }, this);
        _popover_box.add_controller(&_popover_click_controller);
    }

    Box& Toolbox::ClickForPopover::get_label_box()
    {
        return _label_box;
    }

    Box& Toolbox::ClickForPopover::get_popover_box()
    {
        return _popover_box;
    }

    Toolbox::ClickForPopover::operator Widget*()
    {
        return &_label_box;
    }

    Toolbox::Toolbox()
    {
        for (auto id : {
            MOVE_SELECTION,
            MARQUEE_NEIGHBORHODD_SELECT,
            MARQUEE_RECTANGLE,
            MARQUEE_CIRCLE,
            MARQUEE_POLYGON,
            BRUSH,
            ERASER,
            COLOR_SELECT,
            BUCKET_FILL,
            LINE,
            RECTANGLE_OUTLINE,
            RECTANGLE_FILL,
            CIRCLE_OUTLINE,
            CIRCLE_FILL,
            POLYGON_OUTLINE,
            POLYGON_FILL,
            GRADIENT
        })
        {
            auto* inserted = _id_to_icons.insert({id, new ToolIcon(tool_id_to_string(id), this)}).first->second;
            inserted->set_on_click([id = id](){
                active_state->set_current_tool(id);
            });
        }

        _filled_shapes_popover.get_label_box().push_back(_filled_shapes_icon);
        for (auto id : {ToolID::RECTANGLE_FILL, ToolID::CIRCLE_FILL, ToolID::POLYGON_FILL})
            _filled_shapes_popover.get_popover_box().push_back(_id_to_icons.at(id)->operator Widget*());

        _outline_shapes_popover.get_label_box().push_back(_outline_shapes_icon);
        for (auto id : {ToolID::RECTANGLE_OUTLINE, ToolID::CIRCLE_OUTLINE, ToolID::POLYGON_OUTLINE})
            _outline_shapes_popover.get_popover_box().push_back(_id_to_icons.at(id)->operator Widget*());

        auto add_tooltip = [](ToolIcon* to, ToolID id_in, Toolbox* instance)
        {
            auto id = tool_id_to_string(id_in);
            auto& tooltip = instance->_tooltips.emplace_back();
            tooltip.set_title(state::tooltips_file->get_value("toolbox." + id, "title"));
            tooltip.set_description(state::tooltips_file->get_value("toolbox." + id, "description"));

            auto value = state::keybindings_file->get_value("toolbox", "select_" + id);

            if (value != "never")
                tooltip.add_shortcut(value, state::keybindings_file->get_comment_above("toolbox", "select_" + id));

            to->operator Widget*()->set_tooltip_widget(tooltip);
        };

        for (auto& pair : _id_to_icons)
            add_tooltip(pair.second, pair.first, this);

        auto add_compound_tooltip = [](ClickForPopover* popover, const std::string& compound_id, const std::vector<ToolID>& child_ids, Toolbox* instance){

            auto& tooltip = instance->_tooltips.emplace_back();
            tooltip.set_title(state::tooltips_file->get_value("toolbox." + compound_id, "title"));
            tooltip.set_description(state::tooltips_file->get_value("toolbox." + compound_id, "description"));

            for (auto child_id : child_ids)
            {
                auto id = tool_id_to_string(child_id);

                auto value = state::keybindings_file->get_value("toolbox", "select_" + id);
                if (value != "never")
                    tooltip.add_shortcut(value, state::keybindings_file->get_comment_above("toolbox", "select_" + id));
            }

            popover->get_label_box().set_tooltip_widget(tooltip);
        };

        add_compound_tooltip(
            &_filled_shapes_popover, "shapes_fill", {RECTANGLE_FILL, CIRCLE_FILL, POLYGON_FILL}, this
        );

        add_compound_tooltip(
            &_outline_shapes_popover, "shapes_outline", {RECTANGLE_OUTLINE, CIRCLE_OUTLINE, POLYGON_OUTLINE}, this
        );

        _list_view.push_back(*_id_to_icons.at(MOVE_SELECTION));
        _list_view.push_back(*_id_to_icons.at(MARQUEE_NEIGHBORHODD_SELECT));
        _list_view.push_back(*_id_to_icons.at(MARQUEE_RECTANGLE));
        _list_view.push_back(*_id_to_icons.at(MARQUEE_CIRCLE));
        _list_view.push_back(*_id_to_icons.at(MARQUEE_POLYGON));
        _list_view.push_back(*_id_to_icons.at(BRUSH));
        _list_view.push_back(*_id_to_icons.at(ERASER));
        _list_view.push_back(*_id_to_icons.at(BUCKET_FILL));
        _list_view.push_back(*_id_to_icons.at(COLOR_SELECT));
        _list_view.push_back(*_id_to_icons.at(LINE));

        _list_view.push_back(_outline_shapes_popover);
        _list_view.push_back(_filled_shapes_popover);

        _list_view.push_back(*_id_to_icons.at(GRADIENT));

        _list_view.get_selection_model()->connect_signal_selection_changed([](SelectionModel*, size_t i, size_t, Toolbox* instance)
        {
            for (auto& pair : instance->_id_to_listview_positions)
            {
                std::cout << tool_id_to_string(pair.first) << std::endl;
                if (pair.second == i)
                {
                    active_state->set_current_tool(pair.first);
                    return;
                }
            }
        }, this);

        _id_to_listview_positions = {
                {MOVE_SELECTION, 0},
                {MARQUEE_NEIGHBORHODD_SELECT, 1},

                {MARQUEE_RECTANGLE, 2},
                {MARQUEE_CIRCLE, 3},
                {MARQUEE_POLYGON, 4},

                {BRUSH, 5},
                {ERASER, 6},
                {BUCKET_FILL, 7},
                {COLOR_SELECT, 8},
                {LINE, 9},

                {RECTANGLE_OUTLINE, 10},
                {CIRCLE_OUTLINE, 10},
                {POLYGON_OUTLINE, 10},

                {CIRCLE_FILL, 11},
                {RECTANGLE_FILL, 11},
                {POLYGON_FILL, 11},

                {GRADIENT, 12}
        };

        _main_spacer_left.set_expand(true);
        _main_spacer_left.set_halign(GTK_ALIGN_START);
        _list_view.set_hexpand(false);
        _list_view.set_halign(GTK_ALIGN_CENTER);
        _main_spacer_right.set_expand(true);
        _main_spacer_right.set_halign(GTK_ALIGN_END);

        _main.push_back(&_main_spacer_left);
        _main.push_back(&_list_view);
        _main.push_back(&_main_spacer_right);

        _scrolled_window.set_child(&_main);
        _scrolled_window.set_policy(GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
        _scrolled_window.set_propagate_natural_height(true);
        _scrolled_window.set_hexpand(true);

        for (auto* icon : {
            &_filled_shapes_icon,
            &_outline_shapes_icon
        })
            icon->set_has_popover_indicator_visible(true);

        using namespace state::actions;
        for (auto* action : {
                &toolbox_select_move_selection,
                &toolbox_select_marquee_neighborhood_select,
                &toolbox_select_marquee_rectangle_replace,
                &toolbox_select_marquee_rectangle_add,
                &toolbox_select_marquee_rectangle_subtract,
                &toolbox_select_marquee_circle_replace,
                &toolbox_select_marquee_circle_add,
                &toolbox_select_marquee_circle_subtract,
                &toolbox_select_marquee_polygon_replace,
                &toolbox_select_marquee_polygon_add,
                &toolbox_select_marquee_polygon_subtract,
                &toolbox_select_brush,
                &toolbox_select_eraser,
                &toolbox_select_eyedropper,
                &toolbox_select_bucket_fill,
                &toolbox_select_line,
                &toolbox_select_rectangle_outline,
                &toolbox_select_rectangle_fill,
                &toolbox_select_circle_outline,
                &toolbox_select_circle_fill,
                &toolbox_select_polygon_outline,
                &toolbox_select_polygon_fill,
                &toolbox_select_gradient_dithered,
                &toolbox_select_gradient_smooth
        }){
            std::stringstream which;
            for (size_t i = std::string("toolbox.select_").size(); i < action->get_id().size(); ++i)
                which << (char) action->get_id().at(i);

            action->set_function([id = std::string(which.str())](){
                active_state->set_current_tool(string_to_tool_id(id));
            });
            state::add_shortcut_action(*action);
        }
    }

    Toolbox::operator Widget*()
    {
        return &_scrolled_window;
    }

    void Toolbox::select(ToolID id)
    {
        for (auto& pair : _id_to_icons)
        {
            bool selected = pair.first == id;
            auto& icon = pair.second;
            icon->set_selection_indicator_visible(selected);
        }

        auto shapes_fill_selected = id == CIRCLE_FILL or
                id == RECTANGLE_FILL or
                id == POLYGON_FILL;

        _filled_shapes_icon.set_child_selection_indicator_visible(shapes_fill_selected);
        _filled_shapes_icon.set_has_popover_indicator_visible(not shapes_fill_selected);

        auto shapes_outline_selected = id == CIRCLE_OUTLINE or
                id == RECTANGLE_OUTLINE or
                id == POLYGON_OUTLINE;

        _outline_shapes_icon.set_child_selection_indicator_visible(shapes_outline_selected);
        _outline_shapes_icon.set_has_popover_indicator_visible(not shapes_outline_selected);

        _list_view.get_selection_model()->set_signal_selection_changed_blocked(true);
        _list_view.get_selection_model()->select(_id_to_listview_positions.at(id));
        _list_view.get_selection_model()->set_signal_selection_changed_blocked(false);
    }

    void Toolbox::on_active_tool_changed()
    {
        select(active_state->get_current_tool());
    }

    Toolbox::~Toolbox()
    {
        for (auto& pair : _id_to_icons)
            delete pair.second;
    }
}
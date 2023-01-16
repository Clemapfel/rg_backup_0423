// 
// Copyright 2022 Clemens Cords
// Created on 10/16/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/tools.hpp>
#include <app/app_component.hpp>
#include <app/global_state.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        Action toolbox_select_shapes_fill_action = Action("toolbox.select_shapes_fill");
        Action toolbox_select_shapes_outline_action = Action("toolbox.select_shapes_outline");

        Action toolbox_select_marquee_neighborhood_select_action = Action("toolbox.select_marquee_neighborhood_select");
        Action toolbox_select_marquee_rectangle_action = Action("toolbox.select_marquee_rectangle");
        Action toolbox_select_marquee_rectangle_add_action = Action("toolbox.select_marquee_rectangle_add");
        Action toolbox_select_marquee_rectangle_subtract_action = Action("toolbox.select_marquee_rectangle_subtract");
        Action toolbox_select_marquee_circle_action = Action("toolbox.select_marquee_circle");
        Action toolbox_select_marquee_circle_add_action = Action("toolbox.select_marquee_circle_add");
        Action toolbox_select_marquee_circle_subtract_action = Action("toolbox.select_marquee_circle_subtract");
        Action toolbox_select_marquee_polygon_action = Action("toolbox.select_marquee_polygon");
        Action toolbox_select_marquee_polygon_add_action = Action("toolbox.select_marquee_polygon_add");
        Action toolbox_select_marquee_polygon_subtract_action = Action("toolbox.select_marquee_polygon_subtract");

        Action toolbox_select_brush_action = Action("toolbox.select_brush");
        Action toolbox_select_eraser_action = Action("toolbox.select_eraser");
        Action toolbox_select_eyedropper_action = Action("toolbox.select_eyedropper");
        Action toolbox_select_bucket_fill_action = Action("toolbox.select_bucket_fill");
        Action toolbox_select_line_action = Action("toolbox.select_line");

        Action toolbox_select_rectangle_outline_action = Action("toolbox.select_rectangle_outline");
        Action toolbox_select_rectangle_fill_action = Action("toolbox.select_rectangle_fill");
        Action toolbox_select_circle_outline_action = Action("toolbox.select_circle_outline");
        Action toolbox_select_circle_fill_action = Action("toolbox.select_circle_fill");
        Action toolbox_select_polygon_outline_action = Action("toolbox.select_polygon_outline");
        Action toolbox_select_polygon_fill_action = Action("toolbox.select_polygon_fill");
        Action toolbox_select_gradient_dithered_action = Action("toolbox.select_gradient_dithered");
        Action toolbox_select_gradient_smooth_action = Action("toolbox.select_gradient_smooth");
    }

    class Toolbox : public AppComponent
    {
        public:
            Toolbox();

            void update() override;
            operator Widget*() override;

            void select(ToolID);

        private:
            ToolID _currently_selected = state::active_tool;

            class Icon
            {
                public:
                    Icon(ToolID id, Toolbox*);
                    operator Widget*();

                    void set_selection_indicator_visible(bool);
                    void set_child_selection_indicator_visible(bool);
                    void set_has_popover_indicator_visible(bool);

                    ToolID get_id();

                private:
                    Toolbox* _owner;
                    ToolID _id;

                    ActionID _action_id;

                    ImageDisplay _has_popover_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "has_popover_indicator" + ".png", state::icon_scale);
                    ImageDisplay _child_selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "child_selected_horizontal" + ".png", state::icon_scale);
                    ImageDisplay _selected_indicator_icon = ImageDisplay(get_resource_path() + "icons/" + "selected_indicator" + ".png", state::icon_scale);

                    ImageDisplay _tool_icon = ImageDisplay(get_resource_path() + "icons/" + _id + ".png", state::icon_scale);

                    Overlay _overlay;
                    AspectFrame _aspect_frame = AspectFrame(1);

                    ListView _main = ListView(GTK_SELECTION_NONE);

                    ClickEventController _click_event_controller;
                    static void on_click_pressed(ClickEventController*, size_t, double, double, Icon* instance);

                    Tooltip _tooltip;
            };

            class IconWithPopover
            {
                public:
                    IconWithPopover(ToolID main, std::vector<std::vector<ToolID>> children, Toolbox* owner);
                    operator Widget*();

                    void set_tool_selected(ToolID);

                private:
                    Toolbox* _owner = nullptr;
                    Viewport _main;

                    std::vector<Box> _popover_rows;
                    Box _popover_box = Box(GTK_ORIENTATION_VERTICAL);
                    Popover _popover;

                    Icon* _parent_icon;
                    std::vector<Icon*> _child_icons;

                    ClickEventController _click_event_controller;
                    static void on_parent_icon_click_pressed(ClickEventController*, size_t, double, double, IconWithPopover* instance);
            };

            std::vector<IconWithPopover*> _elements =
            {
                new IconWithPopover(MARQUEE_RECTANGLE, {{MARQUEE_RECTANGLE_ADD, MARQUEE_RECTANGLE, MARQUEE_RECTANGLE_SUBTRACT}}, this),
                new IconWithPopover(MARQUEE_CIRCLE, {{MARQUEE_CIRCLE_ADD, MARQUEE_CIRCLE, MARQUEE_CIRCLE_SUBTRACT}}, this),
                new IconWithPopover(MARQUEE_POLYGON, {{MARQUEE_POLYGON_ADD, MARQUEE_POLYGON, MARQUEE_POLYGON_SUBTRACT}}, this),
                new IconWithPopover(MARQUEE_NEIGHBORHODD_SELECT, {}, this),
                new IconWithPopover(BRUSH, {}, this),
                new IconWithPopover(ERASER, {}, this),
                new IconWithPopover(EYEDROPPER, {}, this),
                new IconWithPopover(BUCKET_FILL, {}, this),
                new IconWithPopover(LINE, {}, this),
                new IconWithPopover(SHAPES_OUTLINE, {{RECTANGLE_OUTLINE, CIRCLE_OUTLINE, POLYGON_OUTLINE}}, this),
                new IconWithPopover(SHAPES_FILL, {{RECTANGLE_FILL, CIRCLE_FILL, POLYGON_FILL}}, this),
                new IconWithPopover(GRADIENT_DITHERED, {{GRADIENT_DITHERED, GRADIENT_SMOOTH}}, this)
            };

            static inline ToolID _shapes_fill_forwarding_id = POLYGON_FILL;
            static inline ToolID _shapes_outline_forwarding_id = POLYGON_OUTLINE;

            ListView _element_container = ListView(GTK_ORIENTATION_HORIZONTAL, GTK_SELECTION_NONE);

            SeparatorLine _spacer_left, _spacer_right;
            Box _outer = Box(GTK_ORIENTATION_HORIZONTAL);
    };
}


//
// Copyright 2022 Clemens Cords
// Created on 9/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/scale.hpp>
#include <include/spin_button.hpp>
#include <include/gl_area.hpp>

namespace mousetrap
{
    class BrushOptions : public Widget
    {
        public:
            BrushOptions();
            void update() override;
            operator GtkWidget*() override;

        private:
            static inline constexpr size_t max_brush_size_on_scale = 50;
            static inline constexpr const char* brush_size_label_text = "Brush Size "; // trailing space sic
            static inline constexpr const char* brush_shape_label_text = "Brush Shape ";
            static inline constexpr const char* opacity_label_text = "Opacity ";
            static inline constexpr const char* brush_size_tooltip_text = "Brush radius, in pixels";
            static inline constexpr const char* brush_shape_tooltip_text = "Brush shape";

            static inline constexpr const char* opacity_tooltip_text = "Opacity (alpha)";
            static inline constexpr const char* brush_preview_tooltip_text = "<b>Brush Preview</b>\nPreview brush shape, size and opacity";

            Label* _brush_size_label;
            SeparatorLine* _brush_size_label_separator;
            Box* _brush_size_label_box;

            Scale* _brush_size_scale;
            SpinButton* _brush_size_spin_button;
            Box* _brush_size_scale_box;

            static inline const std::map<BrushType, std::string> brush_type_to_icon_id = {
                {BrushType::SQUARE, "brush_shape_square"},
                {BrushType::CIRCLE, "brush_shape_circle"},
                {BrushType::ELLIPSE_VERTICAL, "brush_shape_ellipse_vertical"},
                {BrushType::ELLIPSE_HORIZONTAL, "brush_shape_ellipse_horizontal"},
                {BrushType::RECTANGLE_VERTICAL, "brush_shape_rectangle_vertical"},
                {BrushType::RECTANGLE_HORIZONTAL, "brush_shape_rectangle_horizontal"},
                {BrushType::LINE_VERTICAL, "brush_shape_line_vertical"},
                {BrushType::LINE_HORIZONTAL, "brush_shape_line_horizontal"}
            };
            static inline constexpr const char* brush_type_more_icon_id = "brush_shape_other";

            static inline const std::map<BrushType, std::string> brush_type_to_tooltip = {
                {BrushType::SQUARE, "Brush Shape: Square"},
                {BrushType::CIRCLE, "Brush Shape: Circle"},
                {BrushType::ELLIPSE_VERTICAL, "Brush Shape: Ellipse (Vertical)"},
                {BrushType::ELLIPSE_HORIZONTAL, "Brush Shape: Ellipse (Horizontal)"},
                {BrushType::RECTANGLE_VERTICAL, "Brush Shape: Rectangle (Vertical)"},
                {BrushType::RECTANGLE_HORIZONTAL, "Brush Shape: Rectangle (Horizontal)"},
                {BrushType::LINE_VERTICAL, "Brush Shape: Line (Vertical)"},
                {BrushType::LINE_HORIZONTAL, "Brush Shape: Line (Horizontal)"}
            };

            ImageDisplay* _other_brush_icon;
            MenuButton* _other_brush_menu_button;

            Popover* _other_brush_popover;
            FlowBox* _other_brush_popover_box;

            using BrushToggleButton = struct {ToggleButton* button; ImageDisplay* icon;};
            std::map<BrushType, BrushToggleButton*> _brush_toggle_buttons;

            Box* _brush_shape_hbox;

            Label* _brush_shape_label;
            SeparatorLine* _brush_shape_label_separator;
            Box* _brush_shape_label_box;

            Scale* _opacity_scale;
            SpinButton* _opacity_spin_button;
            Box* _opacity_scale_box;

            Label* _opacity_label;
            SeparatorLine* _opacity_label_separator;
            Box* _opacity_label_box;

            using on_brush_size_changed_data = BrushOptions*;
            static void on_brush_size_changed(void*, void* instance);

            using on_brush_type_selected_data = struct{ BrushOptions* instance; BrushType which; };
            static void on_brush_type_selected(GtkToggleButton*, void*);

            using on_brush_opacity_changed_data = BrushOptions*;
            static void on_brush_opacity_changed(void*, void* instance);

            struct BrushPreviewCanvas : public Widget
            {
                BrushPreviewCanvas();
                void set_resolution(size_t);

                GLArea* canvas;
                AspectFrame* main;

                static inline const RGBA line_color = RGBA(1, 1, 1, 0.33);
                std::vector<Shape*> pixel_lines;

                Shape* transparency_tiling_shape;
                Vector2f* canvas_size;
                Shader* transparency_tiling_shader;

                Shape* brush_shape;
                Shape* outline_frame;

                operator GtkWidget*() override {
                    return main->operator GtkWidget*();
                }

                static void on_resize(GtkGLArea*, int, int, void*);
                static void on_realize(GtkGLArea*, void* instance);
            };

            BrushPreviewCanvas* _brush_preview;

            Box* _main;
    };
}

// ###

namespace mousetrap
{
    void BrushOptions::on_brush_size_changed(void* range_or_spin_button, void* data)
    {
        auto* instance = (BrushOptions*) data;

        size_t value;

        if (GTK_IS_RANGE(range_or_spin_button))
            value = std::round(gtk_range_get_value((GtkRange*) range_or_spin_button));
        else if (GTK_IS_SPIN_BUTTON(range_or_spin_button))
            value = std::round(gtk_spin_button_get_value((GtkSpinButton*) range_or_spin_button));

        instance->_brush_size_scale->set_all_signals_blocked(true);
        instance->_brush_size_spin_button->set_all_signals_blocked(true);

        instance->_brush_size_scale->set_value(value);
        instance->_brush_size_spin_button->set_value(value);

        if (value > instance->max_brush_size_on_scale)
            instance->_brush_size_scale->set_opacity(0.5);
        else
            instance->_brush_size_scale->set_opacity(1);

        instance->_brush_size_scale->set_all_signals_blocked(false);
        instance->_brush_size_spin_button->set_all_signals_blocked(false);

        state::brush_size = value;
        state::update_brush_texture();

        instance->_brush_preview->set_resolution(value);
    }

    void BrushOptions::on_brush_opacity_changed(void* range_or_spin_button, void* data)
    {
        auto* instance = (BrushOptions*) data;

        float value;

        if (GTK_IS_RANGE(range_or_spin_button))
            value = gtk_range_get_value((GtkRange*) range_or_spin_button);
        else if (GTK_IS_SPIN_BUTTON(range_or_spin_button))
            value = gtk_spin_button_get_value((GtkSpinButton*) range_or_spin_button);

        instance->_opacity_scale->set_all_signals_blocked(true);
        instance->_opacity_spin_button->set_all_signals_blocked(true);

        instance->_opacity_scale->set_value(value);
        instance->_opacity_spin_button->set_value(value);

        instance->_opacity_scale->set_all_signals_blocked(false);
        instance->_opacity_spin_button->set_all_signals_blocked(false);

        state::brush_opacity = value;

        instance->_brush_preview->brush_shape->set_color(RGBA(0, 0, 0, value));
        instance->_brush_preview->canvas->queue_render();
    }

    void BrushOptions::on_brush_type_selected(GtkToggleButton* button, void* data)
    {
        auto which = ((on_brush_type_selected_data *) data)->which;
        auto* instance = ((on_brush_type_selected_data *) data)->instance;

        for (auto& pair : instance->_brush_toggle_buttons)
        {
            auto* button = pair.second->button;
            button->set_all_signals_blocked(true);

            if (pair.first == which)
                button->set_active(true);
            else
                button->set_active(false);

            button->set_all_signals_blocked(false);
        }

        state::brush_type = which;
        state::update_brush_texture();
        instance->_brush_preview->set_resolution(state::brush_size);
    }

    void BrushOptions::BrushPreviewCanvas::on_resize(GtkGLArea* area, int x, int y, void* data)
    {
        gtk_gl_area_make_current(area);

        auto* instance = (BrushOptions::BrushPreviewCanvas*) data;
        instance->canvas_size->x = x;
        instance->canvas_size->y = y;

        gtk_gl_area_queue_render(area);
    }

    void BrushOptions::BrushPreviewCanvas::on_realize(GtkGLArea* area, void* data)
    {
        auto* instance = (BrushOptions::BrushPreviewCanvas*) data;
        gtk_gl_area_make_current(area);

        instance->transparency_tiling_shader = new Shader();
        instance->transparency_tiling_shader->create_from_file(get_resource_path() + "/shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        instance->canvas_size = new Vector2f(1, 1);

        instance->transparency_tiling_shape = new Shape();
        instance->transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        instance->outline_frame = new Shape();
        auto eps = 0.001;
        instance->outline_frame->as_wireframe({
              {eps, 0},
              {eps, 1 - eps},
              {1 - eps, 1 - eps},
              {1 - eps, 0}
        });
        instance->outline_frame->set_color(RGBA(0, 0, 0, 1));

        float brush_shape_margin = 0.05;

        if (state::brush_texture_image == nullptr)
        {
            state::brush_texture_image = new Image();
            state::brush_texture_image->create(1, 1, RGBA(1, 0, 1, 1));
        }

        if (state::brush_texture == nullptr)
        {
            state::brush_texture = new Texture();
            state::brush_texture->create_from_image(*state::brush_texture_image);
        }

        instance->brush_shape = new Shape();
        instance->brush_shape->as_rectangle(Vector2f(brush_shape_margin), Vector2f(1 - 2 * brush_shape_margin));
        instance->brush_shape->set_color(RGBA(0, 0, 0, 1));
        instance->brush_shape->set_texture(state::brush_texture);

        instance->set_resolution(2);
    }

    void BrushOptions::BrushPreviewCanvas::set_resolution(size_t resolution)
    {
        canvas->make_current();

        for (auto* ptr : pixel_lines)
            delete ptr;

        pixel_lines.clear();

        auto topleft = brush_shape->get_top_left();
        auto size = brush_shape->get_size();

        float square_size = size.x / float(resolution);
        bool draw_lines = square_size * canvas_size->x > 5;

        for (size_t i = 1; draw_lines and i < resolution; ++i)
        {
            pixel_lines.emplace_back(new Shape());
            pixel_lines.back()->as_line(
                {topleft.x, float(i) * square_size + topleft.x},
                {topleft.x + size.x, float(i) * square_size + topleft.x}
            );
            pixel_lines.back()->set_color(line_color);

            pixel_lines.emplace_back(new Shape());
            pixel_lines.back()->as_line(
                {float(i) * square_size + topleft.y, topleft.y},
                {float(i) * square_size + topleft.y, topleft.y + size.y}
            );
            pixel_lines.back()->set_color(line_color);
        }

        canvas->clear_render_tasks();

        auto transparency_tiling_task = RenderTask(transparency_tiling_shape, transparency_tiling_shader);
        transparency_tiling_task.register_vec2("_canvas_size", canvas_size);

        canvas->add_render_task(transparency_tiling_task);
        canvas->add_render_task(brush_shape);
        canvas->add_render_task(outline_frame);

        for (auto* line : pixel_lines)
            canvas->add_render_task(line);

        canvas->queue_render();
    }

    BrushOptions::BrushPreviewCanvas::BrushPreviewCanvas()
    {
        canvas = new GLArea();
        canvas->connect_signal("realize", on_realize, this);
        canvas->connect_signal("resize", on_resize, this);
        canvas->set_expand(true);

        main = new AspectFrame(1);
        main->set_child(canvas);
    }

    BrushOptions::BrushOptions()
    {
        _brush_size_scale = new Scale(1, 50, 1);
        _brush_size_scale->set_hexpand(true);
        _brush_size_scale->set_vexpand(false);
        _brush_size_scale->set_margin_end(state::margin_unit);
        _brush_size_scale->connect_signal("value-changed", on_brush_size_changed, this);
        _brush_size_scale->set_size_request({state::slider_min_width, 1});

        _brush_size_spin_button = new SpinButton(1, 999, 1);
        _brush_size_spin_button->set_digits(0);
        _brush_size_spin_button->set_expand(false);
        _brush_size_spin_button->set_halign(GTK_ALIGN_END);
        _brush_size_spin_button->connect_signal("value-changed", on_brush_size_changed, this);

        _brush_size_scale_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _brush_size_scale_box->push_back(_brush_size_scale);
        _brush_size_scale_box->push_back(_brush_size_spin_button);
        _brush_size_scale_box->set_vexpand(false);
        _brush_size_scale_box->set_tooltip_text(brush_size_tooltip_text);

        _brush_size_label = new Label(brush_size_label_text);
        _brush_size_label->set_halign(GTK_ALIGN_START);
        _brush_size_label->set_hexpand(false);
        _brush_size_label->set_margin_start(0.5 * state::margin_unit);

        _brush_size_label_separator = new SeparatorLine();
        _brush_size_label_separator->set_hexpand(true);

        _brush_size_label_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _brush_size_label_box->push_back(_brush_size_label);
        _brush_size_label_box->push_back(_brush_size_label_separator);
        _brush_size_label_box->set_tooltip_text(brush_size_tooltip_text);

        for (auto& pair : brush_type_to_icon_id)
        {
            auto type = pair.first;
            auto inserted = _brush_toggle_buttons.insert({type, new BrushToggleButton{
                new ToggleButton(),
                new ImageDisplay(get_resource_path() + "icons/" + brush_type_to_icon_id.at(type) + ".png", state::icon_scale)
            }}).first->second;

            inserted->icon->set_size_request({32 * state::icon_scale, 32 * state::icon_scale});
            inserted->button->set_child(inserted->icon);
            inserted->button->set_hexpand(true);
            inserted->button->set_margin_start(0.5 * state::margin_unit);
            inserted->button->set_margin_end(0.5 * state::margin_unit);
            inserted->button->set_cursor(GtkCursorType::POINTER);
            inserted->button->set_tooltip_text(brush_type_to_tooltip.at(type));

            if (type == BrushType::SQUARE)
                inserted->button->set_active(true);

            inserted->button->connect_signal(
                "toggled",
                on_brush_type_selected,
                new on_brush_type_selected_data{this, type}
            );
        }

        _other_brush_icon = new ImageDisplay(get_resource_path() + "icons/" + brush_type_more_icon_id + ".png");
        _other_brush_icon->set_size_request({32, 32});

        _other_brush_menu_button = new MenuButton();
        _other_brush_menu_button->set_child(_other_brush_icon);
        _other_brush_menu_button->set_always_show_arrow(false);
        _other_brush_menu_button->set_hexpand(true);
        _other_brush_menu_button->set_margin_start(0.5 * state::margin_unit);
        _other_brush_menu_button->set_margin_end(0.5 * state::margin_unit);
        _other_brush_menu_button->set_cursor(GtkCursorType::POINTER);
        _other_brush_menu_button->set_tooltip_text("More Brush Shapes");

        _other_brush_popover = new Popover();
        _other_brush_popover_box = new FlowBox();
        _other_brush_popover_box->set_min_children_per_line(2);
        _other_brush_popover_box->set_max_children_per_line(2);
        _other_brush_popover->set_child(_other_brush_popover_box);
        _other_brush_menu_button->set_popover(_other_brush_popover);
        _other_brush_menu_button->set_popover_position(GTK_POS_RIGHT);

        _brush_shape_hbox = new Box(GTK_ORIENTATION_HORIZONTAL);
        _brush_shape_hbox->push_back(_brush_toggle_buttons.at(BrushType::SQUARE)->button);
        _brush_shape_hbox->push_back(_brush_toggle_buttons.at(BrushType::CIRCLE)->button);
        _brush_shape_hbox->push_back(_other_brush_menu_button);
        _brush_shape_hbox->set_hexpand(true);

        for (auto& pair : _brush_toggle_buttons)
        {
            if (pair.first == BrushType::SQUARE or pair.first == BrushType::CIRCLE or pair.first == BrushType::OTHER)
                continue;

            _other_brush_popover_box->push_back(pair.second->button);
        }

        _brush_shape_label = new Label(brush_shape_label_text);
        _brush_shape_label->set_margin_start(0.5 * state::margin_unit);
        _brush_shape_label_separator = new SeparatorLine();
        _brush_shape_label_separator->set_hexpand(true);

        _brush_shape_label_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _brush_shape_label_box->push_back(_brush_shape_label);
        _brush_shape_label_box->push_back(_brush_shape_label_separator);
        _brush_shape_label_box->set_tooltip_text(brush_shape_tooltip_text);

        _opacity_label = new Label(opacity_label_text);
        _opacity_label->set_margin_start(0.5 * state::margin_unit);

        _opacity_label_separator = new SeparatorLine();
        _opacity_label_separator->set_hexpand(true);

        _opacity_label_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _opacity_label_box->push_back(_opacity_label);
        _opacity_label_box->push_back(_opacity_label_separator);
        _opacity_label_box->set_tooltip_text(opacity_tooltip_text);

        _opacity_scale = new Scale(0, 1, 0.05);
        _opacity_scale->set_hexpand(true);
        _opacity_scale->set_vexpand(false);
        _opacity_scale->set_margin_end(state::margin_unit);
        _opacity_scale->set_value(1);
        _opacity_scale->connect_signal("value-changed", on_brush_opacity_changed, this);
        _opacity_scale->set_size_request({state::slider_min_width, 1});

        _opacity_spin_button = new SpinButton(0, 1, 0.05);
        _opacity_spin_button->set_digits(2);
        _opacity_spin_button->set_expand(false);
        _opacity_spin_button->set_vexpand(false);
        _opacity_spin_button->set_halign(GTK_ALIGN_END);
        _opacity_spin_button->set_value(1);
        _opacity_spin_button->connect_signal("value-changed", on_brush_opacity_changed, this);

        _opacity_scale_box = new Box(GTK_ORIENTATION_HORIZONTAL);
        _opacity_scale_box->push_back(_opacity_scale);
        _opacity_scale_box->push_back(_opacity_spin_button);
        _opacity_scale_box->set_tooltip_text(opacity_tooltip_text);

        _brush_preview = new BrushPreviewCanvas();
        _brush_preview->set_size_request(Vector2f(4 * 32));
        _brush_preview->set_tooltip_text(brush_preview_tooltip_text);

        _main = new Box(GTK_ORIENTATION_VERTICAL, state::margin_unit);
        _main->set_expand(true);
        _main->push_back(_brush_size_label_box);
        _main->push_back(_brush_size_scale_box);
        _main->push_back(_brush_shape_label_box);
        _main->push_back(_brush_shape_hbox);
        _main->push_back(_brush_preview);
        _main->push_back(_opacity_label_box);
        _main->push_back(_opacity_scale_box);
    }

    void BrushOptions::update()
    {
        _brush_preview->set_resolution(state::brush_size);
    }

    BrushOptions::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }
}
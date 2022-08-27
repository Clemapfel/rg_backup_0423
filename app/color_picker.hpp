// 
// Copyright 2022 Clemens Cords
// Created on 8/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/gl_area.hpp>
#include <include/aspect_frame.hpp>
#include <include/event_controller.hpp>
#include <include/get_resource_path.hpp>

#include <app/global_state.hpp>

namespace mousetrap
{
    class ColorPicker : public Widget
    {
        public:
            ColorPicker();
            ~ColorPicker();

            operator GtkWidget*() override;

            void update() override;

        private:
            void update(HSVA Color);

            static void on_render_area_realize(GtkGLArea*, ColorPicker* instance);
            static void on_render_area_resize(GtkGLArea*, int, int , ColorPicker* instance);

            bool _hue_bar_active = false;
            void set_hue_bar_cursor(Vector2f);

            bool _hsv_shape_active = false;
            void set_hsv_shape_cursor(Vector2f);

            Vector2f align_to_pixelgrid(Vector2f);

            AspectFrame* _main;
            GLArea* _render_area;

            Shader* _hue_bar_shader;
            Shape* _hue_bar_shape;
            Shape* _hue_bar_frame;

            Shape* _hue_bar_cursor;
            Shape* _hue_bar_cursor_frame;
            Shape* _hue_bar_cursor_window;
            Shape* _hue_bar_cursor_window_frame;

            Shader* _hsv_shape_shader;
            Shape* _hsv_shape;
            Shape* _hsv_shape_frame;

            Shape* _hsv_shape_cursor;
            Shape* _hsv_shape_cursor_frame;

            Shape* _hsv_shape_cursor_window;
            Shape* _hsv_shape_cursor_window_frame;

            Vector2f* _canvas_size;
            HSVA* _current_color_hsva;
            Vector2f* _square_top_left;
            Vector2f* _square_size;

            static void on_render_area_motion_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* user_data);
            static void on_render_area_motion(GtkEventControllerMotion* self, gdouble x, gdouble y, void* user_data);
            static void on_render_area_button_press(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* user_data);
            static void on_render_area_button_release(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* user_data);

            ClickEventController* _render_area_button_event_controller;
            MotionEventController* _render_area_motion_event_controller;

            void update_primary_color(double x, double y);
            void reformat();
    };
}

// ###

namespace mousetrap
{
    ColorPicker::ColorPicker()
    {
        _hue_bar_shader = new Shader();
        _render_area = new GLArea();

        _render_area_motion_event_controller = new MotionEventController();
        _render_area_button_event_controller = new ClickEventController();

        _render_area->connect_signal("realize", on_render_area_realize, this);
        _render_area->connect_signal("resize", on_render_area_resize, this);

        _render_area_motion_event_controller->connect_enter(on_render_area_motion_enter, this);
        _render_area_motion_event_controller->connect_motion(on_render_area_motion, this);
        _render_area_button_event_controller->connect_pressed(on_render_area_button_press, this);
        _render_area_button_event_controller->connect_released(on_render_area_button_release, this);

        _render_area->add_controller(_render_area_motion_event_controller);
        _render_area->add_controller(_render_area_button_event_controller);

        _main = new AspectFrame(1);
        _main->set_child(_render_area);
    }

    ColorPicker::~ColorPicker()
    {
        delete _hue_bar_shader;
        delete _hue_bar_shape;
        delete _hue_bar_frame;

        delete _hue_bar_cursor;
        delete _hue_bar_cursor_frame;
        delete _hue_bar_cursor_window;
        delete _hue_bar_cursor_window_frame;

        delete _hsv_shape_shader;
        delete _hsv_shape;
        delete _hsv_shape_frame;

        delete _hsv_shape_cursor;
        delete _hsv_shape_cursor_frame;

        delete _hsv_shape_cursor_window;
        delete _hsv_shape_cursor_window_frame;

        delete _canvas_size;
        delete _current_color_hsva;
        delete _square_top_left;
        delete _square_size;
    }

    ColorPicker::operator GtkWidget*()
    {
        return _main->operator GtkWidget*();
    }

    Vector2f ColorPicker::align_to_pixelgrid(Vector2f in)
    {
        in.x = int(in.x * _canvas_size->x) / float(_canvas_size->x);
        in.y = int(in.y * _canvas_size->y) / float(_canvas_size->y);
        return in;
    }

    void ColorPicker::on_render_area_realize(GtkGLArea* area, ColorPicker* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_hue_bar_shape = new Shape();
        instance->_hue_bar_frame = new Shape();

        instance->_hue_bar_cursor = new Shape();
        instance->_hue_bar_cursor_frame = new Shape();

        instance->_hue_bar_cursor_window = new Shape();
        instance->_hue_bar_cursor_window_frame = new Shape();

        instance->_hsv_shape = new Shape();
        instance->_hsv_shape_frame = new Shape();

        instance->_hsv_shape_cursor = new Shape();
        instance->_hsv_shape_cursor_frame = new Shape();

        instance->_hsv_shape_cursor_window = new Shape();
        instance->_hsv_shape_cursor_window_frame = new Shape();

        instance->_hue_bar_shader = new Shader();
        instance->_hue_bar_shader->create_from_file(get_resource_path() + "shaders/color_picker_hue_bar.frag", ShaderType::FRAGMENT);

        instance->_hsv_shape_shader = new Shader();
        instance->_hsv_shape_shader->create_from_file(get_resource_path() + "shaders/color_picker_hsv_square.frag", ShaderType::FRAGMENT);

        instance->_canvas_size = new Vector2f{1, 1};
        instance->_current_color_hsva = new HSVA();
        instance->_square_top_left = new Vector2f(0, 0);
        instance->_square_size = new Vector2f(1, 1);

        auto* self = instance->_render_area;

        auto hue_bar_task = RenderTask(instance->_hue_bar_shape, instance->_hue_bar_shader);
        hue_bar_task.register_color("_current_color_hsva", instance->_current_color_hsva);
        self->add_render_task(hue_bar_task);

        self->add_render_task(instance->_hue_bar_frame);

        self->add_render_task(instance->_hue_bar_cursor);
        self->add_render_task(instance->_hue_bar_cursor_frame);

        self->add_render_task(instance->_hue_bar_cursor_window);
        self->add_render_task(instance->_hue_bar_cursor_window_frame);

        auto hsv_shape_task = RenderTask(instance->_hsv_shape, instance->_hsv_shape_shader);
        hsv_shape_task.register_color("_current_color_hsva", instance->_current_color_hsva);
        hsv_shape_task.register_vec2("_square_top_left", instance->_square_top_left);
        hsv_shape_task.register_vec2("_square_size", instance->_square_size);

        self->add_render_task(hsv_shape_task);

        self->add_render_task(instance->_hsv_shape_frame);

        self->add_render_task(instance->_hsv_shape_cursor);
        self->add_render_task(instance->_hsv_shape_cursor_frame);

        self->add_render_task(instance->_hsv_shape_cursor_window);
        self->add_render_task(instance->_hsv_shape_cursor_window_frame);

        instance->reformat();
        instance->update();

        gtk_gl_area_queue_render(area);
    }

    void ColorPicker::on_render_area_resize(GtkGLArea* area, int w, int h, ColorPicker* instance)
    {
        gtk_gl_area_make_current(area);

        instance->_canvas_size->x = w;
        instance->_canvas_size->y = h;

        instance->reformat();

        gtk_gl_area_queue_render(area);
    }

    void ColorPicker::reformat()
    {
        float x_margin = state::margin_unit / _canvas_size->x;
        float y_margin = state::margin_unit / _canvas_size->y;

        float hsv_shape_size = 1;
        float cursor_size = 0.1;

        _hue_bar_shape->as_rectangle(
                {0, 0},
                {cursor_size, hsv_shape_size}
        );

        float hsv_shape_x = _hue_bar_shape->get_top_left().x + _hue_bar_shape->get_size().x + 1.5 * x_margin;
        _hsv_shape->as_rectangle(
            {hsv_shape_x, 0},
            {1 - hsv_shape_x, hsv_shape_size}
        );

        Vector2f cursor_frame_width = {
            4 * 1 / _canvas_size->x,
            4 * 1 / _canvas_size->y
        };

        if (int(cursor_frame_width.x) % 2 != 0)
            cursor_frame_width -= 1;

        if (int(cursor_frame_width.y) % 2 != 0)
            cursor_frame_width -= 1;

        _hsv_shape_cursor->as_rectangle({0, 0}, {cursor_size, cursor_size * (_canvas_size->x / _canvas_size->y)});
        _hsv_shape_cursor_window->as_rectangle({0, 0},
            _hsv_shape_cursor->get_size() - cursor_frame_width
        );

        _hue_bar_cursor->as_rectangle({0, 0}, {
            _hsv_shape_cursor->get_size().x,
            _hsv_shape_cursor->get_size().y * (2 / (1 + sqrt(5)))
        });

        _hue_bar_cursor_window->as_rectangle({0, 0},
            _hue_bar_cursor->get_size() - cursor_frame_width
        );

        _hue_bar_cursor->set_centroid(align_to_pixelgrid(_hue_bar_cursor->get_centroid()));
        _hsv_shape_cursor->set_centroid(align_to_pixelgrid(_hsv_shape_cursor->get_centroid()));

        _hue_bar_cursor->set_centroid(_hue_bar_shape->get_centroid());
        _hue_bar_cursor_window->set_centroid(_hue_bar_cursor->get_centroid());

        _hsv_shape_cursor->set_centroid(_hsv_shape->get_centroid());
        _hsv_shape_cursor_window->set_centroid(_hsv_shape_cursor->get_centroid());

        {
            auto top_left = _hsv_shape->get_vertex_position(0);
            auto size = _hsv_shape->get_size();
            std::vector<Vector2f> vertices = {
                top_left,
                {top_left.x + size.x, top_left.y},
                {top_left.x + size.x, top_left.y + size.y - 0.001},
                {top_left.x, top_left.y + size.y - 0.001}
            };
            _hsv_shape_frame->as_wireframe(vertices);
        }

        {
            auto top_left = _hue_bar_shape->get_vertex_position(0);
            auto size = _hue_bar_shape->get_size();
            std::vector<Vector2f> vertices = {
                    top_left,
                    {top_left.x + size.x, top_left.y},
                    {top_left.x + size.x, top_left.y + size.y - 0.001},
                    {top_left.x, top_left.y + size.y - 0.001}
            };
            _hue_bar_frame->as_wireframe(vertices);
        }

        _hue_bar_cursor_frame->as_wireframe(*_hue_bar_cursor);
        _hue_bar_cursor_window_frame->as_wireframe(*_hue_bar_cursor_window);

        _hsv_shape_cursor_frame->as_wireframe(*_hsv_shape_cursor);
        _hsv_shape_cursor_window_frame->as_wireframe(*_hsv_shape_cursor_window);

        for (auto* frame : {_hue_bar_frame, _hue_bar_cursor_frame, _hue_bar_cursor_window_frame, _hsv_shape_frame, _hsv_shape_cursor_frame, _hsv_shape_cursor_window_frame})
            frame->set_color(RGBA(0, 0, 0, 1));

        _main->set_ratio(_hsv_shape->get_size().y / _hsv_shape->get_size().x);
        *_square_top_left = _hsv_shape->get_top_left();
        *_square_size = _hsv_shape->get_size();

        update();

        _render_area->queue_render();
    }

    void ColorPicker::update()
    {
        update(state::primary_color);
    }

    void ColorPicker::update(HSVA color)
    {
        set_hue_bar_cursor({0, _hue_bar_shape->get_top_left().y + (1 - color.h) * _hue_bar_shape->get_size().y});
        set_hsv_shape_cursor({
            _hsv_shape->get_top_left().x + (color.v) * _hsv_shape->get_size().x,
            _hsv_shape->get_top_left().y + (1 - color.s) * _hsv_shape->get_size().y,
        });

        _hsv_shape_cursor_window->set_color(color);
        _render_area->queue_render();
    }

    void ColorPicker::set_hue_bar_cursor(Vector2f pos)
    {
        float hue = (pos.y - _hue_bar_shape->get_top_left().y) / _hue_bar_shape->get_size().y;
        hue = glm::clamp<float>(hue, 0, 1);
        _current_color_hsva->h = 1 - hue;

        _hue_bar_cursor_window->set_color(HSVA(_current_color_hsva->h, 1, 1, 1));
        _hsv_shape_cursor_window->set_color(*_current_color_hsva);

        float y = glm::clamp<float>(pos.y,
             _hue_bar_shape->get_top_left().y + _hue_bar_cursor->get_size().y * 0.5,
             _hue_bar_shape->get_top_left().y + _hue_bar_shape->get_size().y - _hue_bar_cursor->get_size().y * 0.5
        );

        _hue_bar_cursor->set_centroid(align_to_pixelgrid({_hue_bar_shape->get_centroid().x, y}));

        auto centroid = _hue_bar_cursor->get_centroid();
        _hue_bar_cursor_frame->set_centroid(centroid);
        _hue_bar_cursor_window->set_centroid(centroid);
        _hue_bar_cursor_window_frame->set_centroid(centroid);
    }

    void ColorPicker::set_hsv_shape_cursor(Vector2f pos)
    {
        auto top_left = _hsv_shape->get_top_left();
        auto size = _hsv_shape->get_size();

        auto half_cursor = Vector2f(_hsv_shape_cursor->get_size() * Vector2f(0.5));
        pos = glm::clamp(pos,
             _hsv_shape->get_top_left() + half_cursor,
             _hsv_shape->get_top_left() + _hsv_shape->get_size() - half_cursor
        );

        _hsv_shape_cursor->set_centroid(align_to_pixelgrid(pos));
        _hsv_shape_cursor_frame->set_centroid(_hsv_shape_cursor->get_centroid());
        _hsv_shape_cursor_window->set_centroid(_hsv_shape_cursor->get_centroid());
        _hsv_shape_cursor_window_frame->set_centroid(_hsv_shape_cursor->get_centroid());
    }

    void ColorPicker::on_render_area_motion_enter(GtkEventControllerMotion* self, gdouble x, gdouble y, void* user_data)
    {}

    void ColorPicker::on_render_area_motion(GtkEventControllerMotion* self, gdouble x, gdouble y, void* user_data)
    {
        auto* instance = (ColorPicker*) user_data;
        auto pos = Vector2f(x / instance->_canvas_size->x, y / instance->_canvas_size->y);

        bool in_hue_bar = is_point_in_rectangle(pos, Rectangle{instance->_hue_bar_shape->get_top_left(), instance->_hue_bar_shape->get_size()});
        bool in_hsv_shape = is_point_in_rectangle(pos, Rectangle{instance->_hsv_shape->get_top_left(), instance->_hsv_shape->get_size()});

        if (instance->_hue_bar_active)
        {
            instance->set_hue_bar_cursor(pos);
            instance->_render_area->queue_render();
            instance->_render_area->set_cursor(GtkCursorType::GRABBING);
            instance->update_primary_color(x, y);
            return;
        }
        else if (instance->_hsv_shape_active)
        {
            instance->set_hsv_shape_cursor(pos);
            instance->_render_area->queue_render();
            instance->_render_area->set_cursor(GtkCursorType::NONE);
            instance->update_primary_color(x, y);
            return;
        }
        else if (in_hue_bar)
            instance->_render_area->set_cursor(GtkCursorType::GRAB);
        else if (in_hsv_shape)
            instance->_render_area->set_cursor(GtkCursorType::CELL);
        else
            instance->_render_area->set_cursor(GtkCursorType::DEFAULT);
    }

    void ColorPicker::on_render_area_button_press(GtkGestureClick* self, gint n_press, gdouble x, gdouble y, void* user_data)
    {
        auto* instance = (ColorPicker*) user_data;
        auto pos = Vector2f(x / instance->_canvas_size->x, y / instance->_canvas_size->y);

        if (is_point_in_rectangle(pos, Rectangle{instance->_hue_bar_shape->get_top_left(), instance->_hue_bar_shape->get_size()}))
        {
            instance->_hue_bar_active = true;
            instance->_render_area->set_cursor(GtkCursorType::GRABBING);
            instance->set_hue_bar_cursor(pos);
            instance->_render_area->queue_render();
            instance->update_primary_color(x, y);
        }
        else if (is_point_in_rectangle(pos, Rectangle{instance->_hsv_shape->get_top_left(), instance->_hsv_shape->get_size()}))
        {
            instance->_hsv_shape_active = true;
            instance->_render_area->set_cursor(GtkCursorType::NONE);
            instance->set_hsv_shape_cursor(pos);
            instance->_render_area->queue_render();
            instance->update_primary_color(x, y);
        }
    }

    void ColorPicker::on_render_area_button_release(GtkGestureClick* self, gint n_press, gdouble x, gdouble y,
                                                    void* user_data)
    {
        auto* instance = (ColorPicker*) user_data;

        instance->_hue_bar_active = false;
        instance->_hsv_shape_active = false;

        auto pos = Vector2f(x / instance->_canvas_size->x, y / instance->_canvas_size->y);
        if (is_point_in_rectangle(pos, Rectangle{instance->_hue_bar_shape->get_top_left(), instance->_hue_bar_shape->get_size()}))
            instance->_render_area->set_cursor(GtkCursorType::GRAB);
        else if (is_point_in_rectangle(pos, Rectangle{instance->_hsv_shape->get_top_left(), instance->_hsv_shape->get_size()}))
            instance->_render_area->set_cursor(GtkCursorType::CELL);
        else
            instance->_render_area->set_cursor(GtkCursorType::DEFAULT);
    }

    void ColorPicker::update_primary_color(double x, double y)
    {
        if (_hue_bar_active)
        {
            float hue = y / _canvas_size->y;
            hue = glm::clamp<float>(hue, 0, 1);

            auto color = state::primary_color;
            color.h = 1 - (hue - _hue_bar_shape->get_top_left().y) / _hue_bar_shape->get_size().y;

            state::primary_color = color;
            state::primary_secondary_color_swapper->update();
        }
        else if (_hsv_shape_active)
        {
            float value = ((x / _canvas_size->x) - _hsv_shape->get_top_left().x) / _hsv_shape->get_size().x;
            float saturation = 1 - ((y / _canvas_size->y) - _hsv_shape->get_top_left().y / _hsv_shape->get_size().y);

            auto color = state::primary_color;
            color.v = value;
            color.s = saturation;

            color.v = glm::clamp<float>(color.v, 0, 1);
            color.s = glm::clamp<float>(color.s, 0, 1);

            state::primary_color = color;
            state::primary_secondary_color_swapper->update();
        }

        _hsv_shape_cursor_window->set_color(state::primary_color);
        *_current_color_hsva = state::primary_color;
    }
}

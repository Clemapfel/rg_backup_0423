// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/frame.hpp>
#include <include/shape.hpp>
#include <include/get_resource_path.hpp>
#include <include/pixel_buffer.hpp>

namespace mousetrap
{
    class Canvas : public Widget
    {
        public:
            Canvas(size_t width, size_t height);

            GtkWidget* get_native() override {
                return GTK_WIDGET(_frame->get_native());
            }

        private:
            static gboolean on_render(GtkGLArea*, GdkGLContext*, Canvas* instance);
            static void on_resize(GtkGLArea*, int, int, Canvas* instance);
            static void on_realize(GtkGLArea*, Canvas* instance);

            static gboolean on_scroll_event(GtkWidget*, GdkEventScroll*, Canvas* instance);

            Vector2ui _resolution;
            GLTransform _transform;

            Frame* _frame;

            WidgetWrapper<GtkGLArea>* _gl_area;
            Vector2f _widget_size;

            Shape* _transparency_tiling;
            Shader* _transparency_tiling_shader;

            bool _grid_enabled = true; // application interface
            bool _show_grid = true;     // depends on scale
            std::vector<Shape*> _grid;

            // guides
            static inline const RGBA _default_guide_color = RGBA(0.9, 0.9, 0.9, 1);
            void set_guide_color(RGBA);

            Shape* _canvas_frame;
            std::array<Shape*, 2> _half_guides;
            std::array<Shape*, 4> _third_guides;

            bool _draw_canvas_frame = true;
            bool _draw_half_guides = true;
            bool _draw_third_guides = true;

            std::array<Shape*, 4> _infinity_frame_overlay;

            struct Layer
            {
                Layer(size_t width, size_t height);

                Shape* _shape;
                Texture* _texture;
                PixelBuffer* _pixel_buffer;
            };

            Layer* _layer;
    };
}

// ###

namespace mousetrap
{
    Canvas::Layer::Layer(size_t width, size_t height)
    {
        _shape = new Shape();
        _shape->as_rectangle({0, 0}, {1, 1});

        _texture = new Texture();
        _shape->set_texture(_texture);

        _pixel_buffer = new PixelBuffer(width, height);

        // TODO
        auto temp = Image();
        temp.create_from_file(get_resource_path() + "mole.png");

        for (size_t x = 0; x < temp.get_size().x; ++x)
            for (size_t y = 0; y < temp.get_size().y; ++y)
                _pixel_buffer->set_pixel(
                    x + 0.5 * width - 0.5 * temp.get_size().x,
                    y + 0.5 * height - 0.5 * temp.get_size().y,
                    temp.get_pixel(x, y)
                );
        // TODO

        _pixel_buffer->flush();
        _pixel_buffer->unbind();
    }

    Canvas::Canvas(size_t width, size_t height)
        : _resolution(width, height)
    {
        _gl_area = new WidgetWrapper<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new()));
        gtk_gl_area_set_has_alpha(_gl_area->_native, TRUE);
        _gl_area->set_size_request({width * 3, height * 3});

        _frame = new Frame(float(width) / height, 0.0, 0.5, true);
        _frame->add(_gl_area->get_native());
        _frame->set_shadow_type(GtkShadowType::GTK_SHADOW_IN);
        _frame->set_margin(10);

        _gl_area->connect_signal("resize", on_resize, this);
        _gl_area->connect_signal("render", on_render, this);
        _gl_area->connect_signal("realize", on_realize, this);

        _gl_area->add_events(GDK_SCROLL_MASK);
        _gl_area->connect_signal("scroll-event", on_scroll_event, this);
    }

    void Canvas::set_guide_color(RGBA color)
    {
        _canvas_frame->set_color(color);

        for (auto* s : _half_guides)
            s->set_color(color);

        for (auto* s : _third_guides)
            s->set_color(color);
    }

    void Canvas::on_realize(GtkGLArea* area, Canvas* instance)
    {
        gtk_gl_area_make_current(area);
        glEnable(GL_BLEND);

        assert(instance->_resolution.x > 1 and instance->_resolution.y > 1);

        instance->_transparency_tiling = new Shape();
        instance->_transparency_tiling->as_rectangle({0, 0}, {1, 1});

        instance->_transparency_tiling_shader = new Shader();
        instance->_transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);

        Vector2f one_pixel = {1.f / instance->_resolution.x, 1.f / instance->_resolution.y};

        float lower_bound = -1;
        float upper_bound = +2;

        for (size_t i = 1; i < instance->_resolution.x; ++i)
        {
            instance->_grid.push_back(new Shape());
            instance->_grid.back()->as_line({i * one_pixel.x, lower_bound}, {i * one_pixel.x, upper_bound});
        }

        for (size_t i = 1; i < instance->_resolution.y; ++i)
        {
            instance->_grid.push_back(new Shape());
            instance->_grid.back()->as_line({lower_bound, i * one_pixel.y}, {upper_bound, i * one_pixel.y});
        }

        for (auto* s : instance->_grid)
            s->set_color(RGBA(0, 0, 0, 1));

        // guides

        instance->_canvas_frame = new Shape();
        float eps = 0.001;
        instance->_canvas_frame->as_wireframe({
            {eps, eps}, {eps, 1 - eps}, {1 - eps, 1 - eps}, {1 - eps, eps}
        });

        for (size_t i = 0; i < instance->_third_guides.size(); ++i)
            instance->_half_guides[i] = new Shape();

        instance->_half_guides[0]->as_line({0, 0.5}, {1, 0.5});
        instance->_half_guides[1]->as_line({0.5, 0}, {0.5, 1});

        for (size_t i = 0; i < instance->_third_guides.size(); ++i)
            instance->_third_guides[i] = new Shape();

        instance->_third_guides[0]->as_line({1/3.f, 0}, {1/3.f, 1});
        instance->_third_guides[1]->as_line({2/3.f, 0}, {2/3.f, 1});
        instance->_third_guides[2]->as_line({0, 1/3.f}, {1, 1/3.f});
        instance->_third_guides[3]->as_line({0, 2/3.f}, {1, 2/3.f});

        instance->set_guide_color(_default_guide_color);

        // infinity frame overlay

        for (size_t i = 0; i < 4; ++i)
            instance->_infinity_frame_overlay[i] = new Shape();

        float dist = 100000; // simulates infinite plane

        instance->_infinity_frame_overlay[0]->as_rectangle({-dist, -dist}, {1 + dist, -dist}, {1 + dist, 0}, {-dist, 0});
        instance->_infinity_frame_overlay[1]->as_rectangle({1, 0}, {1 + dist, 0}, {1 + dist, 1}, {1, 1});
        instance->_infinity_frame_overlay[2]->as_rectangle({-dist, 1}, {1 + dist, 1}, {1 + dist, 1 + dist}, {-dist, 1 + dist});
        instance->_infinity_frame_overlay[3]->as_rectangle({-dist, 0}, {0, 0}, {0, 1}, {-dist, 1});

        for (auto* s : instance->_infinity_frame_overlay)
            s->set_color(RGBA(0, 0, 0, 0));

        instance->_layer = new Layer(instance->_resolution.x, instance->_resolution.y);

        // TODO
        instance->_transform.scale(0.75, 0.75);
    }

    gboolean Canvas::on_render(GtkGLArea* area, GdkGLContext*, Canvas* instance)
    {
        gtk_gl_area_make_current(area);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        static auto noop_transform = GLTransform();
        static auto noop_shader = Shader();

        // tiling

        glUseProgram(instance->_transparency_tiling_shader->get_program_id());
        instance->_transparency_tiling_shader->set_uniform_vec2("_canvas_size", instance->_widget_size);
        instance->_transparency_tiling->render(*(instance->_transparency_tiling_shader), noop_transform);
        glUseProgram(0);

        // TODO
        instance->_layer->_texture->bind();
        instance->_layer->_pixel_buffer->bind();
        instance->_layer->_shape->render(noop_shader, instance->_transform);
        instance->_layer->_pixel_buffer->unbind();
        instance->_layer->_texture->unbind();
        // TODO

        // grid
        if (instance->_grid_enabled and instance->_show_grid)
        {
            for (auto* line : instance->_grid)
                line->render(noop_shader, instance->_transform);
        }

        // guides

        if (instance->_draw_canvas_frame)
            instance->_canvas_frame->render(noop_shader, instance->_transform);

        if (instance->_draw_half_guides)
            for (auto* s : instance->_half_guides)
                s->render(noop_shader, instance->_transform);

        if (instance->_draw_third_guides)
            for (auto* s : instance->_third_guides)
                s->render(noop_shader, instance->_transform);

        // inf frame
        glBlendFunc(GL_SRC_COLOR, GL_ZERO); // override source with overlay, which is fully transparent

        for (auto& shape : instance->_infinity_frame_overlay)
            shape->render(noop_shader, instance->_transform);

        glFlush();
        return FALSE;
    }

    void Canvas::on_resize(GtkGLArea*, int w, int h, Canvas* instance)
    {
        if (h % 2 != 0)
            h -= 1;

        if (w % 2 != 0)
            w -= 1;

        instance->_widget_size = {w, h};

        if (std::min(float(w) / instance->_resolution.x, float(h) / instance->_resolution.y) < 7)
            instance->_show_grid = false;
        else
            instance->_show_grid = true;

        gtk_gl_area_queue_render(instance->_gl_area->_native);
    }

    gboolean Canvas::on_scroll_event(GtkWidget*, GdkEventScroll* event, Canvas* instance)
    {
        Vector2f pointer_coord = {event->x_root, event->y_root}; // relative to widget

        std::cout << "root: " << event->x_root << " " << event->y_root << std::endl;
        std::cout << "xy  : " << event->x << " " << event->y << std::endl;
        std::cout << "delta  : " << event->delta_x << " " << event->delta_y << std::endl;

    }

}
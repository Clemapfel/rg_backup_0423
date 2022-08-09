// 
// Copyright 2022 Clemens Cords
// Created on 8/9/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>
#include <include/frame.hpp>
#include <include/shape.hpp>
#include <include/get_resource_path.hpp>

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

            Vector2ui _resolution;

            Frame* _frame;

            GtkGLArea* _gl_area;
            Vector2f _widget_size;

            Shape* _transparency_tiling;
            Shader* _transparency_tiling_shader;

            bool _grid_enabled = false; // application interface
            bool _show_grid = true;     // depends on scale
            std::vector<Shape*> _grid;

            struct Layer
            {
                Layer(size_t width, size_t height);

                Shape* _shape;
                Texture* _texture;

                GLNativeHandle _pixel_buffer;
                void* _pixel_buffer_data;
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

        size_t n = width * height * 4;

        glGenBuffers(1, &_pixel_buffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _pixel_buffer);
        glBufferData(GL_PIXEL_UNPACK_BUFFER, n * sizeof(float), nullptr, GL_STREAM_DRAW);
        auto* buffer_data = (float*) glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        for (size_t i = 0; i < 4 * n; i += 4)
        {
            buffer_data[i] = rand() / float(RAND_MAX);
            buffer_data[i+1] = rand() / float(RAND_MAX);
            buffer_data[i+2] = rand() / float(RAND_MAX);
            buffer_data[i+3] = 1;
        }

        glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    }

    Canvas::Canvas(size_t width, size_t height)
        : _resolution(width, height)
    {
        _gl_area = GTK_GL_AREA(gtk_gl_area_new());
        gtk_gl_area_set_has_alpha(_gl_area, TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(_gl_area), width, height);

        _frame = new Frame(float(width) / height);
        _frame->add(GTK_WIDGET(_gl_area));
        _frame->set_shadow_type(GtkShadowType::GTK_SHADOW_NONE);
        _frame->set_margin(10);

        g_signal_connect(_gl_area, "resize", G_CALLBACK(on_resize), this);
        g_signal_connect(_gl_area, "render", G_CALLBACK(on_render), this);
        g_signal_connect(_gl_area, "realize", G_CALLBACK(on_realize), this);
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

        for (size_t i = 1; i < instance->_resolution.x; ++i)
        {
            instance->_grid.push_back(new Shape());
            instance->_grid.back()->as_line({i * one_pixel.x, 0}, {i * one_pixel.x, 1});
        }

        for (size_t i = 1; i < instance->_resolution.y; ++i)
        {
            instance->_grid.push_back(new Shape());
            instance->_grid.back()->as_line({0, i * one_pixel.y}, {1, i * one_pixel.y});
        }

        for (auto* s : instance->_grid)
            s->set_color(RGBA(0, 0, 0, 1));

        // TODO
        instance->_layer = new Layer(instance->_resolution.x, instance->_resolution.y);
        // TODO

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

        // layers

        // TODO
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, instance->_layer->_pixel_buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, instance->_resolution.x, instance->_resolution.y, 0, GL_RGBA, GL_FLOAT, 0);

        instance->_layer->_shape->render(noop_shader, noop_transform);
        // TODO
        // grid

        if (instance->_grid_enabled and instance->_show_grid)
        {
            for (auto* line: instance->_grid)
                line->render(noop_shader, noop_transform);
        }

        glFlush();
        return FALSE;
    }

    void Canvas::on_resize(GtkGLArea*, int w, int h, Canvas* instance)
    {
        instance->_widget_size = {w, h};

        if (std::min(float(w) / instance->_resolution.x, float(h) / instance->_resolution.y) < 7)
            instance->_show_grid = false;
        else
            instance->_show_grid = true;

        gtk_gl_area_queue_render(instance->_gl_area);
    }

}
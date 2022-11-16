// 
// Copyright 2022 Clemens Cords
// Created on 10/17/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

#include <app/global_state.hpp>
#include <app/tooltip.hpp>

namespace mousetrap
{
    class ColorPreview : public AppComponent
    {
        public:
            ColorPreview();
            ~ColorPreview();

            operator Widget*() override;
            void update() override;

        private:
            void update(HSVA current, HSVA previous);
            static inline const float _previous_to_color_current_width_ratio = 0.15;

            GLArea _gl_area;
            static void on_gl_area_realize(Widget*, ColorPreview*);
            static void on_gl_area_resize(GLArea*, int, int, ColorPreview*);
            Vector2f _canvas_size;

            static inline Shader* transparency_tiling_shader = nullptr;

            Shape* _current_color_shape;
            Shape* _previous_color_shape;
            Shape* _frame_shape;
            Shape* _transparency_tiling_shape;

            Tooltip _tooltip;
    };
}

//

namespace mousetrap
{

    ColorPreview::ColorPreview()
    {
        _gl_area.connect_signal_realize(on_gl_area_realize, this);
        _gl_area.connect_signal_resize(on_gl_area_resize, this);
        _gl_area.set_expand(true);

        _tooltip.create_from("color_preview", state::tooltips_file, state::keybindings_file);
        operator Widget*()->set_tooltip_widget(_tooltip);
    }

    void ColorPreview::on_gl_area_realize(Widget* widget, ColorPreview* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        if (transparency_tiling_shader == nullptr)
        {
            transparency_tiling_shader = new Shader();
            transparency_tiling_shader->create_from_file(get_resource_path() + "shaders/transparency_tiling.frag", ShaderType::FRAGMENT);
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        instance->_previous_color_shape = new Shape();
        instance->_previous_color_shape->as_rectangle({0, 0}, {_previous_to_color_current_width_ratio, 1});
        instance->_previous_color_shape->set_color(state::primary_color);

        instance->_current_color_shape = new Shape();
        instance->_current_color_shape->as_rectangle({_previous_to_color_current_width_ratio, 0}, {1 - _previous_to_color_current_width_ratio, 1});
        instance->_current_color_shape->set_color(state::primary_color);

        {
            auto size = instance->_transparency_tiling_shape->get_size();
            auto top_left = instance->_transparency_tiling_shape->get_top_left();

            float eps = 0.001;
            std::vector<Vector2f> vertices = {
                    {top_left.x + eps, top_left.y + eps},
                    {top_left.x + size.x - eps, top_left.y + eps},
                    {top_left.x + size.x - eps, top_left.y + size.y - eps},
                    {top_left.x + eps, top_left.y + size.y - eps}
            };

            instance->_frame_shape = new Shape();
            instance->_frame_shape->as_wireframe(vertices);
            instance->_frame_shape->set_color(RGBA(0, 0, 0, 1));
        }

        instance->_transparency_tiling_shape = new Shape();
        instance->_transparency_tiling_shape->as_rectangle({0, 0}, {1, 1});

        auto transparency_render_task = RenderTask(instance->_transparency_tiling_shape, transparency_tiling_shader);
        transparency_render_task.register_vec2("_canvas_size", &instance->_canvas_size);

        area->add_render_task(transparency_render_task);
        area->add_render_task(instance->_current_color_shape);
        area->add_render_task(instance->_previous_color_shape);
        area->add_render_task(instance->_frame_shape);

        instance->update(state::primary_color, state::secondary_color);
        area->queue_render();
    }

    void ColorPreview::on_gl_area_resize(GLArea*, int w, int h, ColorPreview* instance)
    {
        instance->_canvas_size = {w, h};
        instance->_gl_area.queue_render();
    }

    void ColorPreview::update()
    {
        update(state::preview_color_current, state::preview_color_previous);
    }

    void ColorPreview::update(HSVA current, HSVA previous)
    {
        if (_current_color_shape == nullptr or _previous_color_shape == nullptr)
            return;

        _current_color_shape->set_color(current.operator RGBA());
        _previous_color_shape->set_color(previous.operator RGBA());
        _gl_area.queue_render();
    }

    ColorPreview::~ColorPreview()
    {
        delete _current_color_shape;
        delete _previous_color_shape;
        delete _frame_shape;
        delete _transparency_tiling_shape;
    }

    ColorPreview::operator Widget*()
    {
        return &_gl_area;
    }
}
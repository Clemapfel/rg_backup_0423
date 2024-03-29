//
// Created by clem on 2/12/23.
//

#include <app/canvas.hpp>

namespace mousetrap
{
    Canvas::GridLayer::GridLayer(Canvas* owner)
        : _owner(owner)
    {
        _area.connect_signal_realize(on_area_realize, this);
        _area.connect_signal_resize(on_area_resize, this);
    }

    Canvas::GridLayer::operator Widget*()
    {
        return &_area;
    }

    void Canvas::GridLayer::on_area_realize(Widget* widget, GridLayer* instance)
    {
        auto* area = (GLArea*) widget;
        area->make_current();

        instance->on_layer_resolution_changed();
        instance->set_visible(instance->_visible_requested);

        area->queue_render();
    }

    void Canvas::GridLayer::on_area_resize(GLArea*, int w, int h, GridLayer* instance)
    {
        *instance->_canvas_size = {w, h};
        instance->reformat();
        instance->_area.queue_render();
    }

    void Canvas::GridLayer::set_scale(float scale)
    {
        _scale = scale;
        reformat();
        _area.queue_render();
    }

    void Canvas::GridLayer::set_offset(Vector2f offset)
    {
        if (_offset == offset)
            return;

        _offset = {offset.x, offset.y};
        reformat();
        _area.queue_render();
    }

    void Canvas::GridLayer::set_visible(bool b)
    {
        _visible_requested = b;
        update_visibility();
        _area.queue_render();
    }

    void Canvas::GridLayer::on_layer_resolution_changed()
    {
        if (not _area.get_is_realized())
            return;

        _area.make_current();

        auto color = state::settings_file->get_value_as<HSVA>("canvas", "grid_color");

        for (auto* shape : _h_shapes)
            delete shape;

        _h_shapes.clear();
        while (_h_shapes.size() <= active_state->get_layer_resolution().y)
        {
            auto* shape = _h_shapes.emplace_back(new Shape());
            shape->set_color(color);
        }

        for (auto* shape : _v_shapes)
            delete shape;

        _v_shapes.clear();
        while (_v_shapes.size() <= active_state->get_layer_resolution().x)
        {
            _v_shapes.emplace_back(new Shape());
            auto* shape = _v_shapes.back();
            shape->set_color(color);
        }

        reformat();

        _area.clear_render_tasks();

        for (auto* shape : _h_shapes)
            _area.add_render_task(shape);

        for (auto* shape : _v_shapes)
            _area.add_render_task(shape);

        _area.queue_render();
    }

    void Canvas::GridLayer::reformat()
    {
        if (not _area.get_is_realized())
            return;

        auto layer_resolution = active_state->get_layer_resolution();

        float width = layer_resolution.x / _canvas_size->x;
        float height = layer_resolution.y / _canvas_size->y;

        width *= _scale;
        height *= _scale;

        Vector2f center = {0.5, 0.5};
        center += _offset;

        Vector2f top_left = center - Vector2f{0.5 * width, 0.5 * height};
        float pixel_w = width / layer_resolution.x;
        float pixel_h = height / layer_resolution.y;

        for (size_t i = 0; i <= layer_resolution.x; ++i)
        {
            _v_shapes.at(i)->as_line(
                top_left + Vector2f{i * pixel_w, 0},
                top_left + Vector2f(i * pixel_w, height)
            );
        }

        for (size_t i = 0; i <= layer_resolution.y; ++i)
        {
            _h_shapes.at(i)->as_line(
                top_left + Vector2f{0, i * pixel_h},
                top_left + Vector2f(width, i * pixel_h)
            );
        }

        update_visibility();
    }

    void Canvas::GridLayer::update_visibility()
    {
        if (not _area.get_is_realized())
            return;

        auto layer_resolution = active_state->get_layer_resolution();
        float width = layer_resolution.x / _canvas_size->x;
        float height = layer_resolution.y / _canvas_size->y;

        width *= _scale;
        height *= _scale;

        float pixel_w = width / layer_resolution.x;
        float pixel_h = height / layer_resolution.y;

        auto hide = std::min(pixel_w * _canvas_size->x, pixel_h * _canvas_size->y) < state::settings_file->get_value_as<float>("canvas", "grid_minimum_square_size");

        for (auto* shape : _v_shapes)
            shape->set_visible(not hide and _visible_requested);

        for (auto* shape : _h_shapes)
            shape->set_visible(not hide and _visible_requested);
    }
}

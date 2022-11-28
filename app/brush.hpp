// 
// Copyright 2022 Clemens Cords
// Created on 11/25/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <mousetrap.hpp>

namespace mousetrap
{
    class Brush
    {
        public:
            Brush();
            ~Brush();

            void create_from_image(Image&, const std::string& name);
            bool create_from_file(const std::string& path);

            Texture* get_texture();
            const Image& get_base_image();

            const std::string& get_name();
            const std::deque<std::pair<Vector2i, Vector2i>>& get_outline_vertices();

            /// \brief update texture as scaled version of base image
            void set_size(size_t px);
            size_t get_size() const;

        private:
            static inline const float alpha_eps = 0.00001; // lowest alpha value that is still registered as non-transparent

            Image _image;
            Texture* _texture = nullptr;
            std::string _name;

            void generate_outline_vertices(Image&);
            std::deque<std::pair<Vector2i, Vector2i>> _outline_vertices;
            // vertex position is top left of pixel coord, where top left of texture is (0, 0)
    };

    void reload_brushes();
}

//

namespace mousetrap
{
    Brush::Brush()
    {}

    Brush::~Brush()
    {
        delete _texture;
    }

    bool Brush::create_from_file(const std::string& path)
    {
        auto image = Image();
        if (not image.create_from_file(path))
        {
            std::cerr << "[WARNING] Unable to load brush at `" << path << "`: Image.create_from_file failed"
                      << std::endl;
            return false;
        }

        FileDescriptor file;
        file.create_for_path(path);

        std::stringstream name;
        for (size_t i = 0; i < file.get_name().size() - file.get_extension().size(); ++i)
            name << file.get_name().at(i);

        create_from_image(image, name.str());

        return true;
    }

    void Brush::create_from_image(Image& image, const std::string& name)
    {
        _image = image;
        auto size = image.get_size();

        for (size_t x = 0; x < size.x; ++x)
        {
            for (size_t y = 0; y < size.y; ++y)
            {
                RGBA color = image.get_pixel(x, y);

                auto as_hsva = color.operator HSVA();
                color.r = as_hsva.v;
                color.g = as_hsva.v;
                color.b = as_hsva.v;

                if (as_hsva.v < 0.05 or color.a < 0.05)
                    color.a = 0;
                else
                    color.a = 1;

                image.set_pixel(x, y, color);
            }
        }

        generate_outline_vertices(image);

        _texture = new Texture();
        _texture->create_from_image(image);

        _name = name;
    }

    Texture* Brush::get_texture()
    {
        return _texture;
    }

    const Image& Brush::get_base_image()
    {
        return _image;
    }

    const std::string& Brush::get_name()
    {
        return _name;
    }

    const std::deque<std::pair<Vector2i, Vector2i>>& Brush::get_outline_vertices()
    {
        return _outline_vertices;
    }

    void Brush::set_size(size_t px)
    {
        int w = _image.get_size().x;
        int h = _image.get_size().y;

        size_t new_w, new_h;

        if (w < h)
        {
            new_w = px;
            new_h = new_w + std::abs(h - w);
        }
        else if (h > w)
        {
            new_h = px;
            new_w = new_h + std::abs(w - h);
        }
        else
        {
            new_w = px;
            new_h = px;
        }

        Image scaled = _image.as_scaled(new_w, new_h);

        // prevent brush going invisible because of artifacting
        for (size_t i = 0; i < scaled.get_n_pixels(); ++i)
            if (scaled.get_pixel(i).a > alpha_eps)
                goto skip_artifact_fix;

        for (size_t i = 0; i < scaled.get_n_pixels(); ++i)
            scaled.set_pixel(i, RGBA(1, 1, 1, 1));

        skip_artifact_fix:

        _texture->create_from_image(scaled);
        generate_outline_vertices(scaled);
    }

    size_t Brush::get_size() const
    {
        return std::min(_image.get_size().x, _image.get_size().y);
    }

    void Brush::generate_outline_vertices(Image& image)
    {
        auto w = image.get_size().x;
        auto h = image.get_size().y;

        _outline_vertices.clear();

        // hlines
        for (size_t x = 0; x < w; ++x)
            for (size_t y = 0; y < h-1; ++y)
                if ((image.get_pixel(x, y).a > alpha_eps) != (image.get_pixel(x, y+1).a > alpha_eps))
                    _outline_vertices.push_back({{x, y+1}, {x+1, y+1}});

        for (size_t x = 0; x < w; ++x)
            if (image.get_pixel(x, 0).a > alpha_eps)
                _outline_vertices.push_back({{x, 0}, {x+1, 0}});

        for (size_t x = 0; x < w; ++x)
            if (image.get_pixel(x, h-1).a > alpha_eps)
                _outline_vertices.push_back({{x, h}, {x+1, h}});

        // vlines
        for (size_t y = 0; y < h; ++y)
            for (size_t x = 0; x < w-1; ++x)
                if (image.get_pixel(x, y).a > alpha_eps != image.get_pixel(x+1, y).a > alpha_eps)
                    _outline_vertices.push_back({{x+1, y}, {x+1, y+1}});

        for (size_t y = 0; y < h; ++y)
            if (image.get_pixel(0, y).a > alpha_eps)
                _outline_vertices.push_back({{0, y}, {0, y+1}});

        for (size_t y = 0; y < h; ++y)
            if (image.get_pixel(w-1, y).a > alpha_eps)
                _outline_vertices.push_back({{w, y}, {w, y+1}});
    }
}
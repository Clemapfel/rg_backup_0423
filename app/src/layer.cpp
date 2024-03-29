#include <app/layer.hpp>
#include <app/algorithms.hpp>
#include <app/project_state.hpp>

namespace mousetrap
{
    Layer::Frame::Frame()
        : _image(new Image()), _texture(new Texture()), _size(0, 0)
    {}

    Layer::Frame::Frame(Vector2i size)
        : Frame()
    {
        _size = size;
        _image->create(size.x, size.y, RGBA(0, 0, 0, 0));
        _texture->create_from_image(*_image);
    }

    Layer::Frame::Frame(const Frame& other)
        : Frame::Frame()
    {
        _image->create(other._image->get_size().x, other._image->get_size().y);
        for (size_t x = 0; x < other._image->get_size().x; ++x)
            for (size_t y = 0; y < other._image->get_size().y; ++y)
                _image->set_pixel(x, y, other._image->get_pixel(x, y));

        _texture->create_from_image(*_image);
    }

    Layer::Frame& Layer::Frame::operator=(const Frame& other)
    {
        if (_image == nullptr)
            _image = new Image();

        if (_texture == nullptr)
            _texture = new Texture();

        _image->create(other._image->get_size().x, other._image->get_size().y);
        for (size_t x = 0; x < other._image->get_size().x; ++x)
            for (size_t y = 0; y < other._image->get_size().y; ++y)
                _image->set_pixel(x, y, other._image->get_pixel(x, y));

        _texture->create_from_image(*_image);
        return *this;
    }

    Layer::Frame::Frame(Frame&& other)
            : _image(new Image()), _texture(new Texture())
    {
        if (_image == nullptr)
            _image = new Image();

        if (_texture == nullptr)
            _texture = new Texture();

        _image->create(other._image->get_size().x, other._image->get_size().y);
        for (size_t x = 0; x < other._image->get_size().x; ++x)
            for (size_t y = 0; y < other._image->get_size().y; ++y)
                _image->set_pixel(x, y, other._image->get_pixel(x, y));

        _texture->create_from_image(*_image);

        delete other._image;
        other._image = nullptr;
        delete other._texture;
        other._texture = nullptr;
    }

    Layer::Frame& Layer::Frame::operator=(Frame&& other)
    {
        if (_image == nullptr)
            _image = new Image();

        if (_texture == nullptr)
            _texture = new Texture();

        _image->create(other._image->get_size().x, other._image->get_size().y);
        for (size_t x = 0; x < other._image->get_size().x; ++x)
            for (size_t y = 0; y < other._image->get_size().y; ++y)
                _image->set_pixel(x, y, other._image->get_pixel(x, y));

        _texture->create_from_image(*_image);

        delete other._image;
        other._image = nullptr;
        delete other._texture;
        other._texture = nullptr;
        return *this;
    }

    Layer::Frame::~Frame()
    {
        delete _image;
        delete _texture;
    }

    RGBA Layer::Frame::get_pixel(size_t x, size_t y) const
    {
        auto coords = Vector2i(x + _offset.x, y + _offset.y);
        if (not (coords.x < 0 or coords.y < 0 or coords.x >= _image->get_size().x or coords.y >= _image->get_size().y))
            return _image->get_pixel(coords.x, coords.y);
        else
            return RGBA(0, 0, 0, 0);
    }

    void Layer::Frame::set_pixel(size_t x, size_t y, RGBA color)
    {
        _image->set_pixel(x - _offset.x, y - _offset.y, color);
    }

    void Layer::Frame::overwrite_image(const Image& image)
    {
        *_image = image;
    }

    void Layer::Frame::set_size(Vector2ui size)
    {
        _size = size;
    }

    Vector2ui Layer::Frame::get_size() const
    {
        return _size;
    }

    Vector2ui Layer::Frame::get_image_size() const
    {
        return _image->get_size();
    }

    const Texture* Layer::Frame::get_texture() const
    {
        return _texture;
    }

    bool Layer::Frame::get_is_keyframe() const
    {
        return _is_keyframe;
    }

    void Layer::Frame::set_is_keyframe(bool b)
    {
        _is_keyframe = b;
    }

    void Layer::Frame::set_offset(Vector2i offset)
    {
        _offset = offset;
    }

    Vector2i Layer::Frame::get_offset() const
    {
        return _offset;
    }

    void Layer::Frame::update_texture()
    {
        auto image = Image();
        image.create(_size.x, _size.y, RGBA(0, 0, 0, 0));

        for (int x = 0; x < _size.x; ++x)
        {
            for (int y = 0; y < _size.y; ++y)
            {
                auto coords = Vector2i(x + _offset.x, y + _offset.y);
                if (not (coords.x < 0 or coords.y < 0 or coords.x >= _image->get_size().x or coords.y >= _image->get_size().y))
                    image.set_pixel(x, y, get_pixel(coords.x, coords.y));
            }
        }

        _texture->create_from_image(image);
    }

    Layer::Layer(const std::string& name, Vector2ui size, size_t n_frames)
        : _name(name)
    {
        if (n_frames == 0)
            n_frames = 1;

        for (size_t i = 0; i < n_frames; ++i)
            add_frame(size, i);
    }

    Layer::Frame* Layer::add_frame(Vector2ui resolution, size_t i)
    {
        Layer::Frame* out;
        if (i >= _frames.size())
            out = _frames.emplace_back(new Frame(resolution));
        else
            out = *(_frames.emplace(_frames.begin() + i, new Frame(resolution)));

        return out;
    }

    void Layer::delete_frame(size_t i)
    {
        if (i >= _frames.size())
        {
            std::cerr << "[ERROR] In Layer::delete_frame: Trying to delete frame at index " << i << " but layer only has " << _frames.size() << " frames" << std::endl;
            return;
        }

        auto to_delete = _frames.at(i);
        _frames.erase(_frames.begin() + i);
        delete to_delete;
    }

    Layer::Layer(const Layer& other)
    {
        _name = other._name;
        _is_locked = other._is_locked;
        _is_visible = other._is_visible;
        _opacity = other._opacity;
        _blend_mode = other._blend_mode;

        _frames.clear();
        for (size_t i = 0; i < other._frames.size(); ++i)
        {
            auto size = other._frames.at(i)->_image->get_size();

            _frames.emplace_back(new Frame(size));
            for (size_t x = 0; x < size.x; ++x)
                for (size_t y = 0; y < size.y; ++y)
                    _frames.back()->_image->set_pixel(x, y, other._frames.at(i)->_image->get_pixel(x, y));

            _frames.back()->update_texture();
            _frames.back()->set_is_keyframe(other._frames.at(i)->get_is_keyframe());
        }
    }

    Layer& Layer::operator=(const Layer& other)
    {
        _name = other._name;
        _is_locked = other._is_locked;
        _is_visible = other._is_visible;
        _opacity = other._opacity;
        _blend_mode = other._blend_mode;

        _frames.clear();
        for (size_t i = 0; i < other._frames.size(); ++i)
        {
            auto size = other._frames.at(i)->_image->get_size();

            _frames.emplace_back(new Frame(size));
            for (size_t x = 0; x < size.x; ++x)
                for (size_t y = 0; y < size.y; ++y)
                    _frames.back()->_image->set_pixel(x, y, other._frames.at(i)->_image->get_pixel(x, y));

            _frames.back()->update_texture();
            _frames.back()->set_is_keyframe(other._frames.at(i)->get_is_keyframe());
        }

        return *this;
    }

    Layer::Frame* Layer::get_frame(size_t index)
    {
        return _frames.at(index);
    }

    const Layer::Frame* Layer::get_frame(size_t index) const
    {
        return _frames.at(index);
    }

    size_t Layer::get_n_frames() const
    {
        return _frames.size();
    }

    std::string Layer::get_name() const
    {
        return _name;
    }

    void Layer::set_name(const std::string& name)
    {
        _name = name;
    }

    bool Layer::get_is_locked() const
    {
        return _is_locked;
    }

    bool Layer::get_is_visible() const
    {
        return _is_visible;
    }

    void Layer::set_is_locked(bool b)
    {
        _is_locked = b;
    }

    void Layer::set_is_visible(bool b)
    {
        _is_visible = b;
    }

    float Layer::get_opacity() const
    {
        return _opacity;
    }

    void Layer::set_opacity(float v)
    {
        _opacity = glm::clamp<float>(v, 0, 1);
    }

    BlendMode Layer::get_blend_mode() const
    {
        return _blend_mode;
    }

    void Layer::set_blend_mode(BlendMode mode)
    {
        _blend_mode = mode;
    }

    /*
    void Layer::Frame::draw_pixel(Vector2i xy, RGBA color, BlendMode blend_mode)
    {
        if (not (xy.x >= 0 and xy.x < image->get_size().x and xy.y >= 0 and xy.y < image->get_size().y))
            return;

        auto dest = image->get_pixel(xy.x, xy.y);
        auto source = color;
        RGBA final;

        bool should_clamp = false;

        switch (blend_mode)
        {
            case BlendMode::NONE:
                final = source;
                break;

            case BlendMode::NORMAL:
            {
                final.r = source.r + dest.r * (1 - source.a);
                final.g = source.g + dest.g * (1 - source.a);
                final.b = source.b + dest.b * (1 - source.a);
                final.a = source.a + dest.a * (1 - source.a);

                if (final.r > source.r)
                    final.r = source.r;

                if (final.g > source.g)
                    final.g = source.g;

                if (final.b > source.b)
                    final.b = source.b;

                break;
            }

            case BlendMode::SUBTRACT:
                final.r = source.r - dest.r;
                final.g = source.g - dest.g;
                final.b = source.b - dest.b;
                final.a = source.a - dest.a;

                should_clamp = true;
                break;

            case BlendMode::MULTIPLY:
                final.r = source.r * dest.r;
                final.g = source.g * dest.g;
                final.b = source.b * dest.b;
                final.a = source.a * dest.a;

                should_clamp = true;
                break;

            case BlendMode::REVERSE_SUBTRACT:
                final.r = dest.r - source.r;
                final.g = dest.g - source.g;
                final.b = dest.b - source.b;
                final.a = dest.a - source.a;

                should_clamp = true;
                break;

            case BlendMode::ADD:
                final.r = source.r + dest.r;
                final.g = source.g + dest.g;
                final.b = source.b + dest.b;
                final.a = source.a + dest.a;

                should_clamp = true;
                break;

            case BlendMode::MAX:
                final.r = std::max<float>(source.r, dest.r);
                final.g = std::max<float>(source.g, dest.g);
                final.b = std::max<float>(source.b, dest.b);
                final.a = std::max<float>(source.a, dest.a);
                break;

            case BlendMode::MIN:
                final.r = std::min<float>(source.r, dest.r);
                final.g = std::min<float>(source.g, dest.g);
                final.b = std::min<float>(source.b, dest.b);
                final.a = std::min<float>(source.a, dest.a);
                break;
        }

        if (should_clamp)
        {
            final.r = glm::clamp<float>(final.r, 0, 1);
            final.g = glm::clamp<float>(final.g, 0, 1);
            final.b = glm::clamp<float>(final.b, 0, 1);
            final.a = glm::clamp<float>(final.a, 0, 1);
        }

        image->set_pixel(xy.x, xy.y, final);
    }

    void Layer::Frame::draw_line(Vector2i start, Vector2i end, RGBA color, BlendMode blend_mode)
    {
        for (auto& pos : generate_line_points(start, end))
            draw_pixel(pos, color, blend_mode);
    }

    void Layer::Frame::draw_polygon(const std::vector<Vector2i>& points, RGBA color, BlendMode blend_mode)
    {
        for (size_t i = 0; i < points.size(); ++i)
            for (auto& pos : generate_line_points(points.at(i), points.at(i + 1)))
                draw_pixel(pos, color, blend_mode);

        for (auto& pos : generate_line_points(points.back(), points.front()))
            draw_pixel(pos, color, blend_mode);
    }

    void Layer::Frame::draw_polygon_filled(const std::vector<Vector2i>& points, RGBA color, BlendMode blend_mode)
    {
        // degenerate area into horizontal bands

        struct Bounds
        {
            int64_t min;
            int64_t max;
        };

        std::map<int, Bounds> coords;
        auto add_point = [&](const Vector2i& pos) -> void
        {
            auto it = coords.find(pos.y);
            if (it == coords.end())
                coords.insert({pos.y, Bounds{pos.x, pos.x}});
            else
            {
                auto& bounds = it->second;
                bounds.min = std::min<int>(bounds.min, pos.x);
                bounds.max = std::max<int>(bounds.max, pos.x);
            }
        };

        for (size_t i = 0; i < points.size(); ++i)
            for (auto& pos : generate_line_points(points.at(i), points.at(i + 1)))
                add_point(pos);

        for (auto& pos : generate_line_points(points.back(), points.front()))
            add_point(pos);

        for (auto& pair : coords)
            for (size_t x = pair.second.min; x < pair.second.max; ++x)
                draw_pixel({x, pair.first}, color, blend_mode);
    }

    void Layer::Frame::draw_rectangle(Vector2i top_left, Vector2i bottom_right, RGBA color, size_t px, BlendMode blend_mode)
    {
        // guaranteed to only call draw_pixel the minimum number of times, no matter the input

        auto min_x = std::min(top_left.x, bottom_right.x);
        auto min_y = std::min(top_left.y, bottom_right.y);
        auto max_x = std::max(top_left.x, bottom_right.x);
        auto max_y = std::max(top_left.y, bottom_right.y);

        if (min_x < 0)
            min_x = 0;

        if (min_y < 0)
            min_y = 0;

        if (max_x >= image->get_size().x - 1)
            max_x = image->get_size().x - 1;

        if (max_y >= image->get_size().y - 1)
            max_y = image->get_size().y - 1;

        auto x = min_x;
        auto y = min_y;
        auto w = max_x - min_x;
        auto h = max_y - min_y;
        auto x_m = std::min<size_t>(px, w / 2);
        auto y_m = std::min<size_t>(px, h / 2);

        for (auto i = x; i < x + w; ++i)
            for (auto j = y; j < y + y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x; i < x + w; ++i)
            for (auto j = y + h - y_m; j < y + h; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x; y_m != h / 2 and i < x + x_m; ++i)
            for (auto j = y + y_m; j < y + h - y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);

        for (auto i = x + w - x_m; y_m != h / 2 and i < x + w; ++i)
            for (auto j = y + y_m; j < y + h - y_m; ++j)
                draw_pixel({i, j}, color, blend_mode);
    }

    void Layer::Frame::draw_rectangle_filled(Vector2i top_left, Vector2i bottom_right, RGBA color, BlendMode blend_mode)
    {
        auto min_x = std::min(top_left.x, bottom_right.x);
        auto min_y = std::min(top_left.y, bottom_right.y);
        auto max_x = std::max(top_left.x, bottom_right.x);
        auto max_y = std::max(top_left.y, bottom_right.y);

        if (min_x < 0)
            min_x = 0;

        if (min_y < 0)
            min_y = 0;

        if (max_x >= image->get_size().x - 1)
            max_x = image->get_size().x - 1;

        if (max_y >= image->get_size().y - 1)
            max_y = image->get_size().y - 1;

        for (size_t x = min_x; x < max_x; ++x)
            for (size_t y = min_y; y < max_y; ++y)
                draw_pixel({x, y}, color, blend_mode);
    }
     */
}


//
// Copyright 2022 Clemens Cords
// Created on 8/15/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/colors.hpp>

#include <vector>
#include <map>
#include <algorithm>

namespace mousetrap
{
    class Palette
    {
        static inline const float truncate_float_after = 10e4;

        public:
            Palette() = default;
            Palette(const std::vector<HSVA>&);

            void set_name(const std::string& name);
            std::string get_name();

            bool load_from(const std::string& file);
            bool save_to(const std::string& file);
            std::string serialize();

            size_t get_n_colors();
            std::vector<HSVA> get_colors();
            std::vector<HSVA> get_colors_by_hue();
            std::vector<HSVA> get_colors_by_value();
            std::vector<HSVA> get_colors_by_saturation();

        private:
            std::string _name = "palette";
            std::map<size_t, HSVA> _colors;
    };
}

// ###

#include <include/config_file.hpp>

namespace mousetrap
{
    Palette::Palette(const std::vector<HSVA>& vec)
    {
        for (size_t i = 0; i < vec.size(); ++i)
            _colors.insert({i, vec.at(i)});
    }

    void Palette::set_name(const std::string& name)
    {
        _name = name;
    }

    std::string Palette::get_name()
    {
        return _name;
    }

    size_t Palette::get_n_colors()
    {
        return _colors.size();
    }

    bool Palette::load_from(const std::string& path)
    {
        auto file = ConfigFile();
        _colors.clear();

        std::string error_reason;
        size_t key_index = 0;

        if (not file.load_from_file(path))
        {
            error_reason = "Unable to open file";
            goto abort;
        }

        if (not (file.get_groups().size() == 1))
        {
            error_reason = "More than one palette name present in file";
            goto abort;
        }

        _name = file.get_groups().at(0);

        for (auto key : file.get_keys(_name))
        {
            auto list = file.get_value_as<std::vector<float>>(_name, key);
            if (not (list.size() == 3 or list.size() == 4))
            {
                error_reason = "Invalid number of color components for key `" + key + "`: `" + file.get_value_as<std::string>(_name, key) + "`";
                goto abort;
            }

            float h = list.at(0);
            float s = list.at(1);
            float v = list.at(2);
            float a = list.size() == 3 ? 1 : list.at(3);

            for (float* component : {&h, &s, &v, &a})
            {
                if (*component < 0 or *component > 1)
                {
                    std::cerr << "[WARNING] In Palette::load_from: Color component for key `" << key << "` has value "
                              << *component << ", which is out of bounds. It will be rounded to the nearest value in [0, 1]" << std::endl;

                    *component = *component > 1 ? 1 : 0;
                }
            }

            static const float factor = truncate_float_after;
            h = float(int(h * factor)) / factor;
            s = float(int(s * factor)) / factor;
            v = float(int(v * factor)) / factor;
            a = float(int(a * factor)) / factor;

            _colors.insert({key_index, HSVA(h, s, v, a)});
            key_index += 1;
        }

        return true;

        abort:
        {
            std::cerr << "[ERROR] In Palette::load_from: Unable to load palette file from `" << path << "`: " <<  error_reason << std::endl;
            if (_colors.empty())
                _colors.insert({0, HSVA(0, 0, 0, 1)});

            return false;
        }
    }

    bool Palette::save_to(const std::string& path)
    {
        auto file = ConfigFile();
        for (auto& pair : _colors)
        {
            auto& color = pair.second;

            auto h = color.h;
            auto s = color.s;
            auto v = color.v;
            auto a = color.a;

            static const float factor = truncate_float_after;
            h = float(int(h * factor)) / factor;
            s = float(int(s * factor)) / factor;
            v = float(int(v * factor)) / factor;
            a = float(int(a * factor)) / factor;

            std::vector<float> list = {h, v, s, a};
            file.set_value_as<std::vector<float>>("palette", std::to_string(pair.first), list);
        }

        file.add_comment_above("palette", "mousetrap palette file\n format: index=hue;saturation;value[;alpha] (index in {0, 1, 2, ...}, components in [0, 1])\n generated " + std::string(g_date_time_format_iso8601(g_date_time_new_now(g_time_zone_new_local()))));
        return file.save_to_file(path);
    }

    std::vector<HSVA> Palette::get_colors()
    {
        std::vector<HSVA> out;
        for (auto& pair : _colors)
            out.push_back(pair.second);

        return out;
    }

    std::vector<HSVA> Palette::get_colors_by_hue()
    {
        auto grayscale = std::vector<HSVA>();
        auto non_grayscale = std::vector<HSVA>();

        for (auto& pair : _colors)
            if (pair.second.s == 0)
                grayscale.push_back(pair.second);
            else
                non_grayscale.push_back(pair.second);

        std::sort(grayscale.begin(), grayscale.end(), [](HSVA a, HSVA b) -> bool {
            return a.v < b.v;
        });

        std::sort(non_grayscale.begin(), non_grayscale.end(), [](HSVA a, HSVA b) -> bool {
           return a.h < b.h;
        });

        std::vector<HSVA> out;
        for (auto& c : non_grayscale)
            out.push_back(c);

        for (auto& c : grayscale)
            out.push_back(c);

        return out;
    }

    std::vector<HSVA> Palette::get_colors_by_saturation()
    {
        auto out = get_colors_by_hue();
        std::sort(out.begin(), out.end(), [](HSVA a, HSVA b) -> bool {
            if (a.s == b.s)
            {
                if (a.v == b.v)
                    return a.h > b.h;
                else
                    return a.v > b.v;
            }
            else
                return a.s > b.s;
        });

        return out;
    }

    std::vector<HSVA> Palette::get_colors_by_value()
    {
        auto out = get_colors_by_hue();
        std::sort(out.begin(), out.end(), [](HSVA a, HSVA b) -> bool {
            if (a.v == b.v)
            {
                if (a.s == b.s)
                    return a.h < b.h;
                else
                    return a.s > b.s;
            }
            else
                return a.v > b.v;
        });

        return out;
    }
}


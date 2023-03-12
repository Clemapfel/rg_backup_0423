//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 1/14/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_signals.hpp>

namespace mousetrap
{
    enum class SelectionMode
    {
        REPLACE,
        ADD,
        SUBTRACT
    };

    class Selection
    {
        public:
            Selection();

            /// @brief create from set of points
            void create_from(const Vector2iSet&);
            void create_from_rectangle(Vector2i top_left, Vector2i size);

            bool at(Vector2i) const;
            void apply_offset(Vector2i);
            void invert();

            operator std::string();

            using OutlineVertices = struct
            {
                std::vector<std::pair<Vector2f, Vector2f>> left_to_right;
                std::vector<std::pair<Vector2f, Vector2f>> right_to_left;
                std::vector<std::pair<Vector2f, Vector2f>> top_to_bottom;
                std::vector<std::pair<Vector2f, Vector2f>> bottom_to_top;
            };
            const OutlineVertices& get_outline_vertices() const;

        private:
            OutlineVertices _outline_vertices;

            // y -> {(x_min, x_max), ...}
            std::map<int, std::vector<std::pair<int, int>>> _data;

            int _x_min = 0;
            int _x_max = 0;
            int _y_min = 0;
            int _y_max = 0;
    };

}

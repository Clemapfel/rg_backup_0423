// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <string>

namespace mousetrap
{
    namespace detail
    {
        static inline const std::string DEFAULT_RESOURCE_PATH = "/home/clem/Workspace/mousetrap/resources/";
    }

    ///
    static inline std::string get_resource_path()
    {
        return detail::DEFAULT_RESOURCE_PATH;
    }
}

// 
// Copyright 2022 Clemens Cords
// Created on 8/26/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <gtk/gtk.h>

#include <string>
#include <map>

namespace mousetrap
{
    class SignalEmitter
    {
        public:
            void set_signal_blocked(const std::string& signal_id, bool);
            void set_all_signals_blocked(bool);

            template<typename Function_t>
            void connect_signal(const std::string& signal_id, Function_t*, void* data = nullptr);

            virtual operator GObject*() = 0;

        private:
            std::map<std::string, size_t> _signal_handlers = {};
    };
}

#include <src/signal_emitter.inl>
// 
// Copyright 2022 Clemens Cords
// Created on 7/31/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <include/widget.hpp>

namespace rat
{
    class Entry : public Widget
    {
        public:
            Entry();
            ~Entry();

            std::string get_text() const;
            GtkWidget* get_native() override;

        protected:
            virtual void on_activate(GtkEntry* self, gpointer user_data);
            virtual void on_paste_clipboard(GtkEntry* self, gpointer user_data);
            virtual void on_cut_clipboard(GtkEntry* self, gpointer user_data);
            virtual void on_copy_clipboard(GtkEntry* self, gpointer user_data);

        private:
            static void on_activate_wrapper(GtkEntry* self, void* instance);
            static void on_paste_clipboard_wrapper(GtkEntry* self, void* instance);
            static void on_cut_clipboard_wrapper(GtkEntry* self, void* instance);
            static void on_copy_clipboard_wrapper(GtkEntry* self, void* instance);

            GtkEntry* _entry;
            GtkEntryBuffer* _buffer;
    };
}

// ###

namespace rat
{
    Entry::Entry()
    {
        _buffer = gtk_entry_buffer_new(nullptr, 0);
        _entry = GTK_ENTRY(gtk_entry_new_with_buffer(_buffer));

        connect_signal("activate", on_activate_wrapper, this);
        connect_signal("paste-clipboard", on_paste_clipboard_wrapper, this);
        connect_signal("copy-clipboard", on_copy_clipboard_wrapper, this);
        connect_signal("cut-clipboard", on_cut_clipboard_wrapper, this);
    }

    Entry::~Entry()
    {
        gtk_widget_destroy(GTK_WIDGET(_entry));
        gtk_widget_destroy(GTK_WIDGET(_buffer));
    };

    GtkWidget* Entry::get_native()
    {
        return GTK_WIDGET(_entry);
    }

    std::string Entry::get_text() const
    {
        return gtk_entry_get_text(_entry);
    }

    void Entry::on_activate(GtkEntry* self, gpointer user_data)
    {
        // noop
    }

    void Entry::on_paste_clipboard(GtkEntry* self, gpointer user_data)
    {
        // noop
    }

    void Entry::on_copy_clipboard(GtkEntry* self, gpointer user_data)
    {
        // noop
    }

    void Entry::on_cut_clipboard(GtkEntry* self, gpointer user_data)
    {
        // noop
    }

    void Entry::on_activate_wrapper(GtkEntry* self, void* instance)
    {
        ((Entry*) instance)->on_activate(self, nullptr);
    }

    void Entry::on_paste_clipboard_wrapper(GtkEntry* self, void* instance)
    {
        ((Entry*) instance)->on_paste_clipboard(self, nullptr);
    }

    void Entry::on_cut_clipboard_wrapper(GtkEntry* self, void* instance)
    {
        ((Entry*) instance)->on_cut_clipboard(self, nullptr);
    }

    void Entry::on_copy_clipboard_wrapper(GtkEntry* self, void* instance)
    {
        ((Entry*) instance)->on_copy_clipboard(self, nullptr);
    }
}
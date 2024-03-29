namespace mousetrap
{
    inline FilePreview::FilePreview()
    {
        _icon_image_box.set_halign(GTK_ALIGN_CENTER);

        _main.push_back(&_icon_image_box);
        _main.push_back(&_file_name_label);
        _main.push_back(&_file_type_label);
        _main.push_back(&_file_size_label);

        _main.set_margin_horizontal(state::margin_unit);

        _file_name_label.set_halign(GTK_ALIGN_CENTER);
        _file_name_label.set_ellipsize_mode(EllipsizeMode::END);
        _file_type_label.set_ellipsize_mode(EllipsizeMode::START);
        _file_size_label.set_ellipsize_mode(EllipsizeMode::NONE);

        for (auto* label : {&_file_name_label, &_file_type_label, &_file_size_label})
        {
            label->set_wrap_mode(LabelWrapMode::ONLY_ON_CHAR);
            label->set_max_width_chars(8);
        }

        _file_name_label.set_margin_bottom(state::margin_unit);
        _main.set_hexpand(false);
        _main.set_vexpand(true);

        _window.set_size_request({preview_icon_pixel_size_factor * state::margin_unit, 0});
        _window.set_policy(GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
        _window.set_propagate_natural_height(true);
        _window.set_child(&_main);
    }

    inline FilePreview::operator Widget*()
    {
        return &_window;
    }

    inline void FilePreview::update()
    {
        update_from(_file);
    }

    inline void FilePreview::update_from(FileDescriptor* file)
    {
        _file = file;

        if (_file == nullptr)
            return;

        auto* pixbuf_maybe = gdk_pixbuf_new_from_file(_file->get_path().c_str(), nullptr);

        _icon_image_box.clear();
        if (pixbuf_maybe != nullptr)
        {
            auto target_width = preview_icon_pixel_size_factor * state::margin_unit;
            auto target_height = (gdk_pixbuf_get_height(pixbuf_maybe) / float(gdk_pixbuf_get_width(pixbuf_maybe))) * target_width;

            auto* pixbuf_scaled = gdk_pixbuf_scale_simple(pixbuf_maybe, target_width, target_height, GDK_INTERP_NEAREST);
            _icon_image = GTK_IMAGE(gtk_image_new_from_pixbuf(pixbuf_scaled));

            gtk_widget_set_size_request(GTK_WIDGET(_icon_image), target_width, target_height);
            gtk_widget_set_halign(GTK_WIDGET(_icon_image), GTK_ALIGN_CENTER);
            gtk_box_append(_icon_image_box.operator GtkBox*(), GTK_WIDGET(_icon_image));

            g_object_unref(pixbuf_maybe);
            g_object_unref(pixbuf_scaled);
        }
        else
        {
            //auto* info = g_file_query_info(_file->operator GFile*(), G_FILE_ATTRIBUTE_PREVIEW_ICON, G_FILE_QUERY_INFO_NONE, nullptr, nullptr);
            auto* icon = g_content_type_get_icon(_file->query_info(G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE).c_str());

            if (icon != nullptr)
            {
                _icon_image = GTK_IMAGE(gtk_image_new_from_gicon(G_ICON(icon)));
                gtk_widget_set_halign(GTK_WIDGET(_icon_image), GTK_ALIGN_CENTER);
                gtk_image_set_pixel_size(_icon_image, preview_icon_pixel_size_factor * state::margin_unit);
                gtk_box_append(_icon_image_box.operator GtkBox*(), GTK_WIDGET(_icon_image));
            }
        }

        std::stringstream file_name_text;
        file_name_text << "<span weight=\"bold\" size=\"100%\">"
                       << _file->query_info(G_FILE_ATTRIBUTE_STANDARD_DISPLAY_NAME)
                       << "</span>";

        _file_name_label.set_text(file_name_text.str());
        _file_type_label.set_text(_file->query_info(G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE));

        size_t size_byte = 0;

        try {
            size_byte = std::stoi(_file->query_info(G_FILE_ATTRIBUTE_STANDARD_ALLOCATED_SIZE));
        }
        catch (...)
        {}

        _file_size_label.set_text(_file->is_folder() ? g_format_size(size_byte) : g_format_size(size_byte));
    }
}

namespace mousetrap
{
    template<FileChooserDialogMode M>
    FileChooserDialog<M>::FileChooserDialog(const std::string& window_title)
            : _dialog(state::main_window, [&]() -> std::string
    {
        if (not window_title.empty())
            return window_title;

        if (M == FileChooserDialogMode::OPEN)
            return "Open File...";
        else if (M == FileChooserDialogMode::SAVE_AS)
            return "Save As...";
        else if (M == FileChooserDialogMode::CHOOSE_FOLDER)
            return "Select Folder...";
    }(), false)
    {
        _dialog.operator Window().set_titlebar_layout("title:close");
        _warn_on_override_dialog.operator Window().set_titlebar_layout("title:close");

        _file_chooser_frame.set_child(&_file_chooser);
        _file_chooser_frame.set_label_widget(nullptr);
        _file_chooser_frame.set_margin_end(state::margin_unit * 0.5);

        _file_chooser.set_expand(true);
        _file_chooser.set_focus_on_click(true);
        _file_chooser.set_can_create_folders(M != FileChooserDialogMode::OPEN);

        if (M == FileChooserDialogMode::SAVE_AS)
            _file_chooser.add_boolean_choice("WARN_ON_OVERRIDE", "Warn if file already exists", true);

        _preview_frame.set_child(_file_preview.operator Widget*());
        _preview_frame.set_margin_start(state::margin_unit * 0.5);
        _preview_frame.set_label_widget(&_preview_label);

        _file_chooser_preview_area.push_back(&_file_chooser_frame);
        _file_chooser_preview_area.push_back(&_preview_frame);
        _file_chooser_preview_area.set_margin_top(state::margin_unit);

        _name_entry.connect_signal_activate([&](Widget*, FileChooserDialog* instance){
            if (instance->_on_accept_pressed)
                instance->_on_accept_pressed(instance);
        }, this);
        _name_entry.set_hexpand(true);
        _name_entry.set_vexpand(false);

        _name_entry.set_margin_start(state::margin_unit);
        _name_entry.set_focus_on_click(true);

        _name_entry_box.push_back(&_name_entry_label);
        _name_entry_box.push_back(&_name_entry);
        _name_entry_box.set_vexpand(false);
        _name_entry_box.set_hexpand(true);
        _name_entry_box.set_margin_start(state::margin_unit);

        if (M == FileChooserDialogMode::SAVE_AS)
            _content_area.push_back(&_name_entry_box);

        _content_area.push_back(&_file_chooser_preview_area);
        _content_area.set_margin(state::margin_unit);

        _show_keybindings_button.set_halign(GTK_ALIGN_START);
        _show_keybindings_button.set_valign(GTK_ALIGN_END);
        _show_keybindings_button.set_always_show_arrow(true);
        //_show_keybindings_button.set_child(&_show_keybindings_button_label);

        _dialog.get_content_area().push_back(&_content_area);

        _warn_on_override_content.set_wrap_mode(LabelWrapMode::ONLY_ON_WORD);
        _warn_on_override_content.set_justify_mode(JustifyMode::LEFT);
        _warn_on_override_content.set_expand(false);
        _warn_on_override_content.set_halign(GTK_ALIGN_START);

        _warn_on_override_content_box.set_margin(state::margin_unit);
        _warn_on_override_content_box.set_child(&_warn_on_override_content);
        _warn_on_override_dialog.get_content_area().push_back(&_warn_on_override_content_box);
        _warn_on_override_dialog.get_content_area().set_expand(true);
        _warn_on_override_content_box.set_margin(2* state::margin_unit);

        _accept_button_label.set_margin_horizontal(state::margin_unit);
        _cancel_button_label.set_margin_horizontal(state::margin_unit);
        _warn_on_override_continue_label.set_margin_horizontal(state::margin_unit);
        _warn_on_override_cancel_label.set_margin_horizontal(state::margin_unit);

        _accept_button.set_child(&_accept_button_label);
        _cancel_button.set_child(&_cancel_button_label);
        _warn_on_override_continue_button.set_child(&_warn_on_override_continue_label);
        _warn_on_override_cancel_button.set_child(&_warn_on_override_cancel_label);

        auto size = std::max(
                _accept_button_label.get_preferred_size().natural_size.x,
                _cancel_button_label.get_preferred_size().natural_size.x
        );

        _accept_button_label.set_size_request({size, 0});
        _cancel_button_label.set_size_request({size, 0});

        size = std::max(
                _warn_on_override_continue_label.get_preferred_size().natural_size.x,
                _warn_on_override_cancel_label.get_preferred_size().natural_size.x
        );

        _warn_on_override_continue_label.set_size_request({size, 0});
        _warn_on_override_cancel_label.set_size_request({size, 0});

        _warn_on_override_dialog.add_action_widget(&_warn_on_override_cancel_button, [](FileChooserDialog* instance){
            instance->_warn_on_override_dialog.close();
        }, this);

        _warn_on_override_dialog.add_action_widget(&_warn_on_override_continue_button, [](FileChooserDialog* instance){
            if (instance->_on_accept_pressed)
                instance->_on_accept_pressed(instance);

            instance->_warn_on_override_dialog.close();
        }, this);

        _dialog.add_action_widget(&_cancel_button, [](FileChooserDialog* instance){
            if (instance->_on_cancel_pressed)
                instance->_on_cancel_pressed(instance);
        }, this);

        _dialog.add_action_widget(&_accept_button, [](FileChooserDialog* instance){

            if (M == FileChooserDialogMode::SAVE_AS and
                    instance->_file_chooser.get_boolean_choice("WARN_ON_OVERRIDE") and
                    FileSystem::file_exists(instance->_file_chooser.get_current_folder().get_path() + "/" + instance->_name_entry.get_text())
                    )
            {
                instance->_warn_on_override_content.set_text("<b>A file named `" + instance->_name_entry.get_text() + "` already exists. Do you want to replace it?</b>\n\nThis will override the files contents. This operation cannot be undone, continue anyway?");
                instance->_warn_on_override_dialog.present();
            }
            else
            {
                if (instance->_on_accept_pressed)
                    instance->_on_accept_pressed(instance);
            }
        }, this);

        // use tempfile instead of keybindings.ini because these cannot be changed, they are hardcoded into GtkFileChooserWidget
        auto temp_file = KeyFile();
        temp_file.load_from_string(R"(
[file_chooser_dialog]

# Close Dialog
close_dialog = Escape

# Enter Folder
enter_folder = Return

# Toggle show location popup
location_popup = <Control>L

# Toggle show search entry
search_shortcut = <Alt>S

# Toggle show hidden files
show_hidden = <Control>H

# Go to parent of current folder
up_folder = <Alt>Up

# Go to child of current folder
down_folder = <Alt>Down

# Jump to `Recent`
recent_shortcut = <Alt>R

# Jump to `Desktop`
desktop_folder = <Alt>D

# Jump to `Home`
home_folder = <Alt>Home
        )");

        _show_keybindings_content.set_title("Keybindings");
        _show_keybindings_content.create_from_group("file_chooser_dialog", &temp_file);
        _show_keybindings_popover.set_child(_show_keybindings_content);
        _show_keybindings_button.set_popover(&_show_keybindings_popover);
        _show_keybindings_button.set_tooltip_text("Show Keybindings");

        auto* button_area = gtk_widget_get_parent(_cancel_button.operator GtkWidget*());
        gtk_widget_set_margin_bottom(button_area, state::margin_unit);
        gtk_widget_set_margin_end(button_area, state::margin_unit);
        gtk_widget_set_margin_start(button_area, state::margin_unit);
        gtk_box_set_spacing(GTK_BOX(button_area), state::margin_unit);

        _accept_button.set_hexpand(false);
        _cancel_button.set_hexpand(false);
        _accept_button.set_halign(GTK_ALIGN_END);
        _cancel_button.set_halign(GTK_ALIGN_END);

        _action_button_area_spacer.set_hexpand(true);
        _action_button_area_spacer.set_halign(GTK_ALIGN_START);

        gtk_box_prepend(GTK_BOX(button_area), _action_button_area_spacer.operator GtkWidget*());
        gtk_box_prepend(GTK_BOX(button_area), _show_keybindings_button.operator GtkWidget*());
        gtk_widget_set_hexpand(GTK_WIDGET(button_area), true);

        // can't get button_area to expand no matter what so the length needs to hardcoded
        // button_area doesn't even have a getter to access it so this is jank to begin with

        float spacer_width = _content_area.get_preferred_size().natural_size.x;
        spacer_width -= _accept_button.get_preferred_size().natural_size.x;
        spacer_width -= _cancel_button.get_preferred_size().natural_size.x;
        spacer_width -= _show_keybindings_button.get_preferred_size().natural_size.x;
        _action_button_area_spacer.set_size_request({spacer_width, 0});
        _action_button_area_spacer.set_opacity(0);

        button_area = gtk_widget_get_parent(_warn_on_override_cancel_button.operator GtkWidget*());
        gtk_widget_set_margin_bottom(button_area, state::margin_unit);
        gtk_widget_set_margin_end(button_area, state::margin_unit);
        gtk_box_set_spacing(GTK_BOX(button_area), state::margin_unit);

        _file_chooser.add_tick_callback([](FrameClock clock, FileChooserDialog* instance) -> bool {

            auto selected = instance->_file_chooser.get_selected();
            if (not selected.empty() and instance->_previously_selected_path != selected.at(0).get_name() and selected.at(0).is_file())
            {
                auto file = selected.at(0);
                instance->_file_preview.update_from(&file);

                if (not instance->_name_entry_focused)
                    instance->_name_entry.set_text(selected.at(0).get_name());

                instance->_previously_selected_path = selected.at(0).get_name();
            }

            if (M == FileChooserDialogMode::SAVE_AS)
                instance->_accept_button.set_can_respond_to_input(not instance->_name_entry.get_text().empty());
            else if (M == FileChooserDialogMode::CHOOSE_FOLDER)
                instance->_accept_button.set_can_respond_to_input(not selected.empty() and selected.at(0).is_folder());
            else
                instance->_accept_button.set_can_respond_to_input(not selected.empty() and selected.at(0).is_file());

            return true;
        }, this);

        _key_event_controller.connect_signal_key_pressed(on_key_pressed, this);
        _dialog.add_controller(&_key_event_controller);

        _name_entry.connect_signal_activate([](Entry*, FileChooserDialog<M>* instance) {
            instance->_on_accept_pressed(instance);
        }, this);

        _focus_event_controller.connect_signal_focus_gained(on_focus_gained, this);
        _focus_event_controller.connect_signal_focus_lost(on_focus_lost, this);
        _name_entry.add_controller(&_focus_event_controller);
    }

    template<FileChooserDialogMode M>
    template<typename Function_t, typename Arg_t>
    void FileChooserDialog<M>::set_on_accept_pressed(Function_t f_in, Arg_t arg_in)
    {
        _on_accept_pressed = [f = f_in, arg = arg_in](FileChooserDialog* instance){
            f(instance, arg);
        };
    }

    template<FileChooserDialogMode M>
    template<typename Function_t, typename Arg_t>
    void FileChooserDialog<M>::set_on_cancel_pressed(Function_t f_in, Arg_t arg_in)
    {
        _on_cancel_pressed = [f = f_in, arg = arg_in](FileChooserDialog* instance){
            f(instance, arg);
        };
    }

    template<FileChooserDialogMode M>
    template<typename Function_t>
    void FileChooserDialog<M>::set_on_accept_pressed(Function_t f_in)
    {
        _on_accept_pressed = [f = f_in](FileChooserDialog* instance){
            f(instance);
        };
    }

    template<FileChooserDialogMode M>
    template<typename Function_t>
    void FileChooserDialog<M>::set_on_cancel_pressed(Function_t f_in)
    {
        _on_cancel_pressed = [f = f_in](FileChooserDialog* instance){
            f(instance);
        };
    }

    template<FileChooserDialogMode M>
    FileChooser& FileChooserDialog<M>::get_file_chooser()
    {
        return _file_chooser;
    }

    template<FileChooserDialogMode M>
    std::string FileChooserDialog<M>::get_current_name()
    {
        if (M == FileChooserDialogMode::SAVE_AS)
            return _file_chooser.get_current_folder().get_path() + "/" + _name_entry.get_text();
        else
        {
            auto selected = _file_chooser.get_selected();
            if (not selected.empty())
                return selected.at(0).get_path();
            else
                return "";
        }
    }

    template<FileChooserDialogMode M>
    Entry& FileChooserDialog<M>::get_name_entry()
    {
        return _name_entry;
    }

    template<FileChooserDialogMode M>
    void FileChooserDialog<M>::show()
    {
        _dialog.show();
    }

    template<FileChooserDialogMode M>
    void FileChooserDialog<M>::close()
    {
        _dialog.close();

        if (M == FileChooserDialogMode::SAVE_AS)
            _warn_on_override_dialog.close();
    }

    template<FileChooserDialogMode M>
    bool FileChooserDialog<M>::on_key_pressed(KeyEventController*, guint keyval, guint keycode, GdkModifierType state, FileChooserDialog<M>* instance)
    {
        if (keyval == GDK_KEY_Escape)
            instance->_on_cancel_pressed(instance);

        // return already consumed by GTK file explorer
        //if (keyval == GDK_KEY_Return and instance->_accept_button.get_can_respond_to_input())
        //   instance->_on_accept_pressed(instance);

        return true;
    }

    template<FileChooserDialogMode M>
    void FileChooserDialog<M>::on_focus_gained(FocusEventController*, double x, double y, FileChooserDialog<M>* instance)
    {
        instance->_name_entry_focused = true;
    }

    template<FileChooserDialogMode M>
    void FileChooserDialog<M>::on_focus_lost(FocusEventController*, double x, double y, FileChooserDialog<M>* instance)
    {
       instance->_name_entry_focused = false;
    }
}

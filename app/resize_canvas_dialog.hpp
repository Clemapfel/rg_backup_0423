//
// Copyright (c) Clemens Cords (mail@clemens-cords.com), created 2/5/23
//

#pragma once

#include <mousetrap.hpp>
#include <app/app_component.hpp>
#include <app/app_signals.hpp>
#include <app/config_files.hpp>

namespace mousetrap
{
    namespace state::actions
    {
        DECLARE_GLOBAL_ACTION(resize_canvas_dialog, open);
    }

    class ResizeCanvasDialog : public AppComponent,
        public signals::LayerFrameSelectionChanged,
        public signals::LayerImageUpdated,
        public signals::LayerCountChanged,
        public signals::LayerPropertiesChanged,
        public signals::LayerResolutionChanged
    {
        public:
            ResizeCanvasDialog();
            operator Widget*() override;

            void present();

        protected:
            void on_layer_resolution_changed() override;
            void on_layer_frame_selection_changed() override;
            void on_layer_image_updated() override;
            void on_layer_count_changed() override;
            void on_layer_properties_changed() override;

        private:
            enum ScaleMode
            {
                ABSOLUTE = false,
                RELATIVE = true
            };

            void set_scale_mode(ScaleMode);
            ScaleMode _scale_mode = ABSOLUTE;

            void set_aspect_ratio_locked(bool);
            bool _aspect_ratio_locked = true;

            size_t _width;
            size_t _height;

            void set_width(float);
            void set_height(float);
            void set_final_size(size_t w, size_t h);
            void update_area_aspect_ratio();

            Window _window;
            Dialog _dialog = Dialog(&_window, "Resize Canvas...");
            Box _window_box = Box(GTK_ORIENTATION_VERTICAL);

            SpinButton _width_spin_button;
            Label _width_label = Label("Width :");
            SeparatorLine _width_spacer;
            Box _width_box = Box(GTK_ORIENTATION_HORIZONTAL);

            SpinButton _height_spin_button;
            Label _height_label = Label("Height :");
            SeparatorLine _height_spacer;
            Box _height_box = Box(GTK_ORIENTATION_HORIZONTAL);

            DropDown _absolute_or_relative_dropdown;
            Label _absolute_list_label = Label("px");
            Label _absolute_when_selected_label = Label("px");
            Label _relative_list_label = Label("%");
            Label _relative_when_selected_label = Label("%");

            Box _spin_button_box = Box(GTK_ORIENTATION_VERTICAL);
            Box _spin_button_and_dropdown_box = Box(GTK_ORIENTATION_HORIZONTAL);

            CheckButton _maintain_aspect_ratio_button = CheckButton();
            Label _maintain_aspect_ratio_label;
            Box _maintain_aspect_ratio_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Label _offset_label = Label("<b>Offset</b>");

            int _x_offset = 0;
            SpinButton _x_offset_button;
            Label _x_offset_label = Label("x :");
            Box _x_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);
            void set_x_offset(int);

            SpinButton _y_offset_button;
            Label _y_offset_label = Label("y :");
            Box _y_offset_box = Box(GTK_ORIENTATION_HORIZONTAL);
            int _y_offset = 0;
            void set_y_offset(int);

            void update_offset_bounds();

            Box _offset_box = Box(GTK_ORIENTATION_VERTICAL);

            Label _center_button_label = Label("Center");
            Button _center_button;
            void center_offset();

            Label _reset_button_label = Label("Reset");
            Button _reset_button;
            void reset_offset();

            Box _offset_button_box = Box(GTK_ORIENTATION_HORIZONTAL);

            Label _instruction_label = Label(state::tooltips_file->get_value("resize_canvas_dialog", "instruction_label"));
            Label _final_size_label;

            Label _preview_label = Label("<b>Preview</b>");

            AspectFrame _aspect_frame = AspectFrame(1);
            GLArea _area;
            Shape* _new_boundary_shape = nullptr;
            Shape* _current_canvas_shape = nullptr;
            Texture* _current_canvas_texture = nullptr;

            Shader* _background_shader = nullptr;
            Shape* _background_shape = nullptr;

            void reformat_area();
            void update_current_image_texture();

            Vector2f* _area_size = new Vector2f{1, 1};
            static void on_area_realize(Widget*, ResizeCanvasDialog*);
            static void on_area_resize(GLArea*, int w, int h, ResizeCanvasDialog*);

            Button _accept_button;
            Label _accept_button_label = Label("Resize");

            Button _cancel_button;
            Label _cancel_button_label = Label("Cancel");
    };

    namespace state
    {
        inline ResizeCanvasDialog* resize_canvas_dialog = nullptr;
    }
}
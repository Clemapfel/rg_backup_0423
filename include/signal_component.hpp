// 
// Copyright 2022 Clemens Cords
// Created on 9/20/22 by clem (mail@clemens-cords.com)
//

#pragma once

#include <functional>

namespace mousetrap
{
    template<typename Owner_t>
    class HasToggledSignal
    {
        public:
            template<typename T>
            using on_toggled_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_toggled(Function_t, T);

            void set_signal_toggled_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasToggledSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_toggled_wrapper(void*, HasToggledSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_toggled_function_t<void*>> _on_toggled_f;
            void* _on_toggled_data;
    };

    template<typename Owner_t>
    class HasStateSetSignal
    {
        public:
            template<typename T>
            using on_state_set_function_t = void(Owner_t*, bool, T);

            template<typename Function_t, typename T>
            void connect_signal_state_set(Function_t, T);

            void set_signal_state_set_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasStateSetSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_state_set_wrapper(void*, bool, HasStateSetSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_state_set_function_t<void*>> _on_state_set_f;
            void* _on_state_set_data;
    };


    template<typename Owner_t>
    class HasClickedSignal
    {
        public:
            template<typename T>
            using on_clicked_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_clicked(Function_t, T);

            void set_signal_clicked_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasClickedSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_clicked_wrapper(void*, HasClickedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_clicked_function_t<void*>> _on_clicked_f;
            void* _on_clicked_data;
    };

    template<typename Owner_t>
    class HasActivateSignal
    {
        public:
            template<typename T>
            using on_activate_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_activate(Function_t, T);

            void set_signal_activate_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasActivateSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_activate_wrapper(void*, HasActivateSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_activate_function_t<void*>> _on_activate_f;
            void* _on_activate_data;
    };

    template<typename Owner_t>
    class HasUpdateSignal
    {
        public:
            template<typename T>
            using on_update_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_update(Function_t, T);

            void set_signal_update_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasUpdateSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_update_wrapper(void*, HasUpdateSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_update_function_t<void*>> _on_update_f;
            void* _on_update_data;
    };

    template<typename Owner_t>
    class HasRealizeSignal
    {
        public:
            template<typename T>
            using on_realize_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_realize(Function_t, T);

            void set_signal_realize_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasRealizeSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_realize_wrapper(void*, HasRealizeSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_realize_function_t<void*>> _on_realize_f;
            void* _on_realize_data;
    };

    template<typename Owner_t>
    class HasMapSignal
    {
        public:
            template<typename T>
            using on_map_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_map(Function_t, T);

            void set_signal_map_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasMapSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_map_wrapper(void*, HasMapSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_map_function_t<void*>> _on_map_f;
            void* _on_map_data;
    };

    template<typename Owner_t>
    class HasShowSignal
    {
        public:
            template<typename T>
            using on_show_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_show(Function_t, T);

            void set_signal_show_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasShowSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_show_wrapper(void*, HasShowSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_show_function_t<void*>> _on_show_f;
            void* _on_show_data;
    };

    template<typename Owner_t>
    class HasRenderSignal
    {
        public:
            template<typename T>
            using on_render_function_t = gboolean(Owner_t*, GdkGLContext*, T);

            template<typename Function_t, typename T>
            void connect_signal_render(Function_t, T);

            void set_signal_render_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasRenderSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static gboolean on_render_wrapper(void*, GdkGLContext*, HasRenderSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_render_function_t<void*>> _on_render_f;
            void* _on_render_data;
    };

    template<typename Owner_t>
    class HasResizeSignal
    {
        public:
            template<typename T>
            using on_resize_function_t = void(Owner_t*, int, int, T);

            template<typename Function_t, typename T>
            void connect_signal_resize(Function_t, T);

            void set_signal_resize_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasResizeSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_resize_wrapper(void*, int, int, HasResizeSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_resize_function_t<void*>> _on_resize_f;
            void* _on_resize_data;
    };

    template<typename Owner_t>
    class HasValueChangedSignal
    {
        public:
            template<typename T>
            using on_value_changed_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_value_changed(Function_t, T);

            void set_signal_value_changed_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasValueChangedSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_value_changed_wrapper(void*, HasValueChangedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_value_changed_function_t<void*>> _on_value_changed_f;
            void* _on_value_changed_data;
    };

    template<typename Owner_t>
    class HasKeyPressedSignal
    {
        public:
            template<typename T>
            using on_key_pressed_function_t = bool(Owner_t*, guint keyval, guint keycode, GdkModifierType state, T);

            template<typename Function_t, typename T>
            void connect_signal_key_pressed(Function_t, T);

            void set_signal_key_pressed_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasKeyPressedSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static bool on_key_pressed_wrapper(void*, guint keyval, guint keycode, GdkModifierType state, HasKeyPressedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_key_pressed_function_t<void*>> _on_key_pressed_f;
            void* _on_key_pressed_data;
    };

    template<typename Owner_t>
    class HasKeyReleasedSignal
    {
        public:
            template<typename T>
            using on_key_released_function_t = void(Owner_t*, guint keyval, guint keycode, GdkModifierType state, T);

            template<typename Function_t, typename T>
            void connect_signal_key_released(Function_t, T);

            void set_signal_key_released_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasKeyReleasedSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_key_released_wrapper(void*, guint keyval, guint keycode, GdkModifierType state, HasKeyReleasedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_key_released_function_t<void*>> _on_key_released_f;
            void* _on_key_released_data;
    };

    template<typename Owner_t>
    class HasModifiersChangedSignal
    {
        public:
            template<typename T>
            using on_modifiers_changed_function_t = bool(Owner_t*, GdkModifierType keyval, T);

            template<typename Function_t, typename T>
            void connect_signal_modifiers_changed(Function_t, T);

            void set_signal_modifiers_changed_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasModifiersChangedSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static bool on_modifiers_changed_wrapper(void*, GdkModifierType, HasModifiersChangedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_modifiers_changed_function_t<void*>> _on_modifiers_changed_f;
            void* _on_modifiers_changed_data;
    };

    template<typename Owner_t>
    class HasMotionEnterSignal
    {
        public:
            template<typename T>
            using on_motion_enter_function_t = void(Owner_t*, double x, double y, T);

            template<typename Function_t, typename T>
            void connect_signal_motion_enter(Function_t, T);

            void set_signal_motion_enter_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasMotionEnterSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_motion_enter_wrapper(void*, double x, double y, HasMotionEnterSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_motion_enter_function_t<void*>> _on_motion_enter_f;
            void* _on_motion_enter_data;
    };

    template<typename Owner_t>
    class HasMotionLeaveSignal
    {
        public:
            template<typename T>
            using on_motion_leave_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_motion_leave(Function_t, T);

            void set_signal_motion_leave_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasMotionLeaveSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_motion_leave_wrapper(void*, HasMotionLeaveSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_motion_leave_function_t<void*>> _on_motion_leave_f;
            void* _on_motion_leave_data;
    };

    template<typename Owner_t>
    class HasMotionSignal
    {
        public:
            template<typename T>
            using on_motion_function_t = void(Owner_t*, double x, double y, T);

            template<typename Function_t, typename T>
            void connect_signal_motion(Function_t, T);

            void set_signal_motion_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasMotionSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_motion_wrapper(void*, double x, double y, HasMotionSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_motion_function_t<void*>> _on_motion_f;
            void* _on_motion_data;
    };

    template<typename Owner_t>
    class HasClickPressedSignal
    {
        public:
            template<typename T>
            using on_click_pressed_function_t = void(Owner_t*, gint n_press, double x, double y, T);

            template<typename Function_t, typename T>
            void connect_signal_click_pressed(Function_t, T);

            void set_signal_click_pressed_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasClickPressedSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_click_pressed_wrapper(void*, gint n_press, double x, double y, HasClickPressedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_click_pressed_function_t<void*>> _on_click_pressed_f;
            void* _on_click_pressed_data;
    };

    template<typename Owner_t>
    class HasClickReleasedSignal
    {
        public:
            template<typename T>
            using on_click_released_function_t = void(Owner_t*, gint n_press, double x, double y, T);

            template<typename Function_t, typename T>
            void connect_signal_click_released(Function_t, T);

            void set_signal_click_released_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasClickReleasedSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_click_released_wrapper(void*, gint n_press, double x, double y, HasClickReleasedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_click_released_function_t<void*>> _on_click_released_f;
            void* _on_click_released_data;
    };

    template<typename Owner_t>
    class HasScrollBeginSignal
    {
        public:
            template<typename T>
            using on_scroll_begin_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_scroll_begin(Function_t, T);

            void set_signal_scroll_begin_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasScrollBeginSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_scroll_begin_wrapper(void*,HasScrollBeginSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_scroll_begin_function_t<void*>> _on_scroll_begin_f;
            void* _on_scroll_begin_data;
    };

    template<typename Owner_t>
    class HasScrollEndSignal
    {
        public:
            template<typename T>
            using on_scroll_end_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_scroll_end(Function_t, T);

            void set_signal_scroll_end_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasScrollEndSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_scroll_end_wrapper(void*,HasScrollEndSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_scroll_end_function_t<void*>> _on_scroll_end_f;
            void* _on_scroll_end_data;
    };

    template<typename Owner_t>
    class HasScrollSignal
    {
        public:
            template<typename T>
            using on_scroll_function_t = void(Owner_t*, double x, double y, T);

            template<typename Function_t, typename T>
            void connect_signal_scroll(Function_t, T);

            void set_signal_scroll_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasScrollSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_scroll_wrapper(void*, double x, double y, HasScrollSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_scroll_function_t<void*>> _on_scroll_f;
            void* _on_scroll_data;
    };

    template<typename Owner_t>
    class HasFocusGainedSignal
    {
        public:
            template<typename T>
            using on_focus_gained_function_t = void(Owner_t*, double x, double y, T);

            template<typename Function_t, typename T>
            void connect_signal_focus_gained(Function_t, T);

            void set_signal_focus_gained_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasFocusGainedSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_focus_gained_wrapper(void*, double x, double y, HasFocusGainedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_focus_gained_function_t<void*>> _on_focus_gained_f;
            void* _on_focus_gained_data;
    };

    template<typename Owner_t>
    class HasFocusLostSignal
    {
        public:
            template<typename T>
            using on_focus_lost_function_t = void(Owner_t*, double x, double y, T);

            template<typename Function_t, typename T>
            void connect_signal_focus_lost(Function_t, T);

            void set_signal_focus_lost_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasFocusLostSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_focus_lost_wrapper(void*, double x, double y, HasFocusLostSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_focus_lost_function_t<void*>> _on_focus_lost_f;
            void* _on_focus_lost_data;
    };

    template<typename Owner_t>
    class HasChildActivatedSignal
    {
        public:
            template<typename T>
            using on_child_activated_function_t = void(Owner_t*, size_t index, T);

            template<typename Function_t, typename T>
            void connect_signal_child_activated(Function_t, T);

            void set_signal_child_activated_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasChildActivatedSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_child_activated_wrapper(GtkFlowBox* self, GtkFlowBoxChild* child, HasChildActivatedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_child_activated_function_t<void*>> _on_child_activated_f;
            void* _on_child_activated_data;
    };

    template<typename Owner_t>
    class HasDragBeginSignal
    {
        public:
            template<typename T>
            using on_drag_begin_function_t = void(Owner_t*, double start_x, double start_y, T);

            template<typename Function_t, typename T>
            void connect_signal_drag_begin(Function_t, T);

            void set_signal_drag_begin_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasDragBeginSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_drag_begin_wrapper(void*, double x, double y, HasDragBeginSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_drag_begin_function_t<void*>> _on_drag_begin_f;
            void* _on_drag_begin_data;
    };

    template<typename Owner_t>
    class HasDragEndSignal
    {
        public:
            template<typename T>
            using on_drag_end_function_t = void(Owner_t*, double offset_x, double offset_y, T);

            template<typename Function_t, typename T>
            void connect_signal_drag_end(Function_t, T);

            void set_signal_drag_end_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasDragEndSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_drag_end_wrapper(void*, double x, double y, HasDragEndSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_drag_end_function_t<void*>> _on_drag_end_f;
            void* _on_drag_end_data;
    };

    template<typename Owner_t>
    class HasDragUpdateSignal
    {
        public:
            template<typename T>
            using on_drag_update_function_t = void(Owner_t*, double offset_x, double offset_y, T);

            template<typename Function_t, typename T>
            void connect_signal_drag_update(Function_t, T);

            void set_signal_drag_update_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasDragUpdateSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_drag_update_wrapper(void*, double x, double y, HasDragUpdateSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_drag_update_function_t<void*>> _on_drag_update_f;
            void* _on_drag_update_data;
    };

    template<typename Owner_t>
    class HasAttachSignal
    {
        public:
            template<typename T>
            using on_attach_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_attach(Function_t , T);

            void set_signal_attach_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasAttachSignal(Owner_t* instance)
                    : _instance(instance)
            {}

            void emit_signal_attach();

        private:
            Owner_t* _instance;

            bool _blocked = false;
            std::function<on_attach_function_t<void*>> _on_attach_f;
            void* _on_attach_data;
    };

    template<typename Owner_t>
    class HasDetachSignal
    {
        public:
            template<typename T>
            using on_detach_function_t = void(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_detach(Function_t , T);

            void set_signal_detach_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasDetachSignal(Owner_t* instance)
                    : _instance(instance)
            {}

            void emit_signal_detach();

        private:
            Owner_t* _instance;

            bool _blocked = false;
            std::function<on_detach_function_t<void*>> _on_detach_f;
            void* _on_detach_data;
    };

    class Widget;

    template<typename Owner_t>
    class HasReorderedSignal
    {
        public:
            template<typename T>
            using on_reordered_function_t = void(Owner_t*, Widget* row_widget_moved, size_t previous_position, size_t next_position, T data);

            template<typename Function_t, typename T>
            void connect_signal_reordered(Function_t , T);

            void set_signal_reordered_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasReorderedSignal(Owner_t* instance)
                    : _instance(instance)
            {}

            void emit_signal_reordered(Widget*, size_t previous_position, size_t next_position);

        private:
            Owner_t* _instance;

            bool _blocked = false;
            std::function<on_reordered_function_t<void*>> _on_reordered_f;
            void* _on_reordered_data;
    };

    template<typename Owner_t>
    class HasListItemActivateSignal
    {
        public:
            template<typename T>
            using on_list_item_activate_function_t = void(Owner_t*, size_t item_position, T);

            template<typename Function_t, typename T>
            void connect_signal_list_item_activate(Function_t, T);

            void set_signal_list_item_activate_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasListItemActivateSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_list_item_activate_wrapper(GtkListView*, guint position, HasListItemActivateSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_list_item_activate_function_t<void*>> _on_list_item_activate_f;
            void* _on_list_item_activate_data;
    };

    template<typename Owner_t>
    class HasSelectionChangedSignal
    {
        public:
            template<typename T>
            using on_selection_changed_function_t = void(Owner_t*, size_t first_item_position, size_t n_items_changed, T);

            template<typename Function_t, typename T>
            void connect_signal_selection_changed(Function_t, T);

            void set_signal_selection_changed_blocked(bool b) {
                _blocked = b;
            }

        protected:
            HasSelectionChangedSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_selection_changed_wrapper(GtkSelectionModel*, guint position, guint n_items, HasSelectionChangedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_selection_changed_function_t<void*>> _on_selection_changed_f;
            void* _on_selection_changed_data;
    };

    template<typename Owner_t>
    class HasCloseSignal
    {
        public:
            template<typename T>
            using on_close_function_t = bool(Owner_t*, T);

            template<typename Function_t, typename T>
            void connect_signal_close(Function_t, T);

            void set_signal_close_blocked(bool b) {
                _blocked = b;
            }

            void emit_signal_close()
            {
                if (_on_close_f != nullptr and not _blocked)
                    _on_close_f(_instance, _on_close_data);
            };

        protected:
            HasCloseSignal(Owner_t* instance)
                    : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static gboolean on_close_wrapper(Widget*, HasCloseSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_close_function_t<void*>> _on_close_f;
            void* _on_close_data;
    };

    template<typename Owner_t>
    class HasScaleChangedSignal
    {
        public:
            template<typename T>
            using on_scale_changed_function_t = void(Owner_t*, double, T);

            template<typename Function_t, typename T>
            void connect_signal_scale_changed(Function_t, T);

            void set_signal_scale_changed_blocked(bool b) {
                _blocked = b;
            }

            void emit_signal_scale_changed(double scale)
            {
                if (_on_scale_changed_f != nullptr and not _blocked)
                    _on_scale_changed_f(_instance, scale, _on_scale_changed_data);
            };

        protected:
            HasScaleChangedSignal(Owner_t* instance)
                : _instance(instance)
            {}

        private:
            Owner_t* _instance;

            static void on_scale_changed_wrapper(Widget*, gdouble, HasScaleChangedSignal<Owner_t>* instance);

            bool _blocked = false;
            std::function<on_scale_changed_function_t<void*>> _on_scale_changed_f;
            void* _on_scale_changed_data;
    };

}

// ###

namespace mousetrap
{
    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasToggledSignal<Owner_t>::connect_signal_toggled(Function_t function, T data)
    {
        auto temp =  std::function<on_toggled_function_t<T>>(function);
        _on_toggled_f = std::function<on_toggled_function_t<void*>>(*((std::function<on_toggled_function_t<void*>>*) &temp));
        _on_toggled_data = data;

        _instance->connect_signal("toggled", on_toggled_wrapper, this);
    }

    template<typename Owner_t>
    void HasToggledSignal<Owner_t>::on_toggled_wrapper(void*, HasToggledSignal<Owner_t>* self)
    {
        if (self->_on_toggled_f != nullptr and not self->_blocked)
            self->_on_toggled_f(self->_instance, self->_on_toggled_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasStateSetSignal<Owner_t>::connect_signal_state_set(Function_t function, T data)
    {
        auto temp =  std::function<on_state_set_function_t<T>>(function);
        _on_state_set_f = std::function<on_state_set_function_t<void*>>(*((std::function<on_state_set_function_t<void*>>*) &temp));
        _on_state_set_data = data;

        _instance->connect_signal("state_set", on_state_set_wrapper, this);
    }

    template<typename Owner_t>
    void HasStateSetSignal<Owner_t>::on_state_set_wrapper(void*, bool state, HasStateSetSignal<Owner_t>* self)
    {
        if (self->_on_state_set_f != nullptr and not self->_blocked)
            self->_on_state_set_f(self->_instance, state, self->_on_state_set_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasClickedSignal<Owner_t>::connect_signal_clicked(Function_t function, T data)
    {
        auto temp =  std::function<on_clicked_function_t<T>>(function);
        _on_clicked_f = std::function<on_clicked_function_t<void*>>(*((std::function<on_clicked_function_t<void*>>*) &temp));
        _on_clicked_data = data;

        _instance->connect_signal("clicked", on_clicked_wrapper, this);
    }

    template<typename Owner_t>
    void HasClickedSignal<Owner_t>::on_clicked_wrapper(void*, HasClickedSignal<Owner_t>* self)
    {
        if (self->_on_clicked_f != nullptr and not self->_blocked)
            self->_on_clicked_f(self->_instance, self->_on_clicked_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasActivateSignal<Owner_t>::connect_signal_activate(Function_t function, T data)
    {
        auto temp =  std::function<on_activate_function_t<T>>(function);
        _on_activate_f = std::function<on_activate_function_t<void*>>(*((std::function<on_activate_function_t<void*>>*) &temp));
        _on_activate_data = data;

        _instance->connect_signal("activate", on_activate_wrapper, this);
    }

    template<typename Owner_t>
    void HasActivateSignal<Owner_t>::on_activate_wrapper(void*, HasActivateSignal<Owner_t>* self)
    {
        if (self->_on_activate_f != nullptr and not self->_blocked)
            self->_on_activate_f(self->_instance, self->_on_activate_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasUpdateSignal<Owner_t>::connect_signal_update(Function_t function, T data)
    {
        auto temp =  std::function<on_update_function_t<T>>(function);
        _on_update_f = std::function<on_update_function_t<void*>>(*((std::function<on_update_function_t<void*>>*) &temp));
        _on_update_data = data;

        _instance->connect_signal("update", on_update_wrapper, this);
    }

    template<typename Owner_t>
    void HasUpdateSignal<Owner_t>::on_update_wrapper(void*, HasUpdateSignal<Owner_t>* self)
    {
        if (self->_on_update_f != nullptr and not self->_blocked)
            self->_on_update_f(self->_instance, self->_on_activate_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasRealizeSignal<Owner_t>::connect_signal_realize(Function_t function, T data)
    {
        auto temp =  std::function<on_realize_function_t<T>>(function);
        _on_realize_f = std::function<on_realize_function_t<void*>>(*((std::function<on_realize_function_t<void*>>*) &temp));
        _on_realize_data = data;

        _instance->connect_signal("realize", on_realize_wrapper, this);
    }

    template<typename Owner_t>
    void HasRealizeSignal<Owner_t>::on_realize_wrapper(void*, HasRealizeSignal<Owner_t>* self)
    {
        if (self->_on_realize_f != nullptr and not self->_blocked)
            self->_on_realize_f(self->_instance, self->_on_realize_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasShowSignal<Owner_t>::connect_signal_show(Function_t function, T data)
    {
        auto temp =  std::function<on_show_function_t<T>>(function);
        _on_show_f = std::function<on_show_function_t<void*>>(*((std::function<on_show_function_t<void*>>*) &temp));
        _on_show_data = data;

        _instance->connect_signal("show", on_show_wrapper, this);
    }

    template<typename Owner_t>
    void HasShowSignal<Owner_t>::on_show_wrapper(void*, HasShowSignal<Owner_t>* self)
    {
        if (self->_on_show_f != nullptr and not self->_blocked)
            self->_on_show_f(self->_instance, self->_on_show_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasMapSignal<Owner_t>::connect_signal_map(Function_t function, T data)
    {
        auto temp =  std::function<on_map_function_t<T>>(function);
        _on_map_f = std::function<on_map_function_t<void*>>(*((std::function<on_map_function_t<void*>>*) &temp));
        _on_map_data = data;

        _instance->connect_signal("map", on_map_wrapper, this);
    }

    template<typename Owner_t>
    void HasMapSignal<Owner_t>::on_map_wrapper(void*, HasMapSignal<Owner_t>* self)
    {
        if (self->_on_map_f != nullptr and not self->_blocked)
            self->_on_map_f(self->_instance, self->_on_map_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasRenderSignal<Owner_t>::connect_signal_render(Function_t function, T data)
    {
        auto temp =  std::function<on_render_function_t<T>>(function);
        _on_render_f = std::function<on_render_function_t<void*>>(*((std::function<on_render_function_t<void*>>*) &temp));
        _on_render_data = data;

        _instance->connect_signal("render", on_render_wrapper, this);
    }

    template<typename Owner_t>
    gboolean HasRenderSignal<Owner_t>::on_render_wrapper(void*, GdkGLContext* context, HasRenderSignal<Owner_t>* self)
    {
        if (self->_on_render_f != nullptr and not self->_blocked)
            return self->_on_render_f(self->_instance, context, self->_on_render_data);

        return FALSE;
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasResizeSignal<Owner_t>::connect_signal_resize(Function_t function, T data)
    {
        auto temp =  std::function<on_resize_function_t<T>>(function);
        _on_resize_f = std::function<on_resize_function_t<void*>>(*((std::function<on_resize_function_t<void*>>*) &temp));
        _on_resize_data = data;

        _instance->connect_signal("resize", on_resize_wrapper, this);
    }

    template<typename Owner_t>
    void HasResizeSignal<Owner_t>::on_resize_wrapper(void*, int w, int h, HasResizeSignal<Owner_t>* self)
    {
        if (self->_on_resize_f != nullptr and not self->_blocked)
            self->_on_resize_f(self->_instance, w, h, self->_on_resize_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasValueChangedSignal<Owner_t>::connect_signal_value_changed(Function_t function, T data)
    {
        auto temp =  std::function<on_value_changed_function_t<T>>(function);
        _on_value_changed_f = std::function<on_value_changed_function_t<void*>>(*((std::function<on_value_changed_function_t<void*>>*) &temp));
        _on_value_changed_data = data;

        _instance->connect_signal("value-changed", on_value_changed_wrapper, this);
    }

    template<typename Owner_t>
    void HasValueChangedSignal<Owner_t>::on_value_changed_wrapper(void*, HasValueChangedSignal<Owner_t>* self)
    {
        if (self->_on_value_changed_f != nullptr and not self->_blocked)
            self->_on_value_changed_f(self->_instance, self->_on_value_changed_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasKeyPressedSignal<Owner_t>::connect_signal_key_pressed(Function_t function, T data)
    {
        auto temp = std::function<on_key_pressed_function_t<T>>(function);
        _on_key_pressed_f = std::function<on_key_pressed_function_t<void*>>(*((std::function<on_key_pressed_function_t<void*>>*) &temp));
        _on_key_pressed_data = data;

        _instance->connect_signal("key-pressed", on_key_pressed_wrapper, this);
    }

    template<typename Owner_t>
    bool HasKeyPressedSignal<Owner_t>::on_key_pressed_wrapper(void*, guint keyval, guint keycode, GdkModifierType state,
                                                              HasKeyPressedSignal<Owner_t>* self)
    {
        if (self->_on_key_pressed_f != nullptr and not self->_blocked)
            return self->_on_key_pressed_f(self->_instance, keyval, keycode, state, self->_on_key_pressed_data);
        else
            return FALSE;
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasKeyReleasedSignal<Owner_t>::connect_signal_key_released(Function_t function, T data)
    {
        auto temp =  std::function<on_key_released_function_t<T>>(function);
        _on_key_released_f = std::function<on_key_released_function_t<void*>>(*((std::function<on_key_released_function_t<void*>>*) &temp));
        _on_key_released_data = data;

        _instance->connect_signal("key-released", on_key_released_wrapper, this);
    }

    template<typename Owner_t>
    void HasKeyReleasedSignal<Owner_t>::on_key_released_wrapper(void*, guint keyval, guint keycode, GdkModifierType state, HasKeyReleasedSignal<Owner_t>* self)
    {
        if (self->_on_key_released_f != nullptr and not self->_blocked)
            self->_on_key_released_f(self->_instance, keyval, keycode, state, self->_on_key_released_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasModifiersChangedSignal<Owner_t>::connect_signal_modifiers_changed(Function_t function, T data)
    {
        auto temp =  std::function<on_modifiers_changed_function_t<T>>(function);
        _on_modifiers_changed_f = std::function<on_modifiers_changed_function_t<void*>>(*((std::function<on_modifiers_changed_function_t<void*>>*) &temp));
        _on_modifiers_changed_data = data;

        _instance->connect_signal("modifiers", on_modifiers_changed_wrapper, this);
    }

    template<typename Owner_t>
    bool HasModifiersChangedSignal<Owner_t>::on_modifiers_changed_wrapper(void*, GdkModifierType state, HasModifiersChangedSignal<Owner_t>* self)
    {
        if (self->_on_modifiers_changed_f != nullptr and not self->_blocked)
            return self->_on_modifiers_changed_f(self->_instance, state, self->_on_modifiers_changed_data);
        else
            return FALSE;
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasMotionEnterSignal<Owner_t>::connect_signal_motion_enter(Function_t function, T data)
    {
        auto temp =  std::function<on_motion_enter_function_t<T>>(function);
        _on_motion_enter_f = std::function<on_motion_enter_function_t<void*>>(*((std::function<on_motion_enter_function_t<void*>>*) &temp));
        _on_motion_enter_data = data;

        _instance->connect_signal("enter", on_motion_enter_wrapper, this);
    }

    template<typename Owner_t>
    void HasMotionEnterSignal<Owner_t>::on_motion_enter_wrapper(void*, double x, double y, HasMotionEnterSignal<Owner_t>* self)
    {
        if (self->_on_motion_enter_f != nullptr and not self->_blocked)
            self->_on_motion_enter_f(self->_instance, x, y, self->_on_motion_enter_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasMotionLeaveSignal<Owner_t>::connect_signal_motion_leave(Function_t function, T data)
    {
        auto temp =  std::function<on_motion_leave_function_t<T>>(function);
        _on_motion_leave_f = std::function<on_motion_leave_function_t<void*>>(*((std::function<on_motion_leave_function_t<void*>>*) &temp));
        _on_motion_leave_data = data;

        _instance->connect_signal("leave", on_motion_leave_wrapper, this);
    }

    template<typename Owner_t>
    void HasMotionLeaveSignal<Owner_t>::on_motion_leave_wrapper(void*, HasMotionLeaveSignal<Owner_t>* self)
    {
        if (self->_on_motion_leave_f != nullptr and not self->_blocked)
            self->_on_motion_leave_f(self->_instance, self->_on_motion_leave_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasMotionSignal<Owner_t>::connect_signal_motion(Function_t function, T data)
    {
        auto temp =  std::function<on_motion_function_t<T>>(function);
        _on_motion_f = std::function<on_motion_function_t<void*>>(*((std::function<on_motion_function_t<void*>>*) &temp));
        _on_motion_data = data;

        _instance->connect_signal("motion", on_motion_wrapper, this);
    }

    template<typename Owner_t>
    void HasMotionSignal<Owner_t>::on_motion_wrapper(void*, double x, double y, HasMotionSignal<Owner_t>* self)
    {
        if (self->_on_motion_f != nullptr and not self->_blocked)
            self->_on_motion_f(self->_instance, x, y, self->_on_motion_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasClickPressedSignal<Owner_t>::connect_signal_click_pressed(Function_t function, T data)
    {
        auto temp =  std::function<on_click_pressed_function_t<T>>(function);
        _on_click_pressed_f = std::function<on_click_pressed_function_t<void*>>(*((std::function<on_click_pressed_function_t<void*>>*) &temp));
        _on_click_pressed_data = data;

        _instance->connect_signal("pressed", on_click_pressed_wrapper, this);
    }

    template<typename Owner_t>
    void HasClickPressedSignal<Owner_t>::on_click_pressed_wrapper(void*, gint n_press, double x, double y, HasClickPressedSignal<Owner_t>* self)
    {
        if (self->_on_click_pressed_f != nullptr and not self->_blocked)
            self->_on_click_pressed_f(self->_instance, n_press, x, y, self->_on_click_pressed_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasClickReleasedSignal<Owner_t>::connect_signal_click_released(Function_t function, T data)
    {
        auto temp =  std::function<on_click_released_function_t<T>>(function);
        _on_click_released_f = std::function<on_click_released_function_t<void*>>(*((std::function<on_click_released_function_t<void*>>*) &temp));
        _on_click_released_data = data;

        _instance->connect_signal("released", on_click_released_wrapper, this);
    }

    template<typename Owner_t>
    void HasClickReleasedSignal<Owner_t>::on_click_released_wrapper(void*, gint n_press, double x, double y, HasClickReleasedSignal<Owner_t>* self)
    {
        if (self->_on_click_released_f != nullptr and not self->_blocked)
            self->_on_click_released_f(self->_instance, n_press, x, y, self->_on_click_released_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasScrollBeginSignal<Owner_t>::connect_signal_scroll_begin(Function_t function, T data)
    {
        auto temp =  std::function<on_scroll_begin_function_t<T>>(function);
        _on_scroll_begin_f = std::function<on_scroll_begin_function_t<void*>>(*((std::function<on_scroll_begin_function_t<void*>>*) &temp));
        _on_scroll_begin_data = data;

        _instance->connect_signal("scroll-begin", on_scroll_begin_wrapper, this);
    }

    template<typename Owner_t>
    void HasScrollBeginSignal<Owner_t>::on_scroll_begin_wrapper(void*, HasScrollBeginSignal<Owner_t>* self)
    {
        if (self->_on_scroll_begin_f != nullptr and not self->_blocked)
            self->_on_scroll_begin_f(self->_instance, self->_on_scroll_begin_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasScrollEndSignal<Owner_t>::connect_signal_scroll_end(Function_t function, T data)
    {
        auto temp =  std::function<on_scroll_end_function_t<T>>(function);
        _on_scroll_end_f = std::function<on_scroll_end_function_t<void*>>(*((std::function<on_scroll_end_function_t<void*>>*) &temp));
        _on_scroll_end_data = data;

        _instance->connect_signal("scroll-end", on_scroll_end_wrapper, this);
    }

    template<typename Owner_t>
    void HasScrollEndSignal<Owner_t>::on_scroll_end_wrapper(void*, HasScrollEndSignal<Owner_t>* self)
    {
        if (self->_on_scroll_end_f != nullptr and not self->_blocked)
            self->_on_scroll_end_f(self->_instance, self->_on_scroll_end_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasScrollSignal<Owner_t>::connect_signal_scroll(Function_t function, T data)
    {
        auto temp =  std::function<on_scroll_function_t<T>>(function);
        _on_scroll_f = std::function<on_scroll_function_t<void*>>(*((std::function<on_scroll_function_t<void*>>*) &temp));
        _on_scroll_data = data;

        _instance->connect_signal("scroll", on_scroll_wrapper, this);
    }

    template<typename Owner_t>
    void HasScrollSignal<Owner_t>::on_scroll_wrapper(void*, double x, double y, HasScrollSignal<Owner_t>* self)
    {
        if (self->_on_scroll_f != nullptr and not self->_blocked)
            self->_on_scroll_f(self->_instance, x, y, self->_on_scroll_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasFocusGainedSignal<Owner_t>::connect_signal_focus_gained(Function_t function, T data)
    {
        auto temp =  std::function<on_focus_gained_function_t<T>>(function);
        _on_focus_gained_f = std::function<on_focus_gained_function_t<void*>>(*((std::function<on_focus_gained_function_t<void*>>*) &temp));
        _on_focus_gained_data = data;

        _instance->connect_signal("enter", on_focus_gained_wrapper, this);
    }

    template<typename Owner_t>
    void HasFocusGainedSignal<Owner_t>::on_focus_gained_wrapper(void*, double x, double y, HasFocusGainedSignal<Owner_t>* self)
    {
        if (self->_on_focus_gained_f != nullptr and not self->_blocked)
            self->_on_focus_gained_f(self->_instance, x, y, self->_on_focus_gained_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasFocusLostSignal<Owner_t>::connect_signal_focus_lost(Function_t function, T data)
    {
        auto temp =  std::function<on_focus_lost_function_t<T>>(function);
        _on_focus_lost_f = std::function<on_focus_lost_function_t<void*>>(*((std::function<on_focus_lost_function_t<void*>>*) &temp));
        _on_focus_lost_data = data;

        _instance->connect_signal("leave", on_focus_lost_wrapper, this);
    }

    template<typename Owner_t>
    void HasFocusLostSignal<Owner_t>::on_focus_lost_wrapper(void*, double x, double y, HasFocusLostSignal<Owner_t>* self)
    {
        if (self->_on_focus_lost_f != nullptr and not self->_blocked)
            self->_on_focus_lost_f(self->_instance, x, y, self->_on_focus_lost_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasChildActivatedSignal<Owner_t>::connect_signal_child_activated(Function_t function, T data)
    {
        auto temp =  std::function<on_child_activated_function_t<T>>(function);
        _on_child_activated_f = std::function<on_child_activated_function_t<void*>>(*((std::function<on_child_activated_function_t<void*>>*) &temp));
        _on_child_activated_data = data;

        _instance->connect_signal("child-activated", on_child_activated_wrapper, this);
    }

    template<typename Owner_t>
    void HasChildActivatedSignal<Owner_t>::on_child_activated_wrapper(GtkFlowBox*, GtkFlowBoxChild* child, HasChildActivatedSignal<Owner_t>* self)
    {
        if (self->_on_child_activated_f != nullptr and not self->_blocked)
            self->_on_child_activated_f(self->_instance, gtk_flow_box_child_get_index(child), self->_on_child_activated_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasDragBeginSignal<Owner_t>::connect_signal_drag_begin(Function_t function, T data)
    {
        auto temp =  std::function<on_drag_begin_function_t<T>>(function);
        _on_drag_begin_f = std::function<on_drag_begin_function_t<void*>>(*((std::function<on_drag_begin_function_t<void*>>*) &temp));
        _on_drag_begin_data = data;

        _instance->connect_signal("drag-begin", on_drag_begin_wrapper, this);
    }

    template<typename Owner_t>
    void HasDragBeginSignal<Owner_t>::on_drag_begin_wrapper(void*, double x, double y, HasDragBeginSignal<Owner_t>* self)
    {
        if (self->_on_drag_begin_f != nullptr and not self->_blocked)
            self->_on_drag_begin_f(self->_instance, x, y, self->_on_drag_begin_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasDragEndSignal<Owner_t>::connect_signal_drag_end(Function_t function, T data)
    {
        auto temp =  std::function<on_drag_end_function_t<T>>(function);
        _on_drag_end_f = std::function<on_drag_end_function_t<void*>>(*((std::function<on_drag_end_function_t<void*>>*) &temp));
        _on_drag_end_data = data;

        _instance->connect_signal("drag-begin", on_drag_end_wrapper, this);
    }

    template<typename Owner_t>
    void HasDragEndSignal<Owner_t>::on_drag_end_wrapper(void*, double x, double y, HasDragEndSignal<Owner_t>* self)
    {
        if (self->_on_drag_end_f != nullptr and not self->_blocked)
            self->_on_drag_end_f(self->_instance, x, y, self->_on_drag_end_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasDragUpdateSignal<Owner_t>::connect_signal_drag_update(Function_t function, T data)
    {
        auto temp =  std::function<on_drag_update_function_t<T>>(function);
        _on_drag_update_f = std::function<on_drag_update_function_t<void*>>(*((std::function<on_drag_update_function_t<void*>>*) &temp));
        _on_drag_update_data = data;

        _instance->connect_signal("drag-update", on_drag_update_wrapper, this);
    }

    template<typename Owner_t>
    void HasDragUpdateSignal<Owner_t>::on_drag_update_wrapper(void*, double x, double y, HasDragUpdateSignal<Owner_t>* self)
    {
        if (self->_on_drag_update_f != nullptr and not self->_blocked)
            self->_on_drag_update_f(self->_instance, x, y, self->_on_drag_update_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasAttachSignal<Owner_t>::connect_signal_attach(Function_t f, T data)
    {
        auto temp =  std::function<on_attach_function_t<T>>(f);
        _on_attach_f = std::function<on_attach_function_t<void*>>(*((std::function<on_attach_function_t<void*>>*) &temp));
        _on_attach_data = data;
    }

    template<typename Owner_t>
    void HasAttachSignal<Owner_t>::emit_signal_attach()
    {
        if (_on_attach_f != nullptr and not _blocked)
            _on_attach_f(_instance, _on_attach_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasDetachSignal<Owner_t>::connect_signal_detach(Function_t f, T data)
    {
        auto temp =  std::function<on_detach_function_t<T>>(f);
        _on_detach_f = std::function<on_detach_function_t<void*>>(*((std::function<on_detach_function_t<void*>>*) &temp));
        _on_detach_data = data;
    }

    template<typename Owner_t>
    void HasDetachSignal<Owner_t>::emit_signal_detach()
    {
        if (_on_detach_f != nullptr and not _blocked)
            _on_detach_f(_instance, _on_detach_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasReorderedSignal<Owner_t>::connect_signal_reordered(Function_t f, T data)
    {
        auto temp =  std::function<on_reordered_function_t<T>>(f);
        _on_reordered_f = std::function<on_reordered_function_t<void*>>(*((std::function<on_reordered_function_t<void*>>*) &temp));
        _on_reordered_data = data;
    }

    template<typename Owner_t>
    void HasReorderedSignal<Owner_t>::emit_signal_reordered(Widget* widget, size_t previous_position, size_t next_position)
    {
        if (_on_reordered_f != nullptr and not _blocked)
            _on_reordered_f(_instance, widget, previous_position, next_position, _on_reordered_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasListItemActivateSignal<Owner_t>::connect_signal_list_item_activate(Function_t f, T data)
    {
        auto temp = std::function<on_list_item_activate_function_t<T>>(f);
        _on_list_item_activate_f = std::function<on_list_item_activate_function_t<void*>>(*((std::function<on_list_item_activate_function_t<void*>>*) &temp));
        _on_list_item_activate_data = data;

        _instance->connect_signal("activate", on_list_item_activate_wrapper, this);
    }

    template<typename Owner_t>
    void HasListItemActivateSignal<Owner_t>::on_list_item_activate_wrapper(GtkListView* view, guint position,
                                                                           HasListItemActivateSignal<Owner_t>* self)
    {
        if (self->_on_list_item_activate_f != nullptr and not self->_blocked)
            self->_on_list_item_activate_f(self->_instance, position, self->_on_list_item_activate_data);
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasSelectionChangedSignal<Owner_t>::connect_signal_selection_changed(Function_t f, T data)
    {
        auto temp = std::function<on_selection_changed_function_t<T>>(f);
        _on_selection_changed_f = std::function<on_selection_changed_function_t<void*>>(*((std::function<on_selection_changed_function_t<void*>>*) &temp));
        _on_selection_changed_data = data;

        _instance->connect_signal("selection-changed", on_selection_changed_wrapper, this);
    }

    template<typename Owner_t>
    void HasSelectionChangedSignal<Owner_t>::on_selection_changed_wrapper(GtkSelectionModel* model, guint position, guint n_items, HasSelectionChangedSignal<Owner_t>* self)
    {
        if (self->_on_selection_changed_f != nullptr and not self->_blocked)
        {
            auto* selected = gtk_selection_model_get_selection(model);

            n_items = gtk_bitset_get_size(selected);
            if (n_items > 0)
                position = gtk_bitset_get_nth(selected, 0);
            else
                position = -1;

            self->_on_selection_changed_f(self->_instance, position, n_items, self->_on_selection_changed_data);
        }
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasCloseSignal<Owner_t>::connect_signal_close(Function_t function, T data)
    {
        auto temp = std::function<on_close_function_t<T>>(function);
        _on_close_f = std::function<on_close_function_t<void*>>(*((std::function<on_close_function_t<void*>>*) &temp));
        _on_close_data = data;

        _instance->connect_signal("close-request", on_close_wrapper, this);
    }

    template<typename Owner_t>
    gboolean HasCloseSignal<Owner_t>::on_close_wrapper(Widget*, HasCloseSignal<Owner_t>* self)
    {
        if (self->_on_close_f != nullptr and not self->_blocked)
            return self->_on_close_f(self->_instance, self->_on_close_data);

        return true;
    }

    // ###

    template<typename Owner_t>
    template<typename Function_t, typename T>
    void HasScaleChangedSignal<Owner_t>::connect_signal_scale_changed(Function_t function, T data)
    {
        auto temp = std::function<on_scale_changed_function_t<T>>(function);
        _on_scale_changed_f = std::function<on_scale_changed_function_t<void*>>(*((std::function<on_scale_changed_function_t<void*>>*) &temp));
        _on_scale_changed_data = data;

        _instance->connect_signal("scale-changed", on_scale_changed_wrapper, this);
    }

    template<typename Owner_t>
    void HasScaleChangedSignal<Owner_t>::on_scale_changed_wrapper(Widget*, gdouble value, HasScaleChangedSignal<Owner_t>* self)
    {
        if (self->_on_scale_changed_f != nullptr and not self->_blocked)
            self->_on_scale_changed_f(self->_instance, value, self->_on_scale_changed_data);
    }
}
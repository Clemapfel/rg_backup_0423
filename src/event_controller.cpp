// 
// Copyright 2022 Clemens Cords
// Created on 9/20/22 by clem (mail@clemens-cords.com)
//

#include <include/event_controller.hpp>
#include <include/vector.hpp>

namespace mousetrap
{
    EventController::EventController(GtkEventController* controller)
    {
        _native = g_object_ref(controller);
    }

    EventController::~EventController()
    {
        g_object_unref(_native);
    }

    EventController::operator GObject*()
    {
        return G_OBJECT(_native);
    }

    EventController::operator GtkEventController*()
    {
        return _native;
    }

    void EventController::set_propagation_phase(GtkPropagationPhase phase)
    {
        gtk_event_controller_set_propagation_phase(_native, phase);
    }

    void EventController::set_propagation_limit(GtkPropagationLimit limit)
    {
        gtk_event_controller_set_propagation_limit(_native, limit);
    }

    GdkEvent* EventController::get_current_event()
    {
        return gtk_event_controller_get_current_event(_native);
    }

    // KEY

    KeyEventController::KeyEventController()
            : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_key_new())),
              HasKeyPressedSignal<KeyEventController>(this),
              HasKeyReleasedSignal<KeyEventController>(this),
              HasModifiersChangedSignal<KeyEventController>(this)
    {}

    MotionEventController::MotionEventController()
            : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_motion_new())),
              HasMotionEnterSignal<MotionEventController>(this),
              HasMotionLeaveSignal<MotionEventController>(this),
              HasMotionSignal<MotionEventController>(this)
    {}

    ClickEventController::ClickEventController()
            : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_click_new())),
              HasClickPressedSignal<ClickEventController>(this),
              HasClickReleasedSignal<ClickEventController>(this)
    {}

    ScrollEventController::ScrollEventController(bool emit_vertical, bool emit_horizontal)
            : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_scroll_new([&]() {

        if (emit_vertical and emit_horizontal)
            return GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES;
        else if (emit_vertical)
            return GTK_EVENT_CONTROLLER_SCROLL_VERTICAL;
        else if (emit_horizontal)
            return GTK_EVENT_CONTROLLER_SCROLL_HORIZONTAL;
        else
            return GTK_EVENT_CONTROLLER_SCROLL_NONE;
    }()))), HasScrollBeginSignal<ScrollEventController>(this),
              HasScrollEndSignal<ScrollEventController>(this),
              HasScrollSignal<ScrollEventController>(this)
    {}

    FocusEventController::FocusEventController()
            : EventController(GTK_EVENT_CONTROLLER(gtk_event_controller_focus_new())),
              HasFocusGainedSignal<FocusEventController>(this),
              HasFocusLostSignal<FocusEventController>(this)
    {}

    DragEventController::DragEventController()
            : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_drag_new())),
              HasDragBeginSignal<DragEventController>(this),
              HasDragEndSignal<DragEventController>(this),
              HasDragUpdateSignal<DragEventController>(this)
    {}

    Vector2f DragEventController::get_start_position()
    {
        double x, y;
        gtk_gesture_drag_get_start_point(GTK_GESTURE_DRAG(_native), &x, &y);
        return Vector2f(x, y);
    }

    Vector2f DragEventController::get_current_offset()
    {
        double x, y;
        gtk_gesture_drag_get_offset(GTK_GESTURE_DRAG(_native), &x, &y);
        return Vector2f(x, y);
    }

    ZoomEventController::ZoomEventController()
        : EventController(GTK_EVENT_CONTROLLER(gtk_gesture_zoom_new())),
            HasScaleChangedSignal<ZoomEventController>(this)
    {}

    float ZoomEventController::get_scale_delta()
    {
        return gtk_gesture_zoom_get_scale_delta(GTK_GESTURE_ZOOM(_native));
    }
}
// 
// Copyright 2022 Clemens Cords
// Created on 8/1/22 by clem (mail@clemens-cords.com)
//

namespace mousetrap
{
    GLArea::GLArea()
        : WidgetImplementation<GtkGLArea>(GTK_GL_AREA(gtk_gl_area_new()))
    {
        gtk_gl_area_set_auto_render(get_native(), TRUE);
        gtk_widget_set_size_request(GTK_WIDGET(get_native()), 1, 1);

        connect_signal("render", on_render_wrapper, this);
        connect_signal("resize", on_resize_wrapper, this);
    }

    void GLArea::add_render_task(Shape* shape, Shader* shader, GLTransform* transform)
    {
        if (shape == nullptr)
            return;

        _render_tasks.emplace_back(shape, shader, transform);
    }

    void GLArea::add_render_task(RenderTask task)
    {
        _render_tasks.push_back(task);
    }

    void GLArea::clear_render_tasks()
    {
        _render_tasks.clear();
    }

    gboolean GLArea::on_render_wrapper(void* area, void* context, void* instance)
    {
        return ((GLArea*) instance)->on_render(GTK_GL_AREA(area), GDK_GL_CONTEXT(context));
    }

    void GLArea::on_resize_wrapper(GtkGLArea* area, gint width, gint height, void* instance)
    {
        ((GLArea*) instance)->on_resize(area, width, height);
    }

    void GLArea::on_resize(GtkGLArea* area, gint width, gint height)
    {
        gtk_gl_area_queue_render(area);
    }

    gboolean GLArea::on_render(GtkGLArea* area, GdkGLContext* context)
    {
        gtk_gl_area_make_current(area);

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto& task : _render_tasks)
            task.render();

        glFlush();
        return FALSE;
    }

    void GLArea::queue_render()
    {
        gtk_gl_area_queue_render(get_native());
        gtk_widget_queue_draw(GTK_WIDGET(get_native()));
    }

    void GLArea::make_current()
    {
        gtk_gl_area_make_current(get_native());
    }
}
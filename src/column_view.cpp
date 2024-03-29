// 
// Copyright 2022 Clemens Cords
// Created on 9/2/22 by clem (mail@clemens-cords.com)
//

#include <include/column_view.hpp>
#include <iostream>

namespace mousetrap::detail
{
    #define G_TYPE_TREE_COLUMN_VIEW_ITEM (tree_column_view_item_get_type())

    G_DECLARE_FINAL_TYPE (TreeColumnViewItem, tree_column_view_item, G, TREE_COLUMN_VIEW_ITEM, GObject)

    struct _TreeColumnViewItem
    {
        GObject parent_instance;

        GtkTreeExpander* expander;

        std::vector<Widget*>* widgets;
        std::vector<GtkWidget*>* widget_refs;
        GListStore* children;
    };

    struct _TreeColumnViewItemClass
    {
        GObjectClass parent_class;
    };

    G_DEFINE_TYPE (TreeColumnViewItem, tree_column_view_item, G_TYPE_OBJECT)

    static void tree_column_view_item_finalize (GObject *object)
    {
        auto* self = G_TREE_COLUMN_VIEW_ITEM(object);
        g_object_unref(self->expander);
        g_object_unref(self->children);

        for (auto* w : *(self->widget_refs))
            g_object_unref(w);

        delete self->widgets;
        delete self->widget_refs;

        G_OBJECT_CLASS (tree_column_view_item_parent_class)->finalize (object);
    }

    static void tree_column_view_item_init(TreeColumnViewItem* item)
    {
        item->expander = g_object_ref(GTK_TREE_EXPANDER(gtk_tree_expander_new()));
        item->widgets = new std::vector<Widget*>();
        item->widget_refs = new std::vector<GtkWidget*>();
        item->children = g_object_ref(g_list_store_new(G_TYPE_OBJECT));
    }

    static void tree_column_view_item_class_init(TreeColumnViewItemClass* klass)
    {
        GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
        gobject_class->finalize = tree_column_view_item_finalize;
    }

    static TreeColumnViewItem* tree_column_view_item_new(const std::vector<Widget*>& in)
    {
        auto* item = (TreeColumnViewItem*) g_object_new(G_TYPE_TREE_COLUMN_VIEW_ITEM, nullptr);
        tree_column_view_item_init(item);

        for (auto* w : in)
        {
            item->widgets->push_back(w);
            item->widget_refs->push_back(g_object_ref(w->operator GtkWidget*()));
        }

        return item;
    }
}

namespace mousetrap
{
    TreeColumnView::TreeColumnView(std::vector<std::string> titles, GtkSelectionMode mode)
        : WidgetImplementation<GtkColumnView>([&]() -> GtkColumnView* {
            _root = g_list_store_new(G_TYPE_OBJECT);
            _tree_list_model = gtk_tree_list_model_new(G_LIST_MODEL(_root), false, false, on_tree_list_model_create, nullptr, on_tree_list_model_destroy);

            if (mode == GTK_SELECTION_MULTIPLE)
                _selection_model = GTK_SELECTION_MODEL(gtk_multi_selection_new(G_LIST_MODEL(_tree_list_model)));
            else if (mode == GTK_SELECTION_SINGLE or mode == GTK_SELECTION_BROWSE)
                _selection_model = GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(_tree_list_model)));
            else if (mode == GTK_SELECTION_NONE)
                _selection_model = GTK_SELECTION_MODEL(gtk_no_selection_new(G_LIST_MODEL(_tree_list_model)));

            _column_view = GTK_COLUMN_VIEW(gtk_column_view_new(_selection_model));
            gtk_column_view_set_reorderable(_column_view, false);

            for (size_t i = 0; i < titles.size(); ++i)
            {
                _factories.push_back(GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new()));
                g_signal_connect(_factories.back(), "bind", G_CALLBACK(on_list_item_factory_bind), new size_t(i));
                g_signal_connect(_factories.back(), "unbind", G_CALLBACK(on_list_item_factory_unbind), new size_t(i));
                g_signal_connect(_factories.back(), "setup", G_CALLBACK(on_list_item_factory_setup), new size_t(i));
                g_signal_connect(_factories.back(), "teardown", G_CALLBACK(on_list_item_factory_teardown), new size_t(i));

                auto* column = gtk_column_view_column_new(titles.at(i).c_str(), GTK_LIST_ITEM_FACTORY(_factories.back()));
                gtk_column_view_column_set_expand(column, true);
                gtk_column_view_append_column(_column_view, column);
            }

            return _column_view;
        }())
    {
        for (auto* factory : _factories)
            Widget::add_reference(factory);

        Widget::add_reference(_root);
        Widget::add_reference(_tree_list_model);
        Widget::add_reference(_selection_model);
    }

    GListModel* TreeColumnView::on_tree_list_model_create(void* item, void* user_data)
    {
        auto* column_view_item = (detail::TreeColumnViewItem*) item;

        auto* out = g_list_store_new(G_TYPE_OBJECT);

        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(column_view_item->children)); ++i)
            g_list_store_append(out, g_list_model_get_item(G_LIST_MODEL(column_view_item->children), i));

        return G_LIST_MODEL(out);
    }

    void TreeColumnView::on_tree_list_model_destroy(void* item)
    {}

    void TreeColumnView::on_list_item_factory_bind(GtkSignalListItemFactory* self, void* object, void* col_data)
    {
        size_t col_i = *((size_t*) col_data);
        auto* list_item = GTK_LIST_ITEM(object);
        auto* tree_list_row = GTK_TREE_LIST_ROW(gtk_list_item_get_item(list_item));
        detail::TreeColumnViewItem* column_view_item = (detail::TreeColumnViewItem*) gtk_tree_list_row_get_item(tree_list_row);

        if (g_list_model_get_n_items(G_LIST_MODEL(column_view_item->children)) != 0 and col_i == 0) // non-leaf
        {
            gtk_tree_expander_set_child(column_view_item->expander, column_view_item->widgets->at(col_i)->operator GtkWidget *());
            gtk_tree_expander_set_list_row(column_view_item->expander, tree_list_row);
            gtk_list_item_set_child(list_item, GTK_WIDGET(column_view_item->expander));
        }
        else // leaf
            gtk_list_item_set_child(list_item, GTK_WIDGET(column_view_item->widgets->at(col_i)->operator GtkWidget*()));
    }

    void TreeColumnView::on_list_item_factory_unbind(GtkSignalListItemFactory* self, void* object, void*)
    {}

    void TreeColumnView::on_list_item_factory_setup(GtkSignalListItemFactory* self, void* object, void*)
    {}

    void TreeColumnView::on_list_item_factory_teardown(GtkSignalListItemFactory* self, void* object, void*)
    {}

    TreeColumnView::Iterator TreeColumnView::append_row(std::vector<Widget*> widgets, Iterator it)
    {
        GListModel* to_append_to;
        if (it == nullptr)
            to_append_to = G_LIST_MODEL(_root);
        else
            to_append_to = G_LIST_MODEL(it->children);

        auto* item = detail::tree_column_view_item_new(widgets);
        g_list_store_append(G_LIST_STORE(to_append_to), item);
        g_object_unref(item);

        return detail::G_TREE_COLUMN_VIEW_ITEM(g_list_model_get_item(to_append_to, g_list_model_get_n_items(to_append_to) - 1));
    }

    TreeColumnView::Iterator TreeColumnView::prepend_row(std::vector<Widget*> widgets, Iterator it)
    {
        return insert_row(0, widgets, it);
    }

    TreeColumnView::Iterator TreeColumnView::insert_row(size_t i, std::vector<Widget*> widgets, Iterator it)
    {
        GListModel* list;
        if (it == nullptr)
            list = G_LIST_MODEL(_root);
        else
            list = G_LIST_MODEL(it->children);

        auto* item = detail::tree_column_view_item_new(widgets);
        g_list_store_insert(G_LIST_STORE(list), 0, item);
        g_object_unref(item);

        return detail::G_TREE_COLUMN_VIEW_ITEM(g_list_model_get_item(list, g_list_model_get_n_items(list) - 1));
    }

    TreeColumnView::Iterator TreeColumnView::move_row_to(size_t old_position, size_t new_position, Iterator old_iterator, Iterator new_iterator)
    {
        std::vector<Widget*> widgets = get_widgets_in_row(old_position, old_iterator);
        remove_row(old_position, old_iterator);
        return insert_row(new_position, widgets, new_iterator);
    }

    void TreeColumnView::remove_row(size_t i, Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _root;
        else
            list = it->children;

        g_list_store_remove(list, i);
    }

    Widget* TreeColumnView::get_widget_at(size_t row_i, size_t col_i, Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _root;
        else
            list = it->children;

        auto* row = (detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(list), row_i);
        return row->widgets->at(col_i);
    }

    void TreeColumnView::set_widget_at(size_t row_i, size_t col_i, Widget* widget, Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _root;
        else
            list = it->children;

        auto* row = (detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(list), row_i);
        row->widgets->at(col_i) = widget;
        g_object_unref(row->widget_refs->at(col_i));
        row->widget_refs->at(col_i) = g_object_ref(widget->operator GtkWidget*());
    }

    std::vector<Widget*> TreeColumnView::get_widgets_in_row(size_t row_i, Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _root;
        else
            list = it->children;

        auto* row = (detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(list), row_i);

        std::vector<Widget*> out;
        for (auto* w : *(row->widgets))
            out.push_back(w);

        return out;
    }

    void TreeColumnView::set_widgets_in_row(size_t row_i, std::vector<Widget*> widgets, Iterator it)
    {
        GListStore* list;
        if (it == nullptr)
            list = _root;
        else
            list = it->children;

        auto* row = (detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(list), row_i);

        for (auto* w : *(row->widget_refs))
            g_object_unref(w);

        row->widgets->clear();
        row->widget_refs->clear();

        for (auto* w : widgets)
        {
            row->widgets->push_back(w);
            row->widget_refs->push_back(g_object_ref(w->operator GtkWidget *()));
        }
    }

    GtkColumnViewColumn* TreeColumnView::get_column(size_t i)
    {
        return (GtkColumnViewColumn*) g_list_model_get_item(gtk_column_view_get_columns(_column_view), i);
    }

    std::vector<Widget*> TreeColumnView::get_widgets_in_column(size_t col_i)
    {
        static std::function<void(detail::TreeColumnViewItem*, std::vector<Widget*>&)> add = [&](
                detail::TreeColumnViewItem* item,
                std::vector<Widget*>& widgets
        )
        {
            widgets.push_back(item->widgets->at(col_i));
            auto* model = G_LIST_MODEL(item->children);
            for (size_t i = 0; i < g_list_model_get_n_items(model); ++i)
                add((detail::TreeColumnViewItem*) g_list_model_get_item(model, i), widgets);
        };

        std::vector<Widget*> widgets;
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_root)); ++i)
            add((detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(_root), i), widgets);

        return widgets;
    }

    void TreeColumnView::set_widgets_in_column(size_t col_i, std::vector<Widget*> widgets)
    {
        static std::function<void(detail::TreeColumnViewItem*, int& index)> set = [&](
                detail::TreeColumnViewItem* item,
                int& index
        )
        {
            index += 1;

            auto* widget = widgets.at(index);
            item->widgets->at(col_i) = widget;
            g_object_unref(item->widget_refs->at(col_i));
            item->widget_refs->at(col_i) = g_object_ref(widget->operator GtkWidget*());

            auto* model = G_LIST_MODEL(item->children);
            for (size_t i = 0; i < g_list_model_get_n_items(model); ++i)
                set((detail::TreeColumnViewItem*) g_list_model_get_item(model, i), index);
        };

        int index = -1;
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_root)); ++i)
            set((detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(_root), i), index);
    }

    size_t TreeColumnView::get_n_rows_total()
    {
        static std::function<void(detail::TreeColumnViewItem*, size_t&)> sum = [](
            detail::TreeColumnViewItem* item,
            size_t& current_sum
        )
        {
            current_sum += 1;
            auto* model = G_LIST_MODEL(item->children);

            for (size_t i = 0; i < g_list_model_get_n_items(model); ++i)
                sum((detail::TreeColumnViewItem*) g_list_model_get_item(model, i), current_sum);
        };

        size_t n = 0;
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_root)); ++i)
            sum((detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(_root), i), n);

        return n;
    }

    void TreeColumnView::set_column_expand(size_t i, bool b)
    {
        gtk_column_view_column_set_expand(get_column(i), b);
    }

    void TreeColumnView::set_column_fixed_width(size_t i, int value)
    {
        gtk_column_view_column_set_fixed_width(get_column(i), value);
    }

    void TreeColumnView::set_column_header_menu(size_t i, MenuModel* model)
    {
        gtk_column_view_column_set_header_menu(get_column(i), model->operator GMenuModel*());
    }

    void TreeColumnView::set_column_visible(size_t i, bool b)
    {
        gtk_column_view_column_set_visible(get_column(i), b);
    }

    void TreeColumnView::set_column_resizable(size_t i, bool b)
    {
        if (b)
            std::cerr << "[WARNING] In ColumnView::set_column_resizable: resizing right-mose column to it's smallest size may lead to freeze. This feature is not safe to use." << std::endl;

        gtk_column_view_column_set_resizable(get_column(i), b);
    }

    size_t TreeColumnView::get_n_columns()
    {
        return g_list_model_get_n_items(gtk_column_view_get_columns(_column_view));
    }

    size_t TreeColumnView::get_n_rows(Iterator it)
    {
        if (it == nullptr)
            return g_list_model_get_n_items(G_LIST_MODEL(_root));
        else
            return g_list_model_get_n_items(G_LIST_MODEL(it->children));
    }

    void TreeColumnView::append_column(const std::string& title, std::vector<Widget*> widgets)
    {
        static std::function<void(detail::TreeColumnViewItem*, int& index)> append = [&](
                detail::TreeColumnViewItem* item,
                int& index)
        {
            index += 1;

            auto* widget = widgets.at(index);
            item->widgets->push_back(widget);
            item->widget_refs->push_back(g_object_ref(widget->operator GtkWidget*()));

            auto* model = G_LIST_MODEL(item->children);
            for (size_t i = 0; i < g_list_model_get_n_items(model); ++i)
                append((detail::TreeColumnViewItem*) g_list_model_get_item(model, i), index);
        };

        int index = -1;
        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_root)); ++i)
            append((detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(_root), i), index);

        index -= 1;
        _factories.push_back(GTK_SIGNAL_LIST_ITEM_FACTORY(gtk_signal_list_item_factory_new()));
        g_signal_connect(_factories.back(), "bind", G_CALLBACK(on_list_item_factory_bind), new size_t(index));
        g_signal_connect(_factories.back(), "unbind", G_CALLBACK(on_list_item_factory_unbind), new size_t(index));
        g_signal_connect(_factories.back(), "setup", G_CALLBACK(on_list_item_factory_setup), new size_t(index));
        g_signal_connect(_factories.back(), "teardown", G_CALLBACK(on_list_item_factory_teardown), new size_t(index));

        gtk_column_view_append_column(_column_view, gtk_column_view_column_new(title.c_str(), GTK_LIST_ITEM_FACTORY(_factories.back())));
    }

    void TreeColumnView::remove_column(size_t column_i)
    {
        static std::function<void(detail::TreeColumnViewItem*)> remove = [&](
                detail::TreeColumnViewItem* item)
        {
            item->widgets->erase(item->widgets->begin() + column_i);
            g_object_unref( item->widget_refs->at(column_i));
            item->widget_refs->erase(item->widget_refs->begin() + column_i);

            auto* model = G_LIST_MODEL(item->children);
            for (size_t i = 0; i < g_list_model_get_n_items(model); ++i)
                remove((detail::TreeColumnViewItem*) g_list_model_get_item(model, i));
        };

        for (size_t i = 0; i < g_list_model_get_n_items(G_LIST_MODEL(_root)); ++i)
            remove((detail::TreeColumnViewItem*) g_list_model_get_item(G_LIST_MODEL(_root), i));

        _factories.erase(_factories.begin() + column_i);

        for (size_t i = 0; i < _factories.size(); ++i)
        {
            g_signal_connect(_factories.at(i), "bind", G_CALLBACK(on_list_item_factory_bind), new size_t(i));
            g_signal_connect(_factories.at(i), "unbind", G_CALLBACK(on_list_item_factory_unbind), new size_t(i));
            g_signal_connect(_factories.at(i), "setup", G_CALLBACK(on_list_item_factory_setup), new size_t(i));
            g_signal_connect(_factories.at(i), "teardown", G_CALLBACK(on_list_item_factory_teardown), new size_t(i));
        }

        gtk_column_view_remove_column(_column_view, get_column(column_i));
    }

    void TreeColumnView::set_show_column_separators(bool b)
    {
        gtk_column_view_set_show_column_separators(_column_view, b);
    }

    void TreeColumnView::set_show_row_separators(bool b)
    {
        gtk_column_view_set_show_row_separators(_column_view, b);
    }

    void TreeColumnView::set_enable_rubberband_selection(bool b)
    {
        gtk_column_view_set_enable_rubberband(_column_view, b);
    }
}
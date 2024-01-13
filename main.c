#include<gtk/gtk.h>
#include<gdk/gdkevents.h>
#include<gtk-4.0/gdk/gdkevents.h>
#include <stdlib.h>
#include<unistd.h>

GtkWidget *window;
GtkEntryBuffer *buffer;
GtkWidget *entry;

uint32_t index_current = -1;
uint32_t k_current = -1;

#if 0
uint64_t array[200] = {
    15, 18, 5, 20, 7, 66, 30, 4, 12, 88, 6, 1, 2, 17, 13, 22, 77, 46, 58, 61
};
#endif

uint64_t *array;

#define ARRAY_ALLOC_SIZE 400

#define SORT_SPEED 500

uint32_t array_count = 0;

void array_append(uint64_t value)
{
    if (array_count % ARRAY_ALLOC_SIZE == 0)
        array = realloc(array, (array_count + ARRAY_ALLOC_SIZE) * sizeof(*array));
    array[array_count++] = value;
}

uint64_t random_number_between(int64_t low, int64_t high)
{
    return rand() % (high - low + 1) + low;
}

void array_print(void)
{
    uint32_t i;
    for (i = 0; i < array_count; i++)
    {
        printf("ARRAY[%d]: %d\n", i, array[i]);
    }
}

uint64_t array_higehst_value_get(void)
{
    uint64_t highest_value = 0;
    uint32_t i;
    for (i = 0; i < array_count; i++)
    {
        if (array[i] > highest_value)
            highest_value = array[i];
    }

    return highest_value * 1.3;
}

void stuf_draw_func(GtkDrawingArea *drawing_area,
                    cairo_t *cr,
                    int width,
                    int height,
                    gpointer user_data)
{
    cairo_set_source_rgba(cr, 0, 0, 0, 1);

    cairo_rectangle(cr, 5, 5, width - 10, height - 10);
    cairo_set_line_width(cr, 2.0);
    cairo_stroke(cr);

    /* drawing array */
    width -= 10;
    height -= 5;

    double array_item_width = (double)width / array_count;

    uint64_t highest_value = array_higehst_value_get();


    cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, array_item_width - 2);
    cairo_set_source_rgb(cr, 0, 0, 0);

    char text[200];

    uint32_t i;
    for (i = 0; i < array_count; i++)
    {
        if (i == index_current)
            cairo_set_source_rgba(cr, 0, 0, 255, 1);
        else if (i == k_current - 1)
            cairo_set_source_rgba(cr, 150, 0, 0, 1);
        else
            cairo_set_source_rgba(cr, 0, 0, 0, 1);

        cairo_rectangle(cr, 5 + i * array_item_width, height - ((height - 5) * (double)(array[i] / (double)highest_value)), array_item_width, ((height - 5) * (double)(array[i] / (double)highest_value)));

        sprintf(text, "%ld", array[i]);
        cairo_save(cr);
        cairo_move_to(cr, 5 + i * array_item_width + array_item_width - 2, height - ((height - 5) * (double)(array[i] / (double)highest_value)) - 5);
        cairo_rotate(cr, -M_PI / 2.0);
        cairo_show_text(cr, text);

        cairo_restore(cr);
        /*
        cairo_fill(cr);
        */
        cairo_stroke(cr);
    }
}

void *insertion_sort(void *data)
{
    uint32_t tmp;

    for (index_current = 1; index_current < array_count; index_current++)
    {

        for (k_current = index_current; k_current > 0 && array[k_current - 1] > array[k_current]; k_current--)
        {
            tmp = array[k_current];
            array[k_current] = array[k_current - 1];
            array[k_current - 1] = tmp;


            gtk_widget_queue_draw((GtkWidget *)data);
            g_usleep(SORT_SPEED);
        }
    }
    k_current = -1;
    index_current = -1;
    gtk_widget_queue_draw((GtkWidget *)data);

    return NULL;
}


#include<pthread.h>
void button_click_func(GtkWidget *widget, gpointer data)
{
    GThread *thread = g_thread_new("thread", insertion_sort, data);
    /*
    insertion_sort(data);
    */
    g_thread_unref(thread);
}

void button_click_insert_func(GtkWidget *widget, gpointer data)
{
    const char *text = gtk_entry_buffer_get_text(buffer);
    uint64_t value = 0;
    uint32_t i;
    for (i = 0; text[i] != 0; i++)
    {
        value *= 10;
        value += text[i] - 0x30;
    }

    array_append(value);
    gtk_entry_buffer_set_text(buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);

    /*
    insertion_sort(data, array, array_count);
    */
}

void button_click_delete_func(GtkWidget *widget, gpointer data)
{
    uint32_t i;
    if ((int64_t)index_current == 0xffffffff)
    {
        const char *text = gtk_entry_buffer_get_text(buffer);
        uint64_t value = 0;
        for (i = 0; text[i] != 0; i++)
        {
            value *= 10;
            value += text[i] - 0x30;
        }

        for (i = 0; i < array_count; i++)
        {
            if (value == array[i])
                break;
        }
        if (i >= array_count)
            return;
    }
    else
        i = index_current;

    for (; i < array_count - 1; i++)
    {
        array[i] = array[i + 1];
    }
    index_current = -1;

    array_count--;

    gtk_entry_buffer_set_text(buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);
}

void button_click_search_func(GtkWidget *widget, gpointer data)
{
    const char *text = gtk_entry_buffer_get_text(buffer);
    uint64_t value = 0;
    uint32_t i;
    for (i = 0; text[i] != 0; i++)
    {
        value *= 10;
        value += text[i] - 0x30;
    }

    for (i = 0; i < array_count; i++)
    {
        if (value == array[i])
        {
            index_current = i;
        }
    }

    gtk_entry_buffer_set_text(buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);
}

int main(int argc, char **argv)
{
    gtk_init();

    window = gtk_window_new();
    gtk_window_set_default_size((GtkWindow *)window, 800, 500);



    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func((GtkDrawingArea *)drawing_area,
                                   stuf_draw_func,
                                   NULL,
                                   NULL);
    gtk_widget_set_hexpand(drawing_area, TRUE);
    gtk_widget_set_vexpand(drawing_area, TRUE);

    GtkWidget *button_sort = gtk_button_new_with_label("SORT");
    g_signal_connect(button_sort, "clicked", G_CALLBACK(button_click_func), drawing_area);

    GtkWidget *button_insert = gtk_button_new_with_label("INSERT");
    g_signal_connect(button_insert, "clicked", G_CALLBACK(button_click_insert_func), drawing_area);

    GtkWidget *button_delete = gtk_button_new_with_label("DELETE");
    g_signal_connect(button_delete, "clicked", G_CALLBACK(button_click_delete_func), drawing_area);

    GtkWidget *button_search = gtk_button_new_with_label("SEARCH");
    g_signal_connect(button_search, "clicked", G_CALLBACK(button_click_search_func), drawing_area);

    buffer = gtk_entry_buffer_new("", 0);
    entry = gtk_entry_new_with_buffer(buffer);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(hbox), button_sort);
    gtk_box_append(GTK_BOX(hbox), button_insert);
    gtk_box_append(GTK_BOX(hbox), button_delete);
    gtk_box_append(GTK_BOX(hbox), button_search);
    gtk_box_append(GTK_BOX(hbox), entry);
    /*
    gtk_box_append(GTK_BOX(hbox), scale);
    */

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(vbox), drawing_area);
    gtk_box_append(GTK_BOX(vbox), hbox);

    gtk_window_set_child((GtkWindow *)window, vbox);

    gtk_window_present(GTK_WINDOW(window));

    int32_t test = 150;
    while (--test)
    {
        array_append(random_number_between(1, 800));
    }

    GMainLoop *main_loop = g_main_loop_new(NULL, TRUE);
    g_main_loop_run(main_loop);
    g_main_loop_unref(main_loop);

    return 0;
}

#include<gtk/gtk.h>

#define APPLICATION_NAME "INSERTION SORT VISUALIZER"

/* gtk globals */
GtkWidget *window;
GtkWidget *entry;
GtkEntryBuffer *entry_buffer;

/* globals */
uint32_t *array;
uint16_t array_count = 0;
#define ARRAY_INDEX_EMPTY 0xffff
uint16_t array_index_current = ARRAY_INDEX_EMPTY;
uint16_t array_kndex_current = ARRAY_INDEX_EMPTY;

#define ARRAY_ALLOC_SIZE 200
#define INSERTION_SORT_ANIMATION_SPEED 10000 /* microseconds sleep between each step */

void array_number_append(int32_t value)
{
    if (array_count % ARRAY_ALLOC_SIZE == 0)
    {
        array = realloc(array,
                        (array_count + ARRAY_ALLOC_SIZE) * sizeof(*array));
        if (array == NULL)
        {
            g_printerr("ERROR F:%s L%d: failed to alloc array\n",
                       __FILE__, __LINE__);
            exit(EXIT_FAILURE);
        }
    }

    array[array_count] = value;
    array_count++;
}

int32_t array_value_max_get(void)
{
    uint32_t value_max = 0;

    uint16_t i;
    for (i = 0; i < array_count; i++)
        if (array[i] > value_max)
            value_max = array[i];

    return value_max; 
}

uint32_t buffer_str_to_number(const char *buffer)
{
    int64_t value = 0;
    uint32_t i;
    for (i = 0; buffer[i] != 0; i++)
    {
        value *= 10;
        value += buffer[i] - 0x30;
    }

    return value;
}

int32_t random_number_between(int32_t min, int32_t max)
{
    return rand() % (max - min + 1) + min;
}

void array_fill_with_random_numbers(uint16_t amount, uint32_t min, uint32_t max)
{
    while (amount--)
        array_number_append(random_number_between(min, max));
}

void insertion_sort_area_draw_func(GtkDrawingArea *drawing_area,
                                   cairo_t *cr,
                                   int width,
                                   int height,
                                   gpointer data)
{
    /* draw the border of drawing area */
    cairo_set_source_rgba(cr, 0, 0, 0, 1);
    cairo_set_line_width(cr, 2.0);
    cairo_rectangle(cr, 5, 5, width - 10, height - 10);
    cairo_stroke(cr);

    /* subtract the border margin */
    width -= 10;
    height -= 5;

    /* array drawing */
    double array_rectangle_width = (double)width / array_count;
    uint32_t value_max = array_value_max_get() * 1.25; /* for vertical scaling  - 1.25 for padding on the top */

    /* setting up font for cairo */
    cairo_select_font_face(cr,
                           "Sans", 
                           CAIRO_FONT_SLANT_NORMAL,
                           CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(cr, array_rectangle_width - 2);
    cairo_set_source_rgb(cr, 0, 0, 0);

    char array_value_str[11];

    uint16_t i;
    for (i = 0; i < array_count; i++)
    {
        if (i == array_index_current)
            cairo_set_source_rgba(cr, 0, 0, 255, 1);
        else if (i == array_kndex_current - 1)
            cairo_set_source_rgba(cr, 150, 0, 0, 1);
        else
            cairo_set_source_rgba(cr, 0, 0, 0, 1);


        cairo_rectangle(cr,
                        5 + i * array_rectangle_width,
                        height - (height - 5) * (double)(array[i] / (double)value_max),
                        array_rectangle_width,
                        (height - 5) * (double)(array[i] / (double)value_max));

        /* draw number text */
        sprintf(array_value_str, "%d", array[i]);
        cairo_save(cr);
        cairo_move_to(cr,
                      5 + i * array_rectangle_width + array_rectangle_width - 2,
                      height - ((height - 5) * (double)(array[i] / (double)value_max)) - 5);
        cairo_rotate(cr, -M_PI / 2.0);
        cairo_show_text(cr, array_value_str);
        cairo_restore(cr);

        cairo_stroke(cr);
    }
}

GSourceFunc drawing_area_redraw_callback(gpointer user_data)
{
    gtk_widget_queue_draw(GTK_WIDGET(user_data));
    return G_SOURCE_REMOVE;
}

void *insertion_sort(void *data)
{
    uint32_t tmp;

    for (array_index_current = 1; array_index_current < array_count; array_index_current++)
    {
        for (array_kndex_current = array_index_current;
             array_kndex_current > 0 && array[array_kndex_current - 1] > array[array_kndex_current];
             array_kndex_current--)
        {
            tmp = array[array_kndex_current];
            array[array_kndex_current] = array[array_kndex_current - 1];
            array[array_kndex_current - 1] = tmp;

            g_idle_add(G_SOURCE_FUNC(drawing_area_redraw_callback), data);
            g_usleep(INSERTION_SORT_ANIMATION_SPEED);
        }
    }

    /* reset indexses */
    array_index_current = ARRAY_INDEX_EMPTY;
    array_kndex_current = ARRAY_INDEX_EMPTY;
    gtk_widget_queue_draw(GTK_WIDGET(data));

    return NULL;
}

void insertion_sort_async(gpointer data)
{
    GThread *thread = g_thread_new("thread", insertion_sort, data);
    g_thread_unref(thread);
}

void button_click_func(GtkWidget *widget, gpointer data)
{
    insertion_sort_async(data);
}

void button_insert_func(GtkWidget *widget, gpointer data)
{
    const char *entry_buffer_str = gtk_entry_buffer_get_text(entry_buffer);
    int32_t value = buffer_str_to_number(entry_buffer_str);

    array_number_append(value);

    /* clear entry_buffer */
    gtk_entry_buffer_set_text(entry_buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);

    insertion_sort_async(data);
}

void button_delete_func(GtkWidget *widget, gpointer data)
{
    uint16_t i;
    if (array_index_current == ARRAY_INDEX_EMPTY)
    {
        const char *entry_buffer_str = gtk_entry_buffer_get_text(entry_buffer);
        int32_t value = buffer_str_to_number(entry_buffer_str);

        /* find the i to delete */
        for (i = 0; i < array_count; i++)
        {
            if (value == array[i])
                break;
        }

        if (i >= array_count)
            return; /* no array with the value found */
    }
    else
        i = array_index_current;

    /* all values behind behind the current go one step left */
    for (; i < array_count - 1; i++)
    {
        array[i] = array[i + 1];
    }
    array_index_current = -1;

    array_count--;

    /* reset indexses */
    array_index_current = ARRAY_INDEX_EMPTY;
    array_kndex_current = ARRAY_INDEX_EMPTY;

    /* clear entry_buffer */
    gtk_entry_buffer_set_text(entry_buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);
}

void button_search_func(GtkWidget *widget, gpointer data)
{
    const char *entry_buffer_str = gtk_entry_buffer_get_text(entry_buffer);
    int32_t value = buffer_str_to_number(entry_buffer_str);

    /* find value in array */
    uint16_t i;
    for (i = 0; i < array_count; i++)
        if (value == array[i])
            array_index_current = i; /* mark the index */

    /* clear entry_buffer */
    gtk_entry_buffer_set_text(entry_buffer, "", 0);

    gtk_widget_queue_draw(data);
    gtk_window_set_focus((GtkWindow *)window, entry);
}

int main(int argc, char **argv)
{
    if (gtk_init_check() == FALSE)
    {
        g_printerr("ERROR F:%s L%d: failed to init gtk\n",
                   __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }

    array_fill_with_random_numbers(80, 1, 500);

    /* window create */
    window = gtk_window_new();
    gtk_window_set_default_size((GtkWindow *)window, 800, 500);
    gtk_window_set_title((GtkWindow *)window, APPLICATION_NAME);

    gtk_window_present((GtkWindow *)window);

    /* append gui UI-elements to window */
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child((GtkWindow *)window, vbox);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_drawing_area_set_draw_func((GtkDrawingArea *)drawing_area,
                                   insertion_sort_area_draw_func,
                                   NULL,
                                   NULL);
    gtk_widget_set_hexpand(drawing_area, TRUE);
    gtk_widget_set_vexpand(drawing_area, TRUE);
    gtk_box_append((GtkBox *)vbox, drawing_area);

    GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append((GtkBox *)vbox, hbox);

    GtkWidget *button = gtk_button_new_with_label("SORT");
    gtk_widget_set_hexpand(button, TRUE);
    g_signal_connect(button, "clicked", G_CALLBACK(button_click_func), drawing_area);
    gtk_box_append((GtkBox *)hbox, button);

    button = gtk_button_new_with_label("INSERT");
    gtk_widget_set_hexpand(button, TRUE);
    g_signal_connect(button, "clicked", G_CALLBACK(button_insert_func), drawing_area);
    gtk_box_append((GtkBox *)hbox, button);

    button = gtk_button_new_with_label("DELETE");
    gtk_widget_set_hexpand(button, TRUE);
    g_signal_connect(button, "clicked", G_CALLBACK(button_delete_func), drawing_area);
    gtk_box_append((GtkBox *)hbox, button);

    button = gtk_button_new_with_label("SEARCH");
    gtk_widget_set_hexpand(button, TRUE);
    g_signal_connect(button, "clicked", G_CALLBACK(button_search_func), drawing_area);
    gtk_box_append((GtkBox *)hbox, button);

    entry_buffer = gtk_entry_buffer_new("", 0);
    entry = gtk_entry_new_with_buffer(entry_buffer);
    gtk_widget_set_hexpand(entry, TRUE);
    gtk_box_append((GtkBox *)hbox, entry);

    gtk_window_set_focus((GtkWindow *)window, entry);

    /* Main Loop */
    GMainLoop *main_loop = g_main_loop_new(NULL, TRUE);
    g_main_loop_run(main_loop);
    g_main_loop_unref(main_loop);

    return EXIT_SUCCESS;
}

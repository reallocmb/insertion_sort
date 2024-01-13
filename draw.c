#include<gtk/gtk.h>

static void
draw_function (GtkDrawingArea *area,
               cairo_t        *cr,
               int             width,
               int             height,
               gpointer        data)
{
  GdkRGBA color;

  cairo_arc (cr,
             width / 2.0, height / 2.0,
             MIN (width, height) / 2.0,
             0, 2 * G_PI);

  gtk_widget_get_color (GTK_WIDGET (area),
                        &color);
  gdk_cairo_set_source_rgba (cr, &color);

  cairo_fill (cr);
}

int
main (int argc, char **argv)
{
  gtk_init ();

  GtkWidget *area = gtk_drawing_area_new ();
  gtk_drawing_area_set_content_width (GTK_DRAWING_AREA (area), 100);
  gtk_drawing_area_set_content_height (GTK_DRAWING_AREA (area), 100);
  gtk_drawing_area_set_draw_func (GTK_DRAWING_AREA (area),
                                  draw_function,
                                  NULL, NULL);
  return 0;
}


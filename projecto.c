#include <gtk/gtk.h>
#include <stdlib.h>

char *TableSize=NULL;
int size;
int *vectElements=NULL;
GtkWidget *Get_Size_Window;
GtkWidget *Get_Elements_Window;
GtkWidget *Options_Window;
GtkWidget *Table_Window;
char *strElements = NULL;
gboolean program_end=FALSE;
gboolean GSdone=FALSE;
gboolean GEdone=FALSE;
gboolean OPdone=FALSE;
gboolean TBdone=FALSE;

void insertionSort(int arr[], int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;

        // Move elements of arr[0..i-1] that are greater than key to one position ahead of their current position
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

//malik
gboolean quit_main_loop(gpointer data) {
    GMainLoop *loop = (GMainLoop *)data;
    g_main_loop_quit(loop);
    return G_SOURCE_REMOVE;
}

//malik
void close_table(GtkButton *button, GtkEntry *entry) {
    TBdone=TRUE;
    gtk_window_destroy(GTK_WINDOW(Table_Window));
}

//malik
void Show_Table(int arr[], int n,char* title) {
    GtkWidget *table;
    
    gtk_init();

    Table_Window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(Table_Window), title);
    gtk_window_set_default_size(GTK_WINDOW(Table_Window), 325, 200);

    table = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(table),TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(table),TRUE);
    gtk_window_set_child(GTK_WINDOW(Table_Window),table);

    char* label;
    int lenght;
    for (int i = 0;i<n;i++) {
            lenght=snprintf(NULL, 0, "%d", arr[i]);
            label=(char*)malloc(sizeof(lenght+1));
            sprintf(label,"%d",arr[i]);

            GtkWidget *text = gtk_label_new(label);
            gtk_grid_attach(GTK_GRID(table), text, i, 0, 4, 2);
           
            free(label);
    }

    gtk_window_present(GTK_WINDOW(Table_Window));

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    g_signal_connect(Table_Window, "destroy", G_CALLBACK(close_table), NULL);

    while (!TBdone) {
        g_idle_add(quit_main_loop, loop);
        g_main_context_iteration(NULL,FALSE);
    }
}

//malik
void GSbutton_clicked(GtkButton *button, GtkEntry *entry) {
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);

    const char *text = gtk_entry_buffer_get_text(buffer);

    TableSize = g_strdup(text);

    GSdone=TRUE;
    gtk_window_destroy(GTK_WINDOW(Get_Size_Window));
}

//anis
void GEbutton_clicked(GtkButton *button, GtkEntry *entry) {
    GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);

    const char *text = gtk_entry_buffer_get_text(buffer);

    strElements = g_strdup(text);

    GEdone=TRUE;
    gtk_window_destroy(GTK_WINDOW(Get_Elements_Window));
}

//malik
GMainLoop *Get_Size() {

    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *button;

    gtk_init();

    Get_Size_Window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(Get_Size_Window), "Get Size");
    gtk_window_set_default_size(GTK_WINDOW(Get_Size_Window), 225, 100);

    grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid),TRUE);
    gtk_window_set_child(GTK_WINDOW(Get_Size_Window),grid);

    GtkEntryBuffer *buffer = gtk_entry_buffer_new(NULL, 0);
    entry = gtk_entry_new_with_buffer(buffer);
    gtk_grid_attach(GTK_GRID(grid),entry,0,0,3,1);

    button = gtk_button_new_with_label("Save and Close");
    gtk_grid_attach(GTK_GRID(grid),button,0,1,3,1);
    g_signal_connect(button, "clicked", G_CALLBACK(GSbutton_clicked), entry);

    gtk_window_present(GTK_WINDOW(Get_Size_Window));

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    while (!GSdone) {
        g_idle_add(quit_main_loop, loop);
        g_main_context_iteration(NULL,FALSE);
    }

    return loop;
}

//anis
GMainLoop *Get_Elements() {

    GtkWidget *grid;
    GtkWidget *entry;
    GtkWidget *button;

    gtk_init();

    Get_Elements_Window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(Get_Elements_Window), "Get Elements");
    gtk_window_set_default_size(GTK_WINDOW(Get_Elements_Window), 225, 100);

    grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_window_set_child(GTK_WINDOW(Get_Elements_Window), grid);

    GtkEntryBuffer *buffer = gtk_entry_buffer_new(NULL, 0);
    entry = gtk_entry_new_with_buffer(buffer);
    gtk_grid_attach(GTK_GRID(grid), entry, 0, 0, 3, 1);

    button = gtk_button_new_with_label("Save and Close");
    gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 3, 1);
    g_signal_connect(button, "clicked", G_CALLBACK(GEbutton_clicked), entry);

    gtk_window_present(GTK_WINDOW(Get_Elements_Window));

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    while (!GEdone) {
        g_idle_add(quit_main_loop, loop);
        g_main_context_iteration(NULL,FALSE);
    }

    return loop;
}

void sort_clicked(GtkButton *button, GtkEntry *entry) {
    gtk_window_destroy(GTK_WINDOW(Options_Window));
    
    Show_Table(vectElements, size,"table before sorting");TBdone=FALSE;
    insertionSort(vectElements, size);
    Show_Table(vectElements, size,"table after sorting");TBdone=FALSE;

    printf("Sorted array: ");
    for (int i = 0; i < size; i++) {
        printf("%d ", vectElements[i]);
    }
    printf("\n");

    OPdone=TRUE;
}

//malik
void size_clicked(GtkButton *button, GtkEntry *entry) {
    gtk_window_destroy(GTK_WINDOW(Options_Window));
    Get_Size();
    OPdone=TRUE;
}

//anis
void elements_clicked(GtkButton *button, GtkEntry *entry) {
    gtk_window_destroy(GTK_WINDOW(Options_Window));
    Get_Elements();
    OPdone=TRUE;
}

//malik
void quit_clicked(GtkButton *button, GtkEntry *entry) {
    OPdone=TRUE;
    program_end=TRUE;
    gtk_window_destroy(GTK_WINDOW(Options_Window));
}

//malik
GMainLoop *Options() {

   GtkWidget *grid;
   GtkWidget *button1;
   GtkWidget *button2;
   GtkWidget *button3;
   GtkWidget *button4;
 
   gtk_init();

   Options_Window = gtk_window_new();
   gtk_window_set_title(GTK_WINDOW(Options_Window), "Options");
   gtk_window_set_default_size(GTK_WINDOW(Options_Window), 325, 200);

   grid = gtk_grid_new();
   gtk_grid_set_column_homogeneous(GTK_GRID(grid),TRUE);
   gtk_grid_set_row_homogeneous(GTK_GRID(grid),TRUE);
   gtk_window_set_child(GTK_WINDOW(Options_Window),grid);
  
   button1 = gtk_button_new_with_label("Size");
   gtk_grid_attach(GTK_GRID(grid),button1,0,0,1,1);
   g_signal_connect(button1, "clicked", G_CALLBACK(size_clicked), NULL);

   button2 = gtk_button_new_with_label("Elements");
   gtk_grid_attach(GTK_GRID(grid),button2,0,1,1,1);
   g_signal_connect(button2, "clicked", G_CALLBACK(elements_clicked), NULL);

   button3 = gtk_button_new_with_label("Sort");
   gtk_grid_attach(GTK_GRID(grid),button3,1,0,1,1);
   g_signal_connect(button3, "clicked", G_CALLBACK(sort_clicked), NULL);

   button4 = gtk_button_new_with_label("quit");
   gtk_grid_attach(GTK_GRID(grid),button4,1,1,1,1);
   g_signal_connect(button4, "clicked", G_CALLBACK(quit_clicked), NULL);

   gtk_window_present(GTK_WINDOW(Options_Window));

   GMainLoop *loop = g_main_loop_new(NULL, FALSE);
  
   while (!OPdone) {
       g_idle_add(quit_main_loop, loop);
       g_main_context_iteration(NULL,FALSE);
   }

   return loop;
}

//anis
int* createvectFromStr(const char* str, int* size) {
    int count = 0;
    const char* ptr = str;

    while (*ptr) {
      if ((*ptr >= '0' && *ptr <= '9') || (*ptr == '-' && *(ptr + 1) >= '0' && *(ptr + 1) <= '9')) {
        count++;
        while (*ptr && ((*ptr >= '0' && *ptr <= '9') || *ptr == '-')) {
        ptr++;
      }
        } else {
            ptr++;
          }
    }

    int* vect = (int*)malloc(count * sizeof(int));
      if (vect == NULL) {
        perror("Erreur d'allocation mémoire");
        exit(EXIT_FAILURE);
      }

    ptr = str;
    int index = 0;

    while (*ptr) {
        if ((*ptr >= '0' && *ptr <= '9') || (*ptr == '-' && *(ptr + 1) >= '0' && *(ptr + 1) <= '9')) {
            int value = atoi(ptr);
            vect[index++] = value;
            while (*ptr && ((*ptr >= '0' && *ptr <= '9') || *ptr == '-')) {
                ptr++;
            }
        } else {
            ptr++;
        }
    }
   
    *size = count;

    return vect;
}

//malik
int main() {
    GMainLoop *Option;

  do{
    Option=Options();
    OPdone=FALSE;

    if(GSdone){
    if (TableSize != NULL) {
        printf("Table Size: %s\n", TableSize);
    } else {
        printf("No input value\n");
     }
    size=atoi(TableSize);
     GSdone=FALSE;
    }
    //anis
    if(GEdone){
    if (strElements != NULL) {
      int size;
      vectElements = createvectFromStr(strElements, &size);
      printf("Tableau d'entiers : ");
      for (int i = 0; i < size; ++i) {
        printf("%d ", vectElements[i]);
      }
      printf("\n");
     } else {
         printf("No input value\n");
       }
     GEdone=FALSE;
    }
   }while(!program_end);

    g_free(TableSize);
    g_free(strElements);
    return 0;
}

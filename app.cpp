#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <ctime>

//виджеты первого окна
GtkWidget* window;
GtkWidget* fixed1;
GtkWidget* button1;
GtkWidget* button2;
GtkWidget* login_entry;
GtkWidget* password_entry;
GtkWidget* pass_incorrect;
//виджеты второго окна
GtkWidget* window2;
GtkWidget* fixed2;
GtkWidget* button3;
GtkWidget* toggle1;
GtkWidget* button4;
GtkWidget* button5;
//какая-то неведомая поебень
GtkBuilder* builder;
GtkWidget* video;
GdkPixbuf* ccc;
//данные датчиков
int temp;
int vlaga;

inline int getRandomNumber(int min, int max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}
//блять ну и язык, даже рандома без static_cast не напишешь

void on_button1_clicked(GtkButton *button, gpointer user_data)
{
  std::string login;
  const char* log = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) login_entry));
  std::string password;
  const char* pas = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) password_entry));
  guint16 len = gtk_entry_get_text_length(GTK_ENTRY((GtkWidget*) login_entry));
  login.assign(log, len);
  len = gtk_entry_get_text_length(GTK_ENTRY((GtkWidget*) password_entry));
  password.assign(pas, len);
  if ((login == "admin")&&(password=="admin"))
  {
    gtk_widget_destroy(window);
    gtk_widget_show(window2);
    gtk_main();
  }
  else gtk_label_set_text(GTK_LABEL(pass_incorrect), "Неправильный логин/пароль!");
}

void on_button2_clicked(GtkButton *button, gpointer user_data)
{
    gtk_entry_set_text(GTK_ENTRY(login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry), "");
    gtk_label_set_text(GTK_LABEL(pass_incorrect), "");
}

void on_button3_clicked (GtkButton *button, gpointer user_data)
{
  gtk_image_set_from_pixbuf(GTK_IMAGE(video),ccc);
  gtk_widget_show(video);
}

void toggle1_toggled(GtkButton *button, gpointer user_data)
{
    std::cout<<"1"<<std::endl;
}

bool stream()
{
  frame = cvQueryFrame( capture );
          if(!frame) break;

         pix = gdk_pixbuf_new_from_data((guchar*) frame->imageData,
         GDK_COLORSPACE_RGB, FALSE, frame->depth, frame->width,
         frame->height, (frame->widthStep), NULL, NULL);


         gdk_draw_pixbuf(widget->window,
   widget->style->fg_gc[GTK_WIDGET_STATE (widget)], pix, 0, 0, 0, 0,
   -1, -1, GDK_RGB_DITHER_NONE, 0, 0); /* Other possible values are  GDK_RGB_DITHER_MAX,  GDK_RGB_DITHER_NORMAL */

         char c = cvWaitKey(33);
         if( c == 27 ) break;
   }

   cvReleaseCapture( &capture );
   return TRUE;
}

bool test_func()
{
  temp = getRandomNumber(0,30);
  vlaga = getRandomNumber(0,100);
  std::string new_label = "Температура: ";
  new_label+=std::to_string(temp);
  gtk_button_set_label(GTK_BUTTON((GtkWidget*) button4), new_label.c_str());
  new_label = "Влажность: "+std::to_string(vlaga)+"%";
  gtk_button_set_label(GTK_BUTTON((GtkWidget*) button5), new_label.c_str());
  return TRUE;
}

int main (int argc, char* argv[])
{
  gtk_init(&argc, &argv);
  srand(time(0));
  builder = gtk_builder_new_from_file("app.glade");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  window2 = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
  fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
  fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
  button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
  button2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2"));
  button3 = GTK_WIDGET(gtk_builder_get_object(builder, "button3"));
  button4 = GTK_WIDGET(gtk_builder_get_object(builder, "button4"));
  button5 = GTK_WIDGET(gtk_builder_get_object(builder, "button5"));
  login_entry = GTK_WIDGET(gtk_builder_get_object(builder, "login_entry"));
  password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry"));
  pass_incorrect = GTK_WIDGET(gtk_builder_get_object(builder, "pass_incorrect"));
  toggle1 = GTK_WIDGET(gtk_builder_get_object(builder, "toggle1"));
  video = GTK_WIDGET(gtk_builder_get_object(builder, "video"));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(toggle1, "toggled", G_CALLBACK(toggle1_toggled), NULL);
  g_signal_connect(GTK_BUTTON(button3), "clicked", G_CALLBACK(on_button3_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button2), "clicked", G_CALLBACK(on_button2_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button1), "clicked", G_CALLBACK(on_button1_clicked), NULL);
  g_timeout_add (10000, GSourceFunc(test_func), NULL);
  GError* error=NULL;
  ccc = gdk_pixbuf_new_from_file("test.jpg", &error);
  if( error != NULL )
{
    std::cout << error->message << std::endl;
    g_clear_error (&error);
}
  gtk_widget_show(window);
  gtk_main();
  return 0;
}

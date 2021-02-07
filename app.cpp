#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <cstring>
#include <iostream>
#include <cstdlib>

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
//какая-то неведомая поебень
GtkBuilder* builder;

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
  else gtk_label_set_text(GTK_LABEL(pass_incorrect), "Данные не верны, попробуйте снова!");
}

void on_button2_clicked(GtkButton *button, gpointer user_data)
{
    gtk_entry_set_text(GTK_ENTRY(login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry), "");
    gtk_label_set_text(GTK_LABEL(pass_incorrect), "");
}

int main (int argc, char* argv[])
{
  gtk_init(&argc, &argv);
  builder = gtk_builder_new_from_file("app.glade");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  window2 = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
  fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
  fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
  button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
  button2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2"));
  button3 = GTK_WIDGET(gtk_builder_get_object(builder, "button3"));
  login_entry = GTK_WIDGET(gtk_builder_get_object(builder, "login_entry"));
  password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry"));
  pass_incorrect = GTK_WIDGET(gtk_builder_get_object(builder, "pass_incorrect"));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(GTK_BUTTON(button2), "clicked", G_CALLBACK(on_button2_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button1), "clicked", G_CALLBACK(on_button1_clicked), NULL);
  gtk_widget_show(window);
  gtk_main();
  return 0;
}

#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <stdio.h>

//эту хуйню не удалять и в main не пихать
GtkWidget* window;
GtkWidget* fixed1;
GtkWidget* button1;
GtkWidget* button2;
GtkWidget* login_entry;
GtkWidget* password_entry;
GtkWidget* pass_incorrect;
GtkBuilder* builder;

/*void button1_clicked(GtkButton *button, gpointer user_data)
{
  char* login = (char*)malloc(sizeof(char)*100);
  char* password = (char*)malloc(sizeof(char)*100);
  login = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) login_entry));
  password = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) password_entry));
  login = (char*)"xxx";
  printf("%s\n",login);
  if ((login == "admin")&&(password=="admin")) gtk_main_quit();
  else gtk_label_set_text(GTK_LABEL(pass_incorrect), "неправильный пароль");
}*/

void on_button2_clicked(GtkButton *button, gpointer user_data)
{
    gtk_entry_set_text(GTK_ENTRY(login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry), "");
}

int main (int argc, char* argv[])
{
  gtk_init(&argc, &argv);
  builder = gtk_builder_new_from_file("app.glade");
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
  button1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1"));
  button2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2"));
  login_entry = GTK_WIDGET(gtk_builder_get_object(builder, "login_entry"));
  password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry"));
  pass_incorrect = GTK_WIDGET(gtk_builder_get_object(builder, "pass_incorrect"));
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(GTK_BUTTON(button2), "clicked", G_CALLBACK(on_button2_clicked), NULL);
  gtk_widget_show(window);
  gtk_main();
  return 0;
}

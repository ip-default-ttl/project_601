#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <glib.h>
/*#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>*/
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <ctime>

#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

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
int serial_port;

bool o=true;

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                std::cout<<"error from tcgetattr"<<errno;
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                std::cout<<"error from tcsetattr"<<errno<<std::endl;
                return -1;
        }
        return 0;
}

void set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                std::cout<<"error from tggetattr"<<errno<<std::endl;
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                std::cout<<"error setting term attributes"<<errno<<std::endl;
}

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
    std::string msg;
    if (o) msg="1";
    else msg="0";
    write (serial_port,msg.c_str() , 1);
    o=!o;
}

bool stream()
{
  /*frame = cvQueryFrame( capture );
          if(!frame) break;

         pix = gdk_pixbuf_new_from_data((guchar*) frame->imageData,
         GDK_COLORSPACE_RGB, FALSE, frame->depth, frame->width,
         frame->height, (frame->widthStep), NULL, NULL);


         gdk_draw_pixbuf(widget->window,
   widget->style->fg_gc[GTK_WIDGET_STATE (widget)], pix, 0, 0, 0, 0,
   -1, -1, GDK_RGB_DITHER_NONE, 0, 0); Other possible values are  GDK_RGB_DITHER_MAX,  GDK_RGB_DITHER_NORMAL

         char c = cvWaitKey(33);
         if( c == 27 ) break;
   }

   cvReleaseCapture( &cap=ture );
   return TRUE;*/
}

bool test_func()
{
  //temp = getRandomNumber(0,30);
  //vlaga = getRandomNumber(0,100);
  char buf[100] = "";
  int n;
  write (serial_port, "tem", 3);
  usleep ((7 + 50) * 100);
  while (buf==""){
  n = read (serial_port, buf, 100);}
  std::cout<<n<<std::endl;
  gtk_button_set_label(GTK_BUTTON((GtkWidget*) button4), buf);
  write (serial_port, "hum", 3);
  usleep ((7 + 50) * 100);
  n = read (serial_port, buf, 100);
  gtk_button_set_label(GTK_BUTTON((GtkWidget*) button5), buf);
  //std::string new_label = "Температура: ";

  //new_label+=std::to_string(temp);
  //gtk_button_set_label(GTK_BUTTON((GtkWidget*) button4), new_label.c_str());
  //new_label = "Влажность: "+std::to_string(vlaga)+"%";
  //gtk_button_set_label(GTK_BUTTON((GtkWidget*) button5), new_label.c_str());
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
  //g_signal_connect(GTK_BUTTON(button5), "clicked", G_CALLBACK(on_button5_clicked), NULL);
  //g_signal_connect(GTK_BUTTON(button4), "clicked", G_CALLBACK(on_button4_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button3), "clicked", G_CALLBACK(on_button3_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button2), "clicked", G_CALLBACK(on_button2_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button1), "clicked", G_CALLBACK(on_button1_clicked), NULL);
  char* portname = "/dev/ttyUSB0";
  serial_port = open(portname, O_RDWR);
  if (serial_port < 0)
  {
    std::cout<<"Error from open:"<<errno<<std::endl<<strerror(errno)<<std::endl;
  }
  set_interface_attribs (serial_port, B9600, 0);
  set_blocking (serial_port, 0);

  g_timeout_add (10000, GSourceFunc(test_func), NULL);
  GError* error=NULL;
  ccc = gdk_pixbuf_new_from_file("test.jpeg", &error);
  if( error != NULL )
{
    std::cout << error->message << std::endl;
    g_clear_error (&error);
}
  gtk_widget_show(window);
  gtk_main();
  return 0;
}

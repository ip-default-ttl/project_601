/*

  параметры конфигурации

*/
#define admin_login "admin" //логин для входа
#define admin_password "nimda"//пароль для входа
#define uart_port "/dev/ttyUSB0"//порт uart

//стрим высокого разрешения для распознавания номеров
#define main_stream_url "rtsp://admin:180500Kn@172.18.18.3:554/live/0/MAIN"

//стрим 800*448 10FPS
#define sub_stream_url "rtsp://admin:180500Kn@172.18.18.3:554/live/0/SUB"

/*
    библиотеки
*/

//подключаем gtk3
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

//подключаем opencv
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//просто нужные библиотеки
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>

// подключаем библиотеки для uart
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

//теперь приложение многопоточное
#include <thread>

/*
    Глобальные переменные
*/

//виджеты первого окна
GtkWidget* window;
GtkWidget* fixed1;

GtkWidget* button1_1;
GtkWidget* button1_2;

GtkWidget* login_entry;
GtkWidget* password_entry;

GtkWidget* pass_incorrect;
//виджеты второго окна
GtkWidget* window2;
GtkWidget* fixed2;

GtkWidget* button2_1;
GtkWidget* button2_2;
GtkWidget* button2_3;
GtkWidget* button2_4;
GtkWidget* button2_5;
GtkWidget* button2_6;

GtkWidget* toggle1;
GtkWidget* toggle2_1;
GtkWidget* toggle2_2;

GtkWidget* video1;
GtkWidget* photo;

GtkWidget* text2_1;
//какая-то неведомая поебень
GtkBuilder* builder;
GdkPixbuf* ccc;
//данные датчиков
int temp;
int vlaga;
//служебные переменныые
int serial_port;
bool o=true;
bool toggle2_1_on = false;
bool toggle2_2_on = false;

/*
    прототипы функций
*/

//функции кнопок
void on_button1_1_clicked(GtkButton *button, gpointer user_data);
void on_button1_2_clicked(GtkButton *button, gpointer user_data);
void on_button3_clicked(GtkButton *button, gpointer user_data);
void toggle1_toggled(GtkButton *button, gpointer user_data);
void toggle2_1_toggled(GtkButton *button, gpointer user_data);
void toggle2_2_toggled(GtkButton *button, gpointer user_data);

//функции для uart
int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);

bool stream();
void request_sensors();

inline int getRandomNumber(int min, int max)
{
    static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
    return static_cast<int>(rand() * fraction * (max - min + 1) + min);
}

bool update_stream(GdkPixbuf* frame)
{
  gtk_image_set_from_pixbuf(GTK_IMAGE(video1),frame);
  return FALSE;
}

bool test_func()
{

}

bool start() {
  thread1 = g_thread_try_new("thread", GThreadFunc(stream), NULL, NULL);
  return TRUE;
}

int main (int argc, char* argv[])
{
  gtk_init(&argc, &argv);
  srand(time(0));
  builder = gtk_builder_new_from_file("app.glade");
  //окно входа
  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

  fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));

  button1_1 = GTK_WIDGET(gtk_builder_get_object(builder, "button1_1"));
  button1_2 = GTK_WIDGET(gtk_builder_get_object(builder, "button1_2"));

  login_entry = GTK_WIDGET(gtk_builder_get_object(builder, "login_entry"));
  password_entry = GTK_WIDGET(gtk_builder_get_object(builder, "password_entry"));

  pass_incorrect = GTK_WIDGET(gtk_builder_get_object(builder, "pass_incorrect"));
  //основное окно
  window2 = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

  fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));

  button2_1 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_1"));
  button2_2 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_2"));
  button2_3 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_3"));
  button2_4 = GTK_WIDGET(gtk_builder_get_object(builder, "button2_4"));

  text2_1 = GTK_WIDGET(gtk_builder_get_object(builder, "text2_1"));

  toggle1 = GTK_WIDGET(gtk_builder_get_object(builder, "toggle1"));
  toggle2_1 = GTK_WIDGET(gtk_builder_get_object(builder, "toggle2_1"));
  toggle2_2 = GTK_WIDGET(gtk_builder_get_object(builder, "toggle2_2"));

  video1 = GTK_WIDGET(gtk_builder_get_object(builder, "video1"));
  //подсоединяем функции кнопок
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  g_signal_connect(toggle1, "toggled", G_CALLBACK(toggle1_toggled), NULL);
  g_signal_connect(toggle2_1, "toggled", G_CALLBACK(toggle2_1_toggled), NULL);
  g_signal_connect(toggle2_2, "toggled", G_CALLBACK(toggle2_2_toggled), NULL);
  g_signal_connect(GTK_BUTTON(button2_1), "clicked", G_CALLBACK(on_button3_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button1_2), "clicked", G_CALLBACK(on_button1_2_clicked), NULL);
  g_signal_connect(GTK_BUTTON(button1_1), "clicked", G_CALLBACK(on_button1_1_clicked), NULL);
  serial_port = open(uart_port, O_RDWR | O_NOCTTY | O_SYNC);
  if (serial_port < 0)
  {
    std::cout<<"Error from open:"<<errno<<std::endl<<strerror(errno)<<std::endl;
  }
  set_interface_attribs (serial_port, B9600, 0);
  set_blocking (serial_port, 0);
  g_timeout_add_seconds (60, GSourceFunc(test_func), NULL);
  gtk_widget_show(window);
  std::thread thread(stream);
  thread.detach();
  gtk_main();
  close(serial_port);
  return 0;
}

inline int set_interface_attribs (int fd, int speed, int parity)
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

inline void set_blocking (int fd, int should_block)
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

void on_button1_1_clicked(GtkButton *button, gpointer user_data)
{
  std::string login;
  const char* log = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) login_entry));
  std::string password;
  const char* pas = gtk_entry_get_text(GTK_ENTRY((GtkWidget*) password_entry));
  guint16 len = gtk_entry_get_text_length(GTK_ENTRY((GtkWidget*) login_entry));
  login.assign(log, len);
  len = gtk_entry_get_text_length(GTK_ENTRY((GtkWidget*) password_entry));
  password.assign(pas, len);
  if ((login == admin_login)&&(password==admin_password))
  {
    gtk_widget_destroy(window);
    gtk_widget_show(window2);
    gtk_main();
  }
  else gtk_label_set_text(GTK_LABEL(pass_incorrect), "Неправильный логин/пароль!");
}

void on_button1_2_clicked(GtkButton *button, gpointer user_data)
{
    gtk_entry_set_text(GTK_ENTRY(login_entry), "");
    gtk_entry_set_text(GTK_ENTRY(password_entry), "");
    gtk_label_set_text(GTK_LABEL(pass_incorrect), "");
}

void on_button3_clicked (GtkButton *button, gpointer user_data)
{
  write (serial_port, "open", 4);
}

void toggle1_toggled(GtkButton *button, gpointer user_data)
{
    std::string msg;
    if (o) msg="1";
    else msg="0";
    write (serial_port,msg.c_str() , 100);
    o=!o;
}

void toggle2_1_toggled(GtkButton *button, gpointer user_data)
{
  toggle2_1_on = !toggle2_1_on;
  if ((!toggle2_1_on)&&(toggle2_2_on))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle2_2), FALSE);
  }
}

void toggle2_2_toggled(GtkButton *button, gpointer user_data)
{
  toggle2_2_on = !toggle2_2_on;
  if ((!toggle2_1_on)&&(toggle2_2_on))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle2_1), TRUE);
  }
}

bool stream()
{
  GdkPixbuf* pix;
  unsigned long long int a=0;
  std::string url="rtsp://admin:180500Kn@172.18.18.3:554/live/0/SUB";
  cv::VideoCapture cap(url);
  if (!cap.isOpened()) {std::cout<<"???";return 1;}
  cv::Mat frame;
  while (true) {
  cap>>frame;
  cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
  pix = gdk_pixbuf_new_from_data(frame.data, GDK_COLORSPACE_RGB, true, 8, frame.cols, frame.rows, frame.step, NULL, NULL);
  g_idle_add(GSourceFunc(update_stream), pix);
  usleep(9000);
  }
  cap.release();
  return TRUE;
}

void request_sensors ()
{
  //temp = getRandomNumber(0,30);
  //vlaga = getRandomNumber(0,100);
  int n=0;
  int time_start;
  char buf[50] = "";
  char buf2[50] = "";
  char buf3[50] = "";
  char buf4[50] = "";
  //echo каждые 10 сек
  //tem каждые 90 сек
  //hum каждые 30 сек
  //газ каждые 60 сек
  write (serial_port, "tem", 3);
  auto t1=std::chrono::high_resolution_clock::now();
  auto t2 = std::chrono::high_resolution_clock::now();
  while (true)
  {
    n = read (serial_port, buf, 100);
    t2 = std::chrono::high_resolution_clock::now();
    unsigned long long int duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    if (duration>2)
    {
        std::cout<<"recieve timeout!"<<std::endl;
        break;
    }
  }
  gtk_button_set_label(GTK_BUTTON((GtkWidget*) button2_2), buf);
  n=0;
  write (serial_port, "hum", 3);
  t1=std::chrono::high_resolution_clock::now();
  while (n==0)
  {
    n = read (serial_port, buf2, 100);
    t2 = std::chrono::high_resolution_clock::now();
    unsigned long long int duration = std::chrono::duration_cast<std::chrono::seconds>(t2 - t1).count();
    if (duration>2)
    {
      std::cout<<"recieve timeout!"<<std::endl;
      break;
    }
  }
  gtk_button_set_label(GTK_BUTTON((GtkWidget*) button2_3), buf2);
  return TRUE;
}

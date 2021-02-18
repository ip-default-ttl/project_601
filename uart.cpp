#include <termios.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

using namespace std;

int set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                cout<<"error from tcgetattr"<<errno;
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
                cout<<"error from tcsetattr"<<errno<<endl;
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
                cout<<"error from tggetattr"<<errno<<endl;
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                cout<<"error setting term attributes"<<errno<<endl;
}

int main()
{
  char* portname = "/dev/ttyUSB0";
  int serial_port = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
  if (serial_port < 0)
  {
    cout<<"Error from open:"<<errno<<endl<<strerror(errno)<<endl;
  }
  //string message;
  //cin>>message;
  //message+="/n";
  set_interface_attribs (serial_port, B9600, 0);
  set_blocking (serial_port, 0);
  /*write (serial_port, message.c_str(), 1);
  char buf [100];
  usleep ((7 + 50) * 100);
  int n = read (serial_port, buf, 100);
  cout<<buf<<endl;*/
  int n;
  char buf[100]="";
  write (serial_port, "tem", 3);
  usleep ((7 + 25) * 100);
  while (buf!=""){
  n = read (serial_port, buf, 100);
  cout<<n<<endl;
  cout<<buf<<endl;}
  close(serial_port);
}

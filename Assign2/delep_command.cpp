#include <iostream>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  int fd;
  struct flock lock;

  if (argc != 2)
  {
    std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
    return 1;
  }

  fd = open(argv[1], O_RDWR);
  if (fd == -1)
  {
    perror("open");
    return 1;
  }
  getchar();
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;

  if (fcntl(fd, F_SETLK, &lock) == -1)
  {
    perror("fcntl");
    return 1;
  }

  std::cout << "Locked file " << argv[1] << std::endl;
  getchar();

  // lock.l_type = F_UNLCK;
  // if (fcntl(fd, F_SETLK, &lock) == -1) {
  //   perror("fcntl");
  //   return 1;
  // }

  close(fd);

  return 0;
}   
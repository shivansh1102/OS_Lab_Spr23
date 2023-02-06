#include <iostream>
#include <unistd.h>

int main()
{	
	int i = 0;
	while(i < 20)
	{
		std::cout << i << std::endl;
		i++;
		sleep(3);
	}
	return 0;
}

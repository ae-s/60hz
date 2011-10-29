/* Thinger to read 60hz cycle times and display diagnostics in a kind
 * of stripcharty way.
 */

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


int main(int *argc, char **argv)
{
	char line[20];
	double bottom = 1/59.9;
	double top = 1/60.1;
	double center = 1/60;
	int fd = open("/dev/ttyACM0", O_RDONLY);

	while (1) {
		double current;
		char* line = "          |          \n";
		int len = read(fd, &line, 20);
		if (len == 0) {
			puts("Fucked\n");
			continue;
		}
		current = (float)atoi(&line);
		printf("%f %f\n", current - bottom, current);
	}
}

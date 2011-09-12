/* Thinger to read 60hz cycle times and display diagnostics in a kind
 * of stripcharty way.
 */

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#define QUEUE 100
#define WIDTH 40

int main(int *argc, char **argv)
{
	char line[WIDTH + 10];
	double bottom = 1000000/59.9;
	double top = 1000000/60.1;
	double center = 1000000/60;
	double queue[QUEUE];
	int counter;
	int qpos = 0;
	int fd = open("/dev/ttyACM0", O_RDONLY);

	while (1) {
		double current, average;
		char line[] = "                    |                    ";
		char inlin[20];
		int position, pos_avg, i;
		int len = read(fd, &inlin, 20);
		if (len == 0) {
			puts("Fucked\n");
			continue;
		}
		current = (double)atoi(&inlin);
		queue[qpos++] = current;
		counter++;
		if (qpos == QUEUE)
			qpos = 0;

		average = 0;
		for (i = 0; i < QUEUE; i++) {
			average += queue[i];
		}
		average /= QUEUE;
		pos_avg = WIDTH-(int)((top-average)/((top-bottom)/WIDTH));
		position = WIDTH-(int)((top-current)/((top-bottom)/WIDTH));

		if (position > WIDTH) {
			line[WIDTH] = '>';
		} else if (position < 0) {
			line[0] = '<';
		} else {
			line[position] = '.';
		}

		if (pos_avg > WIDTH) {
			line[WIDTH] = '_';
		} else if (pos_avg < 0) {
			line[0] = '_';
		} else {
			line[pos_avg] = '*';
		}
		if (counter % 15 == 0)
			printf("[%s] %f\n", line, 1/(average/1000000));
	}
}

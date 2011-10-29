/* Thinger to read 60hz cycle times and display diagnostics in a kind
 * of stripcharty way.
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

// window for frequency averaging, in cycles
#define QUEUE 3600
// window for reporting, in cycles
#define REPORT_FREQ 60
// width of the bargraph
#define WIDTH 40
// timer cycles per second
#define SCALE 1000000

int main(int *argc, char **argv)
{
	char line[WIDTH + 10];
	char blankline[] = "                    |                    ";
	double bottom = SCALE/59.8;
	double top = SCALE/60.2;
	double center = SCALE/60;
	double queue[QUEUE];
	int counter;
	int qpos = 0;
	int fd = open("/dev/ttyACM0", O_RDONLY);

	strcpy(&line, &blankline);


	while (1) {
		double current, average;
		char inlin[20];
		int position, pos_avg, i;
		int len = read(fd, &inlin, 6);
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

		if (counter % REPORT_FREQ == 0) {
			if (pos_avg > WIDTH) {
				line[WIDTH] = '_';
			} else if (pos_avg < 0) {
				line[0] = '_';
			} else {
				line[pos_avg] = '#';
			}
			line[20] = '|';
			printf("[%s] %f\n", line, 1/(average/1000000));
			strcpy(&line, &blankline);
		}
	}
}

/* Thinger to read 60hz cycle times and display diagnostics in a kind
 * of stripcharty way.
 */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

// window for reporting, in cycles
#define REPORT_FREQ 60
// width of the bargraph
#define WIDTH 40
// timer cycles per second
#define SCALE 1000000
// length between nominal timer overflows, in microseconds
// (1 overflow / 65'536 microseconds) * (1'000'000 microseconds)
#define TICKSIZE (SCALE / 65536)

#define NANOSECOND 1000000000
#define MILLISECOND 1000

#define NOMINAL 60
#define MOTION 0.2

int main(int *argc, char **argv)
{
	char line[WIDTH + 10];
	char blankline[] = "                    |                    ";
	double bottom = SCALE/(NOMINAL-MOTION);
	double top = SCALE/(NOMINAL+MOTION);
	double center = SCALE/(NOMINAL);
	int counter;
	int qpos = 0;
	int fd = open("/dev/ttyACM0", O_RDONLY);

	long int count_total = 0;	// total overflows so far ever
	struct timeval initial;	// initial timestamp

	signed long long int initial_nsec;
	signed long int initial_msec;

	strcpy(&line, &blankline);

	gettimeofday(&initial, NULL);

	initial_msec = (initial.tv_sec * 1000) + (initial.tv_usec / 1000);

	while (1) {
		double current, average;
		char inlin[20];
		int position, pos_avg, i;
		int len;

		struct timeval current_time;
		signed long long int cur_nsec;
		signed long int cur_msec;

		long double nominal, measured;

		len = read(fd, &inlin, 1);
		if (len == 0) {
			puts("Fucked\n");
			continue;
		}
		gettimeofday(&current_time, NULL);
		cur_msec = (current_time.tv_usec / 1000) + (current_time.tv_sec * 1000);

		count_total++;

		nominal = (long double) count_total / TICKSIZE;
		//measured = (cur_nsec - initial_nsec) / NANOSECOND;
		measured = (cur_msec - initial_msec) / MILLISECOND;

		printf("% '05.8Lf - % '05.8Lf, % '05.8Lf\n", nominal, measured, measured/nominal);

	}
}

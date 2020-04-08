
#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

#define ERR(source) (perror(source),\
                     fprintf(stderr,"%s:%d\n",__FILE__,__LINE__))

struct gpiod_chip* CreateChip(const char* chipname);
struct gpiod_line* GetLine(struct gpiod_chip* chip, unsigned int line_num);
int LineRequestOutput(struct gpiod_line* line, const char* consumer = CONSUMER, int default_val = 0);
int SetLEDValue(struct gpiod_line* line, int value);

int main(int argc, char* argv[])
{
	const char *chipname = "gpiochip1";
	unsigned int line_num = 24;	// GPIO Pin #24 - led
	unsigned int val;
	struct gpiod_chip *chip;
	struct gpiod_line *line;
	int i;

	chip = CreateChip(chipname);
	if(!chip)
		goto close_chip;

	line = GetLine(chip, line_num);
	if(!line)
		goto release_line;

	if(LineRequestOutput(line) < 0)
		goto release_line;

	/* Blink 20 times */
	for (i = 0; i < 20; i++) {
		if(SetLEDValue(line, i%2) < 0)
			goto release_line;
		printf("Output %u on line #%u\n", i%2, line_num);
		sleep(1);
		//val = !val;
	}

release_line:
	//release all lines
	gpiod_line_release(line);
close_chip:
	gpiod_chip_close(chip);
end:
	return 0;
}

struct gpiod_chip* CreateChip(const char* chipname)
{
	struct gpiod_chip *chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		ERR("Open chip failed\n");
		return NULL;
	}
	return chip;
}

struct gpiod_line* GetLine(struct gpiod_chip* chip, unsigned int line_num)
{
	struct gpiod_line* line = gpiod_chip_get_line(chip, line_num);
	if (!line) {
		ERR("Get line failed\n");
		return NULL;
	}
	return line;
}


int LineRequestOutput(struct gpiod_line* line, const char* consumer, int default_val)
{
	int ret = gpiod_line_request_output(line, consumer, default_val);
	if (ret < 0) {
		ERR("Request line as output failed\n");
		return -1;
	}
	return 0;
}

int SetLEDValue(struct gpiod_line* line, int value)
{
	int ret = gpiod_line_set_value(line, value);
	if (ret < 0) {
		ERR("Set line output failed\n");
		return -1;
	}
	return 0;
}

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include "poker.h"

#ifndef CONSUMER
#define CONSUMER "Consumer"
#endif

#define ERR(source) (perror(source), \
					 fprintf(stderr, "%s:%d\n", __FILE__, __LINE__))

struct gpiod_chip *CreateChip(const char *chipname);
struct gpiod_line *GetLine(struct gpiod_chip *chip, unsigned int line_num);
int LineRequestOutput(struct gpiod_line *line, const char *consumer = CONSUMER, int default_val = 0);
int SetLEDValue(struct gpiod_line *line, int value);

int main(int argc, char *argv[])
{
	const char *chipname = "gpiochip1";
	//unsigned int line_num = 24;	// GPIO Pin #24 - led
	unsigned int line_num = 12; // GPIO Pin #12 - button
	int val;
	struct timespec ts = {1, 0};
	struct gpiod_line_event event;
	struct gpiod_chip *chip;
	struct gpiod_line *line;
	int i, ret;

	bool shouldBeChanged[5];
	for (int i = 0; i < 5; i++)
	{
		if (i % 2 == 0)
			shouldBeChanged[i] = false;
		else
			shouldBeChanged[i] = true;
	}
	std::cout << "Create game" << std::endl;
	auto game = Cards::Poker(1000);
	game.PlayNextRound();
	game.DrawSelectPanel(shouldBeChanged, 5);
	game.ChangeCards(shouldBeChanged);
	game.ShowStakePrompt();

	sleep(2);
	for (int i = 0; i < 5; i++)
	{
		if (i % 2 == 1)
			shouldBeChanged[i] = true;
		else
			shouldBeChanged[i] = true;
	}
	game.PlayNextRound();
	game.DrawSelectPanel(shouldBeChanged, 5);
	game.ChangeCards(shouldBeChanged);
// 	chip = CreateChip(chipname);
// 	if (!chip)
// 		goto close_chip;

// 	line = GetLine(chip, line_num);
// 	if (!line)
// 		goto release_line;

// 	// if(LineRequestOutput(line) < 0)
// 	// 	goto release_line;

// 	ret = gpiod_line_request_both_edges_events(line, CONSUMER);
// 	if (ret < 0)
// 	{
// 		perror("Request event notification failed\n");
// 		ret = -1;
// 		goto release_line;
// 	}

// 	/* Notify event up to 20 times */
// 	i = 0;
// 	while (i <= 20)
// 	{
// 		ret = gpiod_line_event_wait(line, &ts);
// 		if (ret < 0)
// 		{
// 			perror("Wait event notification failed\n");
// 			ret = -1;
// 			goto release_line;
// 		}
// 		else if (ret == 0)
// 		{
// 			printf("Wait event notification on line #%u timeout\n", line_num);
// 			continue;
// 		}

// 		ret = gpiod_line_event_read(line, &event);
// 		printf("Get event notification on line #%u %d times\n", line_num, i);
// 		if (ret < 0)
// 		{
// 			perror("Read last event notification failed\n");
// 			ret = -1;
// 			goto release_line;
// 		}
// 		sleep(1);

// 		i++;
// 	}

// 	ret = 0;

// release_line:
// 	//release all lines
// 	gpiod_line_release(line);
// close_chip:
// 	gpiod_chip_close(chip);
end:
	return 0;
}

struct gpiod_chip *CreateChip(const char *chipname)
{
	struct gpiod_chip *chip = gpiod_chip_open_by_name(chipname);
	if (!chip)
	{
		ERR("Open chip failed\n");
		return NULL;
	}
	return chip;
}

struct gpiod_line *GetLine(struct gpiod_chip *chip, unsigned int line_num)
{
	struct gpiod_line *line = gpiod_chip_get_line(chip, line_num);
	if (!line)
	{
		ERR("Get line failed\n");
		return NULL;
	}
	return line;
}

int LineRequestOutput(struct gpiod_line *line, const char *consumer, int default_val)
{
	int ret = gpiod_line_request_output(line, consumer, default_val);
	if (ret < 0)
	{
		ERR("Request line as output failed\n");
		return -1;
	}
	return 0;
}

int SetLEDValue(struct gpiod_line *line, int value)
{
	int ret = gpiod_line_set_value(line, value);
	if (ret < 0)
	{
		ERR("Set line output failed\n");
		return -1;
	}
	return 0;
}

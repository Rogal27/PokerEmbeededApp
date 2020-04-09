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

constexpr int LEDS_COUNT = 4;
constexpr int BUTTONS_COUNT = 3;

struct gpiod_chip *CreateChip(const char *chipname);
struct gpiod_line *GetLine(struct gpiod_chip *chip, unsigned int line_num);
int LineRequestOutput(struct gpiod_line *line, const char *consumer = CONSUMER, int default_val = 0);
int SetLEDValue(struct gpiod_line *line, int value);
int RequestBulkEvents(struct gpiod_line_bulk *bulk, const char *consumer = CONSUMER);
struct gpiod_line *readPressedButton(struct gpiod_line_bulk *bulk, struct timespec *timeout, struct gpiod_line_bulk *event_bulk);
ButtonType getPressedButtonType(struct gpiod_line *line, struct gpiod_line **buttons, int buttonsCount);

enum class ButtonType
{
	LEFT,
	MIDDLE,
	RIGHT,
	NONE
}

int
main(int argc, char *argv[])
{

	//unsigned int line_num = 24;	// GPIO Pin #24 - led
	//unsigned int line_num = 12; // GPIO Pin #12 - button
	//int val;
	//struct timespec ts = {1, 0};

	//chip
	struct gpiod_chip *chip;
	const char *chipname = "gpiochip1";

	//leds
	struct gpiod_line *led_line[LEDS_COUNT];
	unsigned int led_line_num[LEDS_COUNT];
	for (int i = 0; i < LEDS_COUNT; i++)
	{
		led_line_num[i] = 24 + i; // GPIO Pin #24+i - led
	}

	//buttons
	struct gpiod_line *button_line[BUTTONS_COUNT];
	unsigned int button_line_num[BUTTONS_COUNT];
	struct gpiod_line_bulk *bulkButton;
	struct gpiod_line_bulk *eventBulkButton;
	for (int i = 0; i < BUTTONS_COUNT; i++)
	{
		button_line_num[i] = 12 + i; // GPIO Pin #12+i - button
	}

	//create chip
	chip = CreateChip(chipname);
	if (!chip)
		goto close_chip;

	//get lines for LED
	for (int i = 0; i < LEDS_COUNT; i++)
	{
		led_line[i] = GetLine(chip, led_line_num[i]);
		if (!led_line[i])
			goto release_line_led;
	}
	//get lines for BUTTON
	for (int i = 0; i < BUTTON_COUNT; i++)
	{
		button_line[i] = GetLine(chip, button_line_num[i]);
		if (!button_line[i])
			goto release_line_button;
	}

	//request output for LED
	for (int i = 0; i < LEDS_COUNT; i++)
	{
		if (LineRequestOutput(led_line[i]) < 0)
			goto release_line_button;
	}

	//set bulk button
	gpiod_line_bulk_init(bulkButton);
	for (int i = 0; i < BUTTONS_COUNT; i++)
	{
		gpiod_line_bulk_add(bulkButton, button_line[i]);
	}
	if (RequestBulkEvents(bulkButton) < 0)
		goto release_line_button;

	//gameplay
	bool shouldBeChanged[5];
	for (int i = 0; i < 5; i++)
	{
		if (i % 2 == 0)
			shouldBeChanged[i] = false;
		else
			shouldBeChanged[i] = true;
	}
	std::cout << "Create game" << std::endl;
	Cards::Poker *game;
	try
	{
		game = new Cards::Poker(1000, led_line, LEDS_COUNT, 0);
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << '\n';
		goto delete_game;
	}

	sleep(2);

	try
	{
		game->PlayNextRound();
		sleep(2);
		game->DrawSelectPanel(shouldBeChanged, 5);
		sleep(2);
		game->ChangeCards(shouldBeChanged);
		sleep(2);
		game->ShowStakePrompt();
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << '\n';
		goto delete_game;
	}

	sleep(2);
	for (int i = 0; i < 5; i++)
	{
		if (i % 2 == 1)
			shouldBeChanged[i] = true;
		else
			shouldBeChanged[i] = true;
	}

	try
	{
		game->PlayNextRound();
		sleep(2);
		game->DrawSelectPanel(shouldBeChanged, 5);
		sleep(2);
		game->ChangeCards(shouldBeChanged);
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << '\n';
		goto delete_game;
	}

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
delete_game:
	delete game;
release_line_button:
	for (int i = 0; i < BUTTONS_COUNT; i++)
	{
		gpiod_line_release(button_line[i]);
	}
release_line_led:
	for (int i = 0; i < LEDS_COUNT; i++)
	{
		gpiod_line_release(led_line[i]);
	}
close_chip:
	gpiod_chip_close(chip);
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

int RequestBulkEvents(struct gpiod_line_bulk *bulk, const char *consumer)
{
	int ret = gpiod_line_request_bulk_both_edges_events(bulkButton, consumer);
	if (ret < 0)
	{
		ERR("Request event notification failed\n");
		return -1;
	}
	return 0;
}

struct gpiod_line *readPressedButton(struct gpiod_line_bulk *bulk, struct timespec *timeout, struct gpiod_line_bulk *event_bulk)
{
	struct gpiod_line *readLine;
	struct timespec tbmax = {0, 200 * 1000000};
	struct gpiod_line_event event;
	int val;

	val = gpiod_line_event_wait_bulk(bulk, timeout, event_bulk);
	switch (val)
	{
	case 1:
		readLine = gpiod_line_bulk_get_line(event_bulk, 0);
		break;
	case 0:
		std::cout << "Game ended due to inactivity" << std::endl;
		return NULL;
	default:
		ERR("Wait event bulk notification failed!\n");
		return NULL;
	}

	while (val == 1)
	{
		int ret = gpiod_line_event_read(readLine, &event);
		if (ret < 0)
		{
			ERR("Read last event notification failed")
		}
		val = gpiod_line_event_wait(readLine, &tbmax);
	}

	if (val < 0)
	{
		ERR("Wait event notification failed!\n");
		return NULL;
	}

	return receievedLine;
}

ButtonType getPressedButtonType(struct gpiod_line *line, struct gpiod_line **buttons, int buttonsCount)
{
	//assuming that buttonsCount = 3
	if (line == buttons[0])
		return ButtonType::LEFT;
	if (line == buttons[1])
		return ButtonType::MIDDLE;
	if (line == buttons[2])
		return ButtonType::RIGHT;
	return ButtonType::NONE;
	
}
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
constexpr int CARDS_COUNT = 5;
constexpr int MAX_STAKE_INDEX = 4;

enum class ButtonType
{
	LEFT,
	MIDDLE,
	RIGHT,
	NONE
};

struct gpiod_chip *CreateChip(const char *chipname);
struct gpiod_line *GetLine(struct gpiod_chip *chip, unsigned int line_num);
int LineRequestOutput(struct gpiod_line *line, const char *consumer = CONSUMER, int default_val = 0);
int SetLEDValue(struct gpiod_line *line, int value);
int RequestBulkEvents(struct gpiod_line_bulk *bulk, const char *consumer = CONSUMER);
struct gpiod_line *readPressedButton(struct gpiod_line_bulk *bulk, struct timespec *timeout, struct gpiod_line_bulk *event_bulk);
ButtonType getPressedButtonType(struct gpiod_line *line, struct gpiod_line **buttons, int buttonsCount);

int main(int argc, char *argv[])
{
	//game
	Cards::Poker *game;
	bool shouldBeChanged[CARDS_COUNT];
	int stakeIndex = 0;
	int selectedCardIndex = 0;
	bool inGame = true;
	bool goToNextPart = false;
	struct timespec timeout = {60, 0};
	struct gpiod_line *readLine;

	//chip
	struct gpiod_chip *chip;
	const char *chipname = "gpiochip1";

	//leds
	struct gpiod_line *led_line[LEDS_COUNT];
	unsigned int led_line_num[LEDS_COUNT];

	//buttons
	struct gpiod_line *button_line[BUTTONS_COUNT];
	unsigned int button_line_num[BUTTONS_COUNT];
	struct gpiod_line_bulk bulkButton;
	struct gpiod_line_bulk eventBulkButton;

	//led
	for (int i = 0; i < LEDS_COUNT; i++)
	{
		led_line_num[i] = 24 + i; // GPIO Pin #24+i - led
	}

	//button
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
	for (int i = 0; i < BUTTONS_COUNT; i++)
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
	gpiod_line_bulk_init(&bulkButton);
	for (int i = 0; i < BUTTONS_COUNT; i++)
	{
		gpiod_line_bulk_add(&bulkButton, button_line[i]);
	}

	if (RequestBulkEvents(&bulkButton) < 0)
		goto release_line_button;

	//gameplay
	std::cout << "Create game" << std::endl;
	try
	{
		game = new Cards::Poker(100, led_line, LEDS_COUNT, 0);
	}
	catch (const std::runtime_error &e)
	{
		std::cerr << e.what() << '\n';
		goto delete_game;
	}

	while (inGame)
	{
		selectedCardIndex = 0;
		goToNextPart = false;
		for (int i = 0; i < CARDS_COUNT; i++)
		{
			shouldBeChanged[i] = false;
		}
		try
		{
			game->ShowStakePrompt();
			while (!goToNextPart)
			{
				readLine = readPressedButton(&bulkButton, &timeout, &eventBulkButton);
				if (readLine == NULL)
					break;
				switch (getPressedButtonType(readLine, button_line, BUTTONS_COUNT))
				{
				case ButtonType::LEFT:
					stakeIndex--;
					if (stakeIndex < 0)
						stakeIndex = 0;
					game->SetStakeWithLED(stakeIndex);
					break;
				case ButtonType::RIGHT:
					stakeIndex++;
					if (stakeIndex >= MAX_STAKE_INDEX)
						stakeIndex = MAX_STAKE_INDEX - 1;
					game->SetStakeWithLED(stakeIndex);
					break;
				case ButtonType::MIDDLE:
					goToNextPart = true;
					break;
				default:
					throw std::runtime_error("Unexpected button");
				}
			}
			game->PlayNextRound();
			goToNextPart = false;
			while (!goToNextPart)
			{
				readLine = readPressedButton(&bulkButton, &timeout, &eventBulkButton);
				if (readLine == NULL)
					break;
				switch (getPressedButtonType(readLine, button_line, BUTTONS_COUNT))
				{
				case ButtonType::LEFT:
					selectedCardIndex--;
					if (selectedCardIndex < 0)
						selectedCardIndex = 0;
					game->DrawSelectPanel(shouldBeChanged, selectedCardIndex);
					break;
				case ButtonType::RIGHT:
					selectedCardIndex++;
					if (selectedCardIndex >= CARDS_COUNT + 1)
						selectedCardIndex = CARDS_COUNT;
					game->DrawSelectPanel(shouldBeChanged, selectedCardIndex);
					break;
				case ButtonType::MIDDLE:
					if (selectedCardIndex == CARDS_COUNT)
					{
						if (game->ChangeCards(shouldBeChanged) == true)
							goToNextPart = true;
					}
					else
					{
						shouldBeChanged[selectedCardIndex] = !shouldBeChanged[selectedCardIndex];
						game->DrawSelectPanel(shouldBeChanged, selectedCardIndex);
					}
					break;
				default:
					throw std::runtime_error("Unexpected button");
				}
			}
			goToNextPart = false;
			while (!goToNextPart)
			{
				readLine = readPressedButton(&bulkButton, &timeout, &eventBulkButton);
				if (readLine == NULL)
					break;
				switch (getPressedButtonType(readLine, button_line, BUTTONS_COUNT))
				{
				case ButtonType::LEFT:
				case ButtonType::RIGHT:
					inGame = false;
					std::cout << "Thanks for playing :)" << std::endl;
					break;
				case ButtonType::MIDDLE:
					goToNextPart = true;
					break;
				default:
					throw std::runtime_error("Unexpected button");
				}
			}
		}
		catch (const std::runtime_error &e)
		{
			std::cerr << e.what() << '\n';
			break;
		}
	}

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
	int ret = gpiod_line_request_bulk_both_edges_events(bulk, consumer);
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
			ERR("Read last event notification failed");
		}
		val = gpiod_line_event_wait(readLine, &tbmax);
	}

	if (val < 0)
	{
		ERR("Wait event notification failed!\n");
		return NULL;
	}

	return readLine;
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
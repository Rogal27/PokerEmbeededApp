#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#ifndef	CONSUMER
#define	CONSUMER	"Consumer"
#endif

int lightenLed(struct gpiod_line *line, unsigned int val, unsigned int line_num)
{
	int i, ret;
	ret = gpiod_line_set_value(line, val);
	if (ret < 0) {
		perror("Set line output failed\n");
		return 1;
	}
	return 0;
}

int showResult(struct gpiod_line *line28, struct gpiod_line *line29, struct gpiod_line *line30, struct gpiod_line *line31, int sum, unsigned int line_num28, unsigned int line_num29, unsigned int line_num30, unsigned int line_num31)
{
	int led31Val = sum%2; sum/=2;
	int led30Val = sum%2; sum/=2;
	int led29Val = sum%2; sum/=2;
	int led28Val = sum%2; sum/=2;
	int error0 = lightenLed(line31, led31Val, line_num31);
	int error1 = lightenLed(line30, led30Val, line_num30);
	int error2 = lightenLed(line29, led29Val, line_num29);
	int error3 = lightenLed(line28, led28Val, line_num28);
	if(error0 == 1 || error1 == 1 || error2 == 1 || error3 == 1)
		return 1;
	else
		return 0;
} 

int raiseError(struct gpiod_line *line28, struct gpiod_line *line29, struct gpiod_line *line30, struct gpiod_line *line31, int sum, unsigned int line_num28, unsigned int line_num29, unsigned int line_num30, unsigned int line_num31)
{
	unsigned int val = 1;
	unsigned int _val = 0;
	for(int i = 0; i < 6; i++)
	{
		if(i == 5) 
			{ val = 0; _val = 0; }
		int error0 = lightenLed(line31, val, line_num31);
		int error1 = lightenLed(line30, _val, line_num30);
		int error2 = lightenLed(line29, val, line_num29);
		int error3 = lightenLed(line28, _val, line_num28);
		if(error0 == 1 || error1 == 1 || error2 == 1 || error3 == 1) 
			return 1;
		_val = val;
		val = !val;
		sleep(1);
	}

	printf("Result exceeded 15, screen cleared!\n");
	return 0;
} 

struct gpiod_line* readPressedButton(struct gpiod_line_bulk *bulkButton, struct timespec *ts, struct gpiod_line_bulk *eventBulkButton)
{
	struct gpiod_line* receievedLine;
	struct timespec tbmax = { 0, 200 * 1000000 };
	struct gpiod_line_event event;
	int val;

	val = gpiod_line_event_wait_bulk(bulkButton, ts, eventBulkButton);
	if(val == 1)
	{
		receievedLine = gpiod_line_bulk_get_line(eventBulkButton, 0);
	}
	else if(val == 0)
	{
		printf("SESSION ENDED!\n");
		return NULL;
	}
	else
	{
		printf("Error occured!\n");
		return NULL;
	}
	
	while(val)
	{		
		int ret = gpiod_line_event_read(receievedLine, &event);
		if(ret<0)
			;//ERROR
		val = gpiod_line_event_wait(receievedLine, &tbmax);
	}

	return receievedLine;
}

int main(int argc, char **argv)
{
	char *chipname = "gpiochip1";
	unsigned int line_num20 = 20;	// GPIO Pin #20
	unsigned int line_num21 = 21;	// GPIO Pin #21
	unsigned int line_num22 = 22;	// GPIO Pin #22
	unsigned int line_num28 = 28;	// GPIO Pin #28
	unsigned int line_num29 = 29;	// GPIO Pin #29
	unsigned int line_num30 = 30;	// GPIO Pin #30
	unsigned int line_num31 = 31;	// GPIO Pin #31
	unsigned int running = 1;
	struct gpiod_chip *chip;
	struct timespec ts = { 60, 0 };
	struct gpiod_line *receivedLine, *line20, *line21, *line22, *line28, *line29, *line30, *line31;
	int i, retBulk, ret28, ret29, ret30, ret31;
	struct gpiod_line_bulk bulkButton, eventBulkButton;
	int sum = 0;

	chip = gpiod_chip_open_by_name(chipname);
	if (!chip) {
		perror("Open chip failed\n");
		goto end;
	}
	printf("debug1\n");
	line20 = gpiod_chip_get_line(chip, line_num20);
	line21 = gpiod_chip_get_line(chip, line_num21);
	line22 = gpiod_chip_get_line(chip, line_num22);
	line28 = gpiod_chip_get_line(chip, line_num28);
	line29 = gpiod_chip_get_line(chip, line_num29);
	line30 = gpiod_chip_get_line(chip, line_num30);
	line31 = gpiod_chip_get_line(chip, line_num31);
	if (!line20 || !line21 || !line22 || !line28 || !line29 || !line30 || !line31 ) {
		perror("Get line failed\n");
		goto close_chip;
	}
	printf("debug2\n");
	gpiod_line_bulk_init(&bulkButton);
	gpiod_line_bulk_add(&bulkButton, line20);
	gpiod_line_bulk_add(&bulkButton, line21);
	gpiod_line_bulk_add(&bulkButton, line22);
	retBulk = gpiod_line_request_bulk_both_edges_events(&bulkButton, CONSUMER);
	if (retBulk < 0) {
		perror("Request line as input failed\n");
		goto release_line;
	}
	printf("debug3\n");
	ret28 = gpiod_line_request_output(line28, CONSUMER, 0);
	ret29 = gpiod_line_request_output(line29, CONSUMER, 0);
	ret30 = gpiod_line_request_output(line30, CONSUMER, 0);
	ret31 = gpiod_line_request_output(line31, CONSUMER, 0);
	if (ret28 < 0 || ret29 < 0 || ret30 < 0 || ret31 < 0) {
		perror("Request line as output failed\n");
		goto release_line;
	}

	printf("CALCULATOR STARTED!\n");
	printf("0 - menu\n1 - add one\n2 - add two\n");
	while(running)
	{
		int scannedNumber;
		int scannedMenu;
		printf("Command to execute:\n");
		receivedLine = readPressedButton(&bulkButton, &ts, &eventBulkButton);
		if(receivedLine == NULL) goto release_line;
		if(receivedLine == line20) scannedNumber = 0;
		if(receivedLine == line21) scannedNumber = 1;
		if(receivedLine == line22) scannedNumber = 2;

		switch ( scannedNumber )
		{
			case 0:
				if(showResult(line28, line29, line30, line31, 9, line_num28, line_num29, line_num30, line_num31) == 1) goto release_line;
				printf("***MENU***\n");
				printf("0 - back to calculator\n1 - reset values\n2 - close calculator\n");
				printf("Command to execute:\n");
				receivedLine = readPressedButton(&bulkButton, &ts, &eventBulkButton);
				if(receivedLine == NULL) goto release_line;
				if(receivedLine == line20) scannedMenu = 0;
				if(receivedLine == line21) scannedMenu = 1;
				if(receivedLine == line22) scannedMenu = 2;
		
				if(scannedMenu == 0)
					printf("Back to calculator:\n");
				else if(scannedMenu == 1)
					sum = 0;
				else if(scannedMenu == 2)
				{
					sum = 0;
					running = !running;
				}
				break;

			case 1:
				sum += 1;
				break;

			case 2:
				sum += 2;
				break;

			default:
				printf("WRONG INPUT!!\n");
		}

		if(sum >=0 && sum <=15)
		{
			if(showResult(line28, line29, line30, line31, sum, line_num28, line_num29, line_num30, line_num31) == 1) goto release_line;
		}	
		else
		{
			if(raiseError(line28, line29, line30, line31, sum, line_num28, line_num29, line_num30, line_num31) == 1) goto release_line;
			sum = 0;
		}
		
	}

release_line:
	gpiod_line_release(line28);
	gpiod_line_release(line29);
	gpiod_line_release(line30);
	gpiod_line_release(line31);
close_chip:
	gpiod_chip_close(chip);
end:
	return 0;
}
#include <system.h>
#include <main.h>
#include <defs.h>
#include <rgbdisplay.h>
#include <font.h>

System sys;
RgbDisplay disp;

int main() {
	sys.initClock(); // Configure the system clock
	sys.initGpio();
	sys.initDma(disp.getTxBuffer(), RgbDisplay::BitsTotal);
	sys.initTimerMain();
//	sys.initUsart1();

	sys.delay(1);

	disp.setFont(font, fontSize);
	disp.clearHigh();
	disp.clearLow();


	int offset = 0;
	while (1) {
		disp.clearLow();
		disp.printLow(0, 0,  RgbDisplay::ColorGreen);
		disp.printLow(1, 6,  RgbDisplay::ColorGreen);
		disp.printLow(2, 12, RgbDisplay::ColorGreen);
		disp.printLow(1, 21, RgbDisplay::ColorRed);
		disp.printLow(5, 27, RgbDisplay::ColorRed);
		disp.renderLow();

		disp.waitIdle();

		disp.clearHigh();
		disp.printHigh(0, 0,  RgbDisplay::ColorBlue);
		disp.printHigh(1, 6,  RgbDisplay::ColorBlue);
		disp.printHigh(2, 12, RgbDisplay::ColorBlue);
		disp.printHigh(1, 21, RgbDisplay::ColorRed);
		disp.printHigh(5, 27, RgbDisplay::ColorRed);
		disp.renderHigh();

		sys.delay(1000);
		disp.waitIdle();
		LL_GPIO_TogglePin(LED_RED_Port, LED_RED_Pin);

		disp.clearLow();
		disp.printLow(5, 0,  RgbDisplay::ColorGreen);
		disp.printLow(3, 6,  RgbDisplay::ColorGreen);
		disp.printLow(3, 12, RgbDisplay::ColorGreen);
		disp.printLow(0, 21, RgbDisplay::ColorBlack);
		disp.printLow(0, 27, RgbDisplay::ColorRed);
		disp.renderLow();

		disp.waitIdle();

		disp.clearHigh();
		disp.printHigh(5, 0,  RgbDisplay::ColorBlue);
		disp.printHigh(5, 6,  RgbDisplay::ColorBlue);
		disp.printHigh(6, 12, RgbDisplay::ColorBlue);
		disp.printHigh(3, 21, RgbDisplay::ColorRed);
		disp.printHigh(0, 27, RgbDisplay::ColorRed);
		disp.renderHigh();

		sys.delay(1000);
		disp.waitIdle();
		LL_GPIO_TogglePin(LED_RED_Port, LED_RED_Pin);

		++offset;
		offset %= 8;
}
}

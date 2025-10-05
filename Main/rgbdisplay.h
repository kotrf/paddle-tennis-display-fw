#ifndef RGBDISPLAY_H_
#define RGBDISPLAY_H_

#include <stddef.h>
#include <stdint.h>

class RgbDisplay {
public:
	static constexpr uint32_t ColorWhite	= 0x202020;
	static constexpr uint32_t ColorBlack	= 0x000000;
	static constexpr uint32_t ColorBlue 	= 0x000040;
	static constexpr uint32_t ColorRed 		= 0x004000;
	static constexpr uint32_t ColorGreen	= 0x400000;

	//static constexpr uint32_t ColorWhite	= 0xFFFFFF;
	//static constexpr uint32_t ColorBlack	= 0x000000;
	//static constexpr uint32_t ColorBlue 	= 0x0000FF;
	//static constexpr uint32_t ColorRed 	= 0x00FF00;
	//static constexpr uint32_t ColorGreen = 0xFF0000;

	static constexpr size_t PixelRows = 8;
	static constexpr size_t PixelCols = 32;
	static constexpr size_t PixelNum = PixelRows * PixelCols;
	static constexpr size_t BitsForPixel = 24;
	static constexpr size_t BitsTotal = BitsForPixel * PixelNum;

	static constexpr size_t FontCharWidth = 5;
	static constexpr size_t FontCharHeight = 8;

	struct FontChar {
		uint8_t colPixels[FontCharWidth];
	};

private:
	uint32_t intervals[PixelNum * BitsForPixel];
	uint32_t pixelsHigh[PixelNum];
	uint32_t pixelsLow[PixelNum];
	const FontChar *font;
	size_t fontSize;
	double brightness;
	bool txBusy;

	bool updateIntevals(const uint32_t *pixelData, size_t len = RgbDisplay::PixelNum);
	bool print(uint32_t *pixels, char ch, size_t posX, uint32_t color);
	uint32_t coord2index(uint32_t x, uint32_t y);

public:
	RgbDisplay(const FontChar *font, size_t fontSize);
	void renderHigh();
	void renderLow();
	void putc(char c, int pos = -1);
	uint32_t *getTxBuffer() { return intervals; }
	void onTxFinish();
	void onTxError();
	void timerStartHigh();
	void timerStartLow();
	void timerStop();
	bool isBusy() { return txBusy; }
	void waitIdle();
	void setFont(const FontChar *font, size_t fontSize);
	void setBrightness(double brightness) { this->brightness = brightness; }
	void clearHigh();
	void clearLow();
	bool printHigh(char ch, size_t posX, uint32_t color);
	bool printLow(char ch, size_t posX, uint32_t color);
};

#endif // RGBDISPLAY_H_

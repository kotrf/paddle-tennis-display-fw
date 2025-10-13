#include "rgbdisplay.h"
#include "defs.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_dma.h"

RgbDisplay::RgbDisplay(const FontChar *font, size_t fontSize)
	: font(font)
	, fontSize(fontSize)
	, brightness(1.0)
	, txBusy(false)
{
	for (size_t interval = 0; interval < PixelNum * BitsForPixel; ++interval) {
		intervals[interval] = TIMER_PWM_WIDTH_0;
	}
}

bool RgbDisplay::updateIntevals(const uint32_t *pixelData, size_t len) {
	if (len > PixelNum)
		return false;

	for (size_t pixel = 0, interval = 0; pixel < len; ++pixel) {
		uint32_t mask = 0x800000;
		size_t bit = 0;
		uint32_t pixelValue = pixelData[pixel];

		for (; bit < BitsForPixel; ++bit) {
			intervals[interval++] = pixelValue & mask ? TIMER_PWM_WIDTH_1 : TIMER_PWM_WIDTH_0;
			mask >>= 1;
		}
	}

	for (size_t interval = len * BitsForPixel; interval < PixelNum * BitsForPixel; ++interval)
		intervals[interval] = TIMER_PWM_WIDTH_0;

	return true;
}

void RgbDisplay::renderHigh() {
	if (!updateIntevals(pixelsHigh))
		return;

	txBusy = true;
	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_5);
	timerStartHigh();
}

void RgbDisplay::renderLow() {
	if (!updateIntevals(pixelsLow))
		return;

	txBusy = true;
	LL_DMA_EnableStream(DMA1, LL_DMA_STREAM_4);
	timerStartLow();
}

void RgbDisplay::onTxFinish() {
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_4);
	LL_DMA_DisableStream(DMA1, LL_DMA_STREAM_5);
	txBusy = false;
}

void RgbDisplay::onTxError() {
	txBusy = false;
}

void RgbDisplay::timerStartHigh() {
	LL_TIM_SetCounter(TIMER_PWM, TIMER_PWM_PERIOD-1);
	LL_TIM_OC_SetMode(TIMER_PWM, TIMER_PWM_CHAN_HIGH, LL_TIM_OCMODE_PWM1);
	LL_TIM_EnableDMAReq_CC2(TIMER_PWM);
	LL_TIM_GenerateEvent_CC2(TIMER_PWM);	// start DMA
	LL_TIM_EnableCounter(TIMER_PWM);
}

void RgbDisplay::timerStartLow() {
	LL_TIM_SetCounter(TIMER_PWM, TIMER_PWM_PERIOD-1);
	LL_TIM_OC_SetMode(TIMER_PWM, TIMER_PWM_CHAN_LOW, LL_TIM_OCMODE_PWM1);
	LL_TIM_EnableDMAReq_CC1(TIMER_PWM);
	LL_TIM_GenerateEvent_CC1(TIMER_PWM);	// start DMA
	LL_TIM_EnableCounter(TIMER_PWM);
}

void RgbDisplay::timerStop() {
	LL_TIM_DisableCounter(TIMER_PWM);	// already disabled in IRQ Handler
	LL_TIM_DisableDMAReq_CC1(TIMER_PWM);
	LL_TIM_DisableDMAReq_CC2(TIMER_PWM);
	LL_TIM_ClearFlag_CC1(TIMER_PWM);
	LL_TIM_ClearFlag_CC2(TIMER_PWM);
}

uint32_t RgbDisplay::coord2index(uint32_t x, uint32_t y) {
	if (x & 1)
		y = 7-y;
	return x*8 + y;
}

void RgbDisplay::putc(char c, int pos) {
}

void RgbDisplay::waitIdle() {
	// TODO: add timeout
	while (isBusy())
	{}
}

void RgbDisplay::setFont(const FontChar *font, size_t fontSize) {
	this->font = font;
	this->fontSize = fontSize;
}

void RgbDisplay::clearHigh() {
	for ( size_t x=0; x<PixelCols; ++x )
		for ( size_t y=0; y<PixelRows; ++y )
			pixelsHigh[coord2index(x, y)] = ColorBlack;
}

void RgbDisplay::clearLow() {
	for ( size_t x=0; x<PixelCols; ++x )
		for ( size_t y=0; y<PixelRows; ++y )
			pixelsLow[coord2index(x, y)] = ColorBlack;
}

bool RgbDisplay::print(uint32_t *pixels, char ch, size_t posX, uint32_t color) {
	if (!font)
		return false;

	double colorRed   = (color >> 8) & 0xFF;
	double colorGreen = (color >> 16) & 0xFF;
	double colorBlue  = color & 0xFF;

	colorRed *= brightness;
	colorGreen *= brightness;
	colorBlue *= brightness;

	color = (uint32_t(colorGreen + 0.5) << 16) | (uint32_t(colorRed + 0.5) << 8) | uint32_t(colorBlue + 0.5);

	size_t chIndex = uint8_t(ch);
	if (chIndex >= fontSize)
		return false;

	for (size_t x=0; x<FontCharWidth; ++x) {
		uint8_t charCol = font[chIndex].colPixels[x];
		uint8_t maskCol = 1;
		for (size_t y=0; y<FontCharHeight; ++y) {
			pixels[coord2index(x + posX, y)] = (charCol & maskCol) ? color : ColorBlack;
			maskCol <<= 1;
		}
	}

	return true;
}

bool RgbDisplay::printHigh(char ch, size_t posX, uint32_t color) {
	return print(pixelsHigh, ch, posX, color);
}

bool RgbDisplay::printLow(char ch, size_t posX, uint32_t color) {
	return print(pixelsLow, ch, posX, color);
}

void RgbDisplay::fillPattern(unsigned int offset) {
	uint32_t color = ColorWhite;

	double colorRed   = (color >> 8) & 0xFF;
	double colorGreen = (color >> 16) & 0xFF;
	double colorBlue  = color & 0xFF;

	colorRed *= brightness;
	colorGreen *= brightness;
	colorBlue *= brightness;

	color = (uint32_t(colorGreen + 0.5) << 16) | (uint32_t(colorRed + 0.5) << 8) | uint32_t(colorBlue + 0.5);

	for ( size_t x=0; x<PixelCols; ++x ) {
//		rgbPixels[disp.coord2index(x, (0 + offset) % 8)] = RgbColorWhite;
//		rgbPixels[disp.coord2index(x, (1 + offset) % 8)] = RgbColorWhite;
//		rgbPixels[disp.coord2index(x, (2 + offset) % 8)] = RgbColorBlue;
//		rgbPixels[disp.coord2index(x, (3 + offset) % 8)] = RgbColorBlue;
//		rgbPixels[disp.coord2index(x, (4 + offset) % 8)] = RgbColorRed;
//		rgbPixels[disp.coord2index(x, (5 + offset) % 8)] = RgbColorRed;
//		rgbPixels[disp.coord2index(x, (6 + offset) % 8)] = RgbColorGreen;
//		rgbPixels[disp.coord2index(x, (7 + offset) % 8)] = RgbColorGreen;
		for ( size_t y=0; y<PixelRows; ++y )
			pixelsLow[coord2index(x, (y + offset) % 8)] = (x & 1) ? color : ColorBlack;
	}
}

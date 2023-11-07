#include <naiveConsole.h>

static char buffer[64] = {'0'};
static uint8_t *const video = (uint8_t *)0xB8000;
static uint8_t *currentVideo = (uint8_t *)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25;
static uint8_t defaultFgColor = 0xFF;
static uint8_t defaultBgColor = 0x00;

void ncPrintColor(const char *string, const uint8_t fgColor, const uint8_t bgColor) {
    int i;

    for (i = 0; string[i] != 0; i++) {
        if (string[i] == '\n')
            ncNewline();
        else
            ncPrintCharColor(string[i], fgColor, bgColor);
    }
}

void ncBackspace() {
    if (currentVideo >= video + 0x2) {
        currentVideo -= 2;
        *currentVideo = ' ';
    }
}

void ncPrint(const char *string) {
    ncPrintColor(string, defaultFgColor, defaultBgColor);
}

void setBgColor(uint8_t newBgColor) {
    defaultBgColor = newBgColor;
}

void setFgColor(uint8_t newFgColor) {
    defaultFgColor = newFgColor;
}

void ncPrintCharColor(char character, uint8_t fgColor, uint8_t bgColor) {
    *currentVideo = character;
    currentVideo++;
    *currentVideo = (bgColor & 0xF0) | (fgColor & 0x0F);
    currentVideo++;
}

void ncPrintChar(char character) {
    ncPrintCharColor(character, defaultFgColor, defaultBgColor);
}

void ncNewline() {
    do {
        ncPrintChar(' ');
    } while ((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void ncPrintDec(uint64_t value) {
    ncPrintBase(value, 10);
}

void ncPrintHex(uint64_t value) {
    ncPrintBase(value, 16);
}

void ncPrintBin(uint64_t value) {
    ncPrintBase(value, 2);
}

void ncPrintBase(uint64_t value, uint32_t base) {
    uintToBase(value, buffer, base);
    ncPrint(buffer);
}

void ncClear() {
    int i;

    for (i = 0; i < height * width; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = defaultBgColor;
    }
    currentVideo = video;
}

uint32_t uintToBase(uint64_t value, char *buffer, uint32_t base) {
    char *p = buffer;
    char *p1, *p2;
    uint32_t digits = 0;

    // Calculate characters for each digit
    do {
        uint32_t remainder = value % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
        digits++;
    } while (value /= base);

    // Terminate string in buffer.
    *p = 0;

    // Reverse string in buffer.
    p1 = buffer;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }

    return digits;
}

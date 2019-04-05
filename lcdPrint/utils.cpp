#include "utils.h"

void lcdReset (LiquidCrystal * lcd) {
    lcd->clear();
    lcd->home();
}

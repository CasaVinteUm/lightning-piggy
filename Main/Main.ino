// Code for the Lightning Piggy running on the TTGO LilyGo 2.13 and 2.66 inch ePaper hardware.
//
// Tested with:
// - Arduino IDE version 1.8.13
// - ESP32 Board Support version 2.0.14
// - Preferences -> Compiler warnings: Default
// - Tools -> Board -> ESP32 Arduino -> ESP32 Dev Module
// - Tools -> Upload Speed: 921600
// - Tools -> CPU Frequency: 240Mhz
// - Tools -> Flash Frequency: 80Mhz
// - Tools -> Flash Mode: QIO
// - Tools -> Flash Size: 4MB (32Mb)
// - Tools -> Partition Scheme: Default 4MB with spiffs (1.2MB APP, 1.5MB SPIFFS)
// - Tools -> Core Debug Level: Warn
// - Tools -> PSRAM: Disabled
// - Tools -> Port: /dev/ttyACM0
//
// See README.md for more tips and tricks.

#include <ArduinoJson.h>
#include <WebSocketsClient.h> // Needs to be here, otherwise compilation error...

#include "logos.h"
#include "config.h"
#include "Constants.h"

#define roundEight(x) (((x) + 8 - 1) & -8) // round up to multiple of 8

long lastUpdatedBalance = -UPDATE_BALANCE_PERIOD_MILLIS;  // this makes it update when first run
int lastBalance = -NOT_SPECIFIED;
bool alreadyTriedFindingWalletIDinLNURLpList = false;
int xBeforeLNURLp;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Lightning Piggy " + getFullVersion());

    // turn on the green LED-IO12 on the LilyGo 2.66 inch, to show the board is on
    // it will turn off when the board hibernates
    pinMode(12, OUTPUT);
    digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)

    print_reset_reasons();
    print_wakeup_reason();

    setup_display();

    displayVoltageWarning();
    showBootSlogan();
    showLogo(epd_bitmap_Lightning_Piggy, 104, 250, displayHeight() - 104, (displayWidth() - 250) / 2); // width and height are swapped because display rotation

    setup_watchdog();

    displayWifiConnecting();
    #ifndef DEBUG
    connectWifi();
    short_watchdog_timeout(); // after the long wifi connection stage, the next operations shouldn't take long
    displayWifiStrengthBottom();
    #endif
    displayFetching();

    xBeforeLNURLp = showLNURLpQR(getLNURLp());
    xBeforeLNURLp = displayWidth()-roundEight(displayWidth()-xBeforeLNURLp);

    watchdogWasntTriggered();
}

void loop() {
  // If there is no balance OR it has been a long time since it was refreshed, then refresh it
  if (lastBalance == -NOT_SPECIFIED || (millis() - lastUpdatedBalance) > UPDATE_BALANCE_PERIOD_MILLIS) {
    lastUpdatedBalance = millis();
    disconnectWebsocket();
    displayStatus(xBeforeLNURLp, false);  // takes ~2000ms, which is too much to do with the websocket
    displayBalanceAndPayments(xBeforeLNURLp);
    checkShowUpdateAvailable();
    connectWebsocket();
  }

  // The wallet ID (needed for the websocket) is configured statically or is found in the incoming payments.
  // But if not, because there were no incoming payments, then fetch it from lnurlp/list.
  // Only attempt to do this once, to avoid becoming very slow here
  if (getWalletID().length() == 0 && !alreadyTriedFindingWalletIDinLNURLpList) {
    Serial.println("No wallet ID was configured or found in incoming payments, fetching it from the LNURLp list...");
    alreadyTriedFindingWalletIDinLNURLpList = true;
    getLNURLp(true);
    connectWebsocket(); // slow operation so make sure websocket is connected
  }
  if (getWalletID().length() > 0) websocket_loop();

  feed_watchdog(); // Feed the watchdog regularly, otherwise it will "bark" (= reboot the device)
  if (!hibernateDependingOnBattery()) delay(200);
}

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

    displayStatus(xBeforeLNURLp, false);
    displayBalanceAndPaymentsPeriodically(xBeforeLNURLp);

    if (wifiConnected()) checkShowUpdateAvailable();

    watchdogWasntTriggered();
    hibernateDependingOnBattery();

    // The wallet ID is configured statically or is found in the incoming payments.
    // But if not, because there were no incoming payments, then fetch it from lnurlp/list.
    if (getWalletID().length() == 0) {
      Serial.println("No wallet ID was configured or found in incoming payments, fetching it from the LNURLp list...");
      getLNURLp(true);
    }

    // Connect the websocket if we have a walletID
    if (getWalletID().length() == 0) {
      Serial.println("No wallet ID was configured or found in incoming payments or found in the LNURLp list, this should not happen.");
    } else {
      connectWebsocket();
    }
}

void loop() {
  if (getWalletID().length() > 0) {
    websocket_loop();
    checkShowUpdateAvailable();
  } else {
    // This fallback behavior should never happen because wallet ID will always be found in the LNURLp list.
    displayStatus(xBeforeLNURLp, false);  // takes ~2000ms, which is too much to do with the websocket
    displayBalanceAndPaymentsPeriodically(xBeforeLNURLp);
    // if some time has passed, then check balance and if it changed, then update payments
  }

  feed_watchdog(); // Feed the watchdog regularly, otherwise it will "bark" (= reboot the device)
  if (!hibernateDependingOnBattery()) delay(200);
}

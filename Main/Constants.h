#ifndef CONSTANTS_H
#define CONSTANTS_H

extern const int NOT_SPECIFIED = -1; 

const char * NOTCONFIGURED = "REPLACETHISBY";
const unsigned int NOTCONFIGURED_LENGTH = 13;

// Maximum time to show the bootslogan
extern const int MAX_BOOTSLOGAN_SECONDS = 15;

#define MAX_PAYMENTS 6 // even the 2.66 inch display can only fit 6

extern const int MAX_WATCHDOG_REBOOTS = 3;
extern const int SLEEP_HOURS_AFTER_MAX_WATCHDOG_REBOOTS = 6;

#define AWAKE_SECONDS_AFTER_MANUAL_WAKEUP 3*60

#define CHECK_UPDATE_PERIOD_SECONDS 7*24*60*60 // every week

#define UPDATE_BALANCE_PERIOD_MILLIS 1000 * 60 * 15 // fallback to updating balance every 10 minutes if the instant websocket method is unavailable

#define UPDATE_VOLTAGE_PERIOD_MILLIS 1000 * 60 * 5 // update voltage display every 5 minutes

#define HIBERNATE_CHECK_PERIOD_MILLIS 1000 * 30 // hibernate check every 30 seconds

// In alphabetical order
const char * deWeekdays[] = { "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};
const char * dkWeekdays[] = { "Sø", "Ma", "Ti", "On", "To", "Fr", "Lø"};
const char * esWeekdays[] = { "Do", "Lu", "Ma", "Mi", "Ju", "Vi", "Sá"};
const char * enWeekdays[] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
const char * nlWeekdays[] = { "Zo", "Ma", "Di", "Wo", "Do", "Vr", "Za"};

String currentVersion = "4.2.1";

typedef enum {
    STR2INT_SUCCESS,
    STR2INT_OVERFLOW,
    STR2INT_UNDERFLOW,
    STR2INT_INCONVERTIBLE
} str2int_errno;

String websocketApiUrl = "/api/v1/ws/";

#define DISPLAY_TYPE_213DEPG 1
#define DISPLAY_TYPE_266DEPG 2

#endif // #ifndef CONSTANTS_H

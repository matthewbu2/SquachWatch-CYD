// SquachWatch-CYD -- Serial.flush() that cannot hang the board.
//
// On the ESP32-S3 boards (SQW_S3) the console is the chip's native USB. Its flush()
// spins until the host has read every byte, and with nothing listening on
// the port that is never: the first-boot calibration filled the buffer and
// the board sat in setup() on a black screen for as long as the port stayed
// closed (2026-09-22). Flush only when a host is actually there.
#pragma once
#include <Arduino.h>
#if defined(WAVESHARE_206)
// `Serial` alone is not enough here: after the self-heal's deep sleep the
// core's "a host has read from us" flag comes up set with no cable in, and
// the first flush then spun until a cable went in -- a dark, dead watch on
// the wrist (2026-10-22). A host sends a start-of-frame every millisecond,
// so the frame number moving is the proof one is there.
#include <soc/usb_serial_jtag_struct.h>
static inline bool serialHostThere() {
    const uint32_t f = USB_SERIAL_JTAG.fram_num.sof_frame_index;
    delayMicroseconds(2500);
    return USB_SERIAL_JTAG.fram_num.sof_frame_index != f;
}
#endif
static inline void serialFlush() {
#if defined(WAVESHARE_206)
    if (Serial && serialHostThere()) Serial.flush();
#elif defined(SQW_S3)
    if (Serial) Serial.flush();
#else
    Serial.flush();
#endif
}

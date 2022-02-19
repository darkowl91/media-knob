#include "TrinketHidCombo.h"

#define PIN_ENCODER_A 0
#define PIN_ENCODER_B 2
#define TRINKET_PINx PINB
#define PIN_ENCODER_SWITCH 1

static uint8_t enc_prev_pos = 0;
static uint8_t enc_flags = 0;
static char sw_was_pressed = 0;

void setup()
{
  pinMode(PIN_ENCODER_A, INPUT);
  pinMode(PIN_ENCODER_B, INPUT);
  digitalWrite(PIN_ENCODER_A, HIGH);
  digitalWrite(PIN_ENCODER_B, HIGH);

  pinMode(PIN_ENCODER_SWITCH, INPUT);
  digitalWrite(PIN_ENCODER_SWITCH, LOW);

  TrinketHidCombo.begin();

  if (digitalRead(PIN_ENCODER_A) == LOW)
  {
    enc_prev_pos |= (1 << 0);
  }
  if (digitalRead(PIN_ENCODER_B) == LOW)
  {
    enc_prev_pos |= (1 << 1);
  }
}

void loop()
{
  int8_t enc_action = 0;
  uint8_t enc_cur_pos = 0;

  if (bit_is_clear(TRINKET_PINx, PIN_ENCODER_A))
  {
    enc_cur_pos |= (1 << 0);
  }
  if (bit_is_clear(TRINKET_PINx, PIN_ENCODER_B))
  {
    enc_cur_pos |= (1 << 1);
  }
  if (enc_cur_pos != enc_prev_pos)
  {
    if (enc_prev_pos == 0x00)
    {
      if (enc_cur_pos == 0x01)
      {
        enc_flags |= (1 << 0);
      }
      else if (enc_cur_pos == 0x02)
      {
        enc_flags |= (1 << 1);
      }
    }

    if (enc_cur_pos == 0x03)
    {
      enc_flags |= (1 << 4);
    }
    else if (enc_cur_pos == 0x00)
    {
      if (enc_prev_pos == 0x02)
      {
        enc_flags |= (1 << 2);
      }
      else if (enc_prev_pos == 0x01)
      {
        enc_flags |= (1 << 3);
      }

      if (bit_is_set(enc_flags, 0) && (bit_is_set(enc_flags, 2) || bit_is_set(enc_flags, 4)))
      {
        enc_action = 1;
      }
      else if (bit_is_set(enc_flags, 2) && (bit_is_set(enc_flags, 0) || bit_is_set(enc_flags, 4)))
      {
        enc_action = 1;
      }
      else if (bit_is_set(enc_flags, 1) && (bit_is_set(enc_flags, 3) || bit_is_set(enc_flags, 4)))
      {
        enc_action = -1;
      }
      else if (bit_is_set(enc_flags, 3) && (bit_is_set(enc_flags, 1) || bit_is_set(enc_flags, 4)))
      {
        enc_action = -1;
      }

      enc_flags = 0;
    }
  }

  enc_prev_pos = enc_cur_pos;

  if (enc_action > 0)
  {
    TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_UP);
  }
  else if (enc_action < 0)
  {
    TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_DOWN);
  }

  // remember that the switch is active-high
  if (bit_is_set(TRINKET_PINx, PIN_ENCODER_SWITCH))
  {
    if (sw_was_pressed == 0) // only on initial press, so the keystroke is not repeated while the button is held down
    {
      TrinketHidCombo.pressMultimediaKey(MMKEY_MUTE);
      delay(5); // debounce delay
    }
    sw_was_pressed = 1;
  }
  else
  {
    if (sw_was_pressed != 0)
    {
      delay(5); // debounce delay
    }
    sw_was_pressed = 0;
  }

  TrinketHidCombo.poll(); // check if USB needs anything done
}

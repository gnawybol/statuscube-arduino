#include <Adafruit_NeoPixel.h>
#include <stdint.h>


#define PIXELS_PIN  2
#define PIXELS_NUM  5


enum state_enum {
  WORKING,
  FAULT,
  SUCCESS,
  ERROR,
  NONE,
};

struct build_state {
  enum state_enum state;
  uint8_t step;
};


struct build_state build_states[PIXELS_NUM];
//const int levels[] = {0, 16, 32, 48, 64, 80, 96, 112, 128, 143, 159, 175, 191, 207, 223, 239, 255}; // linear
//const int levels[] = {0, 25, 50, 74, 98, 120, 142, 162, 180, 197, 212, 225, 236, 244, 250, 254, 255}; // sine
const int levels[] = {0, 13, 25, 37, 50, 62, 74, 86, 98, 109, 120, 131, 142, 152, 162, 171, 180, 189, 197, 205, 212, 219, 225, 231, 236, 240, 244, 247, 250, 252, 254, 255}; // sine
const uint8_t num_levels = sizeof(levels) / sizeof(int);
const uint8_t num_steps = 2 * (num_levels - 1);


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(PIXELS_NUM, PIXELS_PIN, NEO_GRB | NEO_KHZ800);


int get_level_for_step(uint8_t step)
{
  if (step < num_levels) {
    return levels[step];
  } else {
    step -= num_levels - 1;
    // Bother securing the value?
    return levels[num_levels - step];
  }
}


uint8_t get_next_step(uint8_t step)
{
  return (step + 1) % num_steps;
}


uint32_t get_color_of_state(struct build_state *state)
{
  uint8_t step;
  int level;

  switch (state->state) {
    case WORKING:
      step = state->step;
      level = get_level_for_step(step);
      state->step = get_next_step(step);
      return Adafruit_NeoPixel::Color(level, level, level);

    case FAULT:
      return Adafruit_NeoPixel::Color(0xff, 0, 0);

    case SUCCESS:
      return Adafruit_NeoPixel::Color(0, 0xff, 0);

    case ERROR:
      return Adafruit_NeoPixel::Color(0xb4, 0xb4, 0);

    case NONE:
    default:
      return Adafruit_NeoPixel::Color(0, 0, 0);
  }

}

void setup()
{
  Serial.begin(115200);
  Serial.write("statuscube");
  pixels.begin();
  pixels.clear();
  pixels.show();

  for (uint8_t i = 0; i < PIXELS_NUM; i++) {
    build_states[i].state = NONE;
    build_states[i].step = 0;
  }
}

void loop()
{
  for (uint8_t pixel = 0; pixel < PIXELS_NUM; pixel++) {
    struct build_state *state = build_states + pixel;
    uint32_t color = get_color_of_state(state);
    pixels.setPixelColor(pixel, color);
  }
  pixels.show();
  handle_serial();
  delay(35);
}

void handle_serial() {
  uint8_t pixel;
  struct build_state *state;

  while (Serial.available() >= 2) {
    char pixel_char = (char)Serial.read();
    if ((pixel_char < '0') || (pixel_char > ('0' + PIXELS_NUM - 1))) {
      continue;
    }
    pixel = pixel_char - '0';
    state = build_states + pixel;

    switch ((char)Serial.read()) {
      case 'w':
        state->state = WORKING;
        break;

      case 'f':
        state->state = FAULT;
        break;

      case 's':
        state->state = SUCCESS;
        break;

      case 'e':
        state->state = ERROR;
        break;

      case 'n':
        state->state = NONE;
        break;

      default:
        continue;
    }
    state->step = 0;
  }
}

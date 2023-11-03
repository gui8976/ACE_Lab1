
#include <Arduino.h>
#include <Bounce2.h>
#include <NeoPixelConnect.h>
#include <elapsedMillis.h>

// define constants here:
#define Sgo 2   // GPI2 , BOTÃO 1
#define Sup 3   // GPI3 , BOTÃO 2
#define Sdown 4 // GPI4, BOTÃO 3
#define INTERVAL 2000
#define TIMER 10000
#define EFFECT_TIME 30000
#define MAXIMUM_NUM_NEOPIXELS 5

#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3
#define LED5 4

NeoPixelConnect strip(6, MAXIMUM_NUM_NEOPIXELS, pio0, 0);

Bounce s_go = Bounce();
Bounce s_up = Bounce();
Bounce s_down = Bounce();

unsigned long interval = 2000; // interval at which to blink (milliseconds)
unsigned long time_waited = 0;
unsigned long tempo_effect = 0;
unsigned long leds = 0;
const unsigned long fast_interval = 100;

int R = 127;
int G = 0;
int B = 255;

int pressed = 0;
int offfade = 0;
int flag = 0;

// define the structures here:

// sturture for the configuration machine
typedef enum config_machine
{
  OFF,
  C1,
  C2,
  C3,
} config_machine;

// structure for the time machine
typedef enum time_machine
{
  IDLE,
  RUN,
  BLINKING,
  PAUSE,
  CONFIG,
  EFFECT,
} time_machine;

typedef enum
{
  S1,
  S2,
  S5,
  S10,
} config1;

typedef enum
{
  Default_timer,
  HalfBlink,
  Fade,
} config2;

typedef enum
{
  VIOLETA,
  AZUL,
  CIANO,
  VERDE,
  AMARELO,
  LARANJA,
  BRANCO,
} color_machine;

typedef enum
{
  ALL,
  LEDS4,
  LEDS3,
  LEDS2,
  LEDS1,
  BLINK,
  PAUSED,
} led_machine;

// initiate the values of the structures
config_machine config = OFF;

time_machine tempo = IDLE;
time_machine tempo_previous = IDLE;

color_machine cor = VIOLETA;

config2 config2_mode = Default_timer;
config1 config1_mode = S2;

led_machine led_mode = ALL;
led_machine led_mode_previous = ALL;

// creates a function that blink the leds
void blink(uint16_t led_to_blink)
{
  static bool led_state = false;
  static unsigned long previousMillis = 0;

  if (millis() - previousMillis >= fast_interval)
  {
    led_state = !led_state;
    previousMillis = millis();

    if (led_state)
    {
      strip.neoPixelSetValue(led_to_blink, R, G, B, 1); // por definir as cores
    }
    else
    {
      strip.neoPixelSetValue(led_to_blink, 0, 0, 0, 1);
    }
  }
}

void blink_multiple(uint16_t leds_to_blink)
{
  static bool led_state = false;
  static unsigned long previousMillis = 0;

  if (millis() - previousMillis >= fast_interval)
  {
    led_state = !led_state;
    previousMillis = millis();

    if (led_state)
    {
      for (int i = 0; i < leds_to_blink; i++)
      {
        strip.neoPixelSetValue(i, R, G, B, 1); // por definir as cores
        delay(10);
      }
    }
    else
    {
      for (int i = 0; i < leds_to_blink; i++)
      {
        strip.neoPixelSetValue(i, 0, 0, 0, 1);
        delay(10);
      }
    }
  }
}

void blink_with_interval(uint16_t led_to_blink)
{
  static bool led_state = false;
  static long previousMillis = 0;

  if (millis() - previousMillis >= interval)
  {
    led_state = !led_state;
    previousMillis = millis();

    if (led_state)
    {
      strip.neoPixelSetValue(led_to_blink, R, G, B, 1); // por definir as cores
    }
    else
    {
      strip.neoPixelSetValue(led_to_blink, 0, 0, 0, 1);
    }
  }
}

void fast_blink_with_interval(uint16_t led_to_blink)
{
  static bool led_state = false;
  static long previousMillis = 0;

  if (millis() - previousMillis >= 150)
  {
    led_state = !led_state;
    previousMillis = millis();

    if (led_state)
    {
      strip.neoPixelSetValue(led_to_blink, R, G, B, 1); // por definir as cores
    }
    else
    {
      strip.neoPixelSetValue(led_to_blink, 0, 0, 0, 1);
    }
  }
}

void blink_all()
{
  static bool led_state = false;
  static unsigned long previousMillis = 0;
  if (millis() - previousMillis >= fast_interval)
  {
    led_state = !led_state;
    previousMillis = millis();

    if (led_state)
    {
      strip.neoPixelFill(0, R, G, B); // por definir as cores
    }
    else
    {
      strip.neoPixelClear(1);
    }
  }
}

void led_show(int leds_to_light_up)
{
  // sees the number of leds that need to be lit up
  for (int leds_to_light = leds_to_light_up; leds_to_light > -1; leds_to_light--)
  {
    strip.neoPixelSetValue(leds_to_light, R, G, B, 1);
  }
}

int led_timer()
{ // não pode ser static porque senão é sempre verdade, se blinking e clicar em go aquilo só liga led 4
  static unsigned long actual_time = millis();
  if (millis() - actual_time >= interval)
  {
    actual_time = millis();
    strip.neoPixelClear(1);
    return 1;
  }
  else
    return 0;
}

void Half_blink(u_int8_t led_to_blink)
{
  static unsigned long previousMillis = 0;

  // sets the LED5 as active until half of the interval
  if (millis() - previousMillis < interval / 2)
  {
    strip.neoPixelSetValue(led_to_blink, R, G, B, 1);
  }
  if (millis() - previousMillis >= interval / 2)
  {
    Serial.println("blink");
    fast_blink_with_interval(led_to_blink);
  }
  if (millis() - previousMillis >= interval)
  {
    previousMillis = millis();
  }
}

void fade(uint8_t led_to_blink)
{
  static unsigned long previousMillis = 0;
  offfade = 0;
  Serial.println(offfade);
  if (millis() - previousMillis <= interval)
  {

    for (int elapsed = interval; elapsed >= 0; elapsed -= 10)
    {

      u_int8_t r = map(elapsed, 0, interval, 0, R);
      u_int8_t g = map(elapsed, 0, interval, 0, G);
      u_int8_t b = map(elapsed, 0, interval, 0, B);
      strip.neoPixelSetValue(led_to_blink, r, g, b, 1);
      Serial.println(r);
      Serial.println(g);
      Serial.println(b);
      if (s_down.fell())
      {
        offfade = 1;
        Serial.println(offfade);
      }
      delay(10);
    }
  }
  if (millis() - previousMillis >= interval)
  {
    previousMillis = millis();
  }
}

void button_manager()
{
  static unsigned long tempo_esperado;
  if (s_up.fell()) // clicar
  {
    Serial.println("Pressionou");
    flag = 1;
    tempo_esperado = millis();
  }
  if (flag)
  {

    Serial.println(tempo_esperado);
    Serial.println(millis());
    if (millis() - tempo_esperado >= 3000) // 3 segundos ou mais
    {
      flag = 0;
      pressed = 1;
    }
    if (s_up.read()) // menos de 3 segundos -> aumentar um intervalo
    {
      flag = 0;
      pressed = 2;
    }
  }
}

void hsvToRgb(int hue, int sat, uint8_t val, uint8_t &r, uint8_t &g, uint8_t &b)
{
  uint8_t region = hue / 43;
  uint8_t remainder = (hue - (region * 43)) * 6;

  uint8_t p = (val * (255 - sat)) >> 8;
  uint8_t q = (val * (255 - ((sat * remainder) >> 8))) >> 8;
  uint8_t t = (val * (255 - ((sat * (255 - remainder)) >> 8))) >> 8;

  switch (region)
  {
  case 0:
    r = val;
    g = t;
    b = p;
    break;
  case 1:
    r = q;
    g = val;
    b = p;
    break;
  case 2:
    r = p;
    g = val;
    b = t;
    break;
  case 3:
    r = p;
    g = q;
    b = val;
    break;
  case 4:
    r = t;
    g = p;
    b = val;
    break;
  default:
    r = val;
    g = p;
    b = q;
    break;
  }
}

void effect_mode()
{
  for (int j = 0; j < 256; j++)
  {
    for (int i = 0; i < 5; i++)
    {
      int hue = (i * 256 / 5 + j) & 0xFF;
      uint8_t r, g, b;
      hsvToRgb(hue, 255, 255, r, g, b);
      strip.neoPixelSetValue(i, r, g, b, 1);
    }
    delay(2);
  }
}

void config_machine_ME()
{
  switch (config)
  {
  case OFF:
    Serial.println("OFF");
    if (tempo == CONFIG)
      config = C1;
    break;
  case C1:
    Serial.println("C1");
    blink(LED1);
    if (s_up.rose())
    {
      config = C2;
      strip.neoPixelClear(1);
    }
    if (tempo != CONFIG)
    {
      config = OFF;
      strip.neoPixelClear(1);
    }
    break;
  case C2:
    Serial.println("C2");
    blink(LED2);
    if (s_up.rose())
    {
      config = C3;
      strip.neoPixelClear(1);
    }
    if (tempo != CONFIG)
    {
      config = OFF;
      strip.neoPixelClear(1);
    }
    break;
  case C3:
    Serial.println("C3");
    blink(LED3);
    if (s_up.rose())
    {
      config = C1;
      strip.neoPixelClear(1);
    }
    if (tempo != CONFIG)
    {
      config = OFF;
      strip.neoPixelClear(1);
    }
    break;
  }
}

void configuration_1_ME()
{
  if (config == C1)
  {
    switch (config1_mode)
    {
    case S1:
      Serial.println("S1");
      blink_with_interval(LED5);
      if (s_down.rose())
      {
        config1_mode = S2;
        interval = 2000;
      }
      break;
    case S2:
      Serial.println("S2");
      blink_with_interval(LED5);
      if (s_down.rose())
      {
        config1_mode = S5;
        interval = 5000;
      }
      break;

    case S5:
      Serial.println("S5");
      blink_with_interval(LED5);
      if (s_down.rose())
      {
        config1_mode = S10;
        interval = 10000;
      }
      break;

    case S10:
      Serial.println("S10");
      blink_with_interval(LED5);
      if (s_down.rose())
      {
        config1_mode = S1;
        interval = 1000;
      }
      break;

    default:
      break;
    }
  }
}

void configuration_2_ME()
{
  if (config == C2)
  {
    switch (config2_mode)
    {
    case Default_timer:
      blink_with_interval(LED5);
      if (s_down.rose())
      {
        config2_mode = HalfBlink;
      }
      break;
    case HalfBlink:
      Half_blink(LED5);
      if (s_down.rose())
        config2_mode = Fade;
      break;
    case Fade:
      if (s_down.rose() || offfade)
        config2_mode = Default_timer;
      fade(LED5);
      break;

    default:
      break;
    }
  }
}

void configuration_3_ME()
{
  if (config == C3)
  {
    switch (cor)
    {
    case VIOLETA:
      strip.neoPixelSetValue(LED5, R, G, B, 1);
      Serial.println("VIOLETA");
      if (s_down.rose())
      {
        cor = AZUL;
        R = 0;
        G = 0;
        B = 255;
      }

      break;
    case AZUL:
      strip.neoPixelSetValue(LED5, R, G, B, 1);
      Serial.println("AZUL");
      if (s_down.rose())
      {
        cor = CIANO;
        R = 0;
        G = 255;
        B = 255;
      }
      break;
    case CIANO:
      strip.neoPixelSetValue(LED5, R, G, B, 1);
      Serial.println("CIANO");
      if (s_down.rose())
      {
        cor = VERDE;
        R = 0;
        G = 255;
        B = 0;
      }
      break;
    case VERDE:
      strip.neoPixelSetValue(LED5, R, G, B, 1);
      Serial.println("VERDE");
      if (s_down.rose())
      {
        cor = AMARELO;
        R = 255;
        G = 255;
        B = 0;
      }
      break;
    case AMARELO:
      strip.neoPixelSetValue(LED5, R, G, B, 1);
      Serial.println("AMARELO");
      if (s_down.rose())
      {
        cor = LARANJA;
        R = 255;
        G = 167;
        B = 0;
      }
      break;
    case LARANJA:
      strip.neoPixelSetValue(LED5, R, G, B, 1);
      Serial.println("LARANJA");
      if (s_down.rose())
      {
        cor = BRANCO;
        R = 255;
        G = 255;
        B = 255;
      }
      break;
    case BRANCO:
      strip.neoPixelSetValue(LED5, R, G, B, 1);
      Serial.println("BRANCO");
      if (s_down.rose())
      {
        cor = VIOLETA;
        R = 127;
        G = 0;
        B = 255;
      }
      break;
    default:
      break;
    }
  }
}

void time_configuration_ME()
{
  switch (tempo)
  {
  case IDLE:
    if (s_go.rose())
      tempo = RUN;

    // o botão foi pressionado durante 3 segundos
    if (pressed == 1)
    {
      tempo = CONFIG;
      tempo_previous = IDLE;
      pressed = 0;
    }
    break;

  case RUN:
    Serial.println("RUN");
    if (led_mode == BLINK)
    {
      tempo = BLINKING;
      tempo_effect = millis();
    }
    if (s_down.rose())
      tempo = PAUSE;

    // o botão foi pressionado durante 3 segundos
    if (pressed == 1)
    {
      tempo = CONFIG;
      tempo_previous = RUN;
      pressed = 0;
    }
    break;
  case CONFIG:
    Serial.println("CONFIG");
    // o botão foi pressionado durante 3 segundos
    if (pressed == 1)
    {
      tempo = tempo_previous;
      pressed = 0;
    }
    break;
  case BLINKING:
    Serial.println("BLINKING");
    if (millis() - tempo_effect >= EFFECT_TIME)
    {
      tempo = EFFECT;
    }
    if (led_mode != BLINK)
      tempo = RUN;

    blink_all();
    break;

  case PAUSE:
    Serial.println("PAUSE");
    if (s_down.rose())
      tempo = RUN;
    // o botão foi pressionado durante 3 segundos
    if (pressed == 1)
    {
      tempo = CONFIG;
      tempo_previous = PAUSE;
      pressed = 0;
    }

    break;
  case EFFECT:
    Serial.println("EFFECT");
    effect_mode();
    if (s_go.rose())
      tempo = RUN;
    break;
  default:
    break;
  }
}

void led_configuration_ME()
{
  if (tempo == RUN || tempo == PAUSE || tempo == BLINKING || tempo == EFFECT)
  {
    switch (led_mode)
    {
    case ALL:
      leds = 5;
      if (config2_mode == HalfBlink)
      {
        Half_blink(LED5);
        led_show(3);
      }
      else if (config2_mode == Fade)
      {
        led_show(3);
        fade(LED5);
      }
      else
        led_show(4);

      if (s_go.rose())
      {
        led_mode = ALL;
        led_mode_previous = ALL;
      }

      if (s_up.rose())
        led_mode = ALL;
      if (led_timer())
        led_mode = LEDS4;

      break;
    case LEDS4:
      leds = 4;
      if (config2_mode == HalfBlink)
      {
        led_show(2);
        Half_blink(LED4);
      }
      else if (config2_mode == Fade)
      {
        led_show(2);
        fade(LED4);
      }
      else
        led_show(3);

      if (tempo == PAUSE)
      {
        led_mode = PAUSED;
        led_mode_previous = LEDS4;
      }

      if (s_go.rose())
      {
        led_mode = ALL;
        led_mode_previous = ALL;
      }

      if (s_up.rose())
        led_mode = ALL;

      if (led_timer())
        led_mode = LEDS3;
      break;
    case LEDS3:
      leds = 3;
      if (config2_mode == HalfBlink)
      {
        Half_blink(LED3);
        led_show(1);
      }
      else if (config2_mode == Fade)
      {
        led_show(1);
        fade(LED3);
      }
      else
        led_show(2);

      if (tempo == PAUSE)
      {
        led_mode = PAUSED;
        led_mode_previous = LEDS3;
      }

      if (s_go.rose())
      {
        led_mode = ALL;
        led_mode_previous = ALL;
      }

      if (s_up.rose())
        led_mode = LEDS4;

      if (led_timer())
        led_mode = LEDS2;
      break;

    case LEDS2:
      leds = 2;
      if (config2_mode == HalfBlink)
      {
        Half_blink(LED2);
        led_show(0);
      }
      else if (config2_mode == Fade)
      {
        led_show(0);
        fade(LED2);
      }
      else
        led_show(1);

      if (tempo == PAUSE)
      {
        led_mode = PAUSED;
        led_mode_previous = LEDS2;
      }

      if (s_go.rose())
      {
        led_mode = ALL;
        led_mode_previous = ALL;
      }

      if (s_up.rose())
        led_mode = LEDS3;

      if (led_timer())
        led_mode = LEDS1;

      break;
    case LEDS1:
      leds = 1;

      if (config2_mode == HalfBlink)
        Half_blink(LED1);
      if (config2_mode == Fade)
        fade(LED1);
      else
        led_show(0);

      if (s_go.rose())
      {
        led_mode = ALL;
        led_mode_previous = ALL;
      }

      if (s_up.rose())
        led_mode = LEDS2;

      if (tempo == PAUSE)
      {
        led_mode = PAUSED;
        led_mode_previous = LEDS1;
      }

      if (led_timer())
        led_mode = BLINK;
      break;

    case BLINK:
      Serial.println("BLINK");
      if (s_go.rose())
        led_mode = ALL;
      break;
    case PAUSED:
      blink_multiple(leds);
      if (tempo == RUN)
        led_mode = led_mode_previous;
      break;
    }
  }
}

void setup()
{
  s_go.attach(Sgo, INPUT_PULLUP);
  s_up.attach(Sup, INPUT_PULLUP);
  s_down.attach(Sdown, INPUT_PULLUP);
  // put your setup code here, to run once:

  Serial.begin(9600);

  s_go.interval(5);
  s_up.interval(5);
  s_down.interval(5);

  strip.neoPixelClear(1);
}

void loop()
{
  s_go.update();
  s_up.update();
  s_down.update();

  button_manager();

  time_configuration_ME();

  led_configuration_ME();

  config_machine_ME();

  configuration_1_ME();

  configuration_2_ME();

  configuration_3_ME();

  delay(10); //! NÃO ESQUECER DELAY, SENÃO FICA ESTRAGADO
}

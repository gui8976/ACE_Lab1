
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
unsigned long time_wait;
const unsigned long fast_interval = 100;

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
  CONFIGURAR,
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
  AZUL,
  VIOLETA,
  VERMELHO,
  VERDE,
  CIANO,
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

color_machine cor = AZUL;

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
      strip.neoPixelSetValue(led_to_blink, 255, 255, 255, 1); // por definir as cores
    }
    else
    {
      strip.neoPixelSetValue(led_to_blink, 0, 0, 0, 1);
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
      strip.neoPixelSetValue(led_to_blink, 255, 255, 255, 1); // por definir as cores
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
      strip.neoPixelFill(0, 255, 255, 1); // por definir as cores
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
    strip.neoPixelSetValue(leds_to_light, 255, 0, 0, 1);
  }
}

void timer_show_leds()
{
  if (tempo == RUN || tempo == PAUSE)
  {
    static unsigned long atual = millis();
    switch (led_mode)
    {
    case ALL:
      led_show(4);
      Serial.println("10s");
      if (tempo == PAUSE)
      {
        led_mode_previous = ALL;
      }
      if (millis() - atual >= interval)
      {
        led_mode = LEDS4;
        Serial.println("8s");
        atual = millis();
        strip.neoPixelClear(1);
      }

      break;
    case LEDS4:
      Serial.println("ENTROU");
      led_show(3);
      if (tempo == PAUSE)
      {
        led_mode_previous = LEDS4;
      }
      if (millis() - atual >= interval)
      {
        led_mode = LEDS3;
        Serial.println("6s");
        atual = millis();
        strip.neoPixelClear(1);
      }
      break;
    case LEDS3:
      led_show(2);
      if (tempo == PAUSE)
      {
        led_mode_previous = LEDS3;
      }
      if (millis() - atual >= interval)
      {
        led_mode = LEDS2;
        Serial.println("4s");
        atual = millis();
        strip.neoPixelClear(1);
      }
      break;
    case LEDS2:
      led_show(1);
      if (tempo == PAUSE)
      {
        led_mode_previous = LEDS2;
      }
      if (millis() - atual >= interval)
      {
        led_mode = LEDS1;
        Serial.println("2s");
        atual = millis();
        strip.neoPixelClear(1);
      }
      break;
    case LEDS1:
      led_show(0);
      if (tempo == PAUSE)
      {
        led_mode_previous = LEDS1;
      }
      if (millis() - atual >= interval)
      {
        led_mode = BLINK;
        Serial.println("0s");
      }

      break;
    case PAUSED:
      if (tempo == RUN)
      {
        led_mode = led_mode_previous;
        atual = millis();
      }
      break;
    default:
      break;
    }
  }
}

void config_machine_ME()
{
  switch (config)
  {
  case OFF:
    if (tempo == CONFIGURAR)
      config = C1;
    break;
  case C1:
    blink(LED1);
    if (s_up.rose())
      config = C2;
    break;
  case C2:
    blink(LED2);
    if (s_up.rose())
      config = C3;
    break;
    break;
  case C3:
    blink(LED3);
    if (s_up.rose())
      config = C1;
    break;
  }
}
void default_function()
{
  static unsigned long tempo_esperado = 0;
  switch (tempo)
  {
  case IDLE:
    Serial.println("IDLE");
    if (s_go.rose())
    {
      tempo = RUN;
    }
    if (s_up.fell())
    {
      tempo = CONFIG;
      tempo_previous = IDLE;
      tempo_esperado = millis();
      Serial.println("A pressionou");
    }
    break;

  case CONFIG:
    if (s_up.read() == 1)
    {
      tempo = tempo_previous;
    }
    if ((millis() - tempo_esperado >= 3000) && (s_up.read() == 0))
    {
      Serial.println("A pressionar durante 3s");
      tempo = CONFIGURAR;
    }
    break;
  case CONFIGURAR:
    Serial.println("CONFIGURAR");
    config_machine_ME();
    break;
  case RUN:
    if (s_up.fell())
    {
      tempo = CONFIG;
      tempo_previous = RUN;
      tempo_esperado = millis();
      Serial.println("A pressionou");
    }
    if (led_mode == BLINK)
    {
      tempo = BLINKING;
    }
    if (s_down.rose())
    {
      tempo = PAUSE;
    }
    Serial.println("RUN");
    timer_show_leds();
    break;
  case BLINKING:
    blink_all();
    break;

  case PAUSE:
    Serial.println("PAUSE");
    if (s_up.fell())
    {
      tempo = CONFIG;
      tempo_previous = PAUSE;
      tempo_esperado = millis();
      Serial.println("A pressionou");
    }
    if (s_down.rose())
    {
      tempo = RUN;
    }

    break;

  default:
    break;
  }
}

// alínea a)

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

    unsigned long currentmillis;
    switch (config2_mode)
    {
    case Default_timer:
      blink_with_interval(LED5);
      if (s_down.rose())
      {
        config2_mode = HalfBlink;
        currentmillis = millis();
      }
      break;
    case HalfBlink:

      strip.neoPixelSetValue(LED5, 255, 0, 0, 1);
      if (millis() - currentmillis >= interval / 2)
      {
        Serial.println("HALF BLINK");
        blink(LED5);
      }
      if (s_down.rose())
        config2_mode = Fade;
      break;
    case Fade:
      if (s_down.rose())
        config2_mode = Default_timer;
      break;

    default:
      break;
    }
  }
}

void choose_color()
{
  if (config == C3)
  {
    switch (cor)
    {
    case AZUL:
      strip.neoPixelSetValue(4, 0, 0, 255, 1);
      Serial.println("Azul");
      if (s_down.rose())
        cor = VIOLETA;
      break;
    case VIOLETA:
      strip.neoPixelSetValue(4, 0, 255, 0, 1);
      Serial.println("VIOLETA");
      if (s_down.rose())
        cor = VERMELHO;
      break;
    case VERMELHO:
      strip.neoPixelSetValue(LED5, 255, 0, 0, 1);
      Serial.println("VERMELHO");
      if (s_down.rose())
        cor = VERDE;
      break;
    case VERDE:
      strip.neoPixelSetValue(LED5, 0, 255, 255, 1);
      Serial.println("VERDE");
      if (s_down.rose())
        cor = CIANO;
      break;
    case CIANO:
      strip.neoPixelSetValue(LED5, 255, 0, 255, 1);
      Serial.println("CIANO");
      if (s_down.rose())
        cor = LARANJA;
      break;
    case LARANJA:
      strip.neoPixelSetValue(LED5, 255, 255, 0, 1);
      Serial.println("LARANJA");
      if (s_down.rose())
        cor = BRANCO;
      break;
    case BRANCO:
      strip.neoPixelSetValue(LED5, 255, 255, 255, 1);
      Serial.println("BRANCO");
      if (s_down.rose())
        cor = AZUL;
      break;
    default:
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

  default_function();
  // configuration_3();

  // config_machine_ME();

  configuration_1_ME();

  configuration_2_ME();



  // put your main code here, to run repeatedly:

  delay(10); //! NÃO ESQUECER DELAY, SENÃO FICA ESTRAGADO
}

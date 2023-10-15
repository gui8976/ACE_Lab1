
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

NeoPixelConnect strip(5, MAXIMUM_NUM_NEOPIXELS, pio0, 0);

Bounce s_go = Bounce();
Bounce s_up = Bounce();
Bounce s_down = Bounce();

const unsigned long interval = 100; // interval at which to blink (milliseconds)

// define the structures here:

// sturture for the configuration machine
typedef enum
{
  OFF,
  ON,
  C1,
  C2,
  C3,
  C4,
} config_machine;

// structure for the time machine
typedef enum
{
  IDLE,
  RUN,
  PAUSE,
} time_machine;

typedef enum 
{
  T1 = 1000, // will store last time LED was updated
  T2 = 2000, // will store last time LED was updated
  T5 = 5000, // will store last time LED was updated
  T10 = 10000, // will store last time LED was updated
}timer_interval;

typedef enum
{
  LED5_STATE1,
  LED5_STATE2,
  LED5_STATE3,
} led5_state;
// initiate the values of the structures

config_machine config = OFF;
config_machine config_previous = OFF;
time_machine time = IDLE;
timer_interval timer = T1;
led5_state state_led5 = LED5_STATE1;

//! meter numa class PFFFFF
// creates a function that blink the leds
void blink(uint16_t led_to_blink)
{
  bool led_state = false;
  unsigned long previousMillis = 0;

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

void blink_with_interval(uint16_t led_to_blink, unsigned long interval)
{
  bool led_state = false;
  unsigned long previousMillis = 0;

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
  bool led_state = false;
  unsigned long previousMillis = 0;
  if (millis() - previousMillis >= 100)
  {
    led_state = !led_state;
    previousMillis = millis();

    if (led_state)
    {
      strip.neoPixelFill(0, 255, 0, 1); // por definir as cores
    }
    else
    {
      strip.neoPixelClear(1);
    }
  }
}

void led_turn_off(uint16_t led_to_turn_off)
{
  strip.neoPixelSetValue(led_to_turn_off, 0, 0, 0, 1);
}

void led_turn_on(uint16_t led_to_turn_on)
{
  strip.neoPixelSetValue(led_to_turn_on, 255, 255, 255, 1);
}

void led_set_color(uint16_t led_to_set_color, uint8_t red, uint8_t green, uint8_t blue)
{
  strip.neoPixelSetValue(led_to_set_color, red, green, blue, 1);
}

void led_state_default()
{
  // turn on all leds
  strip.neoPixelFill(255, 255, 255, 1);
}

// changes led coulour given an input from the user
void changes_led_colour(bool button_pressed)
{
  int times_pressed = 1;
  if (button_pressed && times_pressed == 1)
  {
    times_pressed++;
    led_set_color(LED5, 255, 255, 255); //! FALTA DEFINIR AS CORES
    button_pressed = false;
  }
  if (button_pressed && times_pressed == 2)
  {
    times_pressed++;
    led_set_color(LED5, 255, 255, 255); //! FALTA DEFINIR AS CORES
    button_pressed = false;
  }
  if (button_pressed && times_pressed == 3)
  {
    times_pressed++;
    led_set_color(LED5, 255, 255, 255); //! FALTA DEFINIR AS CORES
    button_pressed = false;
  }
  if (button_pressed && times_pressed == 4)
  {
    times_pressed++;
    led_set_color(LED5, 255, 255, 255); //! FALTA DEFINIR AS CORES
    button_pressed = false;
  }
  if (button_pressed && times_pressed == 5)
  {
    times_pressed++;
    led_set_color(LED5, 255, 255, 255); //! FALTA DEFINIR AS CORES
    button_pressed = false;
  }
  if (button_pressed && times_pressed == 6)
  {
    times_pressed++;
    led_set_color(LED5, 255, 255, 255); //! FALTA DEFINIR AS CORES
    button_pressed = false;
  }
  if (button_pressed && times_pressed == 7)
  {
    times_pressed = 1;
    led_set_color(LED5, 255, 255, 255); //! FALTA DEFINIR AS CORES
    button_pressed = false;
  }
}

// a function that fades the led intensity using the neopixel library
void fade(){
  // Fade in
  // Fade out
  for(int i = 255; i >= 0; i--){
    strip.neoPixelSetValue(LED5, i, i, i, 1);
  }

}

void config_1()
{

  // turns off all the leds
  strip.neoPixelClear(1);
  // led 3 blinking
  blink(LED1);
  // led 5 changes colour
  switch (timer)
  {
  case T1:
    if (s_down.rose())
      timer = T2;
    blink_with_interval(LED5, T1);

    break;

  case T2:
    if (s_down.rose())
      timer = T5;
    blink_with_interval(LED5, T2);

    break;
  case T5:
    if (s_down.rose())
      timer = T10;
    blink_with_interval(LED5, T5);

    break;
  case T10:
    if (s_down.rose())
      timer = T1;
    blink_with_interval(LED5, T10);

    break;
  default:
    break;
  }
}

void config_2()
{
  // turns off all the leds
  strip.neoPixelClear(1);
  // led 4 blinking
  blink(LED2);
  //led 5 switches between 3 states
  if(state_led5 == LED5_STATE1)
  {
    if (s_down.rose())
      state_led5 = LED5_STATE2;
    blink_with_interval(LED5, timer);
  }
  if(state_led5 == LED5_STATE2)
  {
    if (s_down.rose())
      state_led5 = LED5_STATE3;

    strip.neoPixelSetValue(LED5, 255, 255, 255, 1); //! por definir as cores
    unsigned long currentMillis = millis();
    if(currentMillis >= timer/2)
    {
      state_led5 = LED5_STATE1;
      strip.neoPixelSetValue(LED5, 0, 0, 0, 1);
      blink(LED5);
    }

  }
  if(state_led5 == LED5_STATE3)
  {
    if (s_down.rose())
      state_led5 = LED5_STATE1;
    fade(LED5, timer);
  }
}

void config_3()
{
  // turns off all the leds
  strip.neoPixelClear(1);
  // led 2 blinking
  blink(LED3);
  // led 5 changes colour
  if (s_down.rose())
    changes_led_colour(true);
}

void state_machine_config(config_machine config, time_machine time)
{
  bool button_pressed = false;
  int state_led5 = 1;

  switch (config)
  {
  case OFF:
    // se o botão for pressionado

    break;

  case ON:
    if (1 && (config_previous != OFF) /*"botão for pressionado durante 3s"*/)
    {
      config = config_previous;
    }
    else
      config = C1;

    //! falta implementar o timer de 2 segund0s , blink já feito

    // os leds 1,2,3,4,5 desligam com um intervalo predeifinido de tempo 2000
    // no final do tempo os leds entram em blink state.

    blink_all();
    break;

  case C1:
    if (1 /*"botão for pressionado"*/)
    {
      config = C2;
    }
    if (1 /*"botão for pressionado durante 3s"*/)
    {
      config = ON;
      config_previous = C1;
    }
    config_1();
    // led 5 alterna o tempo que tem para o ficar ligado entre: 1,2,5,10 segundos
    // led 1 a piscar
    break;

  case C2:
    if (1 /*"botão for pressionado"*/)
    {
      config = C1;
    }
    if (1 /*"botão for pressionado durante 3s"*/)
    {
      config = ON;
      config_previous = C2;
    }
    config_2();
    // led 5 aterna entre varios estados :
    // estado 1: desliga ao fim do intervalo de tempo
    // estado 2: para a segunda parte do intervalo de tempo o led dá blink
    // estado 3: a luminosidade do led vai diminuindo até ao fim do intervalo de tempo
    // led 2 a piscar
    /* code */
    break;

  case C3:
    if (1 /*"botão for pressionado"*/)
    {
      config = C4;
    }
    if (1 /*"botão for pressionado durante 3s"*/)
    {
      config = ON;
      config_previous = C3;
    }
    config_3();
    // led 5 alterna entre varias cores : violet, blue , cyan, green , yellow, orange , white
    // tem que ler o input do utilizador para mudar de cor EX: pode ser um botão
    // led 3 a piscar
    break;

  case C4:
    if (1 /*"botão for pressionado"*/)
    {
      config = C1;
    }
    if (1 /*"botão for pressionado durante 3s"*/)
    {
      config = ON;
      config_previous = C4;
    }
    //! Falta decidir qual é o modo extra
    // para entrar neste estado é necessario esperar 30 segundos na posição de todos os leds a piscar
    // led 5 alterna entre varias cores : violet, blue , cyan, green , yellow, orange , white e o led esta a piscar
    // tem que ler o input do utilizador para mudar de cor EX: pode ser um botão
    // led 4 a piscar

    /* code */
    break;

  default:
    break;
  }

  switch (time)
  {
  case IDLE:
    /* code */
    break;
  case RUN:
    /* code */
    break;
  case PAUSE:
    /* code */
    break;

  default:
    break;
  }
}

void setup()
{
  s_go.attach(Sgo, INPUT_PULLUP);
  s_up.attach(Sup, INPUT_PULLUP);
  s_down.attach(Sdown, INPUT_PULLUP);
  // put your setup code here, to run once:
}

void loop()
{
  s_go.update();
  s_up.update();
  s_down.update();

  // put your main code here, to run repeatedly:
}

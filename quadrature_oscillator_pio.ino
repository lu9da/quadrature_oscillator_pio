// generador por pio de frecuencia en cuadratura, para exitar directamente un mixer tayloe
// tipo softrock (ahorra un SI5351 para generar I-lo e Q-lo) de 0 a 30 mhz

// Encontrado por casulidad por ahi y adaptado para mis necesidades a Arduino RP2040
// Creditos a quien corresponda, lamentablemente no pude encontrar de nuevo la pagina 
// de donde lo baje.

// anda en un solo core, pero lo probe con los dos cores y funciona perfectamente tambien

// las salidas en cuadratura estan en GPIO0 y GPIO1

// - adaptado y modificado en 2024 por LU9DA -

// Uso publico, si se comparte o usa en proyectos, este encabezado DEBE PERMANECER como esta
// Public use, if shared or used in projects, this header MUST REMAIN as is

#include <Arduino.h>
#include <stdarg.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "nco.pio.h"



double tuned_frequency_Hz;
double nco_frequency_Hz;
double offset_frequency_Hz;

bool settings_changed = false;

uint offset = 0;
uint32_t sm = 0;

// Choose which PIO instance to use (there are two instances)
PIO pio;

// Diagnostics LEDs.

void apply_frequency()
{
  if(settings_changed)
  {
    //apply frequency
    //tuned_frequency_Hz = 14000000;
    
    nco_frequency_Hz = nco_program_init(pio, sm, offset, tuned_frequency_Hz);
    offset_frequency_Hz = tuned_frequency_Hz - nco_frequency_Hz;

    Serial.println("NEW FREQ SET.");
    settings_changed = false;

  }
}






void setup() {
  Serial.begin(115200);

 while (!Serial)  //Caution!!  with Serial, if no USB-serial open, it will be stuck here
  {  //wait for PC USB-serial to open
  
  gpio_set_mask(1<<LED_BUILTIN);
  delay(250);
 
  gpio_clr_mask(1<<LED_BUILTIN);
  delay(250);
  }


  Serial.println("setup");

  // LEDs init
  pinMode(LED_BUILTIN, OUTPUT);


  gpio_set_mask(1 << LED_BUILTIN);

 
  //prepare pio quadrature oscillator
  pio = pio0;
  offset = pio_add_program(pio, &hello_program);
  sm = pio_claim_unused_sm(pio, true);
  hello_program_get_default_config(offset);
  //

  // first start
  
  tuned_frequency_Hz = 14000000;   //power on on 14 mhz
  settings_changed = true;
  
  apply_frequency();


  Serial.println(offset);
  Serial.println(sm);
  Serial.println(nco_frequency_Hz);

}




void loop() {

  if (Serial.available() != 0)
  {
    int dataIn = Serial.parseInt();    // only numbers, frequency in Hz, without any kind of points, cr/lf or /n  
                                       // for example 7060000  tunes to 7.060,00 kHz 
    Serial.print("Received: ");
    Serial.println(dataIn);
    tuned_frequency_Hz = dataIn;
    settings_changed = true;
    apply_frequency();


    Serial.println(offset);
    Serial.println(sm);
    Serial.print("tuneded: ");
    Serial.println(nco_frequency_Hz);
  }
  delay(1);
 
}

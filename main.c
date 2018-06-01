// standard libraries
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "avr/interrupt.h"
// given by instructors
#include "HeaderFiles/dwenguinoBoard.h"
#include "HeaderFiles/dwenguinoLCD.h"
// defines the constants in the program
#include "HeaderFiles/ir_states.h"
#include "HeaderFiles/constants.h"

void setup_dubug_pin();
void setup_ir_receiver_interrupt();
void setup_ir_receive_timers();
void setup_ir_send_timer_interrupt();
void draw_nodes_LCD();
void setup_ui_pins();
void setup_node_list();
void slow_timer();
void speed_timer();
void analyse_buffer();
void draw_command_LCD();
void create_timing_list_NEC();
void save_buffer();
void reset_to_navigate();

// Variables for the sending side of the program
volatile char send_state = NO_SEND;

// receive_buffer contains the length of the intervals
// value_buffer contains whether pin was high or not
unsigned int receive_buffer[MAX_BUFFER];
char value_buffer[MAX_BUFFER];
// gets incremented when value is added to buffer
char buffer_index = 0;
// analyse puts the decoded buffer in these lists
char adress_buffer [8] = {0,0,0,0,0,0,0,0};
char command_buffer [8] = {0,0,0,0,0,0,1,0};

char save_state = SAVE_IGNORE;

// Variables for the UI side of the program
struct command_node{
    char adress[8];
    char command[8];

    char name[NAME_LENGTH];
};

struct command_node node_list [AMOUNT_NODES];
char node_index = 0;

unsigned int NEW_send_array[72];
unsigned int NEW_repeat_array[4];
char send_length = 72;
char repeat_length = 4;

// avoid detecting button twice
// gets to 0 when not pressed
char button_boolean = 0b00000000;

// pulse index is the amount of pulses in a state, defined in de NEC standard
// bit_send_index is the index of the bit that's transmitting
volatile unsigned int pulse_index = 0;
volatile signed int send_index = 0;


// used in navigating in the edit mode
char edit_index = 0;
// used to simulate cursor in edit mode
char underscore_or_not = 0;
char ui_state = NAVIGATE_STATE;

int main(void)
{
    initBoard();
    // setup LCD
    initLCD();
    backlightOn();
    clearLCD();

    // setup program information, registers, timers, ...
    setup_dubug_pin();
    setup_ir_receiver_interrupt();
    setup_ir_receive_timers();
    setup_ir_send_timer_interrupt();
    setup_ui_pins();
    setup_node_list();

    // draw first nodes the LCD
    draw_nodes_LCD();

    // this is used for testing purposes, saves the "on/off/ led" in the first node
    save_buffer();

    while(1){
        // when the buffer is full or a certain amount is reached the pogram will analyse the buffer
        if(buffer_index == MAX_BUFFER || (buffer_index > BUFFER_TRESH && INTERRUPT_TIMER > BUFFER_RESET)){
            analyse_buffer();
        }

        //===========================================//
        //This is the UI part of the pogram, the button values are read here
        // When the button is pressed the loop goes over the buttons multiple times
        // to stop this we set a variable to high when pressed and low when not pressed
        // in order to execute the button code the variable has to be low, so it only
        // activates when the button goes from low to high
        //===========================================//


        // used to scroll up in the edit menu and node list
        if(UP_BUTTON == 0){
            if((button_boolean & _BV(0)) == 0){
                button_boolean |= _BV(0);
                if(ui_state == EDIT_STATE){
                    if(node_list[node_index].name[edit_index] == ' '){
                        node_list[node_index].name[edit_index] = 'a';
                    }else if(node_list[node_index].name[edit_index] == 'z'){
                        node_list[node_index].name[edit_index] = '0';
                    }else if(node_list[node_index].name[edit_index] == '9'){
                        node_list[node_index].name[edit_index] = ' ';
                    }else{
                        node_list[node_index].name[edit_index] = (char)((int)node_list[node_index].name[edit_index] + 1);
                    }

                }else{
                    // NAVIGATE_STATE
                    #ifndef DEBUG
                    node_index = (node_index + 1) % AMOUNT_NODES;
                    #else
                    node_index = (node_index + 1) % MAX_BUFFER;
                    #endif
                }
            }
            draw_nodes_LCD();
            _delay_ms(NO_SEND);
        }else{
            // PORTA &= ~_BV(0);
            if((button_boolean & _BV(0)) == 1)
                _delay_ms(UI_DELAY);
            button_boolean &= ~_BV(0);
        }

        // used to scroll down in the edit menu and node list
        if(DOWN_BUTTON == 0){
            //PORTA |= _BV(1);
            if((button_boolean & _BV(2)) == 0){
                button_boolean |= _BV(2);
                //PORTA |= _BV(4);
                if(ui_state == EDIT_STATE){
                    if(node_list[node_index].name[edit_index] == 'a'){
                        node_list[node_index].name[edit_index] = ' ';
                    }else if(node_list[node_index].name[edit_index] == ' '){
                        node_list[node_index].name[edit_index] = '9';
                    }else if(node_list[node_index].name[edit_index] == '0'){
                        node_list[node_index].name[edit_index] = 'z';
                    }else{
                        node_list[node_index].name[edit_index] = (char)((int)node_list[node_index].name[edit_index] - 1);
                    }
                }else{
                    //
                    if(node_index - 1 < 0){
                        #ifndef DEBUG
                        node_index = AMOUNT_NODES - 1;
                        #else
                        node_index = MAX_BUFFER - 1;
                        #endif
                    }else{
                        node_index -= 1;
                    }

                }
            }
            draw_nodes_LCD();
            _delay_ms(UI_DELAY);
        }else{
            //PORTA &= ~_BV(1);
            if((button_boolean & _BV(2)) == 1)
                _delay_ms(UI_DELAY);
            button_boolean &= ~_BV(2);
        }

        // activates ir function or goes to next char when in edit mode
        if(SELECT_BUTTON == 0){
            //PORTA |= _BV(2);
            if((button_boolean & _BV(1)) == 0){
                button_boolean |= _BV(1);

                if(ui_state == EDIT_STATE){
                    edit_index++;
                    draw_nodes_LCD();
                    if(edit_index == NAME_LENGTH){
                        reset_to_navigate();
                    }
                }else{
                    create_timing_list_NEC();
                    send_index = 0;
                    pulse_index = 0;
                    send_state = SEND_SIGNAL;

                }

            }
            _delay_ms(UI_DELAY);
        }else{
            if((button_boolean & _BV(1)) == 1)
                _delay_ms(UI_DELAY);
            button_boolean &= ~_BV(1);
        }

        // saves the received ir code to the current node
        if(SAVE_BUTTON == 0){
            if((button_boolean & _BV(5)) == 0){
                button_boolean |= _BV(5);
                save_state = SAVE_VALID;
                save_buffer();
                draw_nodes_LCD();
                PORTA = _BV(0);
                // reset buffer
                buffer_index = 0;
            }

            _delay_ms(UI_DELAY);
        }else{
            if((button_boolean & _BV(5)) == 1)
                _delay_ms(UI_DELAY);
            button_boolean &= ~_BV(5);
        }

        // switches from NAVIGATE_STATE to EDIT_STATE
        if(EDIT_BUTTON == 0){
            if((button_boolean & _BV(6)) == 0){
                button_boolean |= _BV(6);
                if(ui_state == NAVIGATE_STATE){
                    slow_timer();
                    ui_state = EDIT_STATE;
                }else{
                    reset_to_navigate();
                }
            }
        }else{
            if((button_boolean & _BV(6)) == 1)
                _delay_ms(UI_DELAY);
            button_boolean &= ~_BV(6);
        }

        // toggles the char under the edit_index to underscore and back
        // this is used to show where the cursor is
        if(ui_state == EDIT_STATE){
            if(INTERRUPT_TIMER > UI_EDIT_REFRESH){
                INTERRUPT_TIMER = 0;
                if(underscore_or_not == 1){
                    printCharToLCD('_', 0, edit_index + 1);
                }else{
                    printCharToLCD(node_list[node_index].name[edit_index], 0, edit_index + 1);
                }
                underscore_or_not = 1 - underscore_or_not;
            }
        }


    }
    return 0;
}


//===========================================//
//===============Setup functions=============//
//===========================================//


// pins used the hang the dwenguino on the oscilloscope
void setup_dubug_pin(){
    // first led
    DDRA |= 0b11111111;
    // PORTA |= 1;
    DDRD |= _BV(PD2);
    DDRD |= _BV(PD3);
}


// setup of the IR receiver interrupt pin (PD0)
// interrupt is configured to run when leven changes
void setup_ir_receiver_interrupt(){
  DDRD &= ~_BV(PD0); // set as input
  //PORTE |= _BV(PE4); //set pull-up
  //set as changing edge, page 93
  EICRA &= ~_BV(ISC01);
  EICRA |= _BV(ISC00);

  //enable interrupt on pin 4, page 93
  EIMSK |= _BV(PIN0);

  SREG |= _BV(7);
}


// these are the timers that are necessary for the receive algoritm
void setup_ir_receive_timers(){
  //Set multiplier to 8, page 140
  //INTERRUPT_TIMER, multiplier 64
  TCCR1B &= ~_BV(CS12);
  TCCR1B |= _BV(CS11);
  TCCR1B |= _BV(CS10);
}


// Sets up the timer interupt register for the send function
void setup_ir_send_timer_interrupt(){
  //enabel output compare to A
  TIMSK2 |= _BV(OCIE2A);

  //Set multiplier to 1
  TCCR2B &= ~_BV(CS22); //CS32
  TCCR2B &= ~_BV(CS21);
  TCCR2B |= _BV(CS20);

  //Set up CTC 0 1 0
  TCCR2B |= _BV(WGM22); // WGM32 -> 1
  TCCR2A &= ~_BV(WGM21); // WGM31 -> 0
  TCCR2A |= _BV(WGM20); // WGM30 -> 0

  //compare register, interrupt goes off at 2 * 38kHz
  OCR2A = 105;
}


// these are the button pins on the dwenguino
// set as input en as pull-up
void setup_ui_pins(){
    // North
    DDRE &= ~_BV(PE7);
    // East
    DDRE &= ~_BV(PE6);
    // South
    DDRE &= ~_BV(PE5);
    // West
    DDRE &= ~_BV(PE4);
    // Central
    DDRC &= ~_BV(PC7);

    PORTE |= _BV(PE7);
    PORTE |= _BV(PE6);
    PORTE |= _BV(PE5);
    PORTE |= _BV(PE4);
    PORTC |= _BV(PC7);
}


// fills up the node list with new + number mod 10
// sets adress to 0 in order to check if assigned
void setup_node_list(){
    for(int i = 0; i < AMOUNT_NODES; i++){
        node_list[i].name[0] = 'n';
        node_list[i].name[1] = 'e';
        node_list[i].name[2] = 'w';
        node_list[i].name[3] = (char)(i%10 + 48);
        for(int w = 4; w < NAME_LENGTH; w++){
            node_list[i].name[w] = ' ';
        }
        node_list[i].adress[0] = -1;
        for(int c = 0; c < 8; c++){
            node_list[i].command[c] = 0;
            node_list[i].adress[c] = 0;
        }
    }
    node_list[0].command[6] = 1;
}


// ============================================================
// ==================== program functions =====================
// ============================================================


// function that saves the buffers generated in analyse_buffer in the node list
void save_buffer(){
    for(int i = 0; i < 8; i++){
        node_list[node_index].command[i] = command_buffer[i];
        node_list[node_index].adress[i] = adress_buffer[i];
    }
}

// this function makes sure that everything stills works when changing from edit to navigation mode
void reset_to_navigate(){
    ui_state = NAVIGATE_STATE;
    speed_timer();
    edit_index = 0;
    draw_nodes_LCD();
}

// this function is used analyse the buffer, currently only supports NEC
// runs when buffer is full or when partialy full and timer runs out
void analyse_buffer(){
    // find average min/max
    char min_index = 4;
    char max_index = 4;
    unsigned int min = receive_buffer[min_index];
    unsigned int max = receive_buffer[max_index];

    // find min and max in the not pulse period
    // needed because delay sensor is not constant
    for(int i = 2; i < 32; i++){
        if(receive_buffer[2*i + 2] > max){
            max_index = 2*i + 2;
            max = receive_buffer[2*i + 2];
        }
        if(receive_buffer[2*i + 2] < min){
            min_index = 2*i + 2;
            min = receive_buffer[2*i + 2];
        }
    }

    // check every not pulse against the min/max and decide 0/1
    for(int i = 1; i < BYTE_LENGTH + 1; i++){
        if(max - receive_buffer[2*i + 2] < receive_buffer[2*i + 2] - min){
            adress_buffer[i - 1] = 1;
        }else{
            adress_buffer[i - 1] = 0;
        }
        if(max - receive_buffer[2*i + 34] < receive_buffer[2*i + 34] - min){
            command_buffer[i - 1] = 1;
        }else{
            command_buffer[i - 1] = 0;
        }
    }

    // when allowed to save save
    if(save_state == SAVE_VALID){
        save_buffer();
        save_state = SAVE_IGNORE;
        PORTA &= ~_BV(0);
    }
}


// this function takes the command and adress from the node list and changes it to a timing list for NEC
void create_timing_list_NEC(){
    NEW_send_array[0] = SYNC_AMOUNT_PULSES * 2;
    NEW_send_array[1] = SYNC_WAIT_PULSES * 2;

    for(int i = 0; i < 8; i++){
        NEW_send_array[i*2 + 2] = BIT_AMOUNT_PULSES * 2;
        NEW_send_array[(i+8)*2 + 2] = BIT_AMOUNT_PULSES * 2;
        NEW_send_array[(i+16)*2 + 2] = BIT_AMOUNT_PULSES * 2;
        NEW_send_array[(i+24)*2 + 2] = BIT_AMOUNT_PULSES * 2;

        if(node_list[node_index].adress[i] == 0){
            NEW_send_array[i*2 + 3] = BIT_0_AMOUNT_PULSES * 2;
            NEW_send_array[(i+8)*2 + 3] = BIT_1_AMOUNT_PULSES * 2;
        }else{
            NEW_send_array[i*2 + 3] = BIT_1_AMOUNT_PULSES * 2;
            NEW_send_array[(i+8)*2 + 3] = BIT_0_AMOUNT_PULSES * 2;
        }

        if(node_list[node_index].command[i] == 0){
            NEW_send_array[(i+16)*2 + 3] = BIT_0_AMOUNT_PULSES * 2;
            NEW_send_array[(i+24)*2 + 3] = BIT_1_AMOUNT_PULSES * 2;
        }else{
            NEW_send_array[(i+16)*2 + 3] = BIT_1_AMOUNT_PULSES * 2;
            NEW_send_array[(i+24)*2 + 3]= BIT_0_AMOUNT_PULSES * 2;
        }
    }
    // add one repeat to end in order to function properly
    NEW_send_array[66] = BIT_AMOUNT_PULSES * 2;
    NEW_send_array[67] = SYNC_REPEAT_WAIT_FIRST_PULSES * 2;
    NEW_send_array[68] = SYNC_AMOUNT_PULSES * 2;
    NEW_send_array[69] = SYNC_WAIT_PULSES * 2;
    NEW_send_array[70] = BIT_AMOUNT_PULSES * 2;
    NEW_send_array[71] = SYNC_REPEAT_WAIT_PULSES * 2;

    // setup repeat part list
    NEW_repeat_array[0] = SYNC_AMOUNT_PULSES * 2;
    NEW_repeat_array[1] = SYNC_WAIT_PULSES * 2;
    NEW_repeat_array[2] = BIT_AMOUNT_PULSES * 2;
    NEW_repeat_array[3] = SYNC_REPEAT_WAIT_PULSES * 2;
 }

// changes timer from x64 to x1024, needed for flashing underscore in EDIT_STATE
void slow_timer(){
    TCCR1B |= _BV(CS12);
    TCCR1B &= ~_BV(CS11);
    TCCR1B |= _BV(CS10);
}

// changes timer from x1024 to x64, needed to receive ir codes
void speed_timer(){
    TCCR1B &= ~_BV(CS12);
    TCCR1B |= _BV(CS11);
    TCCR1B |= _BV(CS10);
}

// print the node names on the LCD
// current node on the top row
// previous node on the bottom row
void draw_nodes_LCD(){
    printCharToLCD('>',0,0);
    for(int i = 1; i < NAME_LENGTH + 1; i++){
        printCharToLCD(node_list[node_index].name[i-1],0,i);
        if(node_index - 1 < 0){
            printCharToLCD(node_list[AMOUNT_NODES - 1].name[i-1],1,i);
        }else{
            printCharToLCD(node_list[node_index - 1].name[i-1],1,i);
        }

    }
}
// draw the buffer on the LCD, should only be run when debugging
void draw_command_LCD(){
    for(int i = 0; i < 8; i++){
        printIntToLCD(adress_buffer[i], 0, i);
        printIntToLCD(command_buffer[i], 1, i);
    }
}

// ============================================================
// ============ Start of the interrupt functions ==============
// ============================================================

// hangs on the IR receiver pin
ISR(INT0_vect){
    if(buffer_index < MAX_BUFFER && save_state == SAVE_VALID){
        PORTD ^= 1 << 2;
        receive_buffer[buffer_index] = INTERRUPT_TIMER;
        value_buffer[buffer_index] = ((PIND & _BV(PD0)));
        buffer_index++;
        INTERRUPT_TIMER = 0;
    }
}

// timer interrupt, works on 76Khz in order to send pulses at 38kHz
ISR(TIMER2_COMPA_vect){
    if(send_state != NO_SEND){
        if(send_state == SEND_SIGNAL){
            if(pulse_index <= NEW_send_array[send_index]){
                if(send_index % 2 == 0){
                    //even number should pulse
                    PORTD ^= 1 << 3;
                }else{
                    PORTD &= ~_BV(3);
                }
                pulse_index++;
            }else{
                // check if last
                if(send_index == send_length -1){
                    //last index just passed
                    send_state = SEND_REPEAT;
                }else{
                    send_index++;
                }
                pulse_index = 0;
                PORTD &= ~_BV(3);
            }
        }else if(send_state == SEND_REPEAT && SELECT_BUTTON == 0){
            if(pulse_index <= NEW_repeat_array[send_index]){
                if(send_index & 1 == 0){
                    //even number should pulse
                    PORTD ^= 1 << 3;
                }
                pulse_index++;
            }else{
                if(send_index == repeat_length - 1){
                    send_index = 0;
                }else{
                    send_index++;
                }
                pulse_index = 0;
                PORTD &= ~_BV(3);
            }
        }else{
            send_state = NO_SEND;
        }
    }
}

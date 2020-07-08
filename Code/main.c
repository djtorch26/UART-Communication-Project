#include <msp430.h>
#include <stdint.h>

volatile int LENGTH = 0;    //length of message
volatile int i = 1;         //index for location of byte
volatile int LED_RED = 0;   //Duty cycle of Red in LED
volatile int LED_GREEN = 0; //Duty cycle of Green in LED
volatile int LED_BLUE = 0;  //Duty cycle of Blue in LED

int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

//Clear all ports and directions
  P1DIR = 0x00; //clears port 1 directions
  P1OUT = 0x00; //clears outputs for port 1
  P2DIR = 0x00; //clears port 2 directions
  P2OUT = 0x00; //clears outputs for port 2

//PIN Instantiations
  P1DIR |= BIT6; //Port 1.6 is an output
  P1SEL |= BIT6; //PWM signal enable on port 1.6
  P2DIR |= BIT1 + BIT5; //Port 2.1 & 2.5 are outputs.
  P2SEL |= BIT1 + BIT5; //PWM signal enable on 2.1 & 2.6

//RxD and TxD Instantiation
  P1SEL |= BIT1; //P1.1 is TA0.0
  P1SEL |= BIT2; //P1.2 is TA0.1
  P1SEL2 |= BIT1; //P1.1 is RxD
  P1SEL2 |= BIT2; //P1.2 is TxD

//Timer0 Instantiation
  TA0CTL |= TASSEL_2 + MC_1; //SMCLK, no divider, up mode
  TA0CCTL1 |= OUTMOD_7; //The set/reset output for PWM
  TA0CCR0 = 255; //Timer0 Duty Cycle at 100%
  TA0CCR1 = 0; //Red Led PWM 0%

//Timer1 Instantiation
  TA1CTL |= TASSEL_2 + MC_1; //SMCLK, no divider, up mode
  TA1CCTL1 |= OUTMOD_7; //The set/reset output for PWM
  TA1CCTL2 |= OUTMOD_7; //The set/reset output for PWM
  TA1CCR0 = 255; //Timer1 Duty Cycle 100%
  TA1CCR1 = 0; //Green LED 0%
  TA1CCR2 = 0; //Blue LED 0%

//UART controls
  UCA0CTL1 |= UCSSEL_2; // smCLK
  UCA0BR0 = 104;    //1MHz 9600baud
  UCA0BR1 = 0; //1MHz 9600baud
  UCA0MCTL = UCBRS0; //modulation control
  UCA0CTL1 &= ~UCSWRST; //Initialize USCI state machine
  IE2 |= UCA0RXIE;  //Enable USCI_A0 RX interrupt

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0, interrupts enabled
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCI0X_ISR(void)
{
    while (!(IFG2 & UCA0TXIFG));    // USCI_A0 TX buffer ready?
    if(i==1){
        UCA0TXBUF = UCA0RXBUF - 3;  //Length transmitted is input - 3
        LENGTH = UCA0RXBUF -3;  //Set LENGTH to input length - 3
    }
    else if (i<5){
      switch(i)
      {
        case 2:
            LED_RED = UCA0RXBUF; //set Red duty cycle to value of 2nd byte received
            break;
        case 3:
            LED_BLUE = UCA0RXBUF; //set Green duty cycle to value of 2nd byte received
            break;
        case 4:
            LED_GREEN = UCA0RXBUF; //set Blue duty cycle to value of 2nd byte received
            break;
        default:
            break;
        }
    }

    else{
        UCA0TXBUF = UCA0RXBUF;  //send bytes
    }

    if(i == LENGTH + 3){    //if more bytes received, reset counter and set timers to pwm of each LED
        i = 0;  //reset counter to zero
        TA0CCR1 = LED_RED;  //Set timer to Red led pwm signal
        TA1CCR2 = LED_GREEN;//Set timer to Green led pwm signal
        TA1CCR1 = LED_BLUE; //Set timer to Blue led pwm signal
    }

    i++;
}




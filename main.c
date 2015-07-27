/* This is the main file that contains the main function and the
 * main while(1) loop.
 * Once you understand our code, please feel free to modify them
 * to meet your project or homework requirement
 */

#define PI 3.141592

/*Major header file includes all system function you need*/
#include "system_init.h"
#include "system.h"
#include "alt_types.h"

/*The header file where you store your ISR*/
#include "fft.h"
#include "yourISR.h"
#include "math.h"
/************************************************************************/
/********************Global Variables for your system********************/
/************************************************************************/


//Value for interrupt ID
alt_u32 switch0_id = SWITCH0_IRQ;
alt_u32 switch1_id = SWITCH1_IRQ;
alt_u32 switch2_id = SWITCH2_IRQ;
alt_u32 switch3_id = SWITCH3_IRQ;
alt_u32 switch4_id = SWITCH4_IRQ;
alt_u32 switch5_id = SWITCH5_IRQ;
alt_u32 switch6_id = SWITCH6_IRQ;
alt_u32 switch7_id = SWITCH7_IRQ;
alt_u32 switch8_id = SWITCH8_IRQ;
alt_u32 key0_id = KEY0_IRQ;
alt_u32 key1_id = KEY1_IRQ;
alt_u32 key2_id = KEY2_IRQ;
alt_u32 key3_id = KEY3_IRQ;
alt_u32 leftready_id = LEFTREADY_IRQ;
alt_u32 rightready_id = RIGHTREADY_IRQ;
alt_u32 uart_id = UART_IRQ;

/*Use for ISR registration*/
volatile int switch0 = 0;
volatile int switch1 = 0;
volatile int switch2 = 0;
volatile int switch3 = 0;
volatile int switch4 = 0;
volatile int switch5 = 0;
volatile int switch6 = 0;
volatile int switch7 = 0;
volatile int switch8 = 0;
volatile int key0 = 0;
volatile int key1 = 0;
volatile int key2 = 0;
volatile int key3 = 0;
volatile int leftready = 0;
volatile int rightready = 0;

//AIC default setting value   L-in     R-in    L-H-Vo  R-H-Vo  Ana-Cl  Dig-Cl  Power   Dig-It  SR-Clt  Dig-Act
//                                                             0x0014->micIn
unsigned int aic23_demo[10] = { 0x0017, 0x0017, 0x01f9, 0x01f9, 0x0012, 0x0000, 0x0000, 0x0042, 0x0001, 0x0001};

//leftChannel and rightChannel are the instant values of the value reading from ADC
int leftChannel = 0;
int rightChannel = 0;
int convIndex = 0;

//leftDelay, rightDelay are the values of delay assigned to the left and right channels, respectively
int leftDelay = 0;
int rightDelay = 0;
//smallDelay, largeDelay are the amounts to skip in the buffer to produce delay
int smallDelay = BUFFERSIZE/2;
int largeDelay = 1;
//leftIntensity, rightIntensity are the values that the left and right channel are scaled by
int leftIntensity = 1;
int rightIntensity = 1;
//the intensity values are used to scale volume to emulate the head shadow
int largeIntensity = 9;
int mediumIntensity = 7;
int smallIntensity = 5;

int sound_intensity = 0;
int sound_lowPassIndex = 0;
int sound_direction = 0;

// constant for convolution filter size
int FilterSize = 0;

//Default ADC Sampling frequency = 8k
int sampleFrequency = 0x000C;

//Buffer that store values from different channels
alt_16 leftChannelData[BUFFERSIZE];
alt_16 rightChannelData[BUFFERSIZE];
alt_16 convResultBuffer[CONVBUFFSIZE];

/*uart-global
 * RxHead: integer indicator tells you the index of where the
 * newest char data you received from host computer
 *
 * rx_buffer-> A ring buffer to collect uart data sent by host computer
 * */
alt_16 datatest[256];
unsigned short RxHead = 0;
unsigned char rx_buffer[RX_BUFFER_SIZE];

/*Channel indicators: indicate the index of the most recent collected data*/
int leftCount = 0;
int lefttestCount = 0;
int rightCount = 0;
int calBuffersize = BUFFERSIZE - 1;
int leftBufferFull = 0;

/*Send flag: 1->ok, now send data to the host
 *           0->ok, will not send any data to the host
 *Recv flag: 1->ok, now check and store data from the host
 *           0->ok, will not receive any data from the host
 */
int uartStartSendFlag = 0;
int uartStartRecvFlag = 0;

/*for uart receive purpose*/
int sr = 0;

/*0->do not update sampling frequency*/
/*1->ok, update sampling frequency to AIC23*/
int setFreqFlag = 0;

/*uart object*/
int uart;

/*System initialization function. Should be called before your while(1)*/
void system_initialization() {
	/*Hard-code to 1 right here, you can use ISR
	 *to change the value by yourself
	 */
	uartStartRecvFlag = 1;

	/*Open Uart port and ready to transmit and receive*/
	uart = open(UART_NAME, O_ACCMODE);
	if (!uart) {
		printf("failed to open uart\n");
		//return 0;
	} else {
		printf("Uart ready!\n");
	}

	//Interrupts Registrations
	alt_irq_register(switch0_id, (void *) &switch0, handle_switch0_interrupt);
	alt_irq_register(switch1_id, (void *) &switch1, handle_switch1_interrupt);
	alt_irq_register(key0_id, (void *) &key0, handle_key0_interrupt);
	alt_irq_register(key1_id, (void *) &key1, handle_key1_interrupt);
	alt_irq_register(key2_id, (void *) &key2, handle_key2_interrupt);
	alt_irq_register(key3_id, (void *) &key3, handle_key3_interrupt);
	alt_irq_register(leftready_id, (void *) &leftready, handle_leftready_interrupt_test);
	alt_irq_register(rightready_id, (void *) &rightready, handle_rightready_interrupt_test);

	/*Interrupt enable -> mask to enable it*/
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH0_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH1_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH2_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH3_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH4_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH5_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH6_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH7_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(SWITCH8_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY0_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY1_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY2_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(KEY3_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(LEFTREADY_BASE, 1);
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(RIGHTREADY_BASE, 1);

	/*Reset edge capture bit*/
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH0_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH1_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH2_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH3_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH4_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH5_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH6_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH7_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(SWITCH8_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY0_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY1_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY2_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(KEY3_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(LEFTREADY_BASE, 0);
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(RIGHTREADY_BASE, 0);

	/*turn off all LEDs*/
	IOWR_ALTERA_AVALON_PIO_DATA(LED_BASE, 0x00);

	/*initialize SPI transmission*/
	IOWR_ALTERA_AVALON_PIO_DATA(CS_BASE, 1);
	// ~CS low
	IOWR_ALTERA_AVALON_PIO_DATA(SCLK_BASE, 0);
	// Initialize SCLK to high
}

//filter coefficients used for various filtering based on parameters sent from MATLAB
int backLPFCoeff[20] = {
     197455181,   -34224026,   -88268477,  -140445842,  -150022247,   -87703198,
      48602354,   228507109,   398217088,   501091382,   501091382,   398217088,
     228507109,    48602354,   -87703198,  -150022247,  -140445842,   -88268477,
     -34224026,   197455181
};
int smallLPFCoeff[20] = {
     -35400083,   110713990,    85967137,    22608225,   -78733518,  -137704572,
     -64186806,   161237142,   450838944,   653559961,   653559961,   450838944,
     161237142,   -64186806,  -137704572,   -78733518,    22608225,    85967137,
     110713990,   -35400083
};
int mediumLPFCoeff[20] = {
    -225102044,   -57612199,   -37861087,     2298877,    60350612,   130089428,
     203610690,   270665464,   321634778,   349138722,   349138722,   321634778,
     270665464,   203610690,   130089428,    60350612,     2298877,   -37861087,
     -57612199,  -225102044
};
int largeLPFCoeff[20] = {
     103701003,   210522768,    26613640,   131060508,    99268163,   129613493,
     128664269,   139732203,   142299926,   145561856,   145561856,   142299926,
     139732203,   128664269,   129613493,    99268163,   131060508,    26613640,
     210522768,   103701003
};

//performs convolution
void convolveC(int dataindex, int hFunction[]) {
	int index; // 0
	int count;
	int convResult = 0;
	index = dataindex;
	for (count = 0; count < FilterSize; count++) {
		convResult += hFunction[(calBuffersize - count)] // calBuffersize = BUFFERSIZE - 1;
		* leftChannelData[index % FilterSize];
		index++;
	}
	leftChannelData[convIndex] = convResult;
	rightChannelData[convIndex] = convResult;
	convIndex = (convIndex + 1) % (CONVBUFFSIZE);
}

//implements the filtering based on the lowPassIndex and isBehind parameters
void sound_impl(int distance, int lowPassIndex, int isBehind) {
	//combine the effects of surround sound intensity manipulation and distance intensity manipulation
	int quantizedL = 0; //0-9 representing the quanization of intensity
	int quantizedR = 0; //0-9 representing the quanization of intensity
	quantizedL = leftIntensity - distance;
	quantizedR = rightIntensity - distance;
	if (quantizedL < 0) {
		quantizedL = 0;
	}
	if (quantizedR < 0) {
		quantizedR = 0;
	}
	//instead of linear scale, use inverse square log
	leftIntensity = pow(quantizedL+1,2);
	rightIntensity = pow(quantizedR+1,2);
//	printf("left: %d, right: %d\n",leftIntensity, rightIntensity);
	if (isBehind == 1) {
		FilterSize = 20;
		convolveC(leftCount, backLPFCoeff);
	}
	//if lowPassIndex is a smaller number, use the smallLPFCoeff
	//if lowPassIndex is a larger number, use a more significant LPF that cuts off more frequencies
	if (lowPassIndex < 3) {
		FilterSize = 20;
		convolveC(leftCount,smallLPFCoeff);
	} else if (lowPassIndex < 7) {
		FilterSize = 20;
		convolveC(leftCount,mediumLPFCoeff);
	} else {
		FilterSize = 20;
		convolveC(leftCount,largeLPFCoeff);
	}
}

//converts the three MATLAB sound parameters from characters to integers
void convertSoundParams() {
	sound_direction = sound_direction - '0';
	sound_intensity = sound_intensity - '0';
	sound_lowPassIndex = sound_lowPassIndex - '0';
}

//sound_module takes in the packaged parameters from MATLAB and implements the appropriate effects
void sound_module(int distance, int lowPassIndex, int direction) {
//	printf("distance: %d, lowPassIndex: %d, direction: %d\n", distance, lowPassIndex, direction);
	switch(direction) {
		case 0:
			//front
			leftDelay = 0;
			rightDelay = 0;
			//inherent intensities to help clarify direction
			leftIntensity = largeIntensity;
			rightIntensity = largeIntensity;
			sound_impl(distance, lowPassIndex, 0);
			break;
		case 1:
			//front-right
			leftDelay = smallDelay;
			rightDelay = 0;
			leftIntensity = mediumIntensity;
			rightIntensity = largeIntensity;
			sound_impl(distance, lowPassIndex, 0);
			break;
		case 2:
			//right
			leftDelay = largeDelay;
			rightDelay = 0;
			leftIntensity = smallIntensity;
			rightIntensity = largeIntensity;
			sound_impl(distance, lowPassIndex, 0);
			break;
		case 3:
			//rear-right
			leftDelay = smallDelay;
			rightDelay = 0;
			leftIntensity = smallIntensity;
			rightIntensity = mediumIntensity;
			sound_impl(distance, lowPassIndex, 1);
			break;
		case 4:
			//rear
			leftDelay = 0;
			rightDelay = 0;
			leftIntensity = mediumIntensity;
			rightIntensity = mediumIntensity;
			sound_impl(distance, lowPassIndex, 1);
			break;
		case 5:
			//rear-left
			leftDelay = 0;
			rightDelay = smallDelay;
			leftIntensity = mediumIntensity;
			rightIntensity = smallIntensity;
			sound_impl(distance, lowPassIndex, 1);
			break;
		case 6:
			//left
			leftDelay = 0;
			rightDelay = largeDelay;
			leftIntensity = largeIntensity;
			rightIntensity = smallIntensity;
			sound_impl(distance, lowPassIndex, 0);
			break;
		case 7:
			//front-left
			leftDelay = 0;
			rightDelay = smallDelay;
			leftIntensity = largeIntensity;
			rightIntensity = mediumIntensity;
			sound_impl(distance, lowPassIndex, 0);
			break;
		default:
			break;
	}
}

int main(void) {
	system_initialization();

	// set frequency
	sampleFrequency = 0x0023; //44.1k
	aic23_demo[8] = sampleFrequency;
	AIC23_demo();

	/*Your main infinity while loop*/
	while (1) {
		// always receive bytes from MATLAB
		if (uartStartRecvFlag == 1) {
			uart_byteRecv();
		}
	}

	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	/*!!!!!!!YOUR CODE SHOULD NEVER REACH HERE AND BELOW!!!!!!!*/
	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
	return 0;
}

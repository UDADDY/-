#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#define TRIG 6  // 포트 E의 6번핀
#define ECHO 7  // 포트 E의 7번핀

unsigned char digit[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7c, 0x07, 0x7f, 0x67};
unsigned char fnd_sel[4] = {0x01, 0x02, 0x04, 0x08};
unsigned char fnd[4];
int is_empty = 0;
int criterion = 100; // 1-cm
int count = 0;

void display_fnd(unsigned int value);
unsigned int get_distance_in();
unsigned int get_distance_out();

int main(void)
{
    unsigned int distance_in;
	unsigned int distance_out;
    // fnd
    DDRC = 0xff;
    DDRG = 0x0f;
    // ultrasonicq
 	// TRIG는 출력으로 ECHO는 입력으로 LEFT(E)
    DDRE = ((DDRE|(1<<TRIG)) & ~(1<<ECHO));  

 	// TRIG는 출력으로 ECHO는 입력으로 RIGHT(
    DDRF = ((DDRG|(1<<TRIG)) & ~(1<<ECHO));  

	// led 
	DDRA = 0xff;

	int in_flag = 0;
	int out_flag = 0;

    while(1)
    {
		distance_in = get_distance_in(); // LEFT
		distance_out = get_distance_out(); // RIGHT

		// 바깥 먼저 다음 안 (들어옴)
		if(distance_out < criterion && (in_flag==0)){
			out_flag = 1;
		}

		if(out_flag == 1){	
			if(distance_in < criterion){
				count = count+1;
				out_flag = 0;
        		_delay_ms(1000);
				distance_in = get_distance_in(); // LEFT
			}
		}
		
		
		if(distance_in < criterion && (out_flag==0)){ // 안 먼저 다음 바깥 (나감)
			in_flag = 1;
		}

		if(in_flag == 1){
			if(distance_out < criterion){
				if(count != 0){
					count = count-1;
					in_flag = 0;
        			_delay_ms(1000);
				}
			}
		}


		if(distance_in > criterion && distance_out > criterion){
			out_flag = 0;
			in_flag = 0;
		}

		// 안에 사람이 있으면
		if(count > 0)
			PORTA = 0xff;	
		else
			PORTA = 0x00;

			
        display_fnd(count);

    }
}

void display_fnd(unsigned int value)
{
    int i;
    fnd[3] = (value/1000)%10;
    fnd[2] = (value/100)%10;
    fnd[1] = (value/10)%10;
    fnd[0] = (value/1)%10;
    for(i=0; i<4; i++)
    {
        PORTC = digit[fnd[i]]; //| (i==1 ? 0x80 : 0x00);
        PORTG = fnd_sel[i];
        _delay_ms(2);
        if(i%2)
        _delay_ms(1);
    }
}

unsigned int get_distance_in(){
        TCCR1B = 0x02;  // ????? ????  0b00000010
        PORTE &= ~(1<<TRIG);
        _delay_us(10);
        PORTE |= (1<<TRIG);
        _delay_us(10);
        PORTE &= ~(1<<TRIG);

        while(!(PINE & (1<<ECHO)));
        TCNT1 = 0x0000;
        while(PINE & (1<<ECHO)) ;

        TCCR1B = 0x00;
        return (unsigned int)(TCNT1 / 2 / 5.8);
}

unsigned int get_distance_out(){
        TCCR1B = 0x02;  // ????? ????  0b00000010
        PORTF &= ~(1<<TRIG);
        _delay_us(10);
        PORTF |= (1<<TRIG);
        _delay_us(10);
        PORTF &= ~(1<<TRIG);

        while(!(PINF & (1<<ECHO)));
        TCNT1 = 0x0000;
        while(PINF & (1<<ECHO)) ;

        TCCR1B = 0x00;
        return (unsigned int)(TCNT1 / 2 / 5.8);
}

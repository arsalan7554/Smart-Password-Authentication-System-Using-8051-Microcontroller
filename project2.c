#include <reg51.h>

#define LCD P2

sbit RS = P3^0;
sbit EN = P3^1;
sbit GREEN = P3^2;
sbit RED = P3^3;

char password[] = "1234";
char entered[5];

unsigned char i, j;

/* Delay */
void delay(unsigned int t)
{
    unsigned int x, y;

    for(x = 0; x < t; x++)
        for(y = 0; y < 1275; y++);
}

/* LCD command */
void lcd_cmd(unsigned char cmd)
{
    LCD = cmd;
    RS = 0;
    EN = 1;
    delay(2);
    EN = 0;
    delay(2);
}

/* LCD data */
void lcd_data(unsigned char dat)
{
    LCD = dat;
    RS = 1;
    EN = 1;
    delay(2);
    EN = 0;
    delay(2);
}

/* LCD initialization */
void lcd_init(void)
{
    delay(20);

    lcd_cmd(0x38);  // 8-bit, 2-line LCD
    lcd_cmd(0x0C);  // Display ON
    lcd_cmd(0x06);  // Cursor increment
    lcd_cmd(0x01);  // Clear LCD
    delay(5);
}

/* Display string */
void lcd_string(char *str)
{
    while(*str)
    {
        lcd_data(*str++);
    }
}

/* Keypad scanning */
char keypad(void)
{
    unsigned char row, col;

    char keymap[4][4] = {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    while(1)
    {
        for(row = 0; row < 4; row++)
        {
            P1 = 0xFF;
            P1 &= ~(1 << row);

            for(col = 0; col < 4; col++)
            {
                if((P1 & (0x10 << col)) == 0)
                {
                    delay(20);

                    if((P1 & (0x10 << col)) == 0)
                    {
                        while((P1 & (0x10 << col)) == 0);

                        return keymap[row][col];
                    }
                }
            }
        }
    }
}

/* Main program */
void main(void)
{
    char key;
    unsigned char correct;

    GREEN = 0;
    RED = 0;

    lcd_init();

    while(1)
    {
        i = 0;

        GREEN = 0;
        RED = 0;

        lcd_cmd(0x01);
        lcd_string("Password Lock");
        lcd_cmd(0xC0);
        lcd_string("Enter: ");

        while(i < 4)
        {
            key = keypad();

            if(key >= '0' && key <= '9')
            {
                entered[i] = key;
                lcd_data('*');
                i++;
            }
            else if(key == '*')
            {
                i = 0;
                lcd_cmd(0xC0);
                lcd_string("Enter:         ");
                lcd_cmd(0xC7);
            }
        }

        entered[4] = '\0';

        correct = 1;

        for(j = 0; j < 4; j++)
        {
            if(entered[j] != password[j])
                correct = 0;
        }

        lcd_cmd(0x01);

        if(correct)
        {
            GREEN = 1;
            RED = 0;

            lcd_string("Access Granted");
            lcd_cmd(0xC0);
            lcd_string("Welcome!");
        }
        else
        {
            GREEN = 0;
            RED = 1;

            lcd_string("Wrong Password");
            lcd_cmd(0xC0);
            lcd_string("Try Again");
        }

        delay(2000);

        GREEN = 0;
        RED = 0;
    }
}
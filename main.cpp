#include <SFML/Graphics.hpp>
#include <time.h>
#include <string.h>
#include <gpiod.h>


#define TEST_LOOP 0


long calcTDelta(struct timespec now, struct timespec start)
{
    long delta_S = now.tv_sec - start.tv_sec;
    long delta_mS = ((now.tv_nsec - start.tv_nsec) * 1e-8);
    return ((delta_S*10)+delta_mS);
}

int main()
{
  const char *chipname = "gpiochip0";
  struct gpiod_chip *chip;
  struct gpiod_line *line_AUDIO_Output;
  
  chip = gpiod_chip_open_by_name(chipname);
  line_AUDIO_Output =  gpiod_chip_get_line(chip,6);
  gpiod_line_request_output(line_AUDIO_Output,"example1",0);
  gpiod_line_set_value(line_AUDIO_Output,0);

sf::RenderWindow window(sf::VideoMode(1280, 720), "SC");
sf::Font font;
//if (!font.loadFromFile("WickedGrit.ttf"))
if (!font.loadFromFile("/home/mike/Desktop/SC/GaboDrive Condensed Bold.ttf"))
{
    // error...
}
sf::Text text;       // select the font
text.setFont(font);  // font is a sf::Font
text.setCharacterSize(700); // set the character size in pixels!
text.setFillColor(sf::Color::Red);// set the color
text.setPosition(10.f, -50.f);// set the absolute position of the entity

char mySTR[6];
char lastSTR[6];
int pause = 0; 

struct timespec start, now, end, dBend;
clock_gettime(CLOCK_MONOTONIC, &end);
end.tv_sec = end.tv_sec+24;

long time_spent= 0;
long time_left = 0;
long nS_1SEC   = 1000000000;
long nS_100mS  = 100000000;
int first_run = 1;
int prevKey1State = 0;
int prevKey2State = 0;
int prevKey3State = 0;
int prevKey4State = 0;
struct timespec delay;
delay.tv_sec =0;
delay.tv_nsec = 332*1000;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();
        if ( (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) && prevKey2State==0)
        {
          gpiod_line_set_value(line_AUDIO_Output,0);
          prevKey2State=1; // Pressed
          clock_gettime(CLOCK_MONOTONIC, &end);
          //printf("\n end.tv_sec %ld",end.tv_sec);
          end.tv_sec = end.tv_sec+24;
          //printf("\n end.tv_sec %ld",end.tv_sec);
          pause=0;
        }
        if ( (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) && prevKey1State==0) 
        {
          gpiod_line_set_value(line_AUDIO_Output,0);
          prevKey1State=1; // Pressed
          clock_gettime(CLOCK_MONOTONIC, &end);
          end.tv_sec = end.tv_sec+14;
          pause=0;
        }
        if ( (sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3)) && prevKey3State==0)
        {
          prevKey3State=1; // Pressed
          if (pause == 0)  // RUNNING
           {
                          // RUNNING => PAUSED
              pause=1;
              clock_gettime(CLOCK_MONOTONIC, &now);
              time_left = calcTDelta(end,now);  //in tenthsofseconds
           }
           else if (pause == 1) // PAUSED
           {
                           // PAUSED => RUNNING
              pause=0;
              clock_gettime(CLOCK_MONOTONIC, &end);
              //printf("time_left %ld \n",time_left);
              //printf("time_left %ld \n",time_left/10);
              //printf("time_left %ld \n",time_left%10);  
              //printf("end.tv_sec %ld  \n",end.tv_sec);
              //printf("end.tv_nsec %ld \n",end.tv_nsec); 
                       
              if ( end.tv_nsec < nS_1SEC-((time_left%10) * nS_100mS) )
              {
                  end.tv_nsec = end.tv_nsec+((time_left%10)*nS_100mS);
                  end.tv_sec  = end.tv_sec+(time_left/10);   
              }
              else
              {
                  end.tv_nsec = end.tv_nsec+((time_left%10)*nS_100mS) - nS_1SEC;
                  end.tv_sec  = end.tv_sec+((time_left/10)+1);
              }   
              //printf("end.tv_sec %ld \n",end.tv_sec);
              //printf("end.tv_nsec %ld \n \n",end.tv_nsec);
           }
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
        {
             pause=99;
             gpiod_line_set_value(line_AUDIO_Output,0);
             sprintf(mySTR,"  ");
             text.setString(mySTR);
             strcpy(lastSTR,mySTR);
             window.clear();
             window.draw(text);
             window.display();
        }
        
        
        if ( (sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply))&& prevKey4State==0)
        {
          gpiod_line_set_value(line_AUDIO_Output,0);
          nanosleep(&delay,&delay);
          gpiod_line_set_value(line_AUDIO_Output,1);
          prevKey4State=1; // Pressed
        }

        if (prevKey1State==1)
        {
           if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1)) prevKey1State=0; // Not Pressed
        }
        
        if (prevKey2State==1)
        {
           if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2)) prevKey2State=0; // Not Pressed
        }
        
        if (prevKey3State==1)
        {
           if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad3)) prevKey3State=0; // Not Pressed
        }
        
        if (prevKey4State==1)
        {        
           if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Multiply)) prevKey4State=0; // Not Pressed
        }
         
        if (pause == 0)
        {
         clock_gettime(CLOCK_MONOTONIC, &now);
         time_spent = calcTDelta(end,now);  //in tenthsofseconds
         if (time_spent<=0)   gpiod_line_set_value(line_AUDIO_Output,1); // Small Fudge to make Buzzer Sound at "0.0"
         if (time_spent<=0) 
         {
             if (TEST_LOOP!=1) pause=99;
             if (TEST_LOOP==1) 
             {
                 // Restart at 14s
                 clock_gettime(CLOCK_MONOTONIC, &end);
                 end.tv_sec = end.tv_sec+14;
             }
         }
          
         if (time_spent >50)
          {
            sprintf(mySTR,"  %2.0f",time_spent/10.0);
          }
          else
          {
            sprintf(mySTR," %2.1f",time_spent/10.0);
          }
          text.setString(mySTR);
          
        }
        if (0 != strcmp(lastSTR,mySTR))
        {
          strcpy(lastSTR,mySTR);
          window.clear();
          //window.draw(shape);
          window.draw(text);
          window.display();
          if (first_run == 1)
          {
            first_run = 0;
            pause=1;
            printf("PAUSED  \n");
          }
        }
    }

    return 0;
}

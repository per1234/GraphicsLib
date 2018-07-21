#include "Graphics.h"

//#define ESPSCREEN
#define ESPSHIELD

#ifdef WAVESHARE
#include "WaveShare42B.h"
#define CS		D3
#define RST		D2
#define DC		D8
#define BUSY	D1
WaveShare42B display(CS, RST, DC, BUSY);
#endif

#ifdef ESPSCREEN
#include "SH1106.h"
SH1106 display(0x3c, D1, D2);
#endif

#ifdef ESPSHIELD
#include "SSD1306.h"
#include "SH1106.h"
SSD1306 display(0x3c, SDA, SCL);
#endif


Graphics graphics(&display);

void setup()
{
	Serial.begin(115200);
	Serial.println("Yay!");
	graphics.begin();
	Serial.println("Started Graphics");
  /* add setup code here */

#ifdef WAVESHARE
	for (int x = 0; x < graphics.getWidth(); x += 100)
		for (int y = 0; y < graphics.getWidth(); y += 100)
			graphics.drawRect(x + 10, y + 10, x + 90, y + 90, WaveShare42B::Black);
#endif
#ifdef ESPSCREEN
	graphics.drawLine(10, 10, 64, 50, SH1106::White);
#endif
	graphics.display();

	Serial.println("Booted");

	myinit();
}

#define POINTCOUNT 5
#define LINECOUNT 7

int currentIndex = 0;
int points[LINECOUNT][POINTCOUNT][2];
int directions[POINTCOUNT][2];

void myinit()
{
	for (int i = 0; i < POINTCOUNT; i++)
	{
		points[currentIndex][i][0] = rand() % graphics.getWidth();
		points[currentIndex][i][1] = rand() % graphics.getHeight();

		directions[i][0] = (rand() % 6) - 3;
		directions[i][1] = (rand() % 6) - 3;
	}
}

void loop()
{
	graphics.clear(SH1106::Black);
	graphics.setColor(SH1106::White);
	graphics.drawRect(0, 0, graphics.getWidth()-1, graphics.getHeight()-1);

	int i = (currentIndex + 1) % LINECOUNT;
	for (int ii = 0; ii < POINTCOUNT; ii++)
	{
		points[i][ii][0] = points[currentIndex][ii][0] + directions[ii][0];
		points[i][ii][1] = points[currentIndex][ii][1] + directions[ii][1];

		if (points[i][ii][0] < 0 || points[i][ii][0] >= graphics.getWidth())
			directions[ii][0] = -directions[ii][0];
		if (points[i][ii][1] < 0 || points[i][ii][1] >= graphics.getHeight())
			directions[ii][1] = -directions[ii][1];

	}


	while (i != currentIndex)
	{
		for (int l = 0; l < POINTCOUNT; l++)
		{
			int l2 = (l + 1) % POINTCOUNT;
			graphics.drawLine(points[i][l][0], points[i][l][1], points[i][l2][0], points[i][l2][1]);
		}
		i = (i + 1) % LINECOUNT;
	}

	currentIndex = (currentIndex + 1) % LINECOUNT;



	graphics.display();

	delay(25);
	/* add main program code here */

}

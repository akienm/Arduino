// LED Fire Effect

int redPin = 9;
int greenPin = 10;
int bluePin = 11;

int redMin = 128;
int greenMin = 32;
int blueMin = 0;

int redBias = 128;
int greenBias = 64;
int blueBias = 0;

int redEnable = 1;
int greenEnable = 1;
int blueEnable = 1;

int minDelayTime = 50;
int delayTime = 300;

void setup()
{
pinMode(redPin, OUTPUT);
pinMode(greenPin, OUTPUT);
pinMode(bluePin, OUTPUT);
}

void loop() {
analogWrite(redPin, (random(redBias) + redMin) * redEnable);
analogWrite(greenPin, (random(greenBias) + greenMin) * greenEnable);
analogWrite(bluePin, (random(blueBias) + blueMin) * blueEnable);

delay(minDelayTime + random(delayTime));
}

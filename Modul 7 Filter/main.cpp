#include <Arduino.h>
#include <Ewma.h>

Ewma adcFilter1(0.1);   // Less smoothing - faster to detect changes, but more prone to noise
Ewma adcFilter2(0.01);  // More smoothing - less prone to noise, but slower to detect changes
// put function declarations here:
double median(int n, double x[]);
double c;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(A0, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  double temps[3];
  double med;
  int n;
  int raw = analogRead(A0);
  for (n = 0; n < 3; n++)
  {
    temps[n] = analogRead(A0)/1;
  }
  med = median(5, temps);

    
    float filtered1 = adcFilter1.filter(raw);
    float filtered2 = adcFilter2.filter(raw);
    //Serial.printl("Median=%d,Raw=%d, Filter1=%.3f, Filter2=%.3f", med,raw, filtered1, filtered2);
    Serial.printf("%d,%1.0f,%1.0f,%1.0f",raw, med,filtered1,filtered2);
    // char data[10];
    // sprintf(data,"%1.4f",filtered1); 
    // Serial.print(data);

    Serial.println();
    delay(100);

}

double median(int n, double x[])
{
  double temp;
  int i, j;
  // the following two loops sort the array x in ascending order
  for (i = 0; i < n - 1; i++) {
    for (j = i + 1; j < n; j++) {
      if (x[j] < x[i]) {
        // swap elements
        temp = x[i];
        x[i] = x[j];
        x[j] = temp;
      }
    }
  }
  if (n % 2 == 0) {
    // if there is an even number of elements, return mean of the two elements in the middle
    return ((x[n / 2] + x[n / 2 - 1]) / 2.0);
  } else {
    // else return the element in the middle
    return x[n / 2];
  }
}
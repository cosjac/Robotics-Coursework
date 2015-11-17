#ifndef _global_functions_h
#define _global_functions_h

extern char gbuffer[160];
extern int gnumbuffer[80];
extern long isGreenVisable;

void takeImage();
void processImage();
int gturnDirection();
void gturn(void);
void forward(void);
void setUpCamera(void);
int greenIsInMiddle(int centreValue);

#endif

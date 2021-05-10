#ifndef PRIMER
#define PRIMER

void hstack(float x[], float y[], float z[], float output[][3], unsigned int arrLen);
void linInter(float fi[], float ti[], unsigned int szeIn, float fo[], float to[], int szeOut);
void cubInter(float fi[], float ti[], unsigned int szeIn, float fo[], float to[], int szeOut);
extern double interPt(float f[], float t[], unsigned int arrSze, float tIn);
extern double cubInterPt(float f[], float t[], unsigned int arrSze, float tIn);



#endif //PRIMER

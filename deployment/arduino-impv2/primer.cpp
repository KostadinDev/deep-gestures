#include "primer.h";

/**
 * Combines the three x/y/z vectors into the included output vector
 */
void hstack(float x[], float y[], float z[], float output[][3], unsigned int arrLen){
    for (int i = 0; i < arrLen; i++){
      output[i][0] = x[i];
      output[i][1] = y[i];
      output[i][2] = z[i];
    }
}



/**
 * Takes input/output time arrays and input/output x/y/z arrays and fills in
 * the output arrays with interpolated data by calling interPt() on each member of the array
 * 
 * fi[] -> x/y/z input
 * ti[] -> time input
 * szeIn -> size of x/y/z input and time array
 * 
 * fo[] -> x/y/z output (all data will be overwritten)
 * to[] -> time output (should be already filled)
 * szeOut -> size of x/y/z output and time array

 */
void linInter(float fi[], float ti[], unsigned int szeIn, float fo[], float to[], int szeOut){
  for(int i = 0; i < szeOut; i++){
    fo[i] = interPt(fi, ti, szeIn, to[i]);
  }
}


/**
 * Takes an array of times, the correstponding x/y/z point associated with each time,
 * the array size, aswell as a vallue of time within the range of the times stated within the
 * input time vector
 * 
 * f[] -> x/y/z array
 * t[] -> associated time array
 * arrSize -> size of the array
 * tIn -> time whose interpolated x/y/z value is desired
 * 
 * returns an x/y/z value between the points.
 */
double interPt(float f[], float t[], unsigned int arrSze, float tIn){
  if(tIn < t[0]) // Exits if the input t is lower than the first value of the array due to unsifficient data
    return(f[0]);

    
  // Code body -> if
  int i;
  float prev = 0;
  for(i = 0; i < arrSze; i++){
    if(tIn < t[i] && tIn > prev ) break;
    prev = t[i];
  }
  i--;
  return((f[i]*(t[i+1] - tIn) + f[i+1]*(tIn - t[i]))/(t[i+1] - t[i]));
}

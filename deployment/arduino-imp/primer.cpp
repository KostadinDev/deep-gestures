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

/**
 * Takes input/output time arrays and input/output x/y/z arrays and fills in
 * the output arrays with interpolated data by calling cubInterPt() on each member of the array
 * 
 * fi[] -> x/y/z input
 * ti[] -> time input
 * szeIn -> size of x/y/z input and time array
 * 
 * fo[] -> x/y/z output (all data will be overwritten)
 * to[] -> time output (should be already filled)
 * szeOut -> size of x/y/z output and time array

 */
void cubInter(float fi[], float ti[], unsigned int szeIn, float fo[], float to[], int szeOut){
  for(int i = 0; i < szeOut; i++){
    fo[i] = cubInterPt(fi, ti, szeIn, to[i]);
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
 * returns an x/y/z value between the points using cubic interpolation.
 */
double cubInterPt(float f[], float t[], unsigned int arrSze, float tIn){
  // Check that the tIn is greater than t[0] and less than t[arrSze-1]
  if(tIn <= t[0]){
      return f[0];
  }
  if (tIn >= t[arrSze-1]){
      return f[arrSze-1];
  }
  // Declare 4 points (p_0, p_1, p_2, and p_3) to compute the cubic interpolation
  float p_0, p_1, p_2, p_3;
  // Find the index of the last point before tIn
  int i;
  float prev = 0;
  for(i = 0; i < arrSze; i++){
  if(tIn < t[i] && tIn > prev ) break;
  prev = t[i];
  }
  i--;
  // Assign the points based on the value of i
  p_0 = f[i-1];
  p_1 = f[i];
  p_2 = f[i+1];
  p_3 = f[i+2];
  // Check if there are sufficient data points for interpolation (4 are required -- 2 before tIn and 2 after)
  if (i < 1){
  p_0 = f[i];
  }
  if (i == arrSze-2){
  p_3 = f[i+1];
  }
  if (i == arrSze-1){
  return f[arrSze-1];
  }
  // Compute the t parameter given tIn and the times of the data points
  float t_param = (tIn - t[i])/(t[i+1] - t[i]);
  // Compute the 4 coefficients (a_0, a_1, a_2, and a_3) for x/y/z for the interpolation computation
  float d_1 = (p_2 - p_0) * 0.5;
  float d_2 = (p_3 - p_1) * 0.5;
  float a_0 = p_1;
  float a_1 = d_1;
  float a_2 = ((p_2 - p_1) * 3.0) - (d_1 * 2.0) - d_2;
  float a_3 = ((p_1 - p_2) * 2.0) + d_1 + d_2;
  // return the computed missing point
  return a_0 + (a_1 * t_param) + (a_2 * t_param * t_param) + (a_3 * t_param * t_param * t_param);
}

/**
*   This file was downloaded from the Open303 github repository
*   
*   https://github.com/maddanio/open303
* 
*   The project is licensed under the MIT License :

Copyright (c) 2009 Robin Schmidt (www.rs-met.com)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*/

#include "rosic_OnePoleFilter.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

OnePoleFilter::OnePoleFilter()
{
  shelvingGain = 1.0;
  setSampleRate(44100.0);  // sampleRate = 44100 Hz by default
  setMode      (0);        // bypass by default
  setCutoff    (20000.0);  // cutoff = 20000 Hz by default
  reset();                 // reset memorized samples to zero
}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void OnePoleFilter::setSampleRate(double newSampleRate)
{
  if( newSampleRate > 0.0 )
    sampleRate = newSampleRate;
  sampleRateRec = 1.0 / sampleRate;

  calcCoeffs();
  return;
}

void OnePoleFilter::setMode(int newMode)
{
  mode = newMode; // 0:bypass, 1:Low Pass, 2:High Pass
  calcCoeffs();
}

void OnePoleFilter::setCutoff(double newCutoff)
{
  if( (newCutoff>0.0) && (newCutoff<=20000.0) )
    cutoff = newCutoff;
  else
    cutoff = 20000.0;

  calcCoeffs();
  return;
}

void OnePoleFilter::setShelvingGain(double newGain)
{
  if( newGain > 0.0 )
  {
    shelvingGain = newGain;
    calcCoeffs();
  }
  else
    DEBUG_BREAK; // this is a linear gain factor and must be >= 0.0
}

void OnePoleFilter::setShelvingGainInDecibels(double newGain)
{
  setShelvingGain(dB2amp(newGain));
}

void OnePoleFilter::setCoefficients(double newB0, double newB1, double newA1)
{
  b0 = newB0;
  b1 = newB1;
  a1 = newA1;
}

void OnePoleFilter::setInternalState(double newX1, double newY1)
{
  x1 = newX1;
  y1 = newY1;
}

//-------------------------------------------------------------------------------------------------
//others:

void OnePoleFilter::calcCoeffs()
{
  switch(mode)
  {
  case LOWPASS: 
    {
      // formula from dspguide:
      double x = exp( -2.0 * PI * cutoff * sampleRateRec); 
      b0 = 1-x;
      b1 = 0.0;
      a1 = x;
    }
    break;
  case HIGHPASS:  
    {
      // formula from dspguide:
      double x = exp( -2.0 * PI * cutoff * sampleRateRec);
      b0 =  0.5*(1+x);
      b1 = -0.5*(1+x);
      a1 = x;
    }
    break;
  case LOWSHELV:
    {
      // formula from DAFX:
      double c = 0.5*(shelvingGain-1.0);
      double t = tan(PI*cutoff*sampleRateRec);
      double a;
      if( shelvingGain >= 1.0 )
        a = (t-1.0)/(t+1.0);
      else
        a = (t-shelvingGain)/(t+shelvingGain);

      b0 = 1.0 + c + c*a;
      b1 = c + c*a + a;
      a1 = -a;
    }
    break;
  case HIGHSHELV:
    {
      // formula from DAFX:
      double c = 0.5*(shelvingGain-1.0);
      double t = tan(PI*cutoff*sampleRateRec);
      double a;
      if( shelvingGain >= 1.0 )
        a = (t-1.0)/(t+1.0);
      else
        a = (shelvingGain*t-1.0)/(shelvingGain*t+1.0);

      b0 = 1.0 + c - c*a;
      b1 = a + c*a - c;
      a1 = -a;
    }
    break;

  case ALLPASS:  
    {
      // formula from DAFX:
      double t = tan(PI*cutoff*sampleRateRec);
      double x = (t-1.0) / (t+1.0);

      b0 = x;
      b1 = 1.0;
      a1 = -x;
    }
    break;

  default: // bypass
    {
      b0 = 1.0;
      b1 = 0.0;
      a1 = 0.0;
    }break;
  }
}

void OnePoleFilter::reset()
{
  x1 = 0.0;
  y1 = 0.0;
}

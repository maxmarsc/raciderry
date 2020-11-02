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

#include "rosic_TeeBeeFilter.h"
using namespace rosic;

//-------------------------------------------------------------------------------------------------
// construction/destruction:

TeeBeeFilter::TeeBeeFilter()
{
  cutoff              =  1000.0;
  drive               =     0.0;
  driveFactor         =     1.0;
  resonanceRaw        =     0.0;
  resonanceSkewed     =     0.0;
  g                   =     1.0;
  sampleRate          = 44100.0;
  twoPiOverSampleRate = 2.0*PI/sampleRate;

  feedbackHighpass.setMode(OnePoleFilter::HIGHPASS);
  feedbackHighpass.setCutoff(150.0);

  //setMode(LP_18);
  setMode(TB_303);
  calculateCoefficientsExact();
  reset();
}

TeeBeeFilter::~TeeBeeFilter()
{

}

//-------------------------------------------------------------------------------------------------
// parameter settings:

void TeeBeeFilter::setSampleRate(double newSampleRate)
{
  if( newSampleRate > 0.0 )
    sampleRate = newSampleRate;
  twoPiOverSampleRate = 2.0*PI/sampleRate;
  feedbackHighpass.setSampleRate(newSampleRate);
  calculateCoefficientsExact();
}

void TeeBeeFilter::setDrive(double newDrive)
{
  drive       = newDrive;
  driveFactor = dB2amp(drive);
}

void TeeBeeFilter::setMode(int newMode)
{
  if( newMode >= 0 && newMode < NUM_MODES )
  {
    mode = newMode;
    switch(mode)
    {
    case FLAT:      c0 =  1.0; c1 =  0.0; c2 =  0.0; c3 =  0.0; c4 =  0.0;  break;
    case LP_6:      c0 =  0.0; c1 =  1.0; c2 =  0.0; c3 =  0.0; c4 =  0.0;  break;
    case LP_12:     c0 =  0.0; c1 =  0.0; c2 =  1.0; c3 =  0.0; c4 =  0.0;  break;
    case LP_18:     c0 =  0.0; c1 =  0.0; c2 =  0.0; c3 =  1.0; c4 =  0.0;  break;
    case LP_24:     c0 =  0.0; c1 =  0.0; c2 =  0.0; c3 =  0.0; c4 =  1.0;  break;
    case HP_6:      c0 =  1.0; c1 = -1.0; c2 =  0.0; c3 =  0.0; c4 =  0.0;  break;
    case HP_12:     c0 =  1.0; c1 = -2.0; c2 =  1.0; c3 =  0.0; c4 =  0.0;  break;
    case HP_18:     c0 =  1.0; c1 = -3.0; c2 =  3.0; c3 = -1.0; c4 =  0.0;  break;
    case HP_24:     c0 =  1.0; c1 = -4.0; c2 =  6.0; c3 = -4.0; c4 =  1.0;  break;
    case BP_12_12:  c0 =  0.0; c1 =  0.0; c2 =  1.0; c3 = -2.0; c4 =  1.0;  break;
    case BP_6_18:   c0 =  0.0; c1 =  0.0; c2 =  0.0; c3 =  1.0; c4 = -1.0;  break;
    case BP_18_6:   c0 =  0.0; c1 =  1.0; c2 = -3.0; c3 =  3.0; c4 = -1.0;  break;
    case BP_6_12:   c0 =  0.0; c1 =  0.0; c2 =  1.0; c3 = -1.0; c4 =  0.0;  break;
    case BP_12_6:   c0 =  0.0; c1 =  1.0; c2 = -2.0; c3 =  1.0; c4 =  0.0;  break;
    case BP_6_6:    c0 =  0.0; c1 =  1.0; c2 = -1.0; c3 =  0.0; c4 =  0.0;  break;
    default:        c0 =  1.0; c1 =  0.0; c2 =  0.0; c3 =  0.0; c4 =  0.0;  // flat
    }
  }
  calculateCoefficientsApprox4();
}

//-------------------------------------------------------------------------------------------------
// others:

void TeeBeeFilter::reset()
{
  feedbackHighpass.reset();
  y1 = 0.0;
  y2 = 0.0;
  y3 = 0.0;
  y4 = 0.0;
}

/////////////// Scanner global/constant vars
float dutytime = 0;
float breaktime = 0;
int iSpeedComp = 0;
float nAngle;
float nRadius;
int sectorAvgDist[iSectorCount];//averaged distances for sectors
int sectorAvgDistBlur[iSectorCount];//blurred averaged distances for sectors
int X, Y, X2, Y2;
int iScanArray[200];
int iScanCount = 0;
int iSectorSize = 0;
bool bNewScan = false;//flag for telling when the scan has completed
bool bScanInProgress = false;
int iSectorSum = 0;
int iDist = 0;
float TicksKeyPulse = 0;

const TTimers  DutyTimer = T2;
int iAngle = 145;
const int iStartAngle = 280;//starting angle for scan display, arrange these so the full angle will divide with the sectorcount
const int iEndAngle = 75;//ending angle for scan display
const bool bScanDirection = true;//from 0 to 100, not 100 to 0

const int iMaxRange = 200; // how far the maximum object can be
int iConfPasses = 2;



//////////////////////////// Scanner task////////////////////////////
task TScanCycle()
{
  bool flipflop = false;
  int lastpulsecount = 0;
  float nSyncError = 0;
 TicksKeyPulse = (float)iAngle / (float)iPulseCount;

  while(true)
  {

    if(SensorValue(Sync) == syncEdgePositive)
    {
      bScanInProgress = true;

      /////////////////////// GET DEFAULT VARIABLES ///////////////////


      ///////////////////////////////// INTERFACE GRAPHICS ///////////////////////////////////

      if(DrawView == 0)//if raw data visualization is enabled
      {
        eraseDisplay();
        nxtDrawLine(50, 0, 50, 64);//draw overlay
        for(int liney = 0; liney <= 60; liney += 5)
        {
          nxtDrawLine(47, liney, 53, liney); // draw minilines
          X = sinDegrees(iStartAngle) * 100;
          Y = cosDegrees(iStartAngle) * 100;
          nxtDrawLine(50, 0, X + 50, Y);
          X = sinDegrees(iEndAngle) * 100;
          Y = cosDegrees(iEndAngle) * 100;
          nxtDrawLine(50, 0, X + 50, Y);
        }
      }

      iScanCount = 0;
      nAngle = iStartAngle;
      //iCountedPulses = 0;
      wait1Msec(25);
      while(nAngle != iEndAngle)///////////////// ACTIVE SCANNING CYCLE BEGINS /////////////////////////
      {
        breaktime = iSyncDelays[iCountedPulses] / TicksKeyPulse;
        dutytime = time1[DutyTimer];

        nSyncError = iScanCount - (float)((float)iCountedPulses * (float)TicksKeyPulse);

        if(nSyncError < 0)
        {
          breaktime = 0;
          dutytime = 0;
        }
        if(nSyncError > 1)
        {
          breaktime += 25;
        }

        wait1Msec(breaktime - dutytime);//set the delay between every scan, so they will be spread out within the scanning cone
        time1[DutyTimer] = 0;
        iDist = (float)SensorValue[Sonar];//get distance

        iScanArray[iScanCount] = iDist;//save results to array

        if (iDist > 200)//set distance upper limit for display
          iDist = 200;

        nRadius = 0.5 * iDist;//scale the values down

        if(flipflop)//draw a line where the minimal crash distance is
        {
          X = sinDegrees(nAngle) * (10);
          Y = cosDegrees(nAngle) * (10);
          if(DrawView == 0)//if raw data visualization is enabled
            nxtSetPixel(X + 50, Y);
          X = sinDegrees(nAngle) * 50;
          Y = cosDegrees(nAngle) * 50;
          if(DrawView == 0)//if raw data visualization is enabled
            nxtSetPixel(X + 50, Y);
        }

        flipflop = !flipflop;
        if(iCountedPulses != lastpulsecount)
        {
          X = sinDegrees(nAngle) * 45;
          Y = cosDegrees(nAngle) * 45;
          X2 = sinDegrees(nAngle) * 50;
          Y2 = cosDegrees(nAngle) * 50;
          if(DrawView == 0)//if raw data visualization is enabled
            nxtDrawLine(X + 50, Y, X2 + 50, Y2);
          lastpulsecount = iCountedPulses;
        }

        if(nRadius > 0)
        {
          X = sinDegrees(nAngle) * nRadius;
          Y = cosDegrees(nAngle) * nRadius;
          X2 = sinDegrees(nAngle) * (nRadius + iScanDisplayThickness);
          Y2 = cosDegrees(nAngle) * (nRadius + iScanDisplayThickness);
          if(DrawView == 0)//if raw data visualization is enabled
            nxtDrawLine(X + 50, Y, X2 + 50, Y2);
        }

        if(bScanDirection)//set drawing direction
          nAngle++;
        else
          nAngle--;

        if (nAngle == -1)//reset angle underfllow
          nAngle = 359;

        if (nAngle == 360)//reset angle overfllow
          nAngle = 0;

        iScanCount++;
        iAngle = iScanCount;


      }

      while(SensorValue(Sync) == syncEdgePositive)
      {}
      TicksKeyPulse = (float)iAngle / (float)iPulseCount;
      if(iConfPasses == 0)
        bScanInProgress = false;
          else
        iConfPasses--;

      bNewScan = true;
    }
  }
}

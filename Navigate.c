//////////////Navigation Vars
//int sectorweights[iSectorCount];//values for determining how much a specific sector "costs", this is used so the robot will prefer going forward when possible
int iSectorWghArray[iSectorCount];//weighted sector values, combined with sectorweights
int iCenterSectorIndx = 0;//automatic

const int iStuckThres = 10;//nose is stuck
const int iBlockedThres = 35;// if the best sector is below this, then back up and turn around
const int iBlurScope = 6;//blur iBlurScope, how mani neigbouring sectors are taken into account when averaging
//const int sectorstrength = 12; // how much every sector affects its own blurred value
const int sPercent = 60;// how much the sector affects its own blurred value

//////////////Display Vars
int iBarThickness = 100 / iSectorCount;

int bestsectorindex;
int bestsectorscore;



//////////////////////////
////////NAVIGATION AI////////
//////////////////////////
int getArIndexMax(int index, int iBlurScope, int maxval)// returns if requested index is within bounds of iBlurScope
{
  int roomtomax = maxval - (index + iBlurScope);
  if(roomtomax < 0)
    return iBlurScope + roomtomax;
  else return iBlurScope;
}
int getArIndexMin(int index, int iBlurScope)// returns if requested index is within bounds of iBlurScope
{
  int roomtozero = index - iBlurScope;
  if(roomtozero < 0)
    return abs(iBlurScope + roomtozero);
  else return iBlurScope;
}
void CalcSectorAvg()
{
  for(int iCurSector = 0; iCurSector < iSectorCount; iCurSector++)
  {
    iSectorSum = 0;
    for(int scnindx = (iSectorSize * iCurSector); scnindx < ((iSectorSize * iCurSector) + iSectorSize); scnindx++)//divide readings into sectors
    {
      iSectorSum += iScanArray[scnindx];
    }
    sectorAvgDist[iCurSector] = iSectorSum / iSectorSize;//get average of readings
    if(sectorAvgDist[iCurSector] > iMaxRange)//cut off too large readings
      sectorAvgDist[iCurSector] = iMaxRange;
  }

  if(DrawView == 1)//proccessed data visualisation enabled
  {
    eraseDisplay();
    for(int iCurSect = 0; iCurSect < iSectorCount; iCurSect++)
    {
      nxtFillRect(iCurSect * iBarThickness, sectorAvgDist[iCurSect] / iDistanceDispScale, (iCurSect * iBarThickness) + iBarThickness, 0);//draw a black bar
    }
  }
}
void CalcSectorBlur()
{
  int roommax = 0;
  int roommin = 0;
  int sum = 0;
  int items = 0;
  int oCount = 2 * iBlurScope;
  int oPercent = 0;
  int sCount = 0;
  for(int iCurSect = 0; iCurSect < iSectorCount; iCurSect++)//blur the sectors with other sector values(to keep away from near misses from sharp edges)
  {
    sum = 0;
    items = 0;
    roommax = getArIndexMax(iCurSect, iBlurScope, iSectorCount);//room to zero element
    roommin = getArIndexMin(iCurSect, iBlurScope);//room to max element
    int minsectval = iCurSect - roommin;
    int maxsectval = iCurSect + roommax;

    for(int cursec = minsectval; cursec < maxsectval; cursec++)//sum up neighbour sectors
    {
      if(cursec != iCurSect)
      {
        items++;
        sum += sectorAvgDist[cursec];
      }
    }
    oCount = 2 * iBlurScope;
    oPercent = 100 - sPercent;
    sCount = (oCount / oPercent) * sPercent;

    items += sCount;
    sum += sectorAvgDist[iCurSect] * sCount;

    sectorAvgDistBlur[iCurSect] = sum / items;

    if(DrawView == 2)//proccessed data visualisation enabled
    {
      eraseDisplay();
      for(int iCurSect = 0; iCurSect < iSectorCount; iCurSect++)
      {
        nxtFillRect(iCurSect * iBarThickness, sectorAvgDistBlur[iCurSect] / iDistanceDispScale, (iCurSect * iBarThickness) + iBarThickness, 0);//draw a black bar
      }
    }
  }

}
void GetWeightedBestSector()
{
  int cursectorscore = 0;
  bestsectorscore = 0;
  for(int iCurSect = 0; iCurSect < iSectorCount; iCurSect++)//account for sector weights
  {
    cursectorscore = sectorAvgDistBlur[iCurSect] - (abs(iCurSect - iCenterSectorIndx) * (abs(iCurSect - iCenterSectorIndx)) / 4);//get the appropriate weight from the weights function
    iSectorWghArray[iCurSect] = cursectorscore;

    if(cursectorscore > bestsectorscore)//determine the most free sector
    {
      bestsectorindex = iCurSect;
      bestsectorscore = cursectorscore;
    }
  }

  if(DrawView == 3)//proccessed data visualisation enabled
  {
    eraseDisplay();
    for(int iCurSect = 0; iCurSect < iSectorCount; iCurSect++)
    {
      nxtFillRect(iCurSect * iBarThickness, iSectorWghArray[iCurSect] / iDistanceDispScale, (iCurSect * iBarThickness) + iBarThickness, 5);//draw a black bar
    }
    nxtDrawCircle(bestsectorindex * iBarThickness, 5, 5);
  }
}
task TNav()
{

  //how much every direction "costs", to keep the machine going mostly forward

  int directiondegrees;

  while(true)
  {
    while(!bNewScan)
    {
      wait1Msec(50);
    }
    bNewScan = false;
    iCenterSectorIndx = (iSectorCount - 1) / 2;//get the middle sector's index
    iSectorSize = iScanCount / iSectorCount;//calculate the size of each sector


    CalcSectorAvg();

    CalcSectorBlur();

    GetWeightedBestSector();

    directiondegrees = (bestsectorindex - iCenterSectorIndx) * iSectorSize;//convert sector num to zero reference

    if(sectorAvgDist[iCenterSectorIndx - 2] < iStuckThres || sectorAvgDist[iCenterSectorIndx - 1] < iStuckThres || sectorAvgDist[iCenterSectorIndx] < iStuckThres || sectorAvgDist[iCenterSectorIndx + 1] < iStuckThres || sectorAvgDist[iCenterSectorIndx + 2] < iStuckThres)//stuck with nose
    {
      Movement(false, 100, 20);//back up a bit
      MovementTurn(false, 100, 10);
    }
    else
    {


      if(bestsectorscore < iBlockedThres)//all blocked, turn 180
      {
        Movement(false, 60, 15);
        TightFlip(true, 90);
      }
      else
      {
        if(directiondegrees > 0)//Right side turn
          MovementTurn(true, 90, directiondegrees);//turn right 100% power to specified angle
        else if(directiondegrees < 0)//left side turn
          MovementTurn(false, 90, -directiondegrees);

        Movement(true, 90, (float)sectorAvgDistBlur[bestsectorindex] * 0.5);//forward 1/3 the measured distance
        while(bScanInProgress)
        {wait1Msec(1);}
        bNewScan = false;
      }
    }
    if(DrawView == 4)//proccessed data visualisation enabled
    {
      eraseDisplay();
      nxtDisplayCenteredTextLine(0, "P.Count,Wides.,Deg");

      nxtDisplayCenteredTextLine(1, "%d", iPulseCount);
      nxtDisplayCenteredTextLine(2, "%d", iSpeedComp);

      nxtDisplayCenteredTextLine(3, "%d", directiondegrees);

      nxtDisplayCenteredTextLine(4, "Planned distance");
      nxtDisplayCenteredTextLine(5, "%d", (float)sectorAvgDistBlur[bestsectorindex] * 0.5);

      nxtDisplayCenteredTextLine(6, "Forward score");
      nxtDisplayCenteredTextLine(7, "%d", sectorAvgDistBlur[iCenterSectorIndx]);


      wait1Msec(1000);
    }
  }
  return;
}

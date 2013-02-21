int iPulseCount = 0;
int iSyncDelays[50];
int iCountedPulses = 0;
bool bLDRValue = false;
bool bEncCal = false;
const TTimers  SyncTimer = T1;
const int LDRThreshold = 50;
//bool bResetEnc = false;
//////////////////////////
///////ENCODER////////////
//////////////////////////

void GetPulseCount()
{
  iCountedPulses = 0;
  bEncCal = true;
  int synctime = 0;
  bool laststate = true;
  wait1Msec(100);
  while(SensorValue(Sync) == 1)
  {
    wait1Msec(500);
  }
  while(SensorValue(Sync) == 0)
  {
    wait1Msec(5);
  }
  time1[SyncTimer] = 0;
  while(SensorValue(Sync) == 1)
  {
    if(SensorValue[LDR] > LDRThreshold)
      bLDRValue = true;
    else
      bLDRValue = false;

    if(bLDRValue != laststate)
    {
      synctime = time1[SyncTimer];
      iSyncDelays[iCountedPulses] = synctime;
      time1[SyncTimer] = 0;
      laststate = bLDRValue;
      iCountedPulses++;
      eraseDisplay();
      nxtDisplayCenteredTextLine(0, "Calibrating ENC");
      nxtDisplayCenteredBigTextLine(2, "%d", iCountedPulses);
    }
    if(time1[SyncTimer] > 1000)
    {
      SetError("Cal. Enc. Error 1");
    }
  }
  if(iCountedPulses == 0)
  {
    SetError("Cal. Enc. Error 2");
    iPulseCount = 21;
  }
  else
    iPulseCount = iCountedPulses;

  wait1Msec(1000);
  bEncCal = false;
}
//////////////////// Encoder task ///////////////////////////////////
task TEnc()
{
  int iBarThickness;
  int synctime;
  bool laststate;
  time1[SyncTimer] = 0;
  while(true)
  {
    //time1[SyncTimer] = 0;

    time1[SyncTimer] = 0;

    while(!bEncCal && SensorValue(Sync) == syncEdgePositive)
    {
      if(SensorValue[LDR] > LDRThreshold)
        bLDRValue = true;
      else
        bLDRValue = false;

      if(bLDRValue != laststate)
      {
        synctime = time1[SyncTimer];
        // if(iCountedPulses >= iPulseCount)
        // iSyncDelays[0] = synctime;
        // else if(iCountedPulses != 0)
        iSyncDelays[iCountedPulses] = synctime;
        time1[SyncTimer] = 0;
        laststate = bLDRValue;
        iCountedPulses++;
      }
      if(time1[SyncTimer] > 500 )
      {
        SetError("Encoder Error");
      }
    }
    iPulseCount = iCountedPulses;

    while(SensorValue(Sync) != syncEdgePositive)
    {
        iCountedPulses = 0;
    }
       if(DrawView == 5)//proccessed data visualisation enabled
    {
      iBarThickness = 100 / iPulseCount;
      eraseDisplay();
      for(int i = 0; i < iPulseCount; i++)
      {
        nxtFillRect(i * iBarThickness, iSyncDelays[i] / iDistanceDispScale, (i * iBarThickness) + iBarThickness, 0);//draw a black bar
      }
    }
    // if(iCountedPulses > iPulseCount)
    // iPulseCount++;
    // if(iCountedPulses < iPulseCount)
    // iPulseCount--;

  }

}

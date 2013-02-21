bool bError = false;
string sError = "No Error";

/////////////////////////////////
//////////ERROR HANDLING/////////
/////////////////////////////////
void SetError(string text)
{
  PlaySound(soundBeepBeep);
  bError = true;
  sError = text;
  while(bError)
    wait1Msec(500);
}
void ClearErrors()
{
  bError = false;
}
void HandleError()
{
  eraseDisplay();
  nxtDisplayCenteredBigTextLine(2, "ERROR");//show selected view number
  nxtDisplayCenteredTextLine(6, sError);
  nxtDisplayCenteredTextLine(7, "Press any key");
  while(nNxtButtonPressed == -1)
    wait10Msec(1);
  ClearErrors();
}

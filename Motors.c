//////////////Motor vars
float cm = 21.2;// how many degrees is one CM on the ground
int mBPwr = 100;
int mCPwr = 100;
bool StopMot = true;
//int iSpeed = 0;
//int iDirection = 0;
//int iRemainingDistance = 0;
//int iSetDistance = 0;
//int iAccdistance = 10;// %
const TTimers  BurnTimer = T3;
/////////////////////////////////
////////MOTOR FUNCTIONS//////////
/////////////////////////////////
// void MotionUpdate(int Speed, int Direction, int Distance)
// {



// }

void Movement(bool Fwd, int Power, int Distance )
{
  //iSetDistance = Distance;
  if(StopMot || bError)
    return;

  nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
  nSyncedTurnRatio = 100; //motors move at 100% alignment to each other

  if (Fwd)
  {
    mBPwr = -Power;
    mCPwr = -Power;
  }
  else
  {
    mBPwr = Power;
    mCPwr = Power;
  }
  nMotorEncoder[motorB] = 0; //reset the value of motorB's encoder to zero
  //nMotorEncoder[motorC] = 0; //reset the value of motorC's encoder to zero

  nMotorEncoderTarget[motorB] = Distance * cm;
  //nMotorEncoderTarget[motorC] = Distance * cm;

  motor[motorB] = mBPwr; //power motorB at X% power
  //motor[motorC] = mCPwr; //power motorB at X% power
  time1[BurnTimer] = 0;
  while(nMotorRunState[motorB] != runStateIdle && !bError)//while motorB is not in an idle position
  {
    // if(time1[BurnTimer] > Distance*50)
      // SetError("Motor stuck 1");

    //continue to monitor nMotorEncoderTarget's target position
    // iRemainingDistance = iSetDistance - nMotorEncoder[motorC] / cm;
    // iAccdistance = (iSetDistance / 100) * 10;
    // iSpeed = (iAccdistance / mBPwr) * (nMotorEncoder[motorC] / cm);
    // if(iSpeed > mBPwr)
    // iSpeed = mBPwr;
    // motor[motorB] = iSpeed; //power motorB at X% power
    // motor[motorC] = iSpeed;
  }


  motor[motorB] = 0; //power motorB at X% power
  motor[motorC] = 0; //power motorB at X% power

}


void MovementTurn(bool Dir, int Power, int degrees)
{
  if(StopMot || bError)
    return;

  nSyncedMotors = synchBC; //motor B is the master, motor C is the slave
  nSyncedTurnRatio = -100; //motors move at 100% alignment to each other

  float fullturn = 50.4;// it takes 50,4 cm to do a full spin
  float onedeg = fullturn / 360;
  if (Dir)//which way to turn?
  {
    mBPwr = Power;//motors at different directions
    mCPwr = -Power;
  }
  else
  {
    mBPwr = -Power;
    mCPwr = Power;
  }
  nMotorEncoder[motorB] = 0; //reset the value of motorB's encoder to zero
  //nMotorEncoder[motorC] = 0; //reset the value of motorC's encoder to zero

  nMotorEncoderTarget[motorB] = cm * (onedeg * degrees);
  //nMotorEncoderTarget[motorC] = cm * (onedeg * degrees);

  motor[motorB] = mBPwr; //power motorB at X% power
  //motor[motorC] = mCPwr; //power motorB at X% power
  time1[BurnTimer] = 0;
  while(nMotorRunState[motorB] != runStateIdle && !bError)//while motorB is not in an idle position
  {
    // if(time1[BurnTimer] > ((onedeg * degrees)*500))
      // SetError("Motors stuck 2");
    wait1Msec(1);
  }

  motor[motorB] = 0; //power motorB at X% power
  motor[motorC] = 0; //power motorB at X% power

}


void TightFlip(bool Dir, int Power)
{
  if(StopMot || bError)
    return;
  nSyncedMotors = synchNone; //motor B is the master, motor C is the slave

  float fullturn = 50.4;
  float onedeg = fullturn / 360;
  if (Dir)
  {
    mBPwr = Power;
    mCPwr = -Power;
  }
  else
  {
    mBPwr = -Power;
    mCPwr = Power;
  }
  nMotorEncoder[motorB] = 0; //reset the value of motorB's encoder to zero
  nMotorEncoder[motorC] = 0; //reset the value of motorC's encoder to zero


  nMotorEncoderTarget[motorC] = cm * (onedeg * 180);
  motor[motorC] = mCPwr; //power motorB at X% power
  time1[BurnTimer] = 0;
  while(nMotorRunState[motorC] != runStateIdle && !bError) //while motorB is not in an idle position
  {
    // if(time1[BurnTimer] > (onedeg * 180) *500)
      // SetError("MotorC stuck");
    wait1Msec(1);
  }
  motor[motorB] = 0; //power motorB at X% power
  motor[motorC] = 0; //power motorB at X% power


  nMotorEncoderTarget[motorB] = cm * (onedeg * 185);
  motor[motorB] = mBPwr; //power motorB at X% power
  time1[BurnTimer] = 0;
  while(nMotorRunState[motorB] != runStateIdle && !bError)//while motorB is not in an idle position
  {
    // if(time1[BurnTimer] >  (onedeg * 180)*500)
      // SetError("MotorB stuck");
    wait1Msec(1);
  }
  motor[motorB] = 0; //power motorB at X% power
  motor[motorC] = 0; //power motorB at X% power


}

// task TMot()//mul on vaja taski, mis jooksvalt haldaks mootorite asukohti, kiiruseid, kiirendusi ja asendeid, funktsioonid muudaks jooksvalt selle v22rtuseid
// {


// }

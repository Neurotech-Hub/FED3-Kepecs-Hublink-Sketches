/*
  Feeding experimentation device 3 (FED3)

  This script demonstrates how to write a menu system so you can access multiple programs from the FED3 startup screen.  You will scroll through programs by assigning them to "modes".

  In this example, we run four modes:
  // FEDmodes:
  // 0 Free feeding
  // 1 FR1
  // 2 Resetting Progressive Ratio''''''''''''''''''''''''''''''''''''''''''''''''

  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz

*/

#include <FED3.h>           //Include the FED3 library
String sketch = "SharonCE"; // Unique identifier text for each sketch
FED3 fed3(sketch);          // Start the FED3 object

// variables for PR tasks
int poke_num = 0;                 // this variable is the number of pokes since last pellet
int pellets_in_current_block = 0; // variable stores pellets number that the mouse is trying to receive. 1 is the first pellet. 2 is the second pellet
int pokes_required = 1;           // increase the number of pokes required each time a pellet is received using this exponential equation
unsigned long poketime = 0;       // time of poke
int resetInterval = 1800;         // number of seconds without a poke until reset

void setup()
{
  fed3.FED3Menu = true;    // Activate the menu function at startup
  fed3.begin();            // Setup the FED3 hardware
  fed3.EnableSleep = true; // Set to false to inhibit sleeping to use the Serial port; Set to true to reduce battery power
}

void loop()
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 1: Free feeding
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 0)
  {
    fed3.sessiontype = "Free_feed"; // The text in "sessiontype" will appear on the screen and in the logfile
    fed3.DisplayPokes = false;      // Turn off poke indicators for free feeding mode
    fed3.UpdateDisplay();           // Update display for free feeding session to remove poke displayt (they are on by default)
    fed3.Feed();
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Modes 2: Fixed Ratio 1
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (fed3.FEDmode == 1)
  {
    fed3.sessiontype = "FR1"; // The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Left)
    {
      fed3.logLeftPoke(); // Log left poke
      if (fed3.LeftCount % fed3.FR == 0)
      {                             // if fixed ratio is  met
        fed3.ConditionedStimulus(); // deliver conditioned stimulus (tone and lights)
        fed3.Feed();                // deliver pellet
        fed3.BNC(500, 1);           // Send one 500ms pulse to the BNC output
      }
    }
    if (fed3.Right)
    { // If right poke is triggered
      fed3.logRightPoke();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 3: Progressive Ratio
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 2)
  {
    Timer_reset();
    fed3.sessiontype = "PRexp"; // The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Left)
    {                           // If left poke is triggered and pellet is not in the well
      fed3.logLeftPoke();       // Run leftPokeResponse to log the Left poke on screen and SD card
      poke_num++;               // store this new poke number as current poke number.
      poketime = fed3.unixtime; // store the current time of poke
      serialoutput();           // print this on the serial monitor -- if you want
      if (poke_num == pokes_required)
      {                             // check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
        fed3.ConditionedStimulus(); // Deliver conditioned stimulus (tone and lights)
        pellets_in_current_block++; // increase the pellet number by 1 after the mouse receives the pellet
        fed3.BlockPelletCount = pellets_in_current_block;
        fed3.Feed();                                                                 // Deliver pellet
        fed3.BNC(500, 1);                                                            // Send one 500ms pulse to the BNC output
        pokes_required = round((5 * exp((pellets_in_current_block + 1) * 0.1)) - 5); // increment the number of pokes required according to the progressive ratio
        fed3.FR = pokes_required;
        poke_num = 0; // reset the number of pokes back to 0, for the next trial
      }
    }

    if (fed3.Right)
    {                      // If right poke is triggered and pellet is not in the well
      fed3.logRightPoke(); // Run rightPokeResponse to log the right poke on screen and SD card
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 4: Linear Progressive Ratio 1
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 3)
  {
    fed3.sessiontype = "PRlin1"; // The text in "sessiontype" will appear on the screen and in the logfile
    Timer_reset();

    if (fed3.Left)
    {                           // If left poke is triggered and pellet is not in the well
      fed3.logLeftPoke();       // Run leftPokeResponse to log the Left poke on screen and SD card
      poke_num++;               // store this new poke number as current poke number.
      poketime = fed3.unixtime; // store the current time of poke
      serialoutput();           // print this on the serial monitor -- if you want
      if (poke_num == pokes_required)
      {                             // check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
        fed3.ConditionedStimulus(); // Deliver conditioned stimulus (tone and lights)
        pellets_in_current_block++; // increase the pellet number by 1 after the mouse receives the pellet
        fed3.BlockPelletCount = pellets_in_current_block;
        fed3.Feed();      // Deliver pellet
        fed3.BNC(500, 1); // Send one 500ms pulse to the BNC output
        pokes_required = pokes_required + 1;
        fed3.FR = pokes_required;
        poke_num = 0; // reset the number of pokes back to 0, for the next trial
      }
    }

    if (fed3.Right)
    {                      // If right poke is triggered and pellet is not in the well
      fed3.logRightPoke(); // Run rightPokeResponse to log the right poke on screen and SD card
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Modes 5: Fixed Ratio 2
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (fed3.FEDmode == 4)
  {
    fed3.sessiontype = "FR2"; // The text in "sessiontype" will appear on the screen and in the logfile
    int FR = 2;
    fed3.FR = FR;
    if (fed3.Left)
    {
      fed3.logLeftPoke(); // Log left poke
      if (fed3.LeftCount % fed3.FR == 0)
      {                             // if fixed ratio is  met
        fed3.ConditionedStimulus(); // deliver conditioned stimulus (tone and lights)
        fed3.Feed();                // deliver pellet
        fed3.BNC(500, 1);           // Send one 500ms pulse to the BNC output
      }
    }
    if (fed3.Right)
    { // If right poke is triggered
      fed3.logRightPoke();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Modes 6: Fixed Ratio 3
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (fed3.FEDmode == 5)
  {
    fed3.sessiontype = "FR3"; // The text in "sessiontype" will appear on the screen and in the logfile
    int FR = 3;
    fed3.FR = FR;
    if (fed3.Left)
    {
      fed3.logLeftPoke(); // Log left poke
      if (fed3.LeftCount % fed3.FR == 0)
      {                             // if fixed ratio is  met
        fed3.ConditionedStimulus(); // deliver conditioned stimulus (tone and lights)
        fed3.Feed();                // deliver pellet
        fed3.BNC(500, 1);           // Send one 500ms pulse to the BNC output
      }
    }
    if (fed3.Right)
    { // If right poke is triggered
      fed3.logRightPoke();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 5: Linear Progressive Ratio 2
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 6)
  {
    fed3.sessiontype = "PRlin2"; // The text in "sessiontype" will appear on the screen and in the logfile
    Timer_reset();

    if (fed3.Left)
    {                           // If left poke is triggered and pellet is not in the well
      fed3.logLeftPoke();       // Run leftPokeResponse to log the Left poke on screen and SD card
      poke_num++;               // store this new poke number as current poke number.
      poketime = fed3.unixtime; // store the current time of poke
      serialoutput();           // print this on the serial monitor -- if you want
      if (poke_num == pokes_required)
      {                             // check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
        fed3.ConditionedStimulus(); // Deliver conditioned stimulus (tone and lights)
        pellets_in_current_block++; // increase the pellet number by 1 after the mouse receives the pellet
        fed3.BlockPelletCount = pellets_in_current_block;
        fed3.Feed();      // Deliver pellet
        fed3.BNC(500, 1); // Send one 500ms pulse to the BNC output
        pokes_required = pokes_required + 2;
        fed3.FR = pokes_required;
        poke_num = 0; // reset the number of pokes back to 0, for the next trial
      }
    }

    if (fed3.Right)
    {                      // If right poke is triggered and pellet is not in the well
      fed3.logRightPoke(); // Run rightPokeResponse to log the right poke on screen and SD card
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     If a mode greater than 6 is selected
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode > 6)
  {
    fed3.DisplayNoProgram();
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                    Call fed.run at least once per loop
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  fed3.run();
}

void Timer_reset()
{
  if (fed3.unixtime - poketime >= resetInterval)
  { // if more than 30 mins has passed since last poke -- reset the block and parameters
    pellets_in_current_block = 0;
    fed3.BlockPelletCount = pellets_in_current_block;
    poke_num = 0;
    pokes_required = 1;
    fed3.FR = pokes_required;
    Serial.println("          ");
    Serial.println("****"); // if more than 30 mins has passed since last poke --print **** on the serial monitor

    fed3.pixelsOn(5, 5, 5, 0);
    delay(200);
    fed3.pixelsOff();
    poketime = fed3.unixtime; // store the current time of poke
  }
}

void serialoutput()
{
  ////////////////////////////////////////////////////
  // Use Serial.print statements for debugging
  ////////////////////////////////////////////////////
  Serial.print("Unixtime: ");
  Serial.println(fed3.unixtime);
  Serial.println("Pellets   RightPokes   LeftPokes   Poke_Num  Pel  Pokes_Required  PokeTime   Reset  FR");
  Serial.print("   ");
  Serial.print(fed3.PelletCount);
  Serial.print("          ");
  Serial.print(fed3.RightCount);
  Serial.print("          ");
  Serial.print(fed3.LeftCount);
  Serial.print("          ");
  Serial.print(poke_num);
  Serial.print("          ");
  Serial.print(pellets_in_current_block);
  Serial.print("          ");
  Serial.print(pokes_required);
  Serial.print("       ");
  Serial.print(poketime);
  Serial.print("          ");
  Serial.print(fed3.FR);
  Serial.println(" ");
  Serial.println(" ");
}

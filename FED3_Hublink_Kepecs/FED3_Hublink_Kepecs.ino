// Note on Setting RTC: The RTC setting is based on the compile date and time. The only reliable
// way to enforce that the __DATE__ and __TIME__ are set correctly is to force a full recompile,
// requiring that the cached sketches be deleted.
// On MacOS, this done by deleting: /Users/<YourUsername>/Library/Caches/arduino/sketches
// On Windows, this done by deleting: C:\Users\<YourUsername>\AppData\Local\Arduino15\cache
// Hublink with automatically set the time if you force sync

#include <FED3.h>
#include <Hublink.h>
Hublink hublink(cardSelect);
String sketch = "SharonCE";
FED3 fed3(sketch);

// PR task variables
int poke_num = 0;                 // pokes since last pellet
int pellets_in_current_block = 0; // pellets in current block
int pokes_required = 1;           // required pokes for next pellet
unsigned long poketime = 0;       // timestamp of last poke
const int resetInterval = 1800;   // seconds until reset

// Fixed ratios for different modes
const int FR2 = 2;
const int FR3 = 3;

void printMemoryStats(String event)
{
  Serial.println("\n--- Memory Statistics: " + event + " ---");
  Serial.printf("Free Heap: %d bytes\n", ESP.getFreeHeap());
  Serial.println("----------------------\n");
}

// Hublink callback function to handle timestamp
void onTimestampReceived(uint32_t timestamp)
{
  Serial.print("Received timestamp: " + String(timestamp));
  fed3.adjustRTC(timestamp);
}

void setup()
{
  fed3.FED3Menu = true;
  fed3.enableSleep();
  fed3.createDailyFile = true;
  fed3.diskDispenseSteps = -300;
  fed3.begin();

  if (hublink.begin())
  {
    Serial.println("✓ Hublink.");
    hublink.setTimestampCallback(onTimestampReceived);

    fed3.DisplayText("Sync Hublink?\nYES: Poke right\nNO: Poke left");
    delay(1000); // debounce previous mouse menu
    unsigned long startTime = millis();
    bool doSync = false;
    bool success = false;
    while (millis() - startTime < 10000)
    {
      if (fed3.Left)
      {
        break;
      }
      if (fed3.Right)
      {
        doSync = true;
        fed3.DisplayText("Syncing Hublink...\nAdvertising name:\n" + String(hublink.advertise));
        success = hublink.sync(120); // sync for 120 seconds
        break;
      }
      delay(100);
    }

    fed3.Left = false;
    fed3.Right = false;

    while (!fed3.Left && !fed3.Right && doSync)
    {
      if (success)
      {
        fed3.DisplayText("Hublink synced!\nPoke any to exit");
      }
      else
      {
        fed3.DisplayText("Hublink sync failed.\nPoke any to exit");
      }
    }
    fed3.DisplayText("Exiting...");
    delay(3000); // debound user fingers in port
  }
  else
  {
    Serial.println("✗ Failed.");
    // stops without SD card
    while (1)
    {
    }
  }

  // reset before use
  fed3.Left = false;
  fed3.Right = false;
  fed3.LeftCount = 0;
  fed3.LeftCount = 0;
  fed3.DisplayText(""); // clear display
  fed3.UpdateDisplay();
}

// Handle feeding based on FR condition
void handleFRFeeding()
{
  if (fed3.LeftCount % fed3.FR == 0)
  {
    fed3.ConditionedStimulus();
    fed3.Feed();
    // fed3.BNC(500, 1); // DO NOT USE, A0 is SD card select
  }
}

// Handle progressive ratio feeding
void handlePRFeeding()
{
  if (poke_num == pokes_required)
  {
    fed3.ConditionedStimulus();
    pellets_in_current_block++;
    fed3.BlockPelletCount = pellets_in_current_block;
    fed3.Feed();
    // fed3.BNC(500, 1); // DO NOT USE, A0 is SD card select
    poke_num = 0;
  }
}

void loop()
{
  delay(1000);
  switch (fed3.FEDmode)
  {
  // Free feeding
  case 0:
    fed3.sessiontype = "Free_feed";
    fed3.DisplayPokes = false;
    fed3.UpdateDisplay();
    fed3.Feed();
    break;

  // FR1
  case 1:
    fed3.sessiontype = "FR1";
    if (fed3.Left)
    {
      fed3.logLeftPoke();
      handleFRFeeding();
    }
    if (fed3.Right)
      fed3.logRightPoke();
    break;

  // Progressive Ratio
  case 2:
    Timer_reset();
    fed3.sessiontype = "PRexp";
    if (fed3.Left)
    {
      fed3.logLeftPoke();
      poke_num++;
      poketime = fed3.unixtime;
      serialoutput();
      handlePRFeeding();
      if (poke_num == 0) // Only update if feeding occurred
      {
        pokes_required = round((5 * exp((pellets_in_current_block + 1) * 0.1)) - 5);
        fed3.FR = pokes_required;
      }
    }
    if (fed3.Right)
      fed3.logRightPoke();
    break;

  // Linear PR1
  case 3:
    Timer_reset();
    fed3.sessiontype = "PRlin1";
    if (fed3.Left)
    {
      fed3.logLeftPoke();
      poke_num++;
      poketime = fed3.unixtime;
      serialoutput();
      handlePRFeeding();
      if (poke_num == 0)
      {
        pokes_required++;
        fed3.FR = pokes_required;
      }
    }
    if (fed3.Right)
      fed3.logRightPoke();
    break;

  // FR2
  case 4:
    fed3.sessiontype = "FR2";
    fed3.FR = FR2;
    if (fed3.Left)
    {
      fed3.logLeftPoke();
      handleFRFeeding();
    }
    if (fed3.Right)
      fed3.logRightPoke();
    break;

  // FR3
  case 5:
    fed3.sessiontype = "FR3";
    fed3.FR = FR3;
    if (fed3.Left)
    {
      fed3.logLeftPoke();
      handleFRFeeding();
    }
    if (fed3.Right)
      fed3.logRightPoke();
    break;

  // Linear PR2
  case 6:
    Timer_reset();
    fed3.sessiontype = "PRlin2";
    if (fed3.Left)
    {
      fed3.logLeftPoke();
      poke_num++;
      poketime = fed3.unixtime;
      serialoutput();
      handlePRFeeding();
      if (poke_num == 0)
      {
        pokes_required += 2;
        fed3.FR = pokes_required;
      }
    }
    if (fed3.Right)
      fed3.logRightPoke();
    break;

  default:
    fed3.DisplayNoProgram();
    break;
  }

  fed3.run();
  if (hublink.sync())
  {
    fed3.Event = "HublinkSync";
    fed3.logdata();
    if (fed3.pelletIsStuck)
    {
      fed3.DisplayJammed();
      fed3.detachWakeupInterrupts();
      fed3.sleepForever();
      // requires reboot to clear jammed state
    }
  }
}

void Timer_reset()
{
  if (fed3.unixtime - poketime >= resetInterval)
  {
    pellets_in_current_block = 0;
    fed3.BlockPelletCount = pellets_in_current_block;
    poke_num = 0;
    pokes_required = 1;
    fed3.FR = pokes_required;

    Serial.println("\n****");
    fed3.pixelsOn(5, 5, 5, 0);
    delay(200);
    fed3.pixelsOff();
    poketime = fed3.unixtime;
  }
}

void serialoutput()
{
  static const char *headers = "Pellets   RightPokes   LeftPokes   Poke_Num  Pel  Pokes_Required  PokeTime   Reset  FR";

  Serial.print("Unixtime: ");
  Serial.println(fed3.unixtime);
  Serial.println(headers);

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
  Serial.println(fed3.FR);
  Serial.println();
}

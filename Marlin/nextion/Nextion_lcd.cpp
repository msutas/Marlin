/**
 * MK & MK4due 3D Printer Firmware
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (C) 2013 - 2016 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * Nextion_lcd.cpp
 *
 * Copyright (c) 2014-2016 Alberto Cotronei @MagoKimbra
 *
 * Grbl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Grbl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Grbl. If not, see <http://www.gnu.org/licenses/>.
 */

#include "../Marlin.h"
#include "../Configuration.h"
#include "../Language.h"
#include "../cardreader.h"
#include "../temperature.h"
#include "../stepper.h"
#include "../configuration_store.h"

#if ENABLED(NEXTION)

  #include "Nextion_lcd.h"
  #include "Nextion.h"

  bool NextionON        = false;
  uint8_t NextionPage   = 0;
  char buffer[30]      = {0};
  uint32_t slidermaxval = 20;
  char lcd_status_message[30] = WELCOME_MSG;
  uint8_t lcd_status_message_level = 0;
  uint8_t lcd_sd_status;
  
  static millis_t next_lcd_update_ms;

  #if ENABLED(SDSUPPORT)
    uint8_t SDstatus    = 0; // 0 SD not insert, 1 SD insert, 2 SD printing
  #endif

  // Page
//  NexPage Pintro          = NexPage(0, 0, "intro");
//  NexPage Pmain           = NexPage(1, 0, "main");
  NexPage Pmove           = NexPage(2, 0, "move");
  NexPage Pinfo           = NexPage(3, 0, "info");
  NexPage Pprepare        = NexPage(4, 0, "prepare");
  NexPage Psd             = NexPage(6, 0, "sd");
//  NexPage Psettemp        = NexPage(7, 0, "settemp");

  // Page 1 main
  NexPicture Sdlist  = NexPicture(1, 3, "sdlist");
    
  // Page 2 move
  NexPicture Moveup      = NexPicture(2, 8, "moveup");              
  NexPicture Movedown    = NexPicture(2, 9, "movedown");            
  NexPicture Home        = NexPicture(2, 10, "home");               
  NexPicture Disable     = NexPicture(2, 11, "disable");            
  NexPicture Fan         = NexPicture(2, 13, "fan"); 
  NexVar Cmd             = NexVar(2,19, "cmd");
  NexTimer Fantimer      = NexTimer(2,17, "tm0");
  NexVar MX              = NexVar(2,22, "mx");                  
  NexVar MY              = NexVar(2,23, "my");                  
  NexVar MZ              = NexVar(2,24, "mz");

  // Page 3 info
  NexPicture Infoplay         = NexPicture(3, 3, "infoplay");
  NexPicture Infostop         = NexPicture(3, 4, "infostop");
  NexPicture Infofilament     = NexPicture(3, 6, "infofilament");
  NexNumber Nozzletemp        = NexNumber(3, 14, "nozzletemp");
  NexHotspot Nozzlearea       = NexHotspot(3, 11, "nozzlearea");
  NexHotspot Bedarea          = NexHotspot(3, 12, "bedarea");
  NexText Infozcoord          = NexText(3, 7, "infozcoord");
  NexText Infostatus          = NexText(3, 8, "infostatus");                    
  NexProgressBar Infoprogress = NexProgressBar(3, 9, "infoprogress");  
  NexVar T0                   = NexVar(3,17, "t0");
  NexVar B                    = NexVar(3,18, "b");
  NexVar T0set                = NexVar(3,19, "t0set");
  NexVar Bset                 = NexVar(3,20, "bset");
  NexVar IZ                   = NexVar(3,21, "iz");
  
    
  // Page 4 Prepare
  NexPicture Cooldown       = NexPicture(4, 1, "cooldown"); 
  NexPicture Preheatabs     = NexPicture(4, 2, "preheatabs"); 
  NexPicture Preheatpla     = NexPicture(4, 6, "preheatpla"); 
  NexPicture Speeddown      = NexPicture(4, 9, "speeddown"); 
  NexPicture Speedup        = NexPicture(4, 10, "speedup"); 
  NexPicture Flowdown       = NexPicture(4, 4, "flowdown"); 
  NexPicture Flowup         = NexPicture(4, 7, "flowup"); 
  NexNumber Flownumber      = NexNumber(4, 11, "flownumber");                  
  NexNumber Speednumber     = NexNumber(4, 12, "speednumber");  
  
  // Page 6 sd 
  NexPicture Sdback    = NexPicture(6, 11,  "sdback");
  NexPicture Sdup      = NexPicture(6, 8,  "sdup");
  NexPicture Sddown    = NexPicture(6, 7,  "sddown");
  NexText Sd0          = NexText(6, 2, "sd0");
  NexText Sd1          = NexText(6, 3, "sd1");
  NexText Sd2          = NexText(6, 4, "sd2");
  NexText Sd3          = NexText(6, 5, "sd3");
  NexText Sd4          = NexText(6, 6, "sd4");
  NexText Foldertext   = NexText(6, 1, "foldertext"); 
  NexSlider Sayfa      = NexSlider(6, 13, "sayfa"); 
  NexVar Sdv0          = NexVar(6, 14, "sdv0");
  NexVar Sdv1          = NexVar(6, 15, "sdv1");
  NexVar Sdv2          = NexVar(6, 16, "sdv2");
  NexVar Sdv3          = NexVar(6, 17, "sdv3");
  NexVar Sdv4          = NexVar(6, 18, "sdv4");
  
  // Page 7 Settemp
  NexPicture Tempset    = NexPicture(7, 2,  "tempset");
  NexNumber Temp        = NexNumber(7, 13, "temp");
  NexVar Tooltemp       = NexVar(7, 14, "tooltemp"); 
  
  NexTouch *nex_listen_list[] =
  {
      &Sdlist,      // Page 1 main
      &Moveup,   // Page 2 move             
      &Movedown,                            
      &Home,                                
      &Disable,                              
      &Fan,                                  
      &Infoplay,   
      &Infostop,   
      &Infofilament,
      &Nozzlearea,
      &Bedarea,   
      &Cooldown,      // Page 4 Prepare
      &Preheatabs,    
      &Preheatpla,    
      &Speeddown,     
      &Speedup,       
      &Flowdown,      
      &Flowup,        
      &Sdback,      // Page 6 sd 
      &Sdup,
      &Sddown,
      &Sd0,
      &Sd1,
      &Sd2,
      &Sd3,
      &Sd4,
      &Sdv0,
      &Sdv1,
      &Sdv2,
      &Sdv3,
      &Sdv4,
      &Tempset,   
      NULL
  };
    
  NexText *row_list[] =
  {
    &Sd0,
    &Sd1,
    &Sd2,
    &Sd3,
    &Sd4,
    NULL
  };

  NexVar *vrow_list[] =
  {
    &Sdv0,
    &Sdv1,
    &Sdv2,
    &Sdv3,
    &Sdv4,
    NULL
  };  
  


  #if ENABLED(SDSUPPORT)
    static void printrowsd(uint8_t row, const bool folder, const char* filename, const char* shortname) {
      if (folder) {
        row_list[row]->attachPop(sdfolderPopCallback, row_list[row]);
      } else if (shortname == "") {
        row_list[row]->detachPop();
      } else {
        row_list[row]->attachPop(sdfilePopCallback, row_list[row]);
      }
      row_list[row]->setText(filename);
      vrow_list[row]->setText(shortname);
    }

    static void setrowsdcard(uint32_t number = 0) {
      uint16_t fileCnt = card.getnrfilenames();
      uint32_t i = 0;
      card.getWorkDirName();

      if (card.filename[0] != '/') {
        Sdback.setPic(46);
        Sdback.attachPop(sdfolderUpPopCallback);
        Foldertext.setText((strlen(card.longFilename)>0 ? card.longFilename : card.filename));
      } else {
        Sdback.detachPop();
        Sdback.setPic(68);
        Foldertext.setText("/");
      }

      for (uint8_t row = 0; row < 5; row++) {
        i = row + number;
        if (i < fileCnt) {
          card.getfilename(i);
          printrowsd(row, card.filenameIsDir, (strlen(card.longFilename)>0 ? card.longFilename : card.filename), card.filename);
        } else {
          printrowsd(row, false, "", "");
        }
      }
      sendCommand("ref sd");
    }

    static void menu_action_sdfile(const char* filename) {
      char cmd[30];
      char* c;
      sprintf_P(cmd, PSTR("M23 %s"), filename);
      for(c = &cmd[4]; *c; c++) *c = tolower(*c);
      enqueue_and_echo_command(cmd);
 //     enqueue_and_echo_commands_P(PSTR("M24"));
      Pinfo.show();
      Infoplay.setPic(3);
      Infostop.setPic(5);
      Infofilament.setPic(2);      
    }

    static void menu_action_sddirectory(const char* filename) {
      card.chdir(filename);
      setpageSDPopCallback(&Sdlist);
    }

    void setpageSDPopCallback(void *ptr) {
      if (!card.cardOK) {
         card.initsd();
      }
      Psd.show();
      uint16_t fileCnt = card.getnrfilenames();
      
      if (fileCnt <= 5)
        slidermaxval = 0;
      else
        slidermaxval  = fileCnt - 5;

      uint16_t hig = 210 - slidermaxval * 10;
      if (hig < 10) hig = 10;

      Sayfa.setHigVal(hig); 
      Sayfa.setMaxVal(slidermaxval);
      Sayfa.setValue(slidermaxval);  
      sendCommand("ref sd");

      setrowsdcard();
    }

    void sdlistPopCallback(void *ptr) {
      uint32_t number = 0;
      Sayfa.getValue(&number);
      number = slidermaxval - number;
      setrowsdcard(number);
    }

    void sdfilePopCallback(void *ptr) {
      memset(buffer, 0, sizeof(buffer));

      if (ptr == &Sd0)
        Sdv0.getText(buffer, sizeof(buffer));
      else if (ptr == &Sd1)
        Sdv1.getText(buffer, sizeof(buffer));
      else if (ptr == &Sd2)
        Sdv2.getText(buffer, sizeof(buffer));
      else if (ptr == &Sd3)
        Sdv3.getText(buffer, sizeof(buffer));
      else if (ptr == &Sd4)
        Sdv4.getText(buffer, sizeof(buffer));

      menu_action_sdfile(buffer);
    }

    void sdfolderPopCallback(void *ptr) {
      memset(buffer, 0, sizeof(buffer));

      if (ptr == &Sd0)
        Sdv0.getText(buffer, sizeof(buffer));
      else if (ptr == &Sd1)
        Sdv1.getText(buffer, sizeof(buffer));
      else if (ptr == &Sd2)
        Sdv2.getText(buffer, sizeof(buffer));
      else if (ptr == &Sd3)
        Sdv3.getText(buffer, sizeof(buffer));
      else if (ptr == &Sd4)
        Sdv4.getText(buffer, sizeof(buffer));

      menu_action_sddirectory(buffer);
    }

    void sdfolderUpPopCallback(void *ptr) {
      card.updir();
      setpageSDPopCallback(&Sdlist);
    }

    void PlayPausePopCallback(void *ptr) {
      if (card.cardOK && card.isFileOpen()) {
        if (IS_SD_PRINTING)
          card.pauseSDPrint();
        else
          card.startFileprint();
      }
    }

    void StopPopCallback(void *ptr) {
    quickStop();
    card.sdprinting = false;
    card.closefile();
    enqueue_and_echo_commands_P(PSTR("M104 T0 S0"));
    enqueue_and_echo_commands_P(PSTR("M140 S0"));
    autotempShutdown();
    cancel_heatup = true;
    lcd_setstatus(MSG_PRINT_ABORTED, true);
    }
  
  #endif
  
  
/* MOVE */  

  void setmovePopCallback(void *ptr) {                   
    char tempaxis[10] = {0};
    char templength[10] = {0};
    char tempspeed[10] = {0};
    memset(buffer, 0, sizeof(buffer));
    Cmd.getText(buffer, sizeof(buffer));
    
    enqueue_and_echo_commands_P(PSTR("G91"));
    enqueue_and_echo_command(buffer);
    enqueue_and_echo_commands_P(PSTR("G90"));
  }
  
  void setfanPopCallback(void *ptr) {                 
    if (fanSpeeds[0]) {
      fanSpeeds[0] = 0;
      Fantimer.disable();
    }
    else {
      fanSpeeds[0] = 255;
      Fantimer.enable();
    }
  }
  
/* INFO */
  void settempPopCallback(void *ptr) {
    uint32_t number;
    char valuetemp[25] = {0};

    memset(buffer, 0, sizeof(buffer));
    Tooltemp.getText(buffer, sizeof(buffer));



    if (buffer[0]=='T') {
       Temp.getValue(&number);
       memset(buffer, 0, sizeof(buffer));
       strcpy(buffer,"M104 T0 S");
       itoa(number,valuetemp,10);
       strcat(buffer,valuetemp);
       enqueue_and_echo_command(buffer);
       T0set.setValue(number);
    }
    else if (buffer[0]=='B') {
       Temp.getValue(&number);
       memset(buffer, 0, sizeof(buffer));
       strcpy(buffer,"M140 S");
       itoa(number,valuetemp,10);
       strcat(buffer,valuetemp);
       enqueue_and_echo_command(buffer);
       Bset.setValue(number);
    } 

    Pinfo.show();
  }

 
/************************************/

  static void temptoLCD(int TX1, int TX2, int BX1, int BX2) {                      //OK
    T0.setValue(TX1);
    T0set.setValue(TX2);    
    B.setValue(BX1);
    Bset.setValue(BX2);
  }

/* PREPARE */
  
  void SpeedupPopCallback(void *ptr) {
    uint32_t temp_feedrate = 0;
    Speednumber.getValue(&temp_feedrate);
    feedrate_percentage = (int)temp_feedrate + 1;
    Speednumber.setValue(feedrate_percentage);
  }
  
  void SpeeddownPopCallback(void *ptr) {
    uint32_t temp_feedrate = 0;
    Speednumber.getValue(&temp_feedrate);
    feedrate_percentage = (int)temp_feedrate - 1;
    Speednumber.setValue(feedrate_percentage);
  }
  
  void FlowdownPopCallback(void *ptr) {
    uint32_t temp_flowrate = 0;
    Flownumber.getValue(&temp_flowrate);
    extruder_multiplier[0] = (int)temp_flowrate - 1;
    Flownumber.setValue(extruder_multiplier[0]);
  }

  void FlowupPopCallback(void *ptr) {
    uint32_t temp_flowrate = 0;
    Flownumber.getValue(&temp_flowrate);
    extruder_multiplier[0] = (int)temp_flowrate + 1;
    Flownumber.setValue(extruder_multiplier[0]);
  } 

  void PreheatabsPopCallback(void *ptr) {
      thermalManager.setTargetHotend(PREHEAT_2_TEMP_HOTEND, 0);
      thermalManager.setTargetBed(PREHEAT_2_TEMP_BED);
      temptoLCD((int)thermalManager.degHotend(0), (int)thermalManager.degTargetHotend(0), (int)thermalManager.degBed(), (int)thermalManager.degTargetBed());
      Pinfo.show();
      lcd_setstatus(lcd_status_message);
      }
 
      
  void PreheatplaPopCallback(void *ptr) {
      thermalManager.setTargetHotend(PREHEAT_1_TEMP_HOTEND, 0);
      thermalManager.setTargetBed(PREHEAT_1_TEMP_BED);
      temptoLCD((int)thermalManager.degHotend(0), (int)thermalManager.degTargetHotend(0), (int)thermalManager.degBed(), (int)thermalManager.degTargetBed());
      Pinfo.show();
      lcd_setstatus(lcd_status_message);
      }  
  
   void cooldownPopCallback(void *ptr) {
       #if FAN_COUNT > 0
          for (uint8_t i = 0; i < FAN_COUNT; i++) fanSpeeds[i] = 0;
       #endif
      thermalManager.disable_all_heaters();
      Pinfo.show();
}

  void lcd_init() {
    delay(2000);

    for (uint8_t i = 0; i < 10; i++) {
      NextionON = nexInit();
      if (NextionON) break;
      delay(1000);
    }

    if (!NextionON) {
      SERIAL_ECHO_START;
      SERIAL_ECHOPGM("Nextion LCD not connected!");
      SERIAL_EOL;
    }
    else {
      SERIAL_ECHO_START;
      SERIAL_ECHOPGM("Nextion LCD connected!");
      SERIAL_EOL;      

      #if ENABLED(SDSUPPORT)
        Sdlist.attachPop(setpageSDPopCallback);      
        Sdup.attachPop(sdlistPopCallback);
        Sddown.attachPop(sdlistPopCallback);
        Infoplay.attachPop(PlayPausePopCallback);
        Infostop.attachPop(StopPopCallback);
      #endif

      Tempset.attachPop(settempPopCallback);

      Fan.attachPop(setfanPopCallback);
      Moveup.attachPop(setmovePopCallback);
      Movedown.attachPop(setmovePopCallback);
      Home.attachPop(setmovePopCallback);
      Disable.attachPop(setmovePopCallback);
     
      Cooldown.attachPop(cooldownPopCallback);
      Speedup.attachPop(SpeedupPopCallback);
      Speeddown.attachPop(SpeeddownPopCallback);
      Flowup.attachPop(FlowupPopCallback);
      Flowdown.attachPop(FlowdownPopCallback);
      Preheatabs.attachPop(PreheatabsPopCallback);
      Preheatpla.attachPop(PreheatplaPopCallback);
            
    }
  }

  static void coordtoLCD() {                                            //OK
    char valuetemp[8] ={0};
    
    memset(buffer, 0, sizeof(buffer));
    if (axis_homed[X_AXIS]) {
      itoa(current_position[X_AXIS],valuetemp,10);
      strcpy(buffer, valuetemp);
      strcat(buffer, ".");
      itoa(getDecimal(current_position[X_AXIS]),valuetemp,10);
      strcat(buffer, valuetemp);
    } else {
      strcpy(buffer, "?");
    }
    MX.setText(buffer);
    memset(buffer, 0, sizeof(buffer));
    if (axis_homed[Y_AXIS]) {
      itoa(current_position[Y_AXIS],valuetemp,10);
      strcpy(buffer, valuetemp);
      strcat(buffer, ".");
      itoa(getDecimal(current_position[Y_AXIS]),valuetemp,10);
      strcat(buffer, valuetemp);
    } else {
      strcpy(buffer, "?");
    }
    MY.setText(buffer);

    memset(buffer, 0, sizeof(buffer));
    if (axis_homed[Z_AXIS]) {
      itoa(current_position[Z_AXIS],valuetemp,10);
      strcpy(buffer, valuetemp);
      strcat(buffer, ".");
      itoa(getDecimal(current_position[Z_AXIS]),valuetemp,10);
      strcat(buffer, valuetemp);
    } else {
      strcpy(buffer, "?");
    }
    MZ.setText(buffer);
  }
  
  static void coordtoLCDinfo() {                                            //OK
    char valuetemp[8] ={0};
    
    memset(buffer, 0, sizeof(buffer));
    if (axis_homed[Z_AXIS]) {
      itoa(current_position[Z_AXIS],valuetemp,10);
      strcpy(buffer, valuetemp);
      strcat(buffer, ".");
      itoa(getDecimal(current_position[Z_AXIS]),valuetemp,10);
      strcat(buffer, valuetemp);
    } else {
      strcpy(buffer, "?");
    }
    IZ.setText(buffer);
  }

  void lcd_update() {

    if (!NextionON) return;

    nexLoop(nex_listen_list);

    millis_t ms = millis();

    if (ms > next_lcd_update_ms) {

      sendCurrentPageId(&NextionPage);
      next_lcd_update_ms = ms + LCD_UPDATE_INTERVAL;  
      
      #if ENABLED(SDSUPPORT)
     
      bool sd_status = IS_SD_INSERTED;
      if (sd_status != lcd_sd_status) {

      if (sd_status) card.initsd();
      
    }
          if (card.isFileOpen()) {
            if (SDstatus != 2) {
              SDstatus = 2;
            }
          }
          else if (card.cardOK && SDstatus != 1) {
            SDstatus = 1;
          }
          else if (!card.cardOK && SDstatus != 0) {
            SDstatus = 0;
          }
        #endif
      

     if (NextionPage == 4) {
      
        Speednumber.setValue((int)feedrate_percentage);
        Flownumber.setValue((int)extruder_multiplier[0]);        
        
      } else if (NextionPage == 2) {
        
        if (fanSpeeds[0] > 0) Fantimer.enable();
        else Fantimer.disable();
        coordtoLCD(); 
        
      } else if (NextionPage == 3) { 
      
        coordtoLCDinfo();
        temptoLCD((int)thermalManager.degHotend(0), (int)thermalManager.degTargetHotend(0), (int)thermalManager.degBed(), (int)thermalManager.degTargetBed()); 
      #if ENABLED(SDSUPPORT)  
         if (sd_status != lcd_sd_status) {

         if (sd_status) {
            if (lcd_sd_status != 2) LCD_MESSAGEPGM(MSG_SD_INSERTED);
          } else if (lcd_sd_status != 2) LCD_MESSAGEPGM(MSG_SD_REMOVED);

        }  
          if (card.isFileOpen()) {
            if(IS_SD_PRINTING) {
              // Progress bar solid part
              Infoprogress.setValue(card.percentDone());
              Infoplay.setPic(3);
              Infofilament.setPic(2);
              Infostop.setPic(5);
              // Estimate End Time
              uint16_t time = print_job_timer.duration() / 60;
              uint16_t end_time = (time * (100 - card.percentDone())) / card.percentDone();
              if (end_time > (60 * 23)) {
                lcd_setstatus("End --:--");
              }
              else if (end_time >= 0) {
                char temp[30];
                sprintf_P(temp, PSTR("End %i:%i"), end_time / 60, end_time%60);
                lcd_setstatus(temp);
              }
            }
            else {
              Infoplay.setPic(4);
              Infostop.setPic(61);
              Infofilament.setPic(65);
            }
          }
          else if (card.cardOK && SDstatus != 1) {
            Infoplay.setPic(60);
            Infostop.setPic(61);
            Infofilament.setPic(65);
          }
          else if (!card.cardOK && SDstatus != 0) {
            Infoplay.setPic(60);
            Infostop.setPic(61);
            Infofilament.setPic(65);           
          }
          
         lcd_sd_status = sd_status;
        #endif                
              
      }     
      
    }
  }

  void lcd_setstatus(const char* message, bool persist) {
    if (lcd_status_message_level > 0 || !NextionON) return;
    strncpy(lcd_status_message, message, 30);
    Infostatus.setText(lcd_status_message);
  }

  void lcd_setstatuspgm(const char* message, uint8_t level) {
    if (level >= lcd_status_message_level && NextionON) {
      strncpy_P(lcd_status_message, message, 30);
      lcd_status_message_level = level;
      Infostatus.setText(lcd_status_message);
    }
  }

  void lcd_setalertstatuspgm(const char* message) {
    lcd_setstatuspgm(message, 1);
  }

  void lcd_reset_alert_level() { lcd_status_message_level = 0; }

  
  
  
  /***********************************/
  /** SUTAS **************************/
  /***********************************/   /*
  int lcd_strlen(const char* s) {
  int i = 0, j = 0;
  while (s[i]) {
    if ((s[i] & 0xc0) != 0x80) j++;
    i++;
  }
  return j;
}

int lcd_strlen_P(const char* s) {
  int j = 0;
  while (pgm_read_byte(s)) {
    if ((pgm_read_byte(s) & 0xc0) != 0x80) j++;
    s++;
  }
  return j;
}

*/ 

  /*********************************/
  /** Number to string conversion **/
  /*********************************/



  // Convert float to rj string with _123, -123, _-12, or __-1 format
/*  char *ftostr4sign(const float& x) { return itostr4sign((int)x); }
    
  // Convert float to space-padded string with -_23.4_ format
  char *ftostr32sp(const float &x) {
    long xx = abs(x * 100);
    uint8_t dig;
    char conv[8];

    if (x < 0) { // negative val = -_0
      conv[0] = '-';
      dig = (xx / 1000) % 10;
      conv[1] = dig ? '0' + dig : ' ';
    }
    else { // positive val = __0
      dig = (xx / 10000) % 10;
      if (dig) {
        conv[0] = '0' + dig;
        conv[1] = '0' + (xx / 1000) % 10;
      }
      else {
        conv[0] = ' ';
        dig = (xx / 1000) % 10;
        conv[1] = dig ? '0' + dig : ' ';
      }
    }

    conv[2] = '0' + (xx / 100) % 10; // lsd always

    dig = xx % 10;
    if (dig) { // 2 decimal places
      conv[5] = '0' + dig;
      conv[4] = '0' + (xx / 10) % 10;
      conv[3] = '.';
    }
    else { // 1 or 0 decimal place
      dig = (xx / 10) % 10;
      if (dig) {
        conv[4] = '0' + dig;
        conv[3] = '.';
      }
      else {
        conv[3] = conv[4] = ' ';
      }
      conv[5] = ' ';
    }
    conv[6] = '\0';
    return conv;
  }    

  // Convert int to rj string with _123, -123, _-12, or __-1 format
  char* itostr4sign(const int& x) {
    int xx = abs(x);
    int sign = 0;
    if (xx >= 100) {
      conv[1] = (xx / 100) % 10 + '0';
      conv[2] = (xx / 10) % 10 + '0';
    }
    else if (xx >= 10) {
      conv[0] = ' ';
      sign = 1;
      conv[2] = (xx / 10) % 10 + '0';
    }
    else {
      conv[0] = ' ';
      conv[1] = ' ';
      sign = 2;
    }
    conv[sign] = x < 0 ? '-' : ' ';
    conv[3] = xx % 10 + '0';
    conv[4] = 0;
    return conv;
  }       
  
// Convert signed float to string with +1234.5 format
char* ftostr51(const float& x) {
  long xx = abs(x * 10);
  char conv[8];
  conv[0] = (x >= 0) ? '+' : '-';
  conv[1] = (int)(xx / 10000) % 10 + '0';
  conv[2] = (int)(xx / 1000) % 10 + '0';
  conv[3] = (int)(xx / 100) % 10 + '0';
  conv[4] = (int)(xx / 10) % 10 + '0';
  conv[5] = '.';
  conv[6] = (int)xx % 10 + '0';
  conv[7] = '/0';
  return conv;
}        */

int getDecimal(float val)
{
  int intPart = int(val);
  int decPart = 100*(val-intPart); //I am multiplying by 100 assuming that the foat values will have a maximum of 3 decimal places. 
                                    //Change to match the number of decimal places you need
  if(decPart>0)return(decPart);           //return the decimal part of float number if it is available 
  else if(decPart<0)return((-1)*decPart); //if negative, multiply by -1
  else if(decPart=0)return(0);           //return 0 if decimal part of float number is not available
}
       
#endif

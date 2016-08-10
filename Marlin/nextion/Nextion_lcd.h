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
 * Nextion_lcd.h
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

#ifndef NEXTIONLCD_H
  #define NEXTIONLCD_H

  #if ENABLED(NEXTION)
    #define LCD_UPDATE_INTERVAL 4000

    void setmovePopCallback(void *ptr);  //MOVE
    void setfanPopCallback(void *ptr);   //MOVE

    void cooldownPopCallback(void *ptr);        //PREPARE
    void PreheatplaPopCallback(void *ptr);      //PREPARE
    void PreheatabsPopCallback(void *ptr);      //PREPARE
    void FlowupPopCallback(void *ptr);          //PREPARE
    void FlowdownPopCallback(void *ptr);        //PREPARE
    void SpeeddownPopCallback(void *ptr);       //PREPARE
    void SpeedupPopCallback(void *ptr);         //PREPARE
    
//    void InfoPopCallback(void *ptr);            //INFO

//    void ExitPopCallback(void *ptr);
//    void hotPopCallback(void *ptr);
//    void sethotPopCallback(void *ptr);
    void settempPopCallback(void *ptr);         //info

    void lcd_update();                          //
    void lcd_init();                            //
    void lcd_setstatus(const char* message, const bool persist = false);   //
    void lcd_setstatuspgm(const char* message, const uint8_t level = 0);    //
    void lcd_setalertstatuspgm(const char* message);                       //
    void lcd_reset_alert_level();                                          //
    
    #if ENABLED(SDSUPPORT)
      void setpageSDPopCallback(void *ptr);          //SD
      void sdlistPopCallback(void *ptr);             //SD
      void sdfilePopCallback(void *ptr);            //SD
      void sdfolderPopCallback(void *ptr);          //SD
      void sdfolderUpPopCallback(void *ptr);        //SD
      void PlayPausePopCallback(void *ptr);         //SD
      void StopPopCallback(void *ptr);              //SD
 //     void UploadNewFirmware();
    #endif

    FORCE_INLINE bool lcd_hasstatus() { return false; }
    FORCE_INLINE void lcd_buttons_update() {}
    FORCE_INLINE bool lcd_detected(void) { return true; }

    #define LCD_MESSAGEPGM(x) lcd_setstatuspgm(PSTR(x))
    #define LCD_ALERTMESSAGEPGM(x) lcd_setalertstatuspgm(PSTR(x))
    
/*    int lcd_strlen(const char* s);      //SUTAS
    int lcd_strlen_P(const char* s);    //SUTAS
*/
/*    char* itostr4sign(const int& x);                                      //
    char* ftostr4sign(const float& x);                                    // 
    char* ftostr32sp(const float& x); // remove zero-padding from ftostr32     
    char* ftostr51(const float& x);   */
    int getDecimal(float val);

  extern bool cancel_heatup; 
/*  
  extern int plaPreheatHotendTemp;
  extern int plaPreheatHPBTemp;
  extern int plaPreheatFanSpeed;
  extern int absPreheatHotendTemp;
  extern int absPreheatHPBTemp;
  extern int absPreheatFanSpeed;     
*/
  #endif
#endif // NEXTIONLCD_H

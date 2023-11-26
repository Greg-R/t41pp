#ifndef BEENHERE
#include "SDT.h"
#endif

//DB2OO, 29-AUG-23: Don't use the overall VERSION for the EEPROM structure version information, but use a combination of an EEPROM_VERSION with the size of the EEPROMData variable.
// The "EEPROM_VERSION" should only be changed, if the structure config_t EEPROMData has changed!
// For V049.1 the new version in EEPROM will be "V049_808", for V049.2 it will be "V049_812"
#define EEPROM_VERSION  "V049"
static char version_size[10];

/*****
  Purpose: void EEPROMSetVersion()

  Parameter list:

  Return value;
    char* pointer to EEPROM version string of the form "V049_808"
*****/
static char* EEPROMSetVersion(void)
{
    size_t  l;
  strncpy(version_size, EEPROM_VERSION, sizeof(version_size));
  l = strlen(version_size);
  //enough space to append '_' and 4 characters for size
  if ((sizeof(version_size)-l) > 5) {
    version_size[l] = '_';
    itoa(sizeof(EEPROMData), version_size+l+1, 10);
  }
  return version_size;
}

/*****
  Purpose: void EEPROMRead()

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/
void EEPROMRead() {
  int i;
  int v049_version=0; //DB2OO, 10-SEP-23
#define MORSE_STRING_DISPLAY(s)  {size_t j; for (j=0;j<strlen(s);j++) MorseCharacterDisplay(s[j]);}  

  //DB2OO, 25-AUG-23: don't read the EEPROM before you are sure, that it is in T41-SDT format!!
  //DB2OO, 25-AUG-23: first read only the version string and compare it with the current version. The version string must also be at the beginning of the EEPROMData structure!
  for (i=0; i<10; i++) { EEPROMData.versionSettings[i]= EEPROM.read(EEPROM_BASE_ADDRESS+i); }
#ifdef DEBUG 
    //display version in EEPROM in last line of display
    MORSE_STRING_DISPLAY("EEPROMVersion ");
    if (strlen(versionSettings) <10) {
      MORSE_STRING_DISPLAY(versionSettings);
    }else {
      MORSE_STRING_DISPLAY("<<INVALID>>");
    }
    MyDelay(1000);
#endif
  //Do we have V049.1 or V049.2 structure in EEPROM?
  if (strcmp("V049.1", EEPROMData.versionSettings) == 0) v049_version=1;
  if (strcmp("V049.2", EEPROMData.versionSettings) == 0) v049_version=2;

  if (v049_version > 0) {
     //DB2OO, 29-AUG-23: allow "V049.1" or "V049.2" instead of the Version with size for a migration to the new format
     strcpy(EEPROMData.versionSettings, EEPROMSetVersion());  // set new version format
    for (i=0; i<10; i++) { EEPROM.write(EEPROM_BASE_ADDRESS+i, EEPROMData.versionSettings[i]); }
  }
  if (strncmp(EEPROMSetVersion(), EEPROMData.versionSettings, 10) != 0) {
    //Different version in EEPROM: set the EEPROM values for THIS version
#ifdef DEBUG  
    const char *wrong_ver = "EEPROMRead(): different version, calling EEPROMSaveDefaults2()";
    MORSE_STRING_DISPLAY(wrong_ver);
    Serial.println(wrong_ver);
    MyDelay(1000);
#endif    
    EEPROMSaveDefaults2();
    // and write it into the EEPROM
    EEPROM.put(EEPROM_BASE_ADDRESS, EEPROMData);
    // after this we will read the default values for this version
  } else {
#ifdef DEBUG  
    MORSE_STRING_DISPLAY("-->Reading EEPROM content...");
    MyDelay(1000);
#endif
  }
#ifdef DEBUG
  //clear the Morse character buffer
  MorseCharacterClear();
#endif  
  
  EEPROM.get(EEPROM_BASE_ADDRESS, EEPROMData);  // Read as one large chunk
}


/*****
  Purpose: To save the configuration data (working variables) to EEPROM

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/
void EEPROMWrite() {
  //strncpy(EEPROMData.versionSettings, EEPROMSetVersion(), 9);  // KF5N

  //EEPROMData.lastFrequencies[EEPROMData.currentBand][EEPROMData.activeVFO] = currentFreq;  // 4 bytes
  EEPROM.put(EEPROM_BASE_ADDRESS, EEPROMData);
  // CopyEEPROMToSD();                                               // JJP 7/26/23
//  syncEEPROM = 0;  // SD EEPROM different that memory EEPROM
}  // end void eeProm SAVE

/*****
  Purpose: To show the current EEPROM values. Used for debugging

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/

// Prints the content of a file to the Serial
void EEPROMShow(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}


/*****
  Purpose: Read default favorite frequencies

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/
void EEPROMStuffFavorites(unsigned long current[]) {
  int i;

  for (i = 0; i < MAX_FAVORITES; i++) {
    current[i] = EEPROMData.favoriteFreqs[i];
  }
}

/*****
  Purpose: Used to save a favortie frequency to EEPROM

  Parameter list:

  Return value;
    void

  CAUTION: This code assumes you have set the curently active VFO frequency to the new
           frequency you wish to save. You them use the menu encoder to scroll through
           the current list of stored frequencies. Stop on the one that you wish to
           replace and press Select to save in EEPROM. The currently active VFO frequency
           is then stored to EEPROM.
*****/
void SetFavoriteFrequency() {
  int index;
  int val;

  tft.setFontScale((enum RA8875tsize)1);

  index = 0;
  tft.setTextColor(RA8875_WHITE);
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setCursor(SECONDARY_MENU_X, MENUS_Y);
  tft.print(EEPROMData.favoriteFreqs[index]);
  while (true) {
    if (filterEncoderMove != 0) {  // Changed encoder?
      index += filterEncoderMove;  // Yep
      if (index < 0) {
        index = MAX_FAVORITES - 1;  // Wrap to last one
      } else {
        if (index > MAX_FAVORITES)
          index = 0;  // Wrap to first one
      }
      tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X, MENUS_Y);
      tft.print(EEPROMData.favoriteFreqs[index]);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);
    if (val == MENU_OPTION_SELECT) {  // Make a choice??
      EraseMenus();
      EEPROMData.favoriteFreqs[index] = TxRxFreq;
      syncEEPROM = 0;  // SD EEPROM different that memory EEPROM
      //UpdateEEPROMSyncIndicator(0);       //  JJP 7/25/23
      if (EEPROMData.activeVFO == VFO_A) {
        EEPROMData.currentFreqA = TxRxFreq;
      } else {
        EEPROMData.currentFreqB = TxRxFreq;
      }
      //      EEPROMWrite();
      SetFreq();
      BandInformation();
      ShowBandwidth();
      FilterBandwidth();
      ShowFrequency();
      break;
    }
  }
}

/*****
  Purpose: Used to fetch a favortie frequency as stored in EEPROM. It then copies that
           frequency to the currently active VFO

  Parameter list:

  Return value;
    void
*****/
void GetFavoriteFrequency() {
  int index = 0;
  int val;
  int currentBand2 = 0;
  tft.setFontScale((enum RA8875tsize)1);

  tft.setTextColor(RA8875_WHITE);
  tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
  tft.setCursor(SECONDARY_MENU_X, MENUS_Y);
  tft.print(EEPROMData.favoriteFreqs[index]);
  while (true) {
    if (filterEncoderMove != 0) {  // Changed encoder?
      index += filterEncoderMove;  // Yep
      if (index < 0) {
        index = MAX_FAVORITES - 1;  // Wrap to last one
      } else {
        if (index > MAX_FAVORITES)
          index = 0;  // Wrap to first one
      }
      tft.fillRect(SECONDARY_MENU_X, MENUS_Y, EACH_MENU_WIDTH, CHAR_HEIGHT, RA8875_MAGENTA);
      tft.setCursor(SECONDARY_MENU_X, MENUS_Y);
      tft.print(EEPROMData.favoriteFreqs[index]);
      filterEncoderMove = 0;
    }

    val = ReadSelectedPushButton();  // Read pin that controls all switches
    val = ProcessButtonPress(val);
    MyDelay(150L);

    //centerFreq = EEPROMData.favoriteFreqs[index];  // current frequency  AFP 09-27-22
    if (EEPROMData.centerFreq >= bands[BAND_80M].fBandLow && EEPROMData.centerFreq <= bands[BAND_80M].fBandHigh) {
      currentBand2 = BAND_80M;
    } else if (EEPROMData.centerFreq >= bands[BAND_80M].fBandHigh && EEPROMData.centerFreq <= 7000000L) {  // covers 5MHz WWV AFP 11-03-22
      currentBand2 = BAND_80M;
    } else if (EEPROMData.centerFreq >= bands[BAND_40M].fBandLow && EEPROMData.centerFreq <= bands[BAND_40M].fBandHigh) {
      currentBand2 = BAND_40M;
    } else if (EEPROMData.centerFreq >= bands[BAND_40M].fBandHigh && EEPROMData.centerFreq <= 14000000L) {  // covers 10MHz WWV AFP 11-03-22
      currentBand2 = BAND_40M;
    } else if (EEPROMData.centerFreq >= bands[BAND_20M].fBandLow && EEPROMData.centerFreq <= bands[BAND_20M].fBandHigh) {
      currentBand2 = BAND_20M;
    } else if (EEPROMData.centerFreq >= 14000000L && EEPROMData.centerFreq <= 18000000L) {  // covers 15MHz WWV AFP 11-03-22
      currentBand2 = BAND_20M;
    } else if (EEPROMData.centerFreq >= bands[BAND_17M].fBandLow && EEPROMData.centerFreq <= bands[BAND_17M].fBandHigh) {
      currentBand2 = BAND_17M;
    } else if (EEPROMData.centerFreq >= bands[BAND_15M].fBandLow && EEPROMData.centerFreq <= bands[BAND_15M].fBandHigh) {
      currentBand2 = BAND_15M;
    } else if (EEPROMData.centerFreq >= bands[BAND_12M].fBandLow && EEPROMData.centerFreq <= bands[BAND_12M].fBandHigh) {
      currentBand2 = BAND_12M;
    } else if (EEPROMData.centerFreq >= bands[BAND_10M].fBandLow && EEPROMData.centerFreq <= bands[BAND_10M].fBandHigh) {
      currentBand2 = BAND_10M;
    }
    EEPROMData.currentBand = currentBand2;


    if (val == MENU_OPTION_SELECT) {  // Make a choice??
      switch (EEPROMData.activeVFO) {
        case VFO_A:
          if (EEPROMData.currentBandA == NUMBER_OF_BANDS) {  // Incremented too far?
            EEPROMData.currentBandA = 0;                     // Yep. Roll to list front.
          }
          EEPROMData.currentBandA = currentBand2;
          TxRxFreq = EEPROMData.centerFreq + NCOFreq;
          EEPROMData.lastFrequencies[EEPROMData.currentBand][VFO_A] = TxRxFreq;
          break;

        case VFO_B:
          if (EEPROMData.currentBandB == NUMBER_OF_BANDS) {  // Incremented too far?
            EEPROMData.currentBandB = 0;                     // Yep. Roll to list front.
          }                                       // Same for VFO B
          EEPROMData.currentBandB = currentBand2;
          TxRxFreq = EEPROMData.centerFreq + NCOFreq;
          EEPROMData.lastFrequencies[EEPROMData.currentBand][VFO_B] = TxRxFreq;
          break;
      }
    }
    if (val == MENU_OPTION_SELECT) {

      EraseSpectrumDisplayContainer();
      currentMode = bands[EEPROMData.currentBand].mode;
      DrawSpectrumDisplayContainer();
      DrawFrequencyBarValue();
      SetBand();
      SetFreq();
      ShowFrequency();
      ShowSpectrumdBScale();
      EraseMenus();
      ResetTuning();
      FilterBandwidth();
      BandInformation();
      NCOFreq = 0L;
      DrawBandWidthIndicatorBar();  // AFP 10-20-22
      digitalWrite(bandswitchPins[EEPROMData.currentBand], LOW);
      SetFreq();
      ShowSpectrumdBScale();
      ShowSpectrum();
      //bands[currentBand].mode = currentBand;
      return;
    }
  }
}


/*****
  Purpose: To save the default setting for EEPROM variables

  Parameter list:
    struct config_t e[]       pointer to the EEPROM structure

  Return value;
    void
*****/

void EEPROMSaveDefaults2() {
  strcpy(EEPROMData.versionSettings, EEPROMSetVersion());  // Update version

  EEPROMData.AGCMode = 1;
  EEPROMData.audioVolume = 30;  // 4 bytes
  EEPROMData.rfGainAllBands = 0;
  EEPROMData.spectrumNoiseFloor = SPECTRUM_NOISE_FLOOR;
  EEPROMData.tuneIndex = 5;
  EEPROMData.stepFineTune = 50L;
  EEPROMData.transmitPowerLevel = 10;
  EEPROMData.xmtMode = 0;
  EEPROMData.nrOptionSelect = 0;  // 1 byte
  EEPROMData.currentScale = 1;
  EEPROMData.spectrum_zoom = 1;
  EEPROMData.spectrum_display_scale = 20.0;  // 4 bytes

  EEPROMData.CWFilterIndex = 5;  // Off
  EEPROMData.paddleDit = 36;
  EEPROMData.paddleDah = 35;
  EEPROMData.decoderFlag = 0;
  EEPROMData.keyType = 0;            // straight key = 0, keyer = 1
  EEPROMData.currentWPM = 15;        // 4 bytes
  EEPROMData.sidetoneVolume = 50.0;  // 4 bytes.  Changed to default 50.  KF5N October 7, 2023.
  EEPROMData.cwTransmitDelay = 750;  // 4 bytes

  EEPROMData.activeVFO = 0;      // 2 bytes, 0 = VFOa
  EEPROMData.freqIncrement = 5;  // 4 bytes
  EEPROMData.currentBand = 1;    // 4 bytes
  EEPROMData.currentBandA = 1;   // 4 bytes
  EEPROMData.currentBandB = 1;   // 4 bytes
  //DB2OO, 23-AUG-23 7.1MHz for Region 1
#if defined(ITU_REGION) && ITU_REGION==1
  EEPROMData.currentFreqA = 7100000;
#else  
  EEPROMData.currentFreqA = 7200000;
#endif
  EEPROMData.currentFreqB = 7030000;
  //DB2OO, 23-AUG-23: with TCXO needs to be 0
#ifdef TCXO_25MHZ  
  EEPROMData.freqCorrectionFactor = 0; //68000;
#else
  //Conventional crystal with freq offset needs a correction factor
  EEPROMData.freqCorrectionFactor = 68000;
#endif  

  for (int i = 0; i < EQUALIZER_CELL_COUNT; i++) {
    EEPROMData.equalizerRec[i] = 100;  // 4 bytes each
  }
  // Use transmit equalizer profile in struct initializer list in SDT.h.  KF5N November 2, 2023

  EEPROMData.currentMicThreshold = -10;  // 4 bytes      // AFP 09-22-22
  EEPROMData.currentMicCompRatio = 8.0;  // Changed to 8.0 from 5.0 based on Neville's tests.  KF5N November 2, 2023
  EEPROMData.currentMicAttack = 0.1;
  EEPROMData.currentMicRelease = 0.1;    // Changed to 0.1 from 2.0 based on Neville's tests.  KF5N November 2, 2023
  //DB2OO, 23-AUG-23: MicGain 20
  EEPROMData.currentMicGain = 20;

  EEPROMData.switchValues[0] = 924;
  EEPROMData.switchValues[1] = 870;
  EEPROMData.switchValues[2] = 817;
  EEPROMData.switchValues[3] = 769;
  EEPROMData.switchValues[4] = 713;
  EEPROMData.switchValues[5] = 669;
  EEPROMData.switchValues[6] = 616;
  EEPROMData.switchValues[7] = 565;
  EEPROMData.switchValues[8] = 513;
  EEPROMData.switchValues[9] = 459;
  EEPROMData.switchValues[10] = 407;
  EEPROMData.switchValues[11] = 356;
  EEPROMData.switchValues[12] = 298;
  EEPROMData.switchValues[13] = 242;
  EEPROMData.switchValues[14] = 183;
  EEPROMData.switchValues[15] = 131;
  EEPROMData.switchValues[16] = 67;
  EEPROMData.switchValues[17] = 10;

  EEPROMData.LPFcoeff = 0.0;     // 4 bytes
  EEPROMData.NR_PSI = 0.0;       // 4 bytes
  EEPROMData.NR_alpha = 0.0;     // 4 bytes
  EEPROMData.NR_beta = 0.0;      // 4 bytes
  EEPROMData.omegaN = 0.0;       // 4 bytes
  EEPROMData.pll_fmax = 4000.0;  // 4 bytes

  EEPROMData.powerOutCW[0] = 0.188;  // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[1] = 0.21;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[2] = 0.34;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[3] = 0.44;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[4] = 0.31;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[5] = 0.31;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutCW[6] = 0.31;   // 4 bytes  AFP 10-21-22

  EEPROMData.powerOutSSB[0] = 0.188;  // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[1] = 0.11;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[2] = 0.188;  // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[3] = 0.21;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[4] = 0.23;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[5] = 0.23;   // 4 bytes  AFP 10-21-22
  EEPROMData.powerOutSSB[6] = 0.24;   // 4 bytes  AFP 10-21-22

  EEPROMData.CWPowerCalibrationFactor[0] = 0.023;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[1] = 0.023;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[2] = 0.038;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[3] = 0.052;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[4] = 0.051;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[5] = 0.028;  //AFP 10-29-22
  EEPROMData.CWPowerCalibrationFactor[6] = 0.028;  //AFP 10-29-22

  EEPROMData.SSBPowerCalibrationFactor[0] = 0.017;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[1] = 0.019;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[2] = 0.017;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[3] = 0.019;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[4] = 0.021;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[5] = 0.020;  //AFP 10-29-22
  EEPROMData.SSBPowerCalibrationFactor[6] = 0.022;  //AFP 10-29-22

  EEPROMData.IQAmpCorrectionFactor[0] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[1] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[2] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[3] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[4] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[5] = 1.0;
  EEPROMData.IQAmpCorrectionFactor[6] = 1.0;

  EEPROMData.IQPhaseCorrectionFactor[0] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[1] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[2] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[3] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[4] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[5] = 0.0;
  EEPROMData.IQPhaseCorrectionFactor[6] = 0.0;

  EEPROMData.IQXAmpCorrectionFactor[0] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[1] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[2] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[3] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[4] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[5] = 1.0;
  EEPROMData.IQXAmpCorrectionFactor[6] = 1.0;

  EEPROMData.IQXPhaseCorrectionFactor[0] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[1] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[2] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[3] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[4] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[5] = 0.0;
  EEPROMData.IQXPhaseCorrectionFactor[6] = 0.0;

  EEPROMData.favoriteFreqs[0] = 3560000L;  // These are CW/SSB calling frequencies for HF bands
  EEPROMData.favoriteFreqs[1] = 3690000L;
  EEPROMData.favoriteFreqs[2] = 7030000L;
  EEPROMData.favoriteFreqs[3] = 7200000L;
  EEPROMData.favoriteFreqs[4] = 14060000L;
  EEPROMData.favoriteFreqs[5] = 14200000L;
  EEPROMData.favoriteFreqs[6] = 21060000L;
  EEPROMData.favoriteFreqs[7] = 21285000L;
  EEPROMData.favoriteFreqs[8] = 28060000L;
  EEPROMData.favoriteFreqs[9] = 28365000L;
  EEPROMData.favoriteFreqs[10] = 5000000L;
  EEPROMData.favoriteFreqs[11] = 10000000L;
  EEPROMData.favoriteFreqs[12] = 15000000L;

  //DB2OO, 23-AUG-23: Region 1 freqs (from https://qrper.com/qrp-calling-frequencies/)
#if defined(ITU_REGION) && ITU_REGION==1  
  EEPROMData.lastFrequencies[0][0] = 3690000L; //3985000L;   // 80 Phone
  EEPROMData.lastFrequencies[1][0] = 7090000L; //7200000L;   // 40
  EEPROMData.lastFrequencies[2][0] = 14285000L;  // 50
  EEPROMData.lastFrequencies[3][0] = 18130000L;  // 17
  EEPROMData.lastFrequencies[4][0] = 21285000L; //21385000L;  // 15
  EEPROMData.lastFrequencies[5][0] = 24950000L;  // 12
  EEPROMData.lastFrequencies[6][0] = 28365000L; //28385800L;  // 10
#else
  EEPROMData.lastFrequencies[0][0] = 3985000L;   // 80 Phone
  EEPROMData.lastFrequencies[1][0] = 7200000L;   // 40
  EEPROMData.lastFrequencies[2][0] = 14285000L;  // 50
  EEPROMData.lastFrequencies[3][0] = 18130000L;  // 17
  EEPROMData.lastFrequencies[4][0] = 21385000L;  // 15
  EEPROMData.lastFrequencies[5][0] = 24950000L;  // 12
  EEPROMData.lastFrequencies[6][0] = 28385800L;  // 10
#endif

  EEPROMData.lastFrequencies[0][1] = 3560000L;   // 80 CW
  EEPROMData.lastFrequencies[1][1] = 7030000L;   // 40
  EEPROMData.lastFrequencies[2][1] = 14060000L;  // 20
  EEPROMData.lastFrequencies[3][1] = 18096000L;  // 17
  EEPROMData.lastFrequencies[4][1] = 21060000L;  // 15
  EEPROMData.lastFrequencies[5][1] = 24906000L;  // 12
  EEPROMData.lastFrequencies[6][1] = 28060000L;  // 10

  EEPROMData.centerFreq = 7150000;

  strncpy(EEPROMData.mapFileName, MAP_FILE_NAME, 50);
  strncpy(EEPROMData.myCall, MY_CALL, 10);
  strncpy(EEPROMData.myTimeZone, MY_TIMEZONE, 10);
  EEPROMData.separationCharacter = (char)FREQ_SEP_CHARACTER;  //   JJP  7/25/23

  //EEPROMData.paddleFlip = paddleFlip;
  EEPROMData.sdCardPresent = 0;  //   JJP  7/18/23

  EEPROMData.myLat = MY_LAT;
  EEPROMData.myLong = MY_LON;

  for (int i = 0; i < NUMBER_OF_BANDS; i++) {
    EEPROMData.currentNoiseFloor[i] = 0;
  }
  EEPROMData.compressorFlag = 0;                    // Off by default JJP  8/28/23
}


/*****
  Purpose: Provides a display indicator that the memory-based EEPROM is different than SD card EEPROM

  Parameter list:
    int inSync                    0 = no, memory and SD card not necessarily the same, 1 = same

  Return value;
    void

*****/
void UpdateEEPROMSyncIndicator(int inSync) {
  return;  // Just go home   JJP  7/25/23
}


/*****
  Purpose: Converts EEPROMData members and value to ASCII

  Parameter list:
    File file             file handle for the SD file
    char *buffer          pointer to the EEPROMData member
    int val               the current value of the member
    int whatDataType      1 = int, 2 = long, 3 = float, 4 = string

  Return value;
    void

*****/
void ConvertForEEPROM(File file, char *buffer, int val, int whatDataType) {
  char temp[10];

  temp[0] = '\0';
  switch (whatDataType) {
    case 1:  // int
      itoa(val, temp, DEC);
      break;
    case 2:  // long
      ltoa(val, temp, DEC);
      break;
    case 3:                      // float
      dtostrf(val, 9, 4, temp);  //Field of up to 9 digits with 4 decimal places
      break;
    case 4:
      strcpy(temp, EEPROMSetVersion());
      break;
    default:
#ifdef DEBUG
      Serial.println("Error");
#endif
      break;
  }
  strcat(buffer, " = ");
  strcat(buffer, temp);
#ifdef DEBUG
  Serial.println(buffer);
#endif
  file.println(buffer);
  buffer[0] = '\0';
}


/*****
  Purpose: See if the EEPROM has ever been set

  Parameter list:
    void

  Return value;
    int               1 = used before, 0 = nope
*****/
int ValidEEPROMData() {
  int val = EEPROMData.switchValues[0];
  if (val > 0 && val < 1023)
    return VALID_EEPROM_DATA;  // return 1
  else
    return INVALID_EEPROM_DATA;  // return 0
}

/*****
  Purpose: Update the version number only in EEPROM

  Parameter list:
    void

  Return value;
    void
*****/
void UpdateEEPROMVersionNumber() {
  strcpy(EEPROMData.versionSettings, EEPROMSetVersion());  // Copy the latest version to EEPROM
}


/*****
  Purpose: Reads the SD EEPROM data and writes it to the Serial object

  Parameter list:
   const char *filename

  Return value;
    int               0 = SD is untouched, 1 = has data
*****/
void SDEEPROMDump(const char *filename) {
  // Open file for reading
  File file = SD.open(filename);
  if (!file) {
    Serial.println(F("Failed to read file"));
    return;
  }

  // Extract each characters by one by one
  while (file.available()) {
    Serial.print((char)file.read());
  }
  Serial.println();

  // Close the file
  file.close();
}

/*****
  Purpose: Clears the first 1K of emulated EEPROM to 0xff

  Parameter list:
    void

  Return value;
    void
*****/
void ClearEEPROM() {
  int i;
  for (i = 0; i < 1000; i++) {
    EEPROM.write(i, 0xFF);
  }
}

/*****
  Purpose: Read the EEPROM from: a) EEPROM memory, b) SD card memory, or c) defaults

  Parameter list:
    void

  Return value;
    void
*****/
void EEPROMStartup() {
  EEPROMRead();  // Read current stored data

  if (strcmp(EEPROMData.versionSettings, EEPROMSetVersion()) == 0) {  // Are the versions the same?
    return;                                                // Yep. Go home and don't mess with the EEPROM
  }
  strcpy(EEPROMData.versionSettings, EEPROMSetVersion());  // Nope, this is a new the version, so copy new version title to EEPROM
                                                //                                                                     Check if calibration has not been done and/or switch values are wonky, okay to use defaults
                                                //                                                                     If the Teensy is unused, these EEPROM values are 0xFF or perhaps cleared to 0.

  if (EEPROMData.switchValues[9] < 440 || EEPROMData.switchValues[9] > 480) {
    EEPROMSaveDefaults2();     // At least give them some starting values
    EEPROMData.switchValues[9] = 0;  // This will force the next code block to set the switch values.
  }
  if (EEPROMData.switchValues[9] < 440 || EEPROMData.switchValues[9] > 480) {  // If the Teensy is unused, these EEPROM values are 0xFF or perhaps cleared to 0.
    SaveAnalogSwitchValues();                                      // In that case, we need to set the switch values.
  }
  //                                                                     If we get here, the switch values have been set, either previously or by the call to
  //                                                                     SaveAnalogSwitchValues() as has the rest of the EEPROM data. This avoids recalibration.
  EEPROM.put(0, EEPROMData);  // This rewrites the entire EEPROM struct as defined in SDT.h
  EEPROMRead();               // Read the EEPROM data, including new switch values. This also resets working variables

#ifdef DEBUG1
  SDEEPROMDump();  // Call this to observe EEPROM struct data
#endif
}

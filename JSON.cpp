#ifndef BEENHERE
#include "SDT.h"
#endif

// JSON experiment.  This was derived from a JSON example from the ArduinoJSON library.
// Our EEPROMDatauration structure.
//
// Never use a JsonDocument to store the EEPROMDatauration!
// A JsonDocument is *not* a permanent storage; it's only a temporary storage
// used during the serialization phase. See:
// https://arduinojson.org/v6/faq/why-must-i-create-a-separate-EEPROMData-object/
                       // <- global EEPROMDatauration object

// Loads the EEPROMDatauration from a file
FLASHMEM void loadConfiguration(const char *filename, config_t &EEPROMData) {
  // Open file for reading
  File file = SD.open(filename);

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use https://arduinojson.org/v6/assistant to compute the capacity.
  // StaticJsonDocument<512> doc;
  DynamicJsonDocument doc(4096);   // This uses the heap.
  //  Need to also create JSON arrays.
  
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, file);
  if (error) {
    Serial.println(F("Failed to read file, using default EEPROMDatauration"));
  EEPROMRead();
  return;
  }

 // Copy values from the JsonDocument to the EEPROMData
  // How to copy numbers:
//  EEPROMData.versionSettings = doc["versionSettings"];
  strlcpy(EEPROMData.versionSettings, doc["versionSettings"] | "t41pp.0", 10);
  EEPROMData.AGCMode = doc["AGCMode"];
  EEPROMData.audioVolume = doc["audioVolume"];
  EEPROMData.rfGainAllBands = doc["rfGainAllBands"];
  EEPROMData.spectrumNoiseFloor = doc["spectrumNoiseFloor"];
  EEPROMData.tuneIndex = doc["tuneIndex"];
  EEPROMData.stepFineTune = doc["stepFineTune"];
  EEPROMData.transmitPowerLevel = doc["powerLevel"];
  EEPROMData.xmtMode = doc["xmtMode"];
  EEPROMData.nrOptionSelect = doc["nrOptionSelect"];
  EEPROMData.currentScale = doc["currentScale"];
 EEPROMData.spectrum_zoom  = doc["spectrum_zoom"];
 EEPROMData.spectrum_display_scale  = doc["spectrum_display_scale"];
 EEPROMData.CWFilterIndex  = doc["CWFilterIndex"];
 EEPROMData.paddleDit  = doc["paddleDit"];
 EEPROMData.paddleDah  = doc["paddleDah"];
 EEPROMData.decoderFlag  = doc["decoderFlag"];
 EEPROMData.keyType  = doc["keyType"];
 EEPROMData.currentWPM  = doc["currentWPM"];
 EEPROMData.sidetoneVolume  = doc["sidetoneVolume"];
EEPROMData.cwTransmitDelay = doc["cwTransmitDelay"];
EEPROMData.activeVFO  = doc["activeVFO"];
EEPROMData.freqIncrement  = doc["freqIncrement"];
EEPROMData.currentBand  = doc["currentBand"];
EEPROMData.currentBandA  = doc["currentBandA"];
EEPROMData.currentBandB  = doc["currentBandB"];
EEPROMData.currentFreqA  = doc["currentFreqA"];
EEPROMData.currentFreqB  = doc["currentFreqB"];
EEPROMData.freqCorrectionFactor  = doc["freqCorrectionFactor"];
for(int i = 0; i < 14; i++) EEPROMData.equalizerRec[i] = doc["equalizerRec"][i];
EEPROMData.equalizerXmt[0]  = doc["equalizerXmt"][0];
EEPROMData.currentMicThreshold  = doc["currentMicThreshold"];
EEPROMData.currentMicCompRatio  = doc["currentMicCompRatio"];
EEPROMData.currentMicAttack  = doc["currentMicAttack"];
EEPROMData.currentMicRelease  = doc["currentMicRelease"];
EEPROMData.currentMicGain  = doc["currentMicGain"];
for(int i = 0; i < 18; i++) EEPROMData.switchValues[i]  = doc["switchValues"][i];
EEPROMData.LPFcoeff  = doc["LPFcoeff"];
EEPROMData.NR_PSI  = doc["NR_PSI"];
EEPROMData.NR_alpha  = doc["NR_alpha"];
EEPROMData.NR_beta  = doc["NR_beta"];
EEPROMData.omegaN  = doc["omegaN"];
EEPROMData.pll_fmax  = doc["pll_fmax"];
EEPROMData.powerOutCW[0]  = doc["powerOutCW"][0];
EEPROMData.powerOutSSB[0]  = doc["powerOutSSB"][0];
for(int i = 0; i < 7; i++) EEPROMData.CWPowerCalibrationFactor[i]  = doc["CWPowerCalibrationFactor"][i];
for(int i = 0; i < 7; i++) EEPROMData.SSBPowerCalibrationFactor[i]  = doc["SSBPowerCalibrationFactor"][i];
for(int i = 0; i < 7; i++) EEPROMData.IQAmpCorrectionFactor[i]  = doc["IQAmpCorrectionFactor"][i];
for(int i = 0; i < 7; i++) EEPROMData.IQPhaseCorrectionFactor[i]  = doc["IQPhaseCorrectionFactor"][i];
for(int i = 0; i < 7; i++) EEPROMData.IQXAmpCorrectionFactor[i]  = doc["IQXAmpCorrectionFactor"][i];
for(int i = 0; i < 7; i++) EEPROMData.IQXPhaseCorrectionFactor[i]  = doc["IQXPhaseCorrectionFactor"][i];
for(int i = 0; i < 13; i++) EEPROMData.favoriteFreqs[i]  = doc["favoriteFreqs"][i];
for(int i = 0; i < 7; i++) {
  for(int j = 0; j < 2; j++) EEPROMData.lastFrequencies[0][0]  = doc["lastFrequencies"][0][0];
}
EEPROMData.centerFreq  = doc["centerFreq"];
//EEPROMData.mapFileName  = doc["mapFileName"] | "Boston";
strlcpy(EEPROMData.mapFileName, doc["mapFileName"] | "Boston", 50);
//EEPROMData.myCall  = doc["myCall"];
strlcpy(EEPROMData.myCall, doc["myCall"] | "Your Call", 10);
//EEPROMData.myTimeZone  = doc["myTimeZone"];
strlcpy(EEPROMData.myTimeZone, doc["myTimeZone"] | "EST", 10);
EEPROMData.separationCharacter  = doc["separationCharacter"];
EEPROMData.paddleFlip  = doc["paddleFlip"];
EEPROMData.sdCardPresent  = doc["sdCardPresent"];
EEPROMData.myLong  = doc["myLong"];
EEPROMData.myLat  = doc["myLat"];
EEPROMData.currentNoiseFloor[0] = doc["currentNoiseFloor"][0];

  // How to copy strings:
//  strlcpy(EEPROMData.myCall,                  // <- destination
//          doc["myCall"],  // <- source
//          sizeof(EEPROMData.myCall));         // <- destination's capacity

  // Close the file (Curiously, File's destructor doesn't close the file)
  file.close();
  //  At this point, the data exists only in the EEPROMData struct.
  //  Now copy the struct data into the Global variables which are used by the radio.
  //EEPROMRead();
  //Serial.printf("myCall after EEPROMRead() = %s\n", myCall);
  //Serial.printf("EEPROMData.AGCMode after EEPROMRead() = %d\n", EEPROMData.AGCMode);
//Serial.printf("EEPROMData.AGCMode after EEPROMRead() = %d\n", EEPROMData.AGCMode);


}

// Saves the EEPROMDatauration to a file
FLASHMEM void saveConfiguration(const char *filename, const config_t &EEPROMData) {
  // Delete existing file, otherwise the EEPROMDatauration is appended to the file
  SD.remove(filename);

  // Open file for writing
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  // Allocate a temporary JsonDocument
  // Don't forget to change the capacity to match your requirements.
  // Use https://arduinojson.org/assistant to compute the capacity.
  //StaticJsonDocument<256> doc;  // This uses the stack.
  DynamicJsonDocument doc(4096);   // This uses the heap.

  // Set the values in the document
  Serial.printf("EEPROMData.versionSettings = %s\n", EEPROMData.versionSettings);
  doc["versionSettings"] = EEPROMData.versionSettings;
  doc["myCall"] = EEPROMData.myCall;
  doc["AGCMode"] = EEPROMData.AGCMode;
  doc["audioVolume"] = EEPROMData.audioVolume;
  doc["rfGainAllBands"] = EEPROMData.rfGainAllBands;
  doc["spectrumNoiseFloor"] = EEPROMData.spectrumNoiseFloor;
  doc["tuneIndex"] = EEPROMData.tuneIndex;
  doc["stepFineTune"] = EEPROMData.stepFineTune;
  doc["powerLevel"] = EEPROMData.transmitPowerLevel;
  doc["xmtMode"] = EEPROMData.xmtMode;
  doc["nrOptionSelect"] = EEPROMData.nrOptionSelect;
  doc["currentScale"] = EEPROMData.currentScale;
  doc["spectrum_zoom"] = EEPROMData.spectrum_zoom;
  doc["spectrum_display_scale"] = EEPROMData.spectrum_display_scale;
  doc["CWFilterIndex"] = EEPROMData.CWFilterIndex;
  doc["paddleDit"] = EEPROMData.paddleDit;
  doc["paddleDah"] = EEPROMData.paddleDah;
  doc["decoderFlag"] = EEPROMData.decoderFlag;
  doc["keyType"] = EEPROMData.keyType;
  doc["currentWPM"] = EEPROMData.currentWPM;
  doc["sidetoneVolume"] = EEPROMData.sidetoneVolume;
  doc["cwTransmitDelay"] = EEPROMData.cwTransmitDelay;
  doc["activeVFO"] = EEPROMData.activeVFO;
  doc["freqIncrement"] = EEPROMData.freqIncrement;
  doc["currentBand"] = EEPROMData.currentBand;
  doc["currentBandA"] = EEPROMData.currentBandA;
  doc["currentBandB"] = EEPROMData.currentBandB;
  doc["freqCorrectionFactor"] = EEPROMData.freqCorrectionFactor;
  for(int i = 0; i < 14; i++) doc["equalizerRec"][i] = EEPROMData.equalizerRec[i];
  doc["currentMicThreshold"] = EEPROMData.currentMicThreshold;
  doc["currentMicCompRatio"] = EEPROMData.currentMicCompRatio;
  doc["currentMicAttack"] = EEPROMData.currentMicAttack;
  doc["currentMicRelease"] = EEPROMData.currentMicRelease;
  doc["currentMicGain"] = EEPROMData.currentMicGain;
  for(int i = 0; i < 18; i++) doc["switchValues"][i] = EEPROMData.switchValues[i];
  doc["LPFcoeff"] = EEPROMData.LPFcoeff;
  doc["NR_PSI"] = EEPROMData.NR_PSI;
  doc["NR_alpha"] = EEPROMData.NR_alpha;
  doc["NR_beta"] = EEPROMData.NR_beta;
  doc["omegaN"] = EEPROMData.omegaN;
  doc["pll_fmax"] = EEPROMData.pll_fmax;
  for(int i = 0; i < 7; i++) doc["powerOutCW"][i] = EEPROMData.powerOutCW[i];
  for(int i = 0; i < 7; i++) doc["powerOutSSB"][i] = EEPROMData.powerOutSSB[i];
  for(int i = 0; i < 7; i++) {
    Serial.printf("CWPowerCalibrationFactor = %f\n", EEPROMData.CWPowerCalibrationFactor[i]);
   doc["CWPowerCalibrationFactor"][i] = EEPROMData.CWPowerCalibrationFactor[i];
  }
  for(int i = 0; i < 7; i++) doc["SSBPowerCalibrationFactor"][i] = EEPROMData.SSBPowerCalibrationFactor[i];
  for(int i = 0; i < 7; i++) doc["IQAmpCorrectionFactor"][i] = EEPROMData.IQAmpCorrectionFactor[i];
  for(int i = 0; i < 7; i++) doc["IQPhaseCorrectionFactor"][i] = EEPROMData.IQPhaseCorrectionFactor[i];
  for(int i = 0; i < 7; i++) doc["IQXAmpCorrectionFactor"][i] = EEPROMData.IQXAmpCorrectionFactor[i];
  for(int i = 0; i < 7; i++) doc["IQXPhaseCorrectionFactor"][i] = EEPROMData.IQXPhaseCorrectionFactor[i];
  for(int i = 0; i < 13; i++) doc["favoriteFreqs"][i] = EEPROMData.favoriteFreqs[i];
  for(int i = 0; i < 7; i++) {
  for(int j = 0; j < 2; j++) doc["lastFrequencies"][i][j] = EEPROMData.lastFrequencies[i][j];
  }
  doc["centerFreq"] = EEPROMData.centerFreq;
  doc["mapFileName"] = EEPROMData.mapFileName;
  doc["myCall"] = EEPROMData.myCall;
  doc["myTimeZone"] = EEPROMData.myTimeZone;
        

  // Serialize JSON to file
  if (serializeJsonPretty(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}

// Prints the content of a file to the Serial
FLASHMEM void printFile(const char *filename) {
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


  /* Should load default EEPROMData if run for the first time
  Serial.println(F("Loading EEPROMDatauration..."));
  loadEEPROMDatauration(filename, EEPROMData);

  // Create EEPROMDatauration file
  Serial.println(F("Saving EEPROMDatauration..."));
  saveEEPROMDatauration(filename, EEPROMData);

  // Dump EEPROMData file
  Serial.println(F("Print EEPROMData file..."));
  printFile(filename);
  */
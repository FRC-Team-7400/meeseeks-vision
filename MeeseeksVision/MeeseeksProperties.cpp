#include "stdafx.h"
#include "MeeseeksProperties.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

MeeseeksProperties::MeeseeksProperties()
   : serverPort(DEFAULT_SERVER_PORT)
{
}

void MeeseeksProperties::Initialize()
{
   LoadFromFile("params/props.txt");
}

const char *MeeseeksProperties::GetString(const char *pName, const char *pDefaultValue)
{
   auto it = nameValueMap.find(pName);

   if (it == nameValueMap.end())
      return pDefaultValue;

   return it->second.c_str();
}

int MeeseeksProperties::GetInt(const char *pName, int defaultValue)
{
   auto it = nameValueMap.find(pName);

   if (it == nameValueMap.end())
      return defaultValue;

   return atoi(it->second.c_str());
}

double MeeseeksProperties::GetDouble(const char *pName, double defaultValue)
{
   auto it = nameValueMap.find(pName);

   if (it == nameValueMap.end())
      return defaultValue;

   return atof(it->second.c_str());
}

const char *MeeseeksProperties::IntToString(int intValue, char *pStringValue)
{
   sprintf(pStringValue, "%d", intValue);

   return pStringValue;
}

const char *MeeseeksProperties::DoubleToString(double doubleValue, char *pStringValue)
{
   sprintf(pStringValue, "%.6f", doubleValue);

   return pStringValue;
}

void MeeseeksProperties::SetProperties()
{
   camera.autoExposure = GetInt(CAMERA_AUTO_EXPOSURE, CAMERA_DEFAULT_AUTO_EXPOSURE);
   camera.exposureAbsolute = GetInt(CAMERA_EXPOSURE_ABSOLUTE, CAMERA_DEFAULT_EXPOSURE_ABSOLUTE);
   camera.autoWhiteBalance = GetInt(CAMERA_AUTO_WHITE_BALANCE, CAMERA_DEFAULT_AUTO_WHITE_BALANCE);
   camera.whiteBalanceTemperature = GetInt(CAMERA_WHITE_BALANCE_TEMP, CAMERA_DEFAULT_WHITE_BALANCE_TEMP);
   camera.brightness = GetInt(CAMERA_BRIGHTNESS, CAMERA_DEFAULT_BRIGHTNESS);
   camera.gain = GetInt(CAMERA_GAIN, CAMERA_DEFAULT_GAIN);
   camera.autoFocus = GetInt(CAMERA_AUTO_FOCUS, CAMERA_DEFAULT_AUTO_FOCUS);
   camera.zoom = GetInt(CAMERA_ZOOM, CAMERA_DEFAULT_ZOOM);
   camera.contrast = GetInt(CAMERA_CONTRAST, CAMERA_DEFAULT_CONTRAST);
   camera.saturation = GetInt(CAMERA_SATURATION, CAMERA_DEFAULT_SATURATION);
   camera.sharpness = GetInt(CAMERA_SHARPNESS, CAMERA_DEFAULT_SHARPNESS);
   camera.focus = GetInt(CAMERA_FOCUS, CAMERA_DEFAULT_FOCUS);

   algorithim.displayType = GetInt(ALGORITHIM_DISPLAY_TYPE, ALGORITHIM_DEFAULT_DISPLAY_TYPE);
   algorithim.minWidth = GetInt(ALGORITHIM_MIN_WIDTH, ALGORITHIM_DEFAULT_MIN_WIDTH);
   algorithim.minHeight = GetInt(ALGORITHIM_MIN_HEIGHT, ALGORITHIM_DEFAULT_MIN_HEIGHT);
   algorithim.maxWidth = GetInt(ALGORITHIM_MAX_WIDTH, ALGORITHIM_DEFAULT_MAX_WIDTH);
   algorithim.maxHeight = GetInt(ALGORITHIM_MAX_HEIGHT, ALGORITHIM_DEFAULT_MAX_HEIGHT);
   algorithim.minRed = GetInt(ALGORITHIM_MIN_RED, ALGORITHIM_DEFAULT_MIN_RED);
   algorithim.maxRed = GetInt(ALGORITHIM_MAX_RED, ALGORITHIM_DEFAULT_MAX_RED);
   algorithim.minGreen = GetInt(ALGORITHIM_MIN_GREEN, ALGORITHIM_DEFAULT_MIN_GREEN);
   algorithim.maxGreen = GetInt(ALGORITHIM_MAX_GREEN, ALGORITHIM_DEFAULT_MAX_GREEN);
   algorithim.minBlue = GetInt(ALGORITHIM_MIN_BLUE, ALGORITHIM_DEFAULT_MIN_BLUE);
   algorithim.maxBlue = GetInt(ALGORITHIM_MAX_BLUE, ALGORITHIM_DEFAULT_MAX_BLUE);
   algorithim.jpgQuality = GetInt(ALGORITHIM_JPEG_QUALITY, ALGORITHIM_DEFAULT_JPEG_QUALITY);
   algorithim.colorSpace = GetInt(ALGORITHIM_COLOR_SPACE, ALGORITHIM_DEFAULT_COLOR_SPACE);
   algorithim.blur = GetInt(ALGORITHIM_BLUR, ALGORITHIM_DEFAULT_BLUR);
   algorithim.erode = GetInt(ALGORITHIM_ERODE, ALGORITHIM_DEFAULT_ERODE);
   algorithim.dilate = GetInt(ALGORITHIM_DILATE, ALGORITHIM_DEFAULT_DILATE);

   algorithim.minTapeArea = GetInt(ALGORITHIM_MIN_TAPE_AREA, ALGORITHIM_DEFAULT_MIN_TAPE_AREA);
   algorithim.minTapeAngle = GetInt(ALGORITHIM_MIN_TAPE_ANGLE, ALGORITHIM_DEFAULT_MIN_TAPE_ANGLE);
   algorithim.maxTapeAngle = GetInt(ALGORITHIM_MAX_TAPE_ANGLE, ALGORITHIM_DEFAULT_MAX_TAPE_ANGLE);
   algorithim.minTapeHeightRatio = GetDouble(ALGORITHIM_MIN_TAPE_HEIGHT_RATIO, ALGORITHIM_DEFAULT_MIN_TAPE_HEIGHT_RATIO);
   algorithim.maxTapeHeightRatio = GetDouble(ALGORITHIM_MAX_TAPE_HEIGHT_RATIO, ALGORITHIM_DEFAULT_MAX_TAPE_HEIGHT_RATIO);
   algorithim.minTapeYDistRatio = GetDouble(ALGORITHIM_MIN_TAPE_Y_DIST_RATIO, ALGORITHIM_DEFAULT_MIN_TAPE_Y_DIST_RATIO);
   algorithim.maxTapeYDistRatio = GetDouble(ALGORITHIM_MAX_TAPE_Y_DIST_RATIO, ALGORITHIM_DEFAULT_MAX_TAPE_Y_DIST_RATIO);
   algorithim.minTapeXDistRatio = GetDouble(ALGORITHIM_MIN_TAPE_X_DIST_RATIO, ALGORITHIM_DEFAULT_MIN_TAPE_X_DIST_RATIO);
   algorithim.maxTapeXDistRatio = GetDouble(ALGORITHIM_MAX_TAPE_X_DIST_RATIO, ALGORITHIM_DEFAULT_MAX_TAPE_X_DIST_RATIO);

   serverPort = GetInt(SERVER_PORT, DEFAULT_SERVER_PORT);
   networkTableAddress = GetString(NETWORK_TABLE_ADDRESS, DEFAULT_NETWORK_TABLE_ADDRESS);
}

void MeeseeksProperties::LoadFromString(const std::string &str)
{
   nameValueMap.clear();
   lineNameMap.clear();
   lines.clear();

   std::istringstream buf(str);

   string line;
   int    lineIndex = 0;

   while (std::getline(buf, line)) {
      lines.push_back(line);
      if (line.length() > 2) {
         if (line[0] != '/' && line[1] != '/') {
            size_t sepIndex = line.find("=");
            if (sepIndex != string::npos) {
               string name = line.substr(0, sepIndex);
               transform(name.begin(), name.end(), name.begin(), ::tolower);
               string value = line.substr(sepIndex + 1);
               nameValueMap[name] = value;
               lineNameMap[lineIndex] = name;
            }
         }
      }
      lineIndex++;
   }

   SetProperties();

   serverPort = GetInt(SERVER_PORT, DEFAULT_SERVER_PORT);
}

void MeeseeksProperties::LoadFromFile(const char *pFile)
{
   std::ifstream t(pFile);
   std::stringstream buffer;
   buffer << t.rdbuf();
   LoadFromString(buffer.str());
}

void MeeseeksProperties::SaveToString(std::string &str)
{
   char stringValue[100];

   StringSet names;

   nameValueMap[CAMERA_AUTO_EXPOSURE] = IntToString(camera.autoExposure, stringValue);
   nameValueMap[CAMERA_EXPOSURE_ABSOLUTE] = IntToString(camera.exposureAbsolute, stringValue);
   nameValueMap[CAMERA_AUTO_WHITE_BALANCE] = IntToString(camera.autoWhiteBalance, stringValue);
   nameValueMap[CAMERA_WHITE_BALANCE_TEMP] = IntToString(camera.whiteBalanceTemperature, stringValue);
   nameValueMap[CAMERA_BRIGHTNESS] = IntToString(camera.brightness, stringValue);
   nameValueMap[CAMERA_GAIN] = IntToString(camera.gain, stringValue);
   nameValueMap[CAMERA_AUTO_FOCUS] = IntToString(camera.autoFocus, stringValue);
   nameValueMap[CAMERA_ZOOM] = IntToString(camera.zoom, stringValue);
   nameValueMap[CAMERA_CONTRAST] = IntToString(camera.contrast, stringValue);
   nameValueMap[CAMERA_SHARPNESS] = IntToString(camera.sharpness, stringValue);
   nameValueMap[CAMERA_SATURATION] = IntToString(camera.saturation, stringValue);
   nameValueMap[CAMERA_FOCUS] = IntToString(camera.focus, stringValue);

   nameValueMap[ALGORITHIM_DISPLAY_TYPE] = IntToString(algorithim.displayType, stringValue);
   nameValueMap[ALGORITHIM_MIN_WIDTH] = IntToString(algorithim.minWidth, stringValue);
   nameValueMap[ALGORITHIM_MIN_HEIGHT] = IntToString(algorithim.minHeight, stringValue);
   nameValueMap[ALGORITHIM_MAX_WIDTH] = IntToString(algorithim.maxWidth, stringValue);
   nameValueMap[ALGORITHIM_MAX_HEIGHT] = IntToString(algorithim.maxHeight, stringValue);
   nameValueMap[ALGORITHIM_MIN_RED] = IntToString(algorithim.minRed, stringValue);
   nameValueMap[ALGORITHIM_MAX_RED] = IntToString(algorithim.maxRed, stringValue);
   nameValueMap[ALGORITHIM_MIN_GREEN] = IntToString(algorithim.minGreen, stringValue);
   nameValueMap[ALGORITHIM_MAX_GREEN] = IntToString(algorithim.maxGreen, stringValue);
   nameValueMap[ALGORITHIM_MIN_BLUE] = IntToString(algorithim.minBlue, stringValue);
   nameValueMap[ALGORITHIM_MAX_BLUE] = IntToString(algorithim.maxBlue, stringValue);
   nameValueMap[ALGORITHIM_JPEG_QUALITY] = IntToString(algorithim.jpgQuality, stringValue);
   nameValueMap[ALGORITHIM_COLOR_SPACE] = IntToString(algorithim.colorSpace, stringValue);
   nameValueMap[ALGORITHIM_BLUR] = IntToString(algorithim.blur, stringValue);
   nameValueMap[ALGORITHIM_ERODE] = IntToString(algorithim.erode, stringValue);
   nameValueMap[ALGORITHIM_DILATE] = IntToString(algorithim.dilate, stringValue);

   nameValueMap[ALGORITHIM_MIN_TAPE_AREA] = IntToString(algorithim.minTapeArea, stringValue);
   nameValueMap[ALGORITHIM_MIN_TAPE_ANGLE] = IntToString(algorithim.minTapeAngle, stringValue);
   nameValueMap[ALGORITHIM_MAX_TAPE_ANGLE] = IntToString(algorithim.maxTapeAngle, stringValue);
   nameValueMap[ALGORITHIM_MIN_TAPE_HEIGHT_RATIO] = DoubleToString(algorithim.minTapeHeightRatio, stringValue);
   nameValueMap[ALGORITHIM_MAX_TAPE_HEIGHT_RATIO] = DoubleToString(algorithim.maxTapeHeightRatio, stringValue);
   nameValueMap[ALGORITHIM_MIN_TAPE_Y_DIST_RATIO] = DoubleToString(algorithim.minTapeYDistRatio, stringValue);
   nameValueMap[ALGORITHIM_MAX_TAPE_Y_DIST_RATIO] = DoubleToString(algorithim.maxTapeYDistRatio, stringValue);
   nameValueMap[ALGORITHIM_MIN_TAPE_X_DIST_RATIO] = DoubleToString(algorithim.minTapeXDistRatio, stringValue);
   nameValueMap[ALGORITHIM_MAX_TAPE_X_DIST_RATIO] = DoubleToString(algorithim.maxTapeXDistRatio, stringValue);

   nameValueMap[SERVER_PORT] = IntToString(serverPort, stringValue);
   nameValueMap[NETWORK_TABLE_ADDRESS] = networkTableAddress;

   for (auto iter = nameValueMap.begin(); iter != nameValueMap.end(); iter++)
      names.insert(iter->first);

   std::ostringstream buf;

   string line;

   for (size_t index = 0; index<lines.size(); index++) {
      auto lnmIter = lineNameMap.find(index);
      if (lnmIter != lineNameMap.end()) {
         StringToStringMap::iterator iter = nameValueMap.find(lnmIter->second);
         line = iter->first + "=" + iter->second + "\n";
         names.erase(lnmIter->second);
      }
      else
         line = lines[index] + "\n";

      buf << line;
   }

   for (StringSet::iterator nameIter = names.begin(); nameIter != names.end(); nameIter++) {
      StringToStringMap::iterator nvIter = nameValueMap.find(*nameIter);
      line = nvIter->first + "=" + nvIter->second + "\n";
      buf << line;
   }

   str = buf.str();
}

void MeeseeksProperties::SaveToFile(const char *pFile)
{
   std::string str;

   SaveToString(str);

   printf("MeeseeksProperties::SaveToFile %s\n", pFile);

   ofstream stream(pFile);

   stream << str;
}


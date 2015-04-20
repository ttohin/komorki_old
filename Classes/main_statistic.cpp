#include <stdlib.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdarg.h>  // For va_start, etc.
#include <memory>    // For std::unique_ptr
#include "PixelDescriptorProvider.h"

#define MAX_EXAMINATION 3000

std::string string_format(const std::string fmt_str, ...) {
  int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
  std::string str;
  std::unique_ptr<char[]> formatted;
  va_list ap;
  while(1) {
    formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
    strcpy(&formatted[0], fmt_str.c_str());
    va_start(ap, fmt_str);
    final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
    va_end(ap);
    if (final_n < 0 || final_n >= n)
      n += abs(final_n - n + 1);
    else
      break;
  }
  return std::string(formatted.get());
}

void WriteResults(const std::vector<std::string>& keysArray, const std::map<std::string, std::vector<int>>& results, int numberOfResults)
{
  std::ofstream totalResultFile;
  totalResultFile.open("total.txt");
  
  int maxValue = 0;
  for (int i = 0; i < numberOfResults; ++i)
  {
    for (auto r: keysArray)
    {
      if (r == "time") {
        continue;
      }
      
      if (results.at(r)[i] > maxValue) {
        maxValue = results.at(r)[i];
      }
    }
  }
  
  totalResultFile << "number ";
  for (auto r: keysArray)
  {
    if (r == "time") {
      continue;
    }
    
    totalResultFile << r << " ";
  }
  totalResultFile << std::endl;
  
  
  for (int valueIndex = 0; valueIndex <= maxValue; ++valueIndex)
  {
    totalResultFile << valueIndex << " ";
    
    for (auto r: keysArray)
    {
      if (r == "time") {
        continue;
      }
      
      int maxTimeForValue = 0;
      for (int i = 0; i < numberOfResults; ++i)
      {
        if (results.at(r)[i] == valueIndex && results.at("time")[i] > maxTimeForValue)
        {
          maxTimeForValue = results.at("time")[i];
        }
      } // for (int i = 0; i < MAX_EXAMINATION; ++i)
      
      totalResultFile << maxTimeForValue << " ";
      
    } // (auto r: keysArray)
    
    totalResultFile << std::endl;
  } // for (int valueIndex = 0; valueIndex < maxValue; ++valueIndex)
  
  
  totalResultFile.close();

}

void WriteTimeLine(const std::vector<int>& wArray,
                   const std::vector<int>& hArray,
                   const std::vector<int>& iArray,
                   const std::vector<int>& pArray,
                   int examinationCounter)
{
  std::ofstream timeLineFile;
  timeLineFile.open (string_format("timeline_%d.txt", examinationCounter));
  timeLineFile << "Time" << " " << "G" << " " << "H" << " " << "P" << " " << "I"  << " " << "Total" << std::endl;
  
  int step = wArray.size() * 0.01;
  if(step == 0)
    step = 1;
  
  for (int i = 0; i < wArray.size(); i += step) {
    timeLineFile << i << " " << wArray[i] << " " << hArray[i] << " " << pArray[i] << " " << iArray[i] << " " << wArray[i]+hArray[i]+pArray[i]+iArray[i] << std::endl;
  }
  
  timeLineFile << wArray.size() - 1 << " "
  << wArray[wArray.size() - 1] << " "
  << hArray[wArray.size() - 1] << " "
  << pArray[wArray.size() - 1] << " "
  << iArray[wArray.size() - 1] << " "
  << " " << wArray[wArray.size() - 1]+hArray[wArray.size() - 1]+pArray[wArray.size() - 1]+iArray[wArray.size() - 1] << " "
  << std::endl;
  
  timeLineFile.close();
}

void GetResultsFromConfig(std::map<std::string, std::vector<int>>& results, const PixelDescriptorProvider::Config& config)
{
  results["greenHealth"].emplace_back(config.greenHealth);
  results["hunterHealth"].emplace_back(config.hunterHealth);
  results["hunterAttack"].emplace_back(config.hunterAttack);
  results["basehealth"].emplace_back(config.basehealth);
  results["hunterHealthIncome"].emplace_back(config.hunterHealthIncome);
  results["greenHealthIncome"].emplace_back(config.greenHealthIncome);
  results["greenAttack"].emplace_back(config.greenAttack);
  results["saladArmor"].emplace_back(config.saladArmor);
  results["improvedSaladArmor"].emplace_back(config.improvedSaladArmor);
  results["maxLifeTime"].emplace_back(config.maxLifeTime);
}

int main(int argc, char *argv[])
{
  timeval tval;
  gettimeofday(&tval, NULL);
  srand(tval.tv_sec);
  
  std::vector<std::string> keysArray;
  keysArray.push_back("greenHealth");
  keysArray.push_back("hunterAttack");
  keysArray.push_back("hunterHealth");
  keysArray.push_back("basehealth");
  keysArray.push_back("hunterHealthIncome");
  keysArray.push_back("greenHealthIncome");
  keysArray.push_back("greenAttack");
  keysArray.push_back("saladArmor");
  keysArray.push_back("improvedSaladArmor");
  keysArray.push_back("maxLifeTime");
  keysArray.push_back("time");
 
  std::vector<int> wArray;
  wArray.reserve(10000);
  std::vector<int> hArray;
  hArray.reserve(10000);
  std::vector<int> iArray;
  iArray.reserve(10000);
  std::vector<int> pArray;
  pArray.reserve(10000);
  
  std::map<std::string, std::vector<int>> results;
  
  int examinationCounter = 0;
  while (examinationCounter != MAX_EXAMINATION)
  {
    PixelDescriptorProvider::Config config;
    PixelDescriptorProvider provider;
    provider.InitWithMutations(&config);
 
    wArray.clear();
    hArray.clear();
    iArray.clear();
    pArray.clear();
    
    while (true)
    {
      provider.Update();
      
      int greenCount = provider.m_population[eCellTypeGreen];
      int hunterCount = provider.m_population[eCellTypeSalad];
      int saladCount = provider.m_population[eCellTypeSalad];
      int improvedSaladCount = provider.m_population[eCellTypeImprovedSalad];
      
      wArray.push_back(greenCount);
      hArray.push_back(hunterCount);
      iArray.push_back(improvedSaladCount);
      pArray.push_back(saladCount);
      
      int numberOfBreeds =
      (greenCount ? 1 : 0) +
      (hunterCount != 0 ? 1 : 0) +
      (saladCount != 0 ? 1 : 0) +
      (improvedSaladCount != 0 ? 1 : 0);
      
      if (numberOfBreeds != 4) {
        break;
      }
      
      if (wArray.size() % 5000 == 0) {
        
        WriteTimeLine(wArray, hArray, iArray, pArray, examinationCounter);
        
        std::map<std::string, std::vector<int>> tmpRes;
        GetResultsFromConfig(tmpRes, config);
        tmpRes["time"].emplace_back(wArray.size());
        
        std::cout << examinationCounter << " ";
        for (auto r: keysArray)
        {
          std::cout << r << " " << tmpRes[r][0] << " ";
        }
        std::cout << std::endl;
        
      }
    }
    
    WriteTimeLine(wArray, hArray, iArray, pArray, examinationCounter);
    
    GetResultsFromConfig(results, config);
    results["time"].emplace_back(wArray.size());
   
    examinationCounter++;
    
    WriteResults(keysArray, results, examinationCounter);
    
  }
  
//
//  for (int i = 0; i < MAX_EXAMINATION; ++i)
//  {
//    totalResultFile << i << " ";
//    for (auto r: keysArray)
//    {
//      totalResultFile << results[r][i] << " ";
//    }
//    
//    totalResultFile << std::endl;
//  }

  return 0;
}


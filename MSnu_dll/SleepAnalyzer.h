#pragma once
#include <string>
#include <vector>
#include "API.h"

struct SleepData {
    std::string date;
    int sleepDuration; // in minutes
    int wakeUpTimes;
};

class EXPORT_API SleepAnalyzer {
public:
    SleepAnalyzer();
    ~SleepAnalyzer();

    void addSleepData(const SleepData& data);
    std::string generateReport() const;
    const std::vector<SleepData>& getSleepData() const;  // Dodajemy metodê getter
    void clearSleepData();  // Dodajemy metodê czyszczenia

private:
    std::vector<SleepData> sleepData;
    int totalSleepDuration() const;
    int totalWakeUpTimes() const;
};

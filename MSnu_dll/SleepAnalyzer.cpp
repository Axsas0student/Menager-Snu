#include "SleepAnalyzer.h"
#include <numeric>
#include <sstream>
#include <iomanip>

SleepAnalyzer::SleepAnalyzer() {}

SleepAnalyzer::~SleepAnalyzer() {}

void SleepAnalyzer::addSleepData(const SleepData& data) {
    sleepData.push_back(data);
}

std::string SleepAnalyzer::generateReport() const {
    int totalDuration = totalSleepDuration();
    int totalWakeUps = totalWakeUpTimes();
    int numberOfNights = sleepData.size();
    double averageSleep = numberOfNights > 0 ? static_cast<double>(totalDuration) / numberOfNights : 0;
    double averageWakeUps = numberOfNights > 0 ? static_cast<double>(totalWakeUps) / numberOfNights : 0;

    std::ostringstream report;
    report << "Raport Analizy Snu\n";
    report << "=====================\n";
    report << "£¹czna liczba nocy: " << numberOfNights << "\n";
    report << "£¹czny czas snu: " << totalDuration << " minut\n";
    report << "£¹czna liczba przebudzeñ: " << totalWakeUps << "\n";
    report << std::fixed << std::setprecision(2);
    report << "Œredni czas snu na noc: " << averageSleep << " minut\n";
    report << "Œrednia liczba przebudzeñ na noc: " << averageWakeUps << "\n";

    // Dodaj informacje o iloœci snu
    if (averageSleep < 420 && averageSleep > 0) {
        report << "Masz niedobory snu.\n";
    } else if (averageSleep >= 420 && averageSleep <= 540) {
        report << "Masz zdrow¹ iloœæ snu.\n";
    } else if(averageSleep >540){
        report << "Masz nadmiar snu.\n";
    }
    else {
        report << "\n";
    }

    // Dodaj informacje o zdrowym œnie
    if (averageSleep >= 420 && averageSleep <= 540) {
        report << "Twoja iloœæ snu jest w zakresie zdrowego snu (7-9 godzin).\n";
    } else {
        if (averageSleep == 0) {
            report << "\n";
        }else {
            report << "Twój sen wymaga uwagi. Zalecana iloœæ snu to 7-9 godzin.\n";
        }
    }

    return report.str();
}



const std::vector<SleepData>& SleepAnalyzer::getSleepData() const {
    return sleepData;
}

void SleepAnalyzer::clearSleepData() {
    sleepData.clear();
}

int SleepAnalyzer::totalSleepDuration() const {
    return std::accumulate(sleepData.begin(), sleepData.end(), 0,
        [](int sum, const SleepData& data) { return sum + data.sleepDuration; });
}

int SleepAnalyzer::totalWakeUpTimes() const {
    return std::accumulate(sleepData.begin(), sleepData.end(), 0,
        [](int sum, const SleepData& data) { return sum + data.wakeUpTimes; });
}

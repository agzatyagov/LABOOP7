#include "observer.h"
#include <iostream>
#include <fstream>

std::mutex& GetCoutMutex()
{
    static std::mutex m;
    return m;
}

void ConsoleObserver::onKill(const std::string& message)
{
    std::lock_guard<std::mutex> lg(GetCoutMutex());
    std::cout << message << std::endl;
}

FileObserver::FileObserver(const std::string& fileName_)
{
    fileName = fileName_;
}

void FileObserver::onKill(const std::string& message)
{
    std::ofstream out(fileName.c_str(), std::ios::app);
    if (!out.is_open()) return;
    out << message << "\n";
}

void BattleSubject::attach(IObserver* obs)
{
    observers.push_back(obs);
}

void BattleSubject::notifyKill(const std::string& message)
{
    size_t i = 0;
    while (i < observers.size())
    {
        observers[i]->onKill(message);
        i = i + 1;
    }
}

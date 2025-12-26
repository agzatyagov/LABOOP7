#pragma once
#include <string>
#include <vector>
#include <mutex>

std::mutex& GetCoutMutex(); 

class IObserver
{
public:
    virtual ~IObserver() {}
    virtual void onKill(const std::string& message) = 0;
};

class ConsoleObserver : public IObserver
{
public:
    void onKill(const std::string& message) override;
};

class FileObserver : public IObserver
{
private:
    std::string fileName;

public:
    FileObserver(const std::string& fileName_);
    void onKill(const std::string& message) override;
};

class BattleSubject
{
private:
    std::vector<IObserver*> observers;

public:
    void attach(IObserver* obs);
    void notifyKill(const std::string& message);
};

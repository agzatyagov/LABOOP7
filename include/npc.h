#pragma once
#include <string>

enum class NPCType
{
    Unknown = 0,
    SlaveTrader = 1,
    Squirrel = 2,
    WanderingKnight = 3
};

std::string TypeToString(NPCType type);
NPCType StringToType(const std::string& s);

class NPCVisitor;

class NPC
{
protected:
    std::string name;
    int x;
    int y;
    bool alive;

public:
    NPC(const std::string& name_, int x_, int y_);
    virtual ~NPC() {}

    std::string getName() const;
    int getX() const;
    int getY() const;

    bool isAlive() const;
    void kill();

    void setPos(int nx, int ny);

    int getMoveDistance() const;
    int getKillDistance() const;

    virtual NPCType getType() const = 0;
    virtual void accept(NPCVisitor& v) = 0;
};

class SlaveTrader : public NPC
{
public:
    SlaveTrader(const std::string& name_, int x_, int y_);
    NPCType getType() const override;
    void accept(NPCVisitor& v) override;
};

class Squirrel : public NPC
{
public:
    Squirrel(const std::string& name_, int x_, int y_);
    NPCType getType() const override;
    void accept(NPCVisitor& v) override;
};

class WanderingKnight : public NPC
{
public:
    WanderingKnight(const std::string& name_, int x_, int y_);
    NPCType getType() const override;
    void accept(NPCVisitor& v) override;
};

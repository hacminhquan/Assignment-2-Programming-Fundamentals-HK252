#include "eniesLobby.h"

/*
 * BattleContext
 */
BattleContext::BattleContext() {
    turnCount = 0;
    morale = 0;
    alarmLevel = 0;
    rescueProgress = 0;
    escapeProgress = 0;
    busterCallTimer = 0;
    mainGateDestroyed = false;
    robinRescued = false;
    bridgeOpened = false;
    battleEnded = false;
    resultCode = "";
}

void BattleContext::nextTurn() {
        #define EL_DEF_CHAR_ACCESS struct ELCharAccess : public Character { \
            int attack(Character*, BattleContext&) { return 0; } \
            int specialSkill(Character*, BattleContext&) { return 0; } \
            string str() const { return ""; } \
            int& HP() { return hp; } \
            int& MHP() { return maxHp; } \
            int& ATK() { return atk; } \
            int& DEF() { return def; } \
            int& SPD() { return speed; } \
            int& EN() { return energy; } \
            bool& ALV() { return alive; } \
        };
        #define EL_DEF_BUILD_ACCESS struct ELBuildAccess : public Building { \
            ELBuildAccess() : Building("", 0) {} \
            void applyEffect(BattleContext&) {} \
            string& NAME() { return name; } \
            int& HP() { return hp; } \
            int& MHP() { return maxHP; } \
            bool& DEST() { return destroyed; } \
        };
        #define EL_C(P) ((ELCharAccess*)(P))
        #define EL_B(P) ((ELBuildAccess*)(P))
        #define EL_CEIL(A,N,D) ((int)((1LL * (A) * (N) <= 0) ? 0 : ((1LL * (A) * (N) + (D) - 1) / (D))))
        #define EL_CLAMP_CONTEXT(C) do { \
            if ((C).morale < 0) (C).morale = 0; \
            if ((C).morale > 100) (C).morale = 100; \
            if ((C).alarmLevel < 0) (C).alarmLevel = 0; \
            if ((C).alarmLevel > 100) (C).alarmLevel = 100; \
            if ((C).rescueProgress < 0) (C).rescueProgress = 0; \
            if ((C).rescueProgress > 100) (C).rescueProgress = 100; \
            if ((C).escapeProgress < 0) (C).escapeProgress = 0; \
            if ((C).escapeProgress > 100) (C).escapeProgress = 100; \
            if ((C).busterCallTimer < 0) (C).busterCallTimer = 0; \
        } while (0)
        #define EL_CLAMP_SELF() do { \
            if (maxHp < 0) maxHp = 0; \
            if (hp < 0) hp = 0; \
            if (hp > maxHp) hp = maxHp; \
            if (energy < 0) energy = 0; \
            if (energy > 100) energy = 100; \
            if (def < 0) def = 0; \
            if (speed < 0) speed = 0; \
            alive = (hp > 0); \
        } while (0)
        #define EL_CLAMP_CHAR(P) do { \
            if (EL_C(P)->MHP() < 0) EL_C(P)->MHP() = 0; \
            if (EL_C(P)->HP() < 0) EL_C(P)->HP() = 0; \
            if (EL_C(P)->HP() > EL_C(P)->MHP()) EL_C(P)->HP() = EL_C(P)->MHP(); \
            if (EL_C(P)->EN() < 0) EL_C(P)->EN() = 0; \
            if (EL_C(P)->EN() > 100) EL_C(P)->EN() = 100; \
            if (EL_C(P)->DEF() < 0) EL_C(P)->DEF() = 0; \
            if (EL_C(P)->SPD() < 0) EL_C(P)->SPD() = 0; \
            EL_C(P)->ALV() = (EL_C(P)->HP() > 0); \
        } while (0)
        #define EL_CLAMP_BUILD(P) do { \
            if (EL_B(P)->MHP() < 0) EL_B(P)->MHP() = 0; \
            if (EL_B(P)->HP() < 0) EL_B(P)->HP() = 0; \
            if (EL_B(P)->HP() > EL_B(P)->MHP()) EL_B(P)->HP() = EL_B(P)->MHP(); \
            EL_B(P)->DEST() = (EL_B(P)->HP() <= 0); \
        } while (0)
            turnCount += 1;
}

/*
 * Character
 */
Character::Character() {
    name = "";
    hp = 0;
    maxHp = 0;
    atk = 0;
    def = 0;
    speed = 0;
    energy = 0;
    alive = false;
}

Character::Character(string name, int hp, int atk, int def, int speed, int energy) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

Character::~Character() {
}

int Character::attack(Building* target, BattleContext& context) {
    return 0;
}

int Character::specialSkill(Building* target, BattleContext& context) {
    return 0;
}

void Character::endTurn(BattleContext& context) {
    return;
}

void Character::receiveDamage(int damage) {
    if (damage < 0) damage = 0;
    int actualDamage = damage - def;
    if (actualDamage < 0) actualDamage = 0;
    hp -= actualDamage;
    if (hp <= 0) {
        hp = 0;
        alive = false;
    }
    if (hp > maxHp) hp = maxHp;
}

bool Character::isAlive() const {
    return alive;
}

string Character::getName() const {
    return name;
}

int Character::getHP() const {
    return hp;
}

int Character::getEnergy() const {
    return energy;
}

bool Character::isStrawHat() const {
    return false;
}

bool Character::isCP9() const {
    return false;
}

/*
 * StrawHat
 */
StrawHat::StrawHat() : Character() {
    bounty = 0;
}

StrawHat::StrawHat(string name, int hp, int atk, int def, int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->bounty = bounty;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

bool StrawHat::isStrawHat() const {
    return true;
}

string StrawHat::str() const {
    ostringstream ss;
    ss << "StrawHat[name=" << name << ", hp=" << hp << ", atk=" << atk << ", def=" << def << ", speed=" << speed << ", energy=" << energy << ", bounty=" << bounty << "]";
    return ss.str();
}

/*
 * Luffy
 */
Luffy::Luffy(string name, int hp, int atk, int def, int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->bounty = bounty;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Luffy::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk;
    if (maxHp > 0) {
        if (hp * 10 <= maxHp * 3) damage += EL_CEIL(atk, 30, 100);
        else if (hp * 2 <= maxHp) damage += EL_CEIL(atk, 15, 100);
    }
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale += 5;
        EL_CLAMP_CONTEXT(context);
        energy += 5;
        EL_CLAMP_SELF();
    }
    return actual;
}

int Luffy::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 20 || (maxHp > 0 && hp * 100 < maxHp * 15)) return 0;
    int oldAtk = atk;
    energy -= 20;
    EL_CLAMP_SELF();
    speed += 15;
    atk += 15;
    EL_CLAMP_SELF();
    hp -= EL_CEIL(maxHp, 8, 100);
    EL_CLAMP_SELF();
    context.alarmLevel += 10;
    EL_CLAMP_CONTEXT(context);
    if (!alive) return 0;
    int damage = EL_CEIL(oldAtk, 200, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        energy += 5;
        EL_CLAMP_SELF();
    }
    return actual;
}

int Luffy::attack(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    int damage = atk;
    if (maxHp > 0) {
        if (hp * 10 <= maxHp * 3) damage += EL_CEIL(atk, 30, 100);
        else if (hp * 2 <= maxHp) damage += EL_CEIL(atk, 15, 100);
    }
    int before = EL_B(target)->HP();
    target->receiveDamage(damage);
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

int Luffy::specialSkill(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    if (energy < 20 || (maxHp > 0 && hp * 100 < maxHp * 15)) return 0;
    int oldAtk = atk;
    energy -= 20;
    EL_CLAMP_SELF();
    speed += 15;
    atk += 15;
    EL_CLAMP_SELF();
    hp -= EL_CEIL(maxHp, 8, 100);
    EL_CLAMP_SELF();
    context.alarmLevel += 10;
    EL_CLAMP_CONTEXT(context);
    if (!alive) return 0;
    int before = EL_B(target)->HP();
    target->receiveDamage(EL_CEIL(oldAtk, 200, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

void Luffy::endTurn(BattleContext& context) {
    if (alive && maxHp > 0 && hp * 10 <= maxHp * 3) {
        context.morale += 3;
        EL_CLAMP_CONTEXT(context);
    }
}

/*
 * Zoro
 */
Zoro::Zoro(string name, int hp, int atk, int def, int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->bounty = bounty;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Zoro::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk + EL_CEIL(def, 20, 100);
    int targetMax = EL_C(target)->MHP();
    if (targetMax > 0 && target->getHP() * 10 < targetMax * 4) {
        damage += EL_CEIL(damage, 15, 100);
    }
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale += 6;
        EL_CLAMP_CONTEXT(context);
        atk += EL_CEIL(atk, 5, 100);
        EL_CLAMP_SELF();
    }
    return actual;
}

int Zoro::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 15) return 0;
    energy -= 15;
    EL_CLAMP_SELF();
    int damage = EL_CEIL(atk, 220, 100);
    int targetMax = EL_C(target)->MHP();
    if (targetMax > 0 && target->getHP() * 2 < targetMax) {
        damage += EL_CEIL(damage, 50, 100);
    }
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        energy += 8;
        context.morale += 4;
        EL_CLAMP_SELF();
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Zoro::attack(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    int before = EL_B(target)->HP();
    target->receiveDamage(atk + EL_CEIL(def, 20, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

int Zoro::specialSkill(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    if (energy < 15) return 0;
    energy -= 15;
    EL_CLAMP_SELF();
    int before = EL_B(target)->HP();
    target->receiveDamage(EL_CEIL(atk, 220, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

void Zoro::endTurn(BattleContext& context) {
    return;
}

/*
 * Sanji
 */
Sanji::Sanji(string name, int hp, int atk, int def, int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->bounty = bounty;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Sanji::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk + EL_CEIL(speed, 50, 100);
    if (EL_C(target)->DEF() < def) damage += EL_CEIL(damage, 10, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale += 8;
        atk += EL_CEIL(atk, 10, 100);
        EL_CLAMP_SELF();
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Sanji::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 18) return 0;
    energy -= 18;
    EL_CLAMP_SELF();
    int damage = EL_CEIL(atk, 210, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    int dec = (target->getName() == "Jabra") ? 12 : 8;
    EL_C(target)->DEF() -= dec;
    EL_CLAMP_CHAR(target);
    if (wasAlive && !target->isAlive()) {
        context.morale += 8;
        atk += EL_CEIL(atk, 10, 100);
        EL_CLAMP_SELF();
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Sanji::attack(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    int before = EL_B(target)->HP();
    target->receiveDamage(atk + EL_CEIL(speed, 50, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

int Sanji::specialSkill(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    if (energy < 18) return 0;
    energy -= 18;
    EL_CLAMP_SELF();
    int before = EL_B(target)->HP();
    target->receiveDamage(EL_CEIL(atk, 210, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

void Sanji::endTurn(BattleContext& context) {
    return;
}

/*
 * Nami
 */
Nami::Nami(string name, int hp, int atk, int def, int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->bounty = bounty;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Nami::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int remainDef = EL_CEIL(EL_C(target)->DEF(), 70, 100);
    int ignoredDef = EL_C(target)->DEF() - remainDef;
    if (ignoredDef < 0) ignoredDef = 0;
    int damage = atk + ignoredDef;
    int actual = atk - remainDef;
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale += 5;
        energy += 6;
        EL_CLAMP_SELF();
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Nami::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 20) return 0;
    energy -= 20;
    EL_CLAMP_SELF();
    int damage = atk + 40;
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    EL_C(target)->SPD() -= 10;
    EL_CLAMP_CHAR(target);
    context.busterCallTimer += 1;
    EL_CLAMP_CONTEXT(context);
    context.alarmLevel -= 5;
    EL_CLAMP_CONTEXT(context);
    if (wasAlive && !target->isAlive()) {
        context.morale += 5;
        energy += 6;
        EL_CLAMP_SELF();
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Nami::attack(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    int before = EL_B(target)->HP();
    target->receiveDamage(EL_CEIL(atk, 50, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

int Nami::specialSkill(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    if (energy < 20) return 0;
    energy -= 20;
    EL_CLAMP_SELF();
    int before = EL_B(target)->HP();
    target->receiveDamage(EL_CEIL(atk + 40, 150, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    context.busterCallTimer += 1;
    EL_CLAMP_CONTEXT(context);
    context.alarmLevel -= 5;
    EL_CLAMP_CONTEXT(context);
    return actual;
}

void Nami::endTurn(BattleContext& context) {
    return;
}

/*
 * Chopper
 */
Chopper::Chopper(string name, int hp, int atk, int def, int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->bounty = bounty;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Chopper::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int actual = atk - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(atk);
    if (wasAlive && !target->isAlive()) {
        context.morale += 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Chopper::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || energy < 15 || !target->isAlive() || !target->isStrawHat()) return 0;
    energy -= 15;
    EL_CLAMP_SELF();
    EL_C(target)->HP() += 35 + EL_CEIL(atk, 50, 100);
    EL_CLAMP_CHAR(target);
    if (target->getName() == "Luffy") {
        context.morale += 5;
        EL_CLAMP_CONTEXT(context);
    }
    return 0;
}

int Chopper::attack(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    int before = EL_B(target)->HP();
    target->receiveDamage(atk);
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

void Chopper::endTurn(BattleContext& context) {
    return;
}

/*
 * Usopp
 */
Usopp::Usopp(string name, int hp, int atk, int def, int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->bounty = bounty;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Usopp::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk;
    if (EL_C(target)->SPD() < 50) damage += EL_CEIL(atk, 20, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale += 5;
        EL_CLAMP_CONTEXT(context);
    }
    context.morale += 10;
    EL_CLAMP_CONTEXT(context);
    return actual;
}

int Usopp::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 16) return 0;
    energy -= 16;
    EL_CLAMP_SELF();
    int damage = EL_CEIL(atk, 80, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    EL_C(target)->SPD() -= 12;
    EL_CLAMP_CHAR(target);
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    context.escapeProgress += 8;
    EL_CLAMP_CONTEXT(context);
    if (wasAlive && !target->isAlive()) {
        context.morale += 5;
        EL_CLAMP_CONTEXT(context);
    }
    context.morale += 10;
    EL_CLAMP_CONTEXT(context);
    return actual;
}

int Usopp::attack(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    int before = EL_B(target)->HP();
    target->receiveDamage(EL_CEIL(atk, 50, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    context.morale += 10;
    EL_CLAMP_CONTEXT(context);
    return actual;
}

int Usopp::specialSkill(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    if (energy < 16) return 0;
    energy -= 16;
    EL_CLAMP_SELF();
    int before = EL_B(target)->HP();
    target->receiveDamage(EL_CEIL(atk, 80, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    context.escapeProgress += 8;
    EL_CLAMP_CONTEXT(context);
    context.morale += 10;
    EL_CLAMP_CONTEXT(context);
    return actual;
}

void Usopp::endTurn(BattleContext& context) {
    return;
}

/*
 * Franky
 */
Franky::Franky(string name, int hp, int atk, int def, int speed, int energy, long long bounty) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->bounty = bounty;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Franky::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk + EL_CEIL(def, 30, 100);
    if (target->isCP9()) damage += EL_CEIL(damage, 10, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale += 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Franky::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage;
    bool strongRight = false;
    if (energy >= 30) {
        energy -= 30;
        damage = EL_CEIL(atk, 120, 100);
    } else if (energy >= 20) {
        energy -= 20;
        damage = EL_CEIL(atk, 180, 100);
        if (target->getName() == "Lucci") damage += EL_CEIL(damage, 20, 100);
        strongRight = true;
    } else return 0;
    EL_CLAMP_SELF();
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (strongRight) {
        EL_C(target)->SPD() -= 8;
        EL_CLAMP_CHAR(target);
    }
    if (wasAlive && !target->isAlive()) {
        context.morale += 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Franky::attack(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    int before = EL_B(target)->HP();
    target->receiveDamage(atk + EL_CEIL(def, 30, 100));
    int actual = before - EL_B(target)->HP();
    if (actual < 0) actual = 0;
    return actual;
}

int Franky::specialSkill(Building* target, BattleContext& context) {
    EL_DEF_BUILD_ACCESS
    if (!target || !alive || target->isDestroyed()) return 0;
    int before = EL_B(target)->HP();
    if (energy >= 30) {
        energy -= 30;
        EL_CLAMP_SELF();
        EL_B(target)->HP() = 0;
        EL_B(target)->DEST() = true;
        return before;
    }
    if (energy >= 20) {
        energy -= 20;
        EL_CLAMP_SELF();
        target->receiveDamage(EL_CEIL(atk, 180, 100));
        int actual = before - EL_B(target)->HP();
        if (actual < 0) actual = 0;
        return actual;
    }
    return 0;
}

void Franky::endTurn(BattleContext& context) {
    if (!alive || maxHp <= 0) return;
    if (hp * 10 > maxHp * 7) def += 5;
    if (hp * 10 < maxHp * 3) atk += EL_CEIL(atk, 10, 100);
    EL_CLAMP_SELF();
}

/*
 * CP9Agent
 */
CP9Agent::CP9Agent() : Character() {
    doriki = 0;
}

CP9Agent::CP9Agent(string name, int hp, int atk, int def, int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->doriki = doriki;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

bool CP9Agent::isCP9() const {
    return true;
}

string CP9Agent::str() const {
    ostringstream ss;
    ss << "CP9[name=" << name << ", hp=" << hp << ", atk=" << atk
       << ", def=" << def << ", speed=" << speed << ", energy=" << energy
       << ", doriki=" << doriki << "]";
    return ss.str();
}

/*
 * Lucci
 */
Lucci::Lucci(string name, int hp, int atk, int def, int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->doriki = doriki;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Lucci::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk + EL_CEIL(doriki, 1, 20);
    int targetMax = EL_C(target)->MHP();
    if (targetMax > 0 && target->getHP() * 2 < targetMax) damage += EL_CEIL(damage, 20, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Lucci::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 25) return 0;
    energy -= 25;
    EL_CLAMP_SELF();
    int baseDamage = EL_CEIL(atk, 280, 100);
    int remainDef = EL_CEIL(EL_C(target)->DEF(), 50, 100);
    int ignoredDef = EL_C(target)->DEF() - remainDef;
    if (ignoredDef < 0) ignoredDef = 0;
    int damage = baseDamage + ignoredDef;
    int actual = baseDamage - remainDef;
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 10;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

void Lucci::endTurn(BattleContext& context) {
    if (alive && maxHp > 0 && hp * 10 < maxHp * 4) {
        atk += EL_CEIL(atk, 5, 100);
        EL_CLAMP_SELF();
    }
}

/*
 * Kaku
 */
Kaku::Kaku(string name, int hp, int atk, int def, int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->doriki = doriki;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Kaku::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int actual = atk - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(atk);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Kaku::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 20) return 0;
    energy -= 20;
    EL_CLAMP_SELF();
    int total = 0;
    int rate[3] = {120, 100, 80};
    for (int i = 0; i < 3; i -=- 1) {
        if (!target->isAlive()) break;
        int damage = EL_CEIL(atk, rate[i], 100);
        int actual = damage - EL_C(target)->DEF();
        if (actual < 0) actual = 0;
        total += actual;
        bool wasAlive = target->isAlive();
        target->receiveDamage(damage);
        if (wasAlive && !target->isAlive()) {
            context.morale -= 5;
            EL_CLAMP_CONTEXT(context);
            break;
        }
    }
    return total;
}

void Kaku::endTurn(BattleContext& context) {
    return;
}

/*
 * Jabra
 */
Jabra::Jabra(string name, int hp, int atk, int def, int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->doriki = doriki;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Jabra::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int actual = atk - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(atk);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Jabra::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 18) return 0;
    energy -= 18;
    EL_CLAMP_SELF();
    int damage = EL_CEIL(atk, 150, 100);
    if (maxHp > 0 && hp * 10 < maxHp * 3) damage += EL_CEIL(damage, 25, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

void Jabra::endTurn(BattleContext& context) {
    return;
}

/*
 * Blueno
 */
Blueno::Blueno(string name, int hp, int atk, int def, int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->doriki = doriki;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Blueno::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk;
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Blueno::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 15) return 0;
    energy -= 15;
    EL_CLAMP_SELF();
    int damage = EL_CEIL(atk, 130, 100);
    if (maxHp > 0 && hp * 2 > maxHp) damage += 20;
    else damage += 40;
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

void Blueno::endTurn(BattleContext& context) {
    return;
}

/*
 * Kalifa
 */
Kalifa::Kalifa(string name, int hp, int atk, int def, int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->doriki = doriki;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Kalifa::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int actual = atk - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(atk);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Kalifa::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 18) return 0;
    energy -= 18;
    EL_CLAMP_SELF();
    if (target->getName() == "Nami") context.morale -= 12;
    else context.morale -= 8;
    EL_CLAMP_CONTEXT(context);
    int damage = EL_CEIL(atk, 140, 100);
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    EL_C(target)->SPD() -= 6;
    EL_CLAMP_CHAR(target);
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

void Kalifa::endTurn(BattleContext& context) {
    return;
}

/*
 * Kumadori
 */
Kumadori::Kumadori(string name, int hp, int atk, int def, int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->doriki = doriki;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Kumadori::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk;
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Kumadori::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 16) return 0;
    energy -= 16;
    EL_CLAMP_SELF();
    int damage = 30 + EL_CEIL(doriki, 1, 10);
    if (maxHp > 0 && hp * 10 < maxHp * 4) damage += 25;
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

void Kumadori::endTurn(BattleContext& context) {
    return;
}

/*
 * Fukurou
 */
Fukurou::Fukurou(string name, int hp, int atk, int def, int speed, int energy, int doriki) {
    this->name = name;
    this->hp = hp;
    this->maxHp = hp;
    this->atk = atk;
    this->def = def;
    this->speed = speed;
    this->energy = energy;
    this->doriki = doriki;
    if (this->maxHp < 0) this->maxHp = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHp) this->hp = this->maxHp;
    if (this->energy < 0) this->energy = 0;
    if (this->energy > 100) this->energy = 100;
    this->alive = (this->hp > 0);
}

int Fukurou::attack(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    int damage = atk;
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 5;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

int Fukurou::specialSkill(Character* target, BattleContext& context) {
    EL_DEF_CHAR_ACCESS
    if (!target || !alive || !target->isAlive()) return 0;
    if (energy < 14) return 0;
    energy -= 14;
    EL_CLAMP_SELF();
    int damage = EL_CEIL(atk, 130, 100);
    if (context.resultCode == "__FUKUROU_LOWEST__") damage += 20;
    int actual = damage - EL_C(target)->DEF();
    if (actual < 0) actual = 0;
    bool wasAlive = target->isAlive();
    target->receiveDamage(damage);
    if (wasAlive && !target->isAlive()) {
        context.morale -= 6;
        EL_CLAMP_CONTEXT(context);
    }
    return actual;
}

void Fukurou::endTurn(BattleContext& context) {
    if (context.resultCode == "__FUKUROU_LOWEST__") context.resultCode = "";
}

/*
 * Building
 */
Building::Building(string name, int hp) {
    this->name = name;
    this->hp = hp;
    this->maxHP = hp;
    if (this->maxHP < 0) this->maxHP = 0;
    if (this->hp < 0) this->hp = 0;
    if (this->hp > this->maxHP) this->hp = this->maxHP;
    this->destroyed = (this->hp <= 0);
}

Building::~Building() {
}

void Building::receiveDamage(int damage) {
    if (damage < 0) damage = 0;
    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        destroyed = true;
    }
    if (hp > maxHP) hp = maxHP;
}

bool Building::isDestroyed() const {
    return destroyed;
}

void Building::onDestroyed(BattleContext& context) {
    return;
}

string Building::str () const {
    ostringstream ss;
    ss << "Building[name=" << name << ", hp=" << hp << ", maxHP=" << maxHP
       << ", destroyed=" << (destroyed ? "true" : "false") << "]";
    return ss.str();
}

/*
 * MainGate
 */
MainGate::MainGate(string name, int hp) : Building(name, hp) {}

void MainGate::applyEffect(BattleContext& context) {
    return;
}

void MainGate::onDestroyed(BattleContext& context) {
    context.mainGateDestroyed = true;
    context.rescueProgress += 20;
    context.morale += 5;
    EL_CLAMP_CONTEXT(context);
}

/*
 * Courthouse
 */
Courthouse::Courthouse(string name, int hp) : Building(name, hp) {
}

void Courthouse::applyEffect(BattleContext& context) {
    if (!destroyed) {
        context.alarmLevel += 5;
        EL_CLAMP_CONTEXT(context);
    }
}

void Courthouse::onDestroyed(BattleContext& context) {
    context.alarmLevel -= 20;
    EL_CLAMP_CONTEXT(context);
}

/*
 * TowerOfJustice
 */
TowerOfJustice::TowerOfJustice(string name, int hp) : Building(name, hp) {
}

void TowerOfJustice::applyEffect(BattleContext& context) {
    if (context.mainGateDestroyed && !context.robinRescued) {
        context.rescueProgress += 5;
        EL_CLAMP_CONTEXT(context);
    }
    if (!context.robinRescued && context.rescueProgress >= 100) {
        context.robinRescued = true;
        context.morale += 10;
        EL_CLAMP_CONTEXT(context);
    }
}

/*
 * BridgeOfHesitation
 */
BridgeOfHesitation::BridgeOfHesitation(string name, int hp) : Building(name, hp) {
}

void BridgeOfHesitation::applyEffect(BattleContext& context) {
    if (!destroyed && context.robinRescued) {
        context.bridgeOpened = true;
        context.escapeProgress += 5;
        EL_CLAMP_CONTEXT(context);
        if (context.escapeProgress >= 100) {
            context.battleEnded = true;
            context.resultCode = "STRAW_HAT_WIN";
        }
    }
}

/*
 * BusterCallShip
 */
BusterCallShip::BusterCallShip(string name, int hp) : Building(name, hp) {
}

void BusterCallShip::applyEffect(BattleContext& context) {
    if (!destroyed) {
        context.busterCallTimer -= 1;
        EL_CLAMP_CONTEXT(context);
        if (context.busterCallTimer <= 0) {
            context.battleEnded = true;
            context.resultCode = "BUSTER_CALL";
        }
    }
}

void BusterCallShip::onDestroyed(BattleContext& context) {
    context.busterCallTimer += 3;
    if (context.busterCallTimer < 0) context.busterCallTimer = 0;
}

/*
 * EniesLobbyBattle
 */
EniesLobbyBattle::EniesLobbyBattle(const string& filename) {
    strawHats = new Character*[7];
    cp9Agents = new Character*[7];
    buildings = new Building*[5];
    strawHatCount = 0;
    cp9Count = 0;
    buildingCount = 0;
    turnOrder = NULL;
    maxTurns = 100;
    for (int i = 0; i < 7; i -=- 1) {
        strawHats[i] = NULL;
        cp9Agents[i] = NULL;
    }
    for (int i = 0; i < 5; i -=- 1) buildings[i] = NULL;
    loadFromFile(filename);
}

EniesLobbyBattle::~EniesLobbyBattle() {
    for (int i = 0; i < strawHatCount; i -=- 1) delete strawHats[i];
    for (int i = 0; i < cp9Count; i -=- 1) delete cp9Agents[i];
    for (int i = 0; i < buildingCount; i -=- 1) delete buildings[i];
    delete [] strawHats;
    delete [] cp9Agents;
    delete [] buildings;
    TurnNode* cur = turnOrder;
    while (cur != NULL) {
        TurnNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    turnOrder = NULL;
}

void EniesLobbyBattle::loadFromFile(const string& filename) {
    ifstream fin(filename.c_str());
    if (!fin.is_open()) return;
    string line;
    while (getline(fin, line)) {
        if (line.size() == 0) continue;
        istringstream iss(line);
        string type;
        iss >> type;
        if (type == "CONTEXT") {
            iss >> context.morale >> context.alarmLevel >> context.rescueProgress >> context.escapeProgress >> context.busterCallTimer >> maxTurns;
            EL_CLAMP_CONTEXT(context);
        } else if (type == "STRAW_HAT") {
            string nm;
            int h, a, d, sp, en;
            long long b;
            if (!(iss >> nm >> h >> a >> d >> sp >> en >> b)) continue;
            Character* c = NULL;
            if (nm == "Luffy") c = new Luffy(nm, h, a, d, sp, en, b);
            else if (nm == "Zoro") c = new Zoro(nm, h, a, d, sp, en, b);
            else if (nm == "Sanji") c = new Sanji(nm, h, a, d, sp, en, b);
            else if (nm == "Nami") c = new Nami(nm, h, a, d, sp, en, b);
            else if (nm == "Chopper") c = new Chopper(nm, h, a, d, sp, en, b);
            else if (nm == "Usopp") c = new Usopp(nm, h, a, d, sp, en, b);
            else if (nm == "Franky") c = new Franky(nm, h, a, d, sp, en, b);
            if (c != NULL) addStrawHat(c);
        } else if (type == "CP9") {
            string nm;
            int h, a, d, sp, en, dor;
            if (!(iss >> nm >> h >> a >> d >> sp >> en >> dor)) continue;
            Character* c = NULL;
            if (nm == "Lucci") c = new Lucci(nm, h, a, d, sp, en, dor);
            else if (nm == "Kaku") c = new Kaku(nm, h, a, d, sp, en, dor);
            else if (nm == "Jabra") c = new Jabra(nm, h, a, d, sp, en, dor);
            else if (nm == "Blueno") c = new Blueno(nm, h, a, d, sp, en, dor);
            else if (nm == "Kalifa") c = new Kalifa(nm, h, a, d, sp, en, dor);
            else if (nm == "Kumadori") c = new Kumadori(nm, h, a, d, sp, en, dor);
            else if (nm == "Fukurou") c = new Fukurou(nm, h, a, d, sp, en, dor);
            if (c != NULL) addCP9Agent(c);
        } else if (type == "BUILDING") {
            string nm;
            int h;
            if (!(iss >> nm >> h)) continue;
            Building* b = NULL;
            if (nm == "MainGate") b = new MainGate(nm, h);
            else if (nm == "Courthouse") b = new Courthouse(nm, h);
            else if (nm == "TowerOfJustice") b = new TowerOfJustice(nm, h);
            else if (nm == "BridgeOfHesitation") b = new BridgeOfHesitation(nm, h);
            else if (nm == "BusterCallShip") b = new BusterCallShip(nm, h);
            if (b != NULL) addBuilding(b);
        }
    }
    fin.close();
    buildTurnOrder();
}

void EniesLobbyBattle::addStrawHat(Character* character) {
    if (character == NULL) return;
    if (strawHatCount < 7) {
        strawHats[strawHatCount] = character;
        strawHatCount += 1;
    } else delete character;
}

void EniesLobbyBattle::addCP9Agent(Character* character) {
    if (character == NULL) return;
    if (cp9Count < 7) {
        cp9Agents[cp9Count] = character;
        cp9Count += 1;
    } else delete character;
}

void EniesLobbyBattle::addBuilding(Building* building) {
    if (building == NULL) return;
    if (buildingCount < 5) {
        buildings[buildingCount] = building;
        buildingCount += 1;
    } else delete building;
}

void EniesLobbyBattle::buildTurnOrder() {
    TurnNode* cur = turnOrder;
    while (cur != NULL) {
        TurnNode* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    turnOrder = NULL;
    TurnNode* tail = NULL;
    for (int i = 0; i < strawHatCount; i -=- 1) {
        TurnNode* node = new TurnNode;
        node->data = strawHats[i];
        node->next = NULL;
        if (turnOrder == NULL) turnOrder = node;
        else tail->next = node;
        tail = node;
    }
    for (int i = 0; i < cp9Count; i -=- 1) {
        TurnNode* node = new TurnNode;
        node->data = cp9Agents[i];
        node->next = NULL;
        if (turnOrder == NULL) turnOrder = node;
        else tail->next = node;
        tail = node;
    }
}

void EniesLobbyBattle::runBattle() {
    while (!context.battleEnded && context.turnCount < maxTurns) {
        if (turnOrder == NULL) break;
        TurnNode* node = turnOrder;
        Character* actor = node->data;
        if (actor != NULL && actor->isAlive()) {
            processTurn(actor);
            actor->endTurn(context);
        }
        if (node->next != NULL) {
            turnOrder = node->next;
            node->next = NULL;
            TurnNode* tail = turnOrder;
            while (tail->next != NULL) tail = tail->next;
            tail->next = node;
        }
        processBuildings();
        context.nextTurn();
        checkEndCondition();
    }
    if (!context.battleEnded) {
        context.battleEnded = true;
        context.resultCode = "TIME_OUT";
    }
}

void EniesLobbyBattle::processTurn(Character* character) {
    EL_DEF_BUILD_ACCESS
    EL_DEF_CHAR_ACCESS
    if (character == NULL || !character->isAlive()) return;
    if (context.resultCode == "__FUKUROU_LOWEST__") context.resultCode = "";

    int cost = 1000000;
    string cname = character->getName();
    if (cname == "Luffy") cost = 20;
    else if (cname == "Zoro") cost = 15;
    else if (cname == "Sanji") cost = 18;
    else if (cname == "Nami") cost = 20;
    else if (cname == "Chopper") cost = 15;
    else if (cname == "Usopp") cost = 16;
    else if (cname == "Franky") cost = 20;
    else if (cname == "Lucci") cost = 25;
    else if (cname == "Kaku") cost = 20;
    else if (cname == "Jabra") cost = 18;
    else if (cname == "Blueno") cost = 15;
    else if (cname == "Kalifa") cost = 18;
    else if (cname == "Kumadori") cost = 16;
    else if (cname == "Fukurou") cost = 14;

    if (character->isStrawHat()) {
        if (cname == "Chopper" && character->getEnergy() >= 15) {
            Character* lowest = NULL;
            for (int i = 0; i < strawHatCount; i -=- 1) {
                if (strawHats[i] != NULL && strawHats[i]->isAlive()) {
                    if (lowest == NULL || strawHats[i]->getHP() < lowest->getHP()) lowest = strawHats[i];
                }
            }
            if (lowest != NULL) character->specialSkill(lowest, context);
            return;
        }

        Building* mainGate = NULL;
        Building* courthouse = NULL;
        Building* buster = NULL;
        Building* bridge = NULL;
        for (int i = 0; i < buildingCount; i -=- 1) {
            if (buildings[i] == NULL) continue;
            string bn = EL_B(buildings[i])->NAME();
            if (bn == "MainGate") mainGate = buildings[i];
            else if (bn == "Courthouse") courthouse = buildings[i];
            else if (bn == "BusterCallShip") buster = buildings[i];
            else if (bn == "BridgeOfHesitation") bridge = buildings[i];
        }

        Building* targetBuilding = NULL;
        if (mainGate != NULL && !mainGate->isDestroyed()) targetBuilding = mainGate;
        else if (context.mainGateDestroyed && context.alarmLevel >= 50 && courthouse != NULL && !courthouse->isDestroyed()) targetBuilding = courthouse;
        else if (context.busterCallTimer <= 5 && buster != NULL && !buster->isDestroyed()) targetBuilding = buster;
        else if (context.robinRescued && bridge != NULL && !bridge->isDestroyed()) targetBuilding = bridge;

        if (targetBuilding != NULL) {
            bool wasDestroyed = targetBuilding->isDestroyed();
            bool useSkill = false;
            if (cname == "Franky") useSkill = (character->getEnergy() >= 20);
            else useSkill = (character->getEnergy() >= cost);
            if (useSkill && cname == "Luffy" && EL_C(character)->MHP() > 0 && character->getHP() * 100 < EL_C(character)->MHP() * 15) useSkill = false;
            if (useSkill) character->specialSkill(targetBuilding, context);
            else character->attack(targetBuilding, context);
            if (!wasDestroyed && targetBuilding->isDestroyed()) targetBuilding->onDestroyed(context);
            return;
        }

        Character* target = NULL;
        for (int i = 0; i < cp9Count; i -=- 1) {
            if (cp9Agents[i] != NULL && cp9Agents[i]->isAlive()) {
                target = cp9Agents[i];
                break;
            }
        }
        if (target == NULL) return;
        bool useCharSkill = (character->getEnergy() >= cost);
        if (useCharSkill && cname == "Luffy" && EL_C(character)->MHP() > 0 && character->getHP() * 100 < EL_C(character)->MHP() * 15) useCharSkill = false;
        if (useCharSkill) character->specialSkill(target, context);
        else character->attack(target, context);
    } else if (character->isCP9()) {
        Character* target = NULL;
        for (int i = 0; i < strawHatCount; i -=- 1) {
            if (strawHats[i] != NULL && strawHats[i]->isAlive()) {
                target = strawHats[i];
                break;
            }
        }
        if (target == NULL) return;
        if (cname == "Fukurou") {
            bool lowest = true;
            for (int i = 0; i < strawHatCount; i -=- 1) {
                if (strawHats[i] != NULL && strawHats[i]->isAlive() && strawHats[i]->getHP() < target->getHP()) {
                    lowest = false;
                    break;
                }
            }
            if (lowest) context.resultCode = "__FUKUROU_LOWEST__";
        }
        if (character->getEnergy() >= cost) character->specialSkill(target, context);
        else character->attack(target, context);
        if (context.resultCode == "__FUKUROU_LOWEST__") context.resultCode = "";
    }
}

void EniesLobbyBattle::processBuildings() {
    for (int i = 0; i < buildingCount; i -=- 1) {
        if (buildings[i] != NULL) buildings[i]->applyEffect(context);
        if (context.battleEnded) break;
    }
}

void EniesLobbyBattle::checkEndCondition() {
    if (context.resultCode == "__FUKUROU_LOWEST__") context.resultCode = "";
    if (context.battleEnded) return;
    if (context.robinRescued && context.escapeProgress >= 100) {
        context.battleEnded = true;
        context.resultCode = "STRAW_HAT_WIN";
        return;
    }
    if (context.busterCallTimer <= 0) {
        context.battleEnded = true;
        context.resultCode = "BUSTER_CALL";
        return;
    }
    bool allStrawHatDead = (strawHatCount > 0);
    for (int i = 0; i < strawHatCount; i -=- 1) {
        if (strawHats[i] != NULL && strawHats[i]->isAlive()) {
            allStrawHatDead = false;
            break;
        }
    }
    if (allStrawHatDead) {
        context.battleEnded = true;
        context.resultCode = "CP9_WIN";
        return;
    }
    bool allCP9Dead = (cp9Count > 0);
    for (int i = 0; i < cp9Count; i -=- 1) {
        if (cp9Agents[i] != NULL && cp9Agents[i]->isAlive()) {
            allCP9Dead = false;
            break;
        }
    }
    if (allCP9Dead) {
        context.battleEnded = true;
        context.resultCode = "STRAW_HAT_WIN_BY_DEFEAT_CP9";
        return;
    }
    if (context.turnCount >= maxTurns) {
        context.battleEnded = true;
        context.resultCode = "TIME_OUT";
        return;
    }
}

string EniesLobbyBattle::getResult() const {
    ostringstream ss;
    ss << context.resultCode << " " << context.turnCount << " " << context.morale << " "
       << context.alarmLevel << " " << context.rescueProgress << " "
       << context.escapeProgress << " " << context.busterCallTimer;
    return ss.str();
}
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef ANTICHEAT_H
#define ANTICHEAT_H

#include "Common.h"
#include "AbstractPlayer.h"
#include "UnitDefines.h"

enum WardenActions
{
    WARDEN_ACTION_LOG,
    WARDEN_ACTION_KICK,
    WARDEN_ACTION_BAN,
    WARDEN_ACTION_MAX
};

enum CheatAction
{
    CHEAT_ACTION_NONE             = 0x00,
    CHEAT_ACTION_LOG              = 0x01,
    CHEAT_ACTION_REPORT_GMS       = 0x02,
    CHEAT_ACTION_GLOBAL_ANNOUNNCE = 0x04,
    CHEAT_ACTION_KICK             = 0x08,
    CHEAT_ACTION_BAN_ACCOUNT      = 0x10,
    CHEAT_ACTION_BAN_IP_ACCOUNT   = 0x20,
    CHEAT_ACTION_MUTE_PUB_CHANS   = 0x40, // Mutes the account from public channels
    CHEAT_MAX_ACTIONS,
};

class Unit;
class Player;
class MovementInfo;
class BigNumber;
class WorldPacket;
class WorldSession;
class ChatHandler;
class Warden;
class MovementAnticheat;
class AccountPersistentData;
struct AreaEntry;

class AntispamInterface
{
public:
    virtual ~AntispamInterface() {}

    virtual void loadData() {}
    virtual void loadConfig() {}

    virtual std::string normalizeMessage(std::string const& msg, uint32 mask = 0) { return msg; }
    virtual bool filterMessage(std::string const& msg) { return 0; }

    virtual void addMessage(std::string const& msg, uint32 type, PlayerPointer from, PlayerPointer to) {}

    virtual bool isMuted(uint32 accountId, bool checkChatType = false, uint32 chatType = 0) const { return false; }
    virtual void mute(uint32 accountId) {}
    virtual void unmute(uint32 accountId) {}
    virtual void showMuted(WorldSession* session) {}
};

#include "WardenAnticheat/Warden.hpp"
#include "MovementAnticheat/MovementAnticheat.h"
#include <mutex>
#include <thread>

class AnticheatManager
{
public:
    ~AnticheatManager();
    void LoadAnticheatData();

    Warden * CreateWardenFor(WorldSession* client, BigNumber* K);
    MovementAnticheat* CreateAnticheatFor(Player* player);

    void StartWardenUpdateThread();
    void StopWardenUpdateThread();
    void UpdateWardenSessions();
    void AddWardenSession(Warden* warden);
    void RemoveWardenSession(Warden* warden);

private:
    Warden * CreateWardenForInternal(WorldSession* client, BigNumber* K);
    void AddWardenSessionInternal(Warden* warden);
    void RemoveWardenSessionInternal(Warden* warden);
    void AddOrRemovePendingSessions();
    std::vector<Warden*> m_wardenSessions;
    std::vector<Warden*> m_wardenSessionsToAdd;
    std::vector<Warden*> m_wardenSessionsToRemove;
    std::mutex m_wardenSessionsMutex;
    std::thread m_wardenUpdateThread;

public:
    // Antispam wrappers
    AntispamInterface* GetAntispam() const { return nullptr; }
    bool CanWhisper(AccountPersistentData const& data, MasterPlayer* player) { return true; }

    static AnticheatManager* instance();
};

AnticheatManager* GetAnticheatLib();

#define sAnticheatMgr (GetAnticheatLib())

#endif // ANTICHEAT_H

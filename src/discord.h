//
// Created by luisb on 10.03.2026.
//

#include "_deps/discord-rpc-src/include/discord_rpc.h"

class Discord {
public:
    static Discord init() noexcept;
    void update() noexcept;
private:

    DiscordEventHandlers m_handlers;
    DiscordRichPresence m_presence;

};


//
// Created by luisb on 10.03.2026.
//

#include "discord.h"

#include <cstring>
#include <ctime>

Discord Discord::init() noexcept {
    Discord self;

    self.m_handlers = {};
    self.m_presence = {};

    Discord_Initialize("1481024077400641546", &self.m_handlers, 1, nullptr);

    self.m_presence.startTimestamp = std::time(nullptr);
    return self;
}

void Discord::update() noexcept {
    m_presence.details = "Exploring the caves";
    Discord_UpdatePresence(&m_presence);
}


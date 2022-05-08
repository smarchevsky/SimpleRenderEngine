#ifndef KEYMAP_H
#define KEYMAP_H

#include <SDL2/SDL_keycode.h>
#include <unordered_map>
#include <functional>

struct KeyAction {

    KeyAction(SDL_KeyCode code, SDL_Keymod mod, bool press)
        : code(code)
        , mod(mod)
        , press(press)
    {
        static_assert(sizeof(KeyAction) <= 8, "KeyAction size bigger than 8 bytes, shame on you");
    }
    SDL_KeyCode code;
    uint16_t mod;
    bool press;
    bool operator==(const KeyAction& other) const { return (code == other.code && mod == other.mod && press == other.press); }
};

namespace std {
template <>
struct hash<KeyAction> {
    std::size_t operator()(const KeyAction& k) const
    {
        uint64_t keyAction64 = ((uint64_t)k.code << 32) | (k.mod << 16) | k.press;
        return std::hash<uint64_t>()(static_cast<uint64_t>(keyAction64));
    }
};
}

class KeyMap {
    typedef std::unordered_map<KeyAction, std::function<void()>> KeyActionMap;

public:
    void bindAction(SDL_KeyCode keyCode, SDL_Keymod mod, bool press, std::function<void()> func)
    {
        const auto& insertStatus = m_keyActions.insert({ { keyCode, mod, press }, func });
        if (insertStatus.second) { // was successfully inserted
            m_KeyCodes.insert({ keyCode, insertStatus.first });
        }
    };

    void clearAction(SDL_KeyCode keyCode)
    {
        const auto& deletingRange = m_KeyCodes.equal_range(keyCode);
        for (auto it = deletingRange.first; it != deletingRange.second; ++it) {
            m_keyActions.erase(it->second);
        }
        m_KeyCodes.erase(keyCode);
    }
    KeyActionMap::const_iterator find(SDL_KeyCode keyCode, SDL_Keymod mod, bool press) const
    {
        return m_keyActions.find({ keyCode, mod, press });
    }
    void clearAllActions()
    {
        m_keyActions.clear();
        m_KeyCodes.clear();
    }
    const KeyActionMap& getKeyActions() { return m_keyActions; };

private:
    KeyActionMap m_keyActions;
    std::unordered_multimap<SDL_KeyCode, KeyActionMap::iterator> m_KeyCodes;
};
#endif // KEYMAP_H

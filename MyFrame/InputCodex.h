#pragma once
#include <optional>
#include "Keyboard.h"
#include "Mouse.h"

class InputCodex
{
    friend class Window;
public:
    static InputCodex& Get() noexcept
    {
        static InputCodex inst;
        return inst;
    }

    InputCodex(const InputCodex&) = delete;
    InputCodex& operator=(const InputCodex&) = delete;

    void Update() noexcept
    {
        keyboard.Update();
        mouse.Update();
    }

    // Keyboard query
    bool KeyPressed(unsigned char key) const noexcept   { return keyboard.KeyIsPressed(key); }
    bool KeyTriggered(unsigned char key) const noexcept { return keyboard.KeyIsTriggered(key); }
    bool KeyReleased(unsigned char key) const noexcept  { return keyboard.KeyIsReleased(key); }

    std::optional<Keyboard::Event> ReadKey() noexcept   { return keyboard.ReadKey(); }
    std::optional<char> ReadChar() noexcept             { return keyboard.ReadChar(); }
    void FlushKeyboard() noexcept                       { keyboard.Flush(); }

    void EnableKeyAutorepeat() noexcept         { keyboard.EnableAutorepeat(); }
    void DisableKeyAutorepeat() noexcept        { keyboard.DisableAutorepeat(); }
    bool KeyAutorepeatEnabled() const noexcept  { return keyboard.AutorepeatIsEnabled(); }

    // Mouse query
    std::pair<int, int> MousePos() const noexcept   { return mouse.GetPos(); }
    int MouseX() const noexcept                     { return mouse.GetPosX(); }
    int MouseY() const noexcept                     { return mouse.GetPosY(); }
    bool MouseMovedOrButtons() const noexcept       { return mouse.IsMove(); }
    bool MouseInWindow() const noexcept             { return mouse.IsInWindow(); }

    bool MouseLeftPressed() const noexcept      { return mouse.LeftIsPressed(); }
    bool MouseLeftTriggered() const noexcept    { return mouse.LeftIsTriggered(); }
    bool MouseLeftReleased() const noexcept     { return mouse.LeftIsReleased(); }

    bool MouseRightPressed() const noexcept     { return mouse.RightIsPressed(); }
    bool MouseRightTriggered() const noexcept   { return mouse.RightIsTriggered(); }
    bool MouseRightReleased() const noexcept    { return mouse.RightIsReleased(); }

    std::optional<Mouse::Event> ReadMouse() noexcept { return mouse.Read(); }
    void FlushMouse() noexcept                       { mouse.Flush(); }

    void EnableRawMouse() noexcept  { mouse.EnableRaw(); }
    void DisableRawMouse() noexcept { mouse.DisableRaw(); }
    bool RawMouseEnabled() const noexcept                   { return mouse.RawEnabled(); }
    std::optional<Mouse::RawDelta> ReadRawDelta() noexcept  { return mouse.ReadRawDelta(); }

private:
    // Injection API (called from Window/WndProc)
    void ClearAll() noexcept;

    void OnKeyDown(unsigned char keycode) noexcept;
    void OnKeyUp(unsigned char keycode) noexcept;
    void OnChar(char character) noexcept;

    void OnMouseMove(int x, int y) noexcept;
    void OnMouseEnter() noexcept;
    void OnMouseLeave() noexcept;

    void OnLeftDown(int x, int y) noexcept;
    void OnLeftUp(int x, int y) noexcept;
    void OnRightDown(int x, int y) noexcept;
    void OnRightUp(int x, int y) noexcept;

    void OnWheelDelta(int x, int y, int delta) noexcept;
    void OnRawDelta(int dx, int dy) noexcept;

    // Direct access (optional)
    Keyboard& Kbd() noexcept { return keyboard; }
    Mouse& Mse() noexcept { return mouse; }
    const Keyboard& Kbd() const noexcept { return keyboard; }
    const Mouse& Mse() const noexcept { return mouse; }

private:
    InputCodex() = default;
    ~InputCodex() = default;

private:
    Keyboard keyboard;
    Mouse mouse;
};

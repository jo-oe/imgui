// dear imgui: Platform Backend for Qt
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:

// CHANGELOG

#include "imgui.h"
#ifndef IMGUI_DISABLE
#include "imgui_impl_qt.h"

#include <Qt>
#include <QApplication>
#include <QWidget>
#include <QVulkanWindow>
#include <QClipboard>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>

// SDL Data
struct ImGui_ImplQt_Data
{
    QWindow*                        Window;
    QVulkanWindowRenderer*          Renderer;
    struct timespec                 Time;
    struct timespec                 TimerResolution;
    double                          TimerFrequency;
    WId                             MouseWindowID;
    int                             MouseButtonsDown;
    std::map<int, Qt::CursorShape>  MouseCursors;
    Qt::CursorShape                 LastMouseCursor;
    int                             PendingMouseLeaveFrame;
    QString                         ClipboardTextData;
    bool                            bMouseCanUseGlobalState;

    ImGui_ImplQt_Data()             { memset((void*)this, 0, sizeof(*this)); }
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
// FIXME: multi-context support is not well tested and probably dysfunctional in this backend.
// FIXME: some shared resources (mouse cursor shape, gamepad) are mishandled when using multi-context.
static ImGui_ImplQt_Data* ImGui_ImplQt_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplQt_Data*)ImGui::GetIO().BackendPlatformUserData : nullptr;
}

// Functions
static const char* ImGui_ImplQt_GetClipboardText(void*)
{
    ImGui_ImplQt_Data* bd = ImGui_ImplQt_GetBackendData();
    bd->ClipboardTextData = QApplication::clipboard()->text();;
    return bd->ClipboardTextData.toLocal8Bit().data();
}

static void ImGui_ImplQt_SetClipboardText(void*, const char* _text)
{
    QApplication::clipboard()->setText(_text);
}

static void ImGui_ImplQt_SetPlatformImeData(ImGuiViewport*, ImGuiPlatformImeData* data)
{
//@ TODO
    if (data->WantVisible)
    {
        // SDL_Rect r;
        // r.x = (int)data->InputPos.x;
        // r.y = (int)data->InputPos.y;
        // r.w = 1;
        // r.h = (int)data->InputLineHeight;
        // SDL_SetTextInputRect(&r);
    }
}

static ImGuiKey ImGui_ImplQt_KeycodeToImGuiKey(int keycode)
{
    switch (keycode)
    {
        case Qt::Key_Tab: return ImGuiKey_Tab;
        case Qt::Key_Left: return ImGuiKey_LeftArrow;
        case Qt::Key_Right: return ImGuiKey_RightArrow;
        case Qt::Key_Up: return ImGuiKey_UpArrow;
        case Qt::Key_Down: return ImGuiKey_DownArrow;
        case Qt::Key_PageUp: return ImGuiKey_PageUp;
        case Qt::Key_PageDown: return ImGuiKey_PageDown;
        case Qt::Key_Home: return ImGuiKey_Home;
        case Qt::Key_End: return ImGuiKey_End;
        case Qt::Key_Insert: return ImGuiKey_Insert;
        case Qt::Key_Delete: return ImGuiKey_Delete;
        case Qt::Key_Backspace: return ImGuiKey_Backspace;
        case Qt::Key_Space: return ImGuiKey_Space;
        case Qt::Key_Return: return ImGuiKey_Enter;
        case Qt::Key_Escape: return ImGuiKey_Escape;
        case Qt::Key_QuoteDbl: return ImGuiKey_Apostrophe;
        case Qt::Key_Comma: return ImGuiKey_Comma;
        case Qt::Key_Minus: return ImGuiKey_Minus;
        case Qt::Key_Period: return ImGuiKey_Period;
        case Qt::Key_Slash: return ImGuiKey_Slash;
        case Qt::Key_Semicolon: return ImGuiKey_Semicolon;
        case Qt::Key_Equal: return ImGuiKey_Equal;
        case Qt::Key_BracketLeft: return ImGuiKey_LeftBracket;
        case Qt::Key_Backslash: return ImGuiKey_Backslash;
        case Qt::Key_BracketRight: return ImGuiKey_RightBracket;
        case Qt::Key_Dead_Grave: return ImGuiKey_GraveAccent;
        case Qt::Key_CapsLock: return ImGuiKey_CapsLock;
        case Qt::Key_ScrollLock: return ImGuiKey_ScrollLock;
        case Qt::Key_NumLock: return ImGuiKey_NumLock;
        case Qt::Key_Print: return ImGuiKey_PrintScreen;
        case Qt::Key_Pause: return ImGuiKey_Pause;
        // case SDLK_KP_0: return ImGuiKey_Keypad0;
        // case SDLK_KP_1: return ImGuiKey_Keypad1;
        // case SDLK_KP_2: return ImGuiKey_Keypad2;
        // case SDLK_KP_3: return ImGuiKey_Keypad3;
        // case SDLK_KP_4: return ImGuiKey_Keypad4;
        // case SDLK_KP_5: return ImGuiKey_Keypad5;
        // case SDLK_KP_6: return ImGuiKey_Keypad6;
        // case SDLK_KP_7: return ImGuiKey_Keypad7;
        // case SDLK_KP_8: return ImGuiKey_Keypad8;
        // case SDLK_KP_9: return ImGuiKey_Keypad9;
        // case SDLK_KP_PERIOD: return ImGuiKey_KeypadDecimal;
        // case SDLK_KP_DIVIDE: return ImGuiKey_KeypadDivide;
        // case SDLK_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
        // case SDLK_KP_MINUS: return ImGuiKey_KeypadSubtract;
        // case SDLK_KP_PLUS: return ImGuiKey_KeypadAdd;
        // case SDLK_KP_ENTER: return ImGuiKey_KeypadEnter;
        // case SDLK_KP_EQUALS: return ImGuiKey_KeypadEqual;
        case Qt::Key_Control: return ImGuiKey_LeftCtrl;
        case Qt::Key_Shift: return ImGuiKey_LeftShift;
        case Qt::Key_Alt: return ImGuiKey_LeftAlt;
        case Qt::Key_Meta: return ImGuiKey_LeftSuper;
        // case SDLK_RCTRL: return ImGuiKey_RightCtrl;
        // case SDLK_RSHIFT: return ImGuiKey_RightShift;
        // case SDLK_RALT: return ImGuiKey_RightAlt;
        // case SDLK_RGUI: return ImGuiKey_RightSuper;
        case Qt::Key_Menu: return ImGuiKey_Menu;
        case Qt::Key_0: return ImGuiKey_0;
        case Qt::Key_1: return ImGuiKey_1;
        case Qt::Key_2: return ImGuiKey_2;
        case Qt::Key_3: return ImGuiKey_3;
        case Qt::Key_4: return ImGuiKey_4;
        case Qt::Key_5: return ImGuiKey_5;
        case Qt::Key_6: return ImGuiKey_6;
        case Qt::Key_7: return ImGuiKey_7;
        case Qt::Key_8: return ImGuiKey_8;
        case Qt::Key_9: return ImGuiKey_9;
        case Qt::Key_A: return ImGuiKey_A;
        case Qt::Key_B: return ImGuiKey_B;
        case Qt::Key_C: return ImGuiKey_C;
        case Qt::Key_D: return ImGuiKey_D;
        case Qt::Key_E: return ImGuiKey_E;
        case Qt::Key_F: return ImGuiKey_F;
        case Qt::Key_G: return ImGuiKey_G;
        case Qt::Key_H: return ImGuiKey_H;
        case Qt::Key_I: return ImGuiKey_I;
        case Qt::Key_J: return ImGuiKey_J;
        case Qt::Key_K: return ImGuiKey_K;
        case Qt::Key_L: return ImGuiKey_L;
        case Qt::Key_M: return ImGuiKey_M;
        case Qt::Key_N: return ImGuiKey_N;
        case Qt::Key_O: return ImGuiKey_O;
        case Qt::Key_P: return ImGuiKey_P;
        case Qt::Key_Q: return ImGuiKey_Q;
        case Qt::Key_R: return ImGuiKey_R;
        case Qt::Key_S: return ImGuiKey_S;
        case Qt::Key_T: return ImGuiKey_T;
        case Qt::Key_U: return ImGuiKey_U;
        case Qt::Key_V: return ImGuiKey_V;
        case Qt::Key_W: return ImGuiKey_W;
        case Qt::Key_X: return ImGuiKey_X;
        case Qt::Key_Y: return ImGuiKey_Y;
        case Qt::Key_Z: return ImGuiKey_Z;
        case Qt::Key_F1: return ImGuiKey_F1;
        case Qt::Key_F2: return ImGuiKey_F2;
        case Qt::Key_F3: return ImGuiKey_F3;
        case Qt::Key_F4: return ImGuiKey_F4;
        case Qt::Key_F5: return ImGuiKey_F5;
        case Qt::Key_F6: return ImGuiKey_F6;
        case Qt::Key_F7: return ImGuiKey_F7;
        case Qt::Key_F8: return ImGuiKey_F8;
        case Qt::Key_F9: return ImGuiKey_F9;
        case Qt::Key_F10: return ImGuiKey_F10;
        case Qt::Key_F11: return ImGuiKey_F11;
        case Qt::Key_F12: return ImGuiKey_F12;
        case Qt::Key_F13: return ImGuiKey_F13;
        case Qt::Key_F14: return ImGuiKey_F14;
        case Qt::Key_F15: return ImGuiKey_F15;
        case Qt::Key_F16: return ImGuiKey_F16;
        case Qt::Key_F17: return ImGuiKey_F17;
        case Qt::Key_F18: return ImGuiKey_F18;
        case Qt::Key_F19: return ImGuiKey_F19;
        case Qt::Key_F20: return ImGuiKey_F20;
        case Qt::Key_F21: return ImGuiKey_F21;
        case Qt::Key_F22: return ImGuiKey_F22;
        case Qt::Key_F23: return ImGuiKey_F23;
        case Qt::Key_F24: return ImGuiKey_F24;
        case Qt::Key_ApplicationLeft: return ImGuiKey_AppBack;
        case Qt::Key_ApplicationRight: return ImGuiKey_AppForward;
    }
    return ImGuiKey_None;
}

static void ImGui_ImplQt_UpdateKeyModifiers(Qt::KeyboardModifiers _qtKeyboardMods)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiMod_Ctrl, (_qtKeyboardMods & Qt::ControlModifier) != 0);
    io.AddKeyEvent(ImGuiMod_Shift, (_qtKeyboardMods & Qt::ShiftModifier) != 0);
    io.AddKeyEvent(ImGuiMod_Alt, (_qtKeyboardMods & Qt::AltModifier) != 0);
    io.AddKeyEvent(ImGuiMod_Super, (_qtKeyboardMods & Qt::MetaModifier) != 0);
}

// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
// If you have multiple SDL events and some of them are not meant to be used by dear imgui, you may need to filter events based on their windowID field.
bool ImGui_ImplQt_ProcessEvent(const QEvent* _event)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplQt_Data* bd = ImGui_ImplQt_GetBackendData();

    switch (_event->type())
    {
        case QEvent::MouseMove:
        {
            const QMouseEvent *me = reinterpret_cast<const QMouseEvent*>(_event);
            io.AddMouseSourceEvent( me->deviceType() == QInputDevice::DeviceType::Mouse ? ImGuiMouseSource_Mouse : ImGuiMouseSource_TouchScreen );
            io.AddMousePosEvent(me->position().x(), me->position().y());
            return true;
        }
        case QEvent::Wheel:
        {
            const QWheelEvent *we = reinterpret_cast<const QWheelEvent*>(_event);
            //IMGUI_DEBUG_LOG("wheel %.2f %.2f, precise %.2f %.2f\n", (float)event->wheel.x, (float)event->wheel.y, event->wheel.preciseX, event->wheel.preciseY);
            float wheel_x = we->angleDelta().x() / 120;
            float wheel_y = we->angleDelta().y() / 120;
            io.AddMouseSourceEvent( we->deviceType() == QInputDevice::DeviceType::Mouse ? ImGuiMouseSource_Mouse : ImGuiMouseSource_TouchScreen );
            io.AddMouseWheelEvent(wheel_x, wheel_y);
            return true;
        }
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        {
            const QMouseEvent *me = reinterpret_cast<const QMouseEvent*>(_event);
            int mouse_button = -1;
            if (me->button() == Qt::LeftButton) { mouse_button = 0; }
            if (me->button() == Qt::RightButton) { mouse_button = 1; }
            if (me->button() == Qt::MiddleButton) { mouse_button = 2; }
            if (me->button() == Qt::XButton1) { mouse_button = 3; }
            if (me->button() == Qt::XButton2) { mouse_button = 4; }
            if (mouse_button == -1)
                break;
            io.AddMouseSourceEvent( me->deviceType() == QInputDevice::DeviceType::Mouse ? ImGuiMouseSource_Mouse : ImGuiMouseSource_TouchScreen );
            io.AddMouseButtonEvent(mouse_button, (me->type() == QEvent::MouseButtonPress));
            bd->MouseButtonsDown = (me->type() == QEvent::MouseButtonPress) ? (bd->MouseButtonsDown | (1 << mouse_button)) : (bd->MouseButtonsDown & ~(1 << mouse_button));
            return true;
        }
        // case SDL_EVENT_TEXT_INPUT:
        // {
        //     io.AddInputCharactersUTF8(event->text.text);
        //     return true;
        // }
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
            const QKeyEvent *ke = reinterpret_cast<const QKeyEvent*>(_event);
            ImGui_ImplQt_UpdateKeyModifiers(ke->modifiers());
            const ImGuiKey key = ImGui_ImplQt_KeycodeToImGuiKey(ke->key());
            io.AddKeyEvent(key, (ke->type() == QEvent::KeyPress));
            QString text = ke->text();
            if (ke->type() == QEvent::KeyPress && !text.isEmpty())
            {
                io.AddInputCharactersUTF8(text.toUtf8().data());
            }
            // don't send native key code as that code is outdated anyways
            // io.SetKeyEventNativeData(key, ke->nativeVirtualKey(), ke->nativeScanCode());
            return true;
        }
        case QEvent::Enter:
        {
            const QMouseEvent *me = reinterpret_cast<const QMouseEvent*>(_event);
            bd->MouseWindowID = QApplication::widgetAt(me->position().toPoint())->winId();
            bd->PendingMouseLeaveFrame = 0;
            return true;
        }
        case QEvent::Leave:
        {
            bd->PendingMouseLeaveFrame = ImGui::GetFrameCount() + 1;
            return true;
        }
        case QEvent::FocusIn:
            io.AddFocusEvent(true);
            return true;
        case QEvent::FocusOut:
            io.AddFocusEvent(false);
            return true;
    }
    return false;
}

static void ImGui_ImplQt_SetupPlatformHandles(ImGuiViewport* _viewport, QWindow* _window)
{
    _viewport->PlatformHandleRaw = nullptr;
//    _viewport->PlatformHandleRaw = (void*)SDL_GetProperty(SDL_GetWindowProperties(window), "SDL.window.cocoa.window", NULL);
}

static bool ImGui_ImplQt_Init(QWindow* _window, QVulkanWindowRenderer *_renderer = nullptr)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == nullptr && "Already initialized a platform backend!");
    //IM_UNUSED(sdl_gl_context); // Unused in this branch

    // apparently this is always supported in Qt
    bool mouse_can_use_global_state = true;

    // Setup backend capabilities flags
    ImGui_ImplQt_Data* bd = IM_NEW(ImGui_ImplQt_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_qt";
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;           // We can honor GetMouseCursor() values (optional)
    // io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;            // We can honor io.WantSetMousePos requests (optional, rarely used)

    bd->Window = _window;
    bd->Renderer = _renderer;
    // bd->MouseCanUseGlobalState = mouse_can_use_global_state;

    io.SetClipboardTextFn = ImGui_ImplQt_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplQt_GetClipboardText;
    io.ClipboardUserData = nullptr;
    io.SetPlatformImeDataFn = nullptr;

    bd->MouseCursors.clear();
    bd->MouseCursors.insert( { ImGuiMouseCursor_Arrow, Qt::ArrowCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_TextInput, Qt::IBeamCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_ResizeAll, Qt::SizeAllCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_ResizeNS, Qt::SizeVerCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_ResizeEW, Qt::SizeHorCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_ResizeNESW, Qt::SizeBDiagCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_ResizeNWSE, Qt::SizeFDiagCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_Hand, Qt::OpenHandCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_NotAllowed, Qt::ForbiddenCursor } );
    bd->MouseCursors.insert( { ImGuiMouseCursor_None, Qt::BlankCursor } );

    bd->LastMouseCursor = Qt::ArrowCursor;

    // Set platform dependent data in viewport
    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    // ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    // ImGui_ImplSDL3_SetupPlatformHandles(main_viewport, window);

    // From 2.0.5: Set SDL hint to receive mouse click events on window focus, otherwise SDL doesn't emit the event.
    // Without this, when clicking to gain focus, our widgets wouldn't activate even though they showed as hovered.
    // (This is unfortunately a global SDL setting, so enabling it might have a side-effect on your application.
    // It is unlikely to make a difference, but if your app absolutely needs to ignore the initial on-focus click:
    // you can ignore SDL_EVENT_MOUSE_BUTTON_DOWN events coming right after a SDL_WINDOWEVENT_FOCUS_GAINED)
// #ifdef SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH
//     SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
// #endif

    // From 2.0.22: Disable auto-capture, this is preventing drag and drop across multiple windows (see #5710)
// #ifdef SDL_HINT_MOUSE_AUTO_CAPTURE
//     SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "0");
// #endif

    clock_getres( CLOCK_REALTIME, &bd->TimerResolution );

    // this will be wrong if TimerResolution is > 1 sec. That should never be the case though!
    bd->TimerFrequency = static_cast<double>(1000l * 1000l * 1000l) / bd->TimerResolution.tv_nsec;

    return true;
}

// bool ImGui_ImplSDL3_InitForOpenGL(SDL_Window* window, void* sdl_gl_context)
// {
//     IM_UNUSED(sdl_gl_context); // Viewport branch will need this.
//     return ImGui_ImplSDL3_Init(window, nullptr, sdl_gl_context);
// }

bool ImGui_ImplQt_InitForVulkan(QVulkanWindow* _window, QVulkanWindowRenderer *_renderer)
{
    return ImGui_ImplQt_Init(_window, _renderer);
}


void ImGui_ImplQt_Shutdown()
{
    ImGui_ImplQt_Data* bd = ImGui_ImplQt_GetBackendData();
    IM_ASSERT(bd != nullptr && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    // for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    //     SDL_DestroyCursor(bd->MouseCursors[cursor_n]);
    bd->LastMouseCursor = Qt::ArrowCursor;

    io.BackendPlatformName = nullptr;
    io.BackendPlatformUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_HasGamepad);
    IM_DELETE(bd);
}

static void ImGui_ImplQt_UpdateMouseData()
{
    ImGui_ImplQt_Data* bd = ImGui_ImplQt_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();

//     // We forward mouse input when hovered or captured (via SDL_EVENT_MOUSE_MOTION) or when focused (below)
// #if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE
//     // SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger other operations outside
//     SDL_CaptureMouse((bd->MouseButtonsDown != 0) ? SDL_TRUE : SDL_FALSE);
//     SDL_Window* focused_window = SDL_GetKeyboardFocus();
//     const bool is_app_focused = (bd->Window == focused_window);
// #else
//     SDL_Window* focused_window = bd->Window;
//     const bool is_app_focused = (SDL_GetWindowFlags(bd->Window) & SDL_WINDOW_INPUT_FOCUS) != 0; // SDL 2.0.3 and non-windowed systems: single-viewport only
// #endif
//     if (is_app_focused)
//     {
//         // (Optional) Set OS mouse position from Dear ImGui if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
//         if (io.WantSetMousePos)
//             SDL_WarpMouseInWindow(bd->Window, io.MousePos.x, io.MousePos.y);
//
//         // (Optional) Fallback to provide mouse position when focused (SDL_EVENT_MOUSE_MOTION already provides this when hovered or captured)
//         if (bd->MouseCanUseGlobalState && bd->MouseButtonsDown == 0)
//         {
//             // Single-viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
//             float mouse_x_global, mouse_y_global;
//             int window_x, window_y;
//             SDL_GetGlobalMouseState(&mouse_x_global, &mouse_y_global);
//             SDL_GetWindowPosition(focused_window, &window_x, &window_y);
//             io.AddMousePosEvent(mouse_x_global - window_x, mouse_y_global - window_y);
//         }
//     }
}

static void ImGui_ImplQt_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;
    ImGui_ImplQt_Data* bd = ImGui_ImplQt_GetBackendData();

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        bd->Window->setCursor(Qt::BlankCursor);
    }
    else
    {
        Qt::CursorShape expectedCursor = (bd->MouseCursors.contains(imgui_cursor)) ?
            expectedCursor = bd->MouseCursors.at(imgui_cursor) :
            expectedCursor = bd->MouseCursors.at(ImGuiMouseCursor_Arrow);

        // Show OS mouse cursor
        if (bd->LastMouseCursor != expectedCursor)
        {
            bd->Window->setCursor(expectedCursor);
            bd->LastMouseCursor = expectedCursor;
        }
    }
}

// static void ImGui_ImplQt_UpdateGamepads()
// {
//     ImGuiIO& io = ImGui::GetIO();
//     if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0) // FIXME: Technically feeding gamepad shouldn't depend on this now that they are regular inputs.
//         return;
//
//     // Get gamepad
//     io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
//     SDL_Gamepad* gamepad = SDL_OpenGamepad(0);
//     if (!gamepad)
//         return;
//     io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
//
//     // Update gamepad inputs
//     #define IM_SATURATE(V)                      (V < 0.0f ? 0.0f : V > 1.0f ? 1.0f : V)
//     #define MAP_BUTTON(KEY_NO, BUTTON_NO)       { io.AddKeyEvent(KEY_NO, SDL_GetGamepadButton(gamepad, BUTTON_NO) != 0); }
//     #define MAP_ANALOG(KEY_NO, AXIS_NO, V0, V1) { float vn = (float)(SDL_GetGamepadAxis(gamepad, AXIS_NO) - V0) / (float)(V1 - V0); vn = IM_SATURATE(vn); io.AddKeyAnalogEvent(KEY_NO, vn > 0.1f, vn); }
//     const int thumb_dead_zone = 8000;           // SDL_gamecontroller.h suggests using this value.
//     MAP_BUTTON(ImGuiKey_GamepadStart,           SDL_GAMEPAD_BUTTON_START);
//     MAP_BUTTON(ImGuiKey_GamepadBack,            SDL_GAMEPAD_BUTTON_BACK);
//     MAP_BUTTON(ImGuiKey_GamepadFaceLeft,        SDL_GAMEPAD_BUTTON_WEST);           // Xbox X, PS Square
//     MAP_BUTTON(ImGuiKey_GamepadFaceRight,       SDL_GAMEPAD_BUTTON_EAST);           // Xbox B, PS Circle
//     MAP_BUTTON(ImGuiKey_GamepadFaceUp,          SDL_GAMEPAD_BUTTON_NORTH);          // Xbox Y, PS Triangle
//     MAP_BUTTON(ImGuiKey_GamepadFaceDown,        SDL_GAMEPAD_BUTTON_SOUTH);          // Xbox A, PS Cross
//     MAP_BUTTON(ImGuiKey_GamepadDpadLeft,        SDL_GAMEPAD_BUTTON_DPAD_LEFT);
//     MAP_BUTTON(ImGuiKey_GamepadDpadRight,       SDL_GAMEPAD_BUTTON_DPAD_RIGHT);
//     MAP_BUTTON(ImGuiKey_GamepadDpadUp,          SDL_GAMEPAD_BUTTON_DPAD_UP);
//     MAP_BUTTON(ImGuiKey_GamepadDpadDown,        SDL_GAMEPAD_BUTTON_DPAD_DOWN);
//     MAP_BUTTON(ImGuiKey_GamepadL1,              SDL_GAMEPAD_BUTTON_LEFT_SHOULDER);
//     MAP_BUTTON(ImGuiKey_GamepadR1,              SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER);
//     MAP_ANALOG(ImGuiKey_GamepadL2,              SDL_GAMEPAD_AXIS_LEFT_TRIGGER,  0.0f, 32767);
//     MAP_ANALOG(ImGuiKey_GamepadR2,              SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, 0.0f, 32767);
//     MAP_BUTTON(ImGuiKey_GamepadL3,              SDL_GAMEPAD_BUTTON_LEFT_STICK);
//     MAP_BUTTON(ImGuiKey_GamepadR3,              SDL_GAMEPAD_BUTTON_RIGHT_STICK);
//     MAP_ANALOG(ImGuiKey_GamepadLStickLeft,      SDL_GAMEPAD_AXIS_LEFTX,  -thumb_dead_zone, -32768);
//     MAP_ANALOG(ImGuiKey_GamepadLStickRight,     SDL_GAMEPAD_AXIS_LEFTX,  +thumb_dead_zone, +32767);
//     MAP_ANALOG(ImGuiKey_GamepadLStickUp,        SDL_GAMEPAD_AXIS_LEFTY,  -thumb_dead_zone, -32768);
//     MAP_ANALOG(ImGuiKey_GamepadLStickDown,      SDL_GAMEPAD_AXIS_LEFTY,  +thumb_dead_zone, +32767);
//     MAP_ANALOG(ImGuiKey_GamepadRStickLeft,      SDL_GAMEPAD_AXIS_RIGHTX, -thumb_dead_zone, -32768);
//     MAP_ANALOG(ImGuiKey_GamepadRStickRight,     SDL_GAMEPAD_AXIS_RIGHTX, +thumb_dead_zone, +32767);
//     MAP_ANALOG(ImGuiKey_GamepadRStickUp,        SDL_GAMEPAD_AXIS_RIGHTY, -thumb_dead_zone, -32768);
//     MAP_ANALOG(ImGuiKey_GamepadRStickDown,      SDL_GAMEPAD_AXIS_RIGHTY, +thumb_dead_zone, +32767);
//     #undef MAP_BUTTON
//     #undef MAP_ANALOG
// }

void ImGui_ImplQt_NewFrame()
{
    ImGui_ImplQt_Data* bd = ImGui_ImplQt_GetBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplQt_Init()?");
    ImGuiIO& io = ImGui::GetIO();

    // Setup display size (every frame to accommodate for window resizing)
    QSize size = bd->Window->size();
    int display_w, display_h;

    if (bd->Window->windowState() & Qt::WindowMinimized)
    {
        size = { 0, 0 };
    }

    io.DisplaySize = ImVec2((float)size.width(), (float)size.height());
    if (size.width() > 0 && size.height() > 0)
    {
        // is this correct? didn't find any function for this in Qt6
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    }

    // Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
    // (Accept SDL_GetPerformanceCounter() not returning a monotonically increasing value. Happens in VMs and Emscripten, see #6189, #6114, #3644)

    struct timespec currentTime;
    clock_gettime ( CLOCK_REALTIME, &currentTime );

    if (currentTime.tv_sec <= bd->Time.tv_sec && currentTime.tv_nsec <= bd->Time.tv_nsec)
    {
        currentTime = bd->Time;
        currentTime.tv_nsec++;
        if(currentTime.tv_nsec == 1000l * 1000l * 1000l)
        {
            currentTime.tv_sec++;
            currentTime.tv_nsec = 0;
        }
    }

    struct timespec dTime;
    dTime.tv_sec = currentTime.tv_sec - bd->Time.tv_sec;
    dTime.tv_nsec = currentTime.tv_nsec - bd->Time.tv_nsec;
    if ( dTime.tv_nsec < 0 )
    {
        dTime.tv_sec--;
        dTime.tv_nsec+= 1000l * 1000l * 1000l;
    }

    long dTimeNSec = dTime.tv_sec * 1000l * 1000l * 1000l + dTime.tv_nsec;

    io.DeltaTime = bd->Time.tv_sec > 0 ? (float)((double)dTimeNSec / bd->TimerFrequency) : (float)(1.0f / 60.0f);
    bd->Time = currentTime;

    if (bd->PendingMouseLeaveFrame && bd->PendingMouseLeaveFrame >= ImGui::GetFrameCount() && bd->MouseButtonsDown == 0)
    {
        bd->MouseWindowID = 0;
        bd->PendingMouseLeaveFrame = 0;
        io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
    }

    ImGui_ImplQt_UpdateMouseData();
    ImGui_ImplQt_UpdateMouseCursor();

    // // Update game controllers (if enabled and available)
    // ImGui_ImplSDL3_UpdateGamepads();
}


#endif // #ifndef IMGUI_DISABLE

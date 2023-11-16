// dear imgui: wrappers for QString type

// Changelog:
// - v0.10: Initial version.

#include "imgui.h"
#include "imgui_qt.h"

struct InputTextCallback_UserData
{
    QString * const         qstr;
    ImGuiInputTextCallback  chainCallback;
    void*                   chainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData* const data)
{
    const InputTextCallback_UserData* const user_data = (InputTextCallback_UserData*)data->UserData;
    if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
    {
        // Resize string callback
        // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
        QString * const s = user_data->qstr;
        IM_ASSERT(data->Buf == s->toLocal8Bit().data());
        if (s->size() < data->BufTextLen)
        {
            s->resize(data->BufTextLen);
        } else if (s->size() > data->BufTextLen)
        {
            s->truncate(data->BufTextLen);
            s->squeeze();
        }
        data->Buf = static_cast<char*>(s->toLocal8Bit().data());
    }
    else if (user_data->chainCallback)
    {
        // Forward to user callback, if any
        data->UserData = user_data->chainCallbackUserData;
        return user_data->chainCallback(data);
    }
    return 0;
}

bool ImGui::InputText(const QString _label, QString _qstr, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data
    {
        .qstr = &_qstr,
        .chainCallback = callback,
        .chainCallbackUserData = user_data
    };

    return InputText(static_cast<char*>(_label.toLocal8Bit().data()),
                     static_cast<char*>(_qstr.toLocal8Bit().data()),
                     _qstr.capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextMultiline(const QString _label, QString _qstr, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data
    {
        .qstr = &_qstr,
        .chainCallback = callback,
        .chainCallbackUserData = user_data
    };

    return InputTextMultiline(static_cast<char*>(_label.toLocal8Bit().data()),
                              static_cast<char*>(_qstr.toLocal8Bit().data()),
                              _qstr.capacity() + 1, size, flags, InputTextCallback, &cb_user_data);
}

bool ImGui::InputTextWithHint(const QString _label, const QString _hint, QString _qstr, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
{
    IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
    flags |= ImGuiInputTextFlags_CallbackResize;

    InputTextCallback_UserData cb_user_data
    {
        .qstr = &_qstr,
        .chainCallback = callback,
        .chainCallbackUserData = user_data
    };

    return InputTextWithHint(static_cast<char*>(_label.toLocal8Bit().data()),
                             static_cast<char*>(_hint.toLocal8Bit().data()),
                             static_cast<char*>(_qstr.toLocal8Bit().data()),
                             _qstr.capacity() + 1, flags, InputTextCallback, &cb_user_data);
}

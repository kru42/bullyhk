#include "stdafx.h"
#include "lua_hook.h"

class ImGuiLuaEditor
{
private:
    std::string m_editorContent;
    char        m_editorBuffer[4096]; // Adjust size as needed
    bool        m_focusEditor;
    ImVec2      m_windowSize;
    ImVec2      m_editorSize;

public:
    ImGuiLuaEditor()
        : m_editorContent("-- Enter your Lua script here"), m_focusEditor(false),
          m_windowSize(400, 300), // Initial window size
          m_editorSize(0, 0)
    {
        strncpy(m_editorBuffer, m_editorContent.c_str(), sizeof(m_editorBuffer));
        m_editorBuffer[sizeof(m_editorBuffer) - 1] = '\0';
    }

    void Render()
    {
        ImGui::SetNextWindowSize(m_windowSize, ImGuiCond_FirstUseEver);
        ImGui::Begin("Lua Script Editor");

        // Store the new window size
        m_windowSize = ImGui::GetWindowSize();

        // Calculate the size for the editor
        m_editorSize = ImGui::GetContentRegionAvail();

        // Editor
        ImGui::Text("Edit your Lua script:");
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5));
        ImGui::InputTextMultiline("##LuaEditor", m_editorBuffer, sizeof(m_editorBuffer), m_editorSize,
                                  ImGuiInputTextFlags_AllowTabInput);
        ImGui::PopStyleVar();

        if (m_focusEditor)
        {
            ImGui::SetKeyboardFocusHere(-1);
            m_focusEditor = false;
        }

        // Execute button
        if (ImGui::Button("Execute Script"))
        {
            ExecuteScript();
        }

        ImGui::SameLine();

        // Clear button
        if (ImGui::Button("Clear"))
        {
            m_editorContent.clear();
            memset(m_editorBuffer, 0, sizeof(m_editorBuffer));
            m_focusEditor = true;
        }

        ImGui::End();
    }

private:
    void ExecuteScript()
    {
        // Update the content string with the buffer contents
        m_editorContent = m_editorBuffer;

        lhk::lua_exec_source(m_editorContent.c_str());
        // print lhk::game_lua_tostring output
        printf("lua_exec_source output: %s\n", lhk::game_lua_tostring(lhk::get_lua_state(), -1));
    }
};
// DevConsole.cpp
//

#include "DevConsole.h"

d3DevConsole console;

d3DevConsole::d3DevConsole() : HistoryPos(-1), AutoScroll(true), ScrollToBottom(false) {
  ClearLog();
  memset(InputBuf, 0, sizeof(InputBuf));
  isVisible = false;
}

d3DevConsole::~d3DevConsole() {
  ClearLog();
  for (int i = 0; i < History.Size; i++)
    free(History[i]);
}

void d3DevConsole::ClearLog() {
  for (int i = 0; i < Items.Size; i++)
    free(Items[i]);
  Items.clear();
}

void d3DevConsole::Printf(const char *fmt, ...) {
  // Create a buffer for formatted output
  char buf[1024];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  buf[sizeof(buf) - 1] = 0;
  va_end(args);
  Items.push_back(_strdup(buf));
}

void d3DevConsole::InsertText(const char *text) {
  // Insert text at current cursor position
  char *item = Items.empty() ? "" : Items.back();
  int cursor = strlen(item);
  int len = strlen(text);
  if (cursor + len >= IM_ARRAYSIZE(InputBuf))
    return; // Cannot insert text, buffer full
  memmove(InputBuf + cursor + len, InputBuf + cursor, strlen(InputBuf) - cursor);
  memcpy(InputBuf + cursor, text, len);
  InputBuf[cursor + len] = '\0';
}

void d3DevConsole::ExecCommand(const char *command_line) {
  Printf("# %s\n", command_line);

  // Insert command into history
  History.push_back(_strdup(command_line));
  HistoryPos = -1;

  // Execute command here (implementation depends on what commands are expected)
  if (strcmp(command_line, "CLEAR") == 0) {
    ClearLog();
  } else {
    // Example: command not recognized
    Printf("Unknown command: '%s'\n", command_line);
  }

  if (AutoScroll)
    ScrollToBottom = true;
}

void d3DevConsole::Draw(const char *title) {
  bool p_open;

  if (!isVisible)
  {
     return;
  }

   // Set the window size to 800x600 for a 4:3 aspect ratio and disable resizing
  ImGui::SetNextWindowSize(ImVec2(800, 600)); // Size enforced every frame
  if (!ImGui::Begin(title, &p_open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
    ImGui::End();
    return;
  }

  // Calculate the height available for the log (total window height - space for input and button)
  float logHeight = ImGui::GetWindowHeight() - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y -
                    ImGui::GetFrameHeightWithSpacing();

  // BeginChild for scrolling log with correct size
  ImGui::BeginChild("ScrollingRegion", ImVec2(0, logHeight-20.0f), false, ImGuiWindowFlags_HorizontalScrollbar);
  if (ImGui::BeginPopupContextWindow()) {
    if (ImGui::Selectable("Clear"))
      ClearLog();
    ImGui::EndPopup();
  }

  for (int i = 0; i < Items.Size; i++) {
    const char *item = Items[i];
    if (!Filter.PassFilter(item))
      continue;
    ImVec4 color;
    bool has_color = false;
    if (strstr(item, "[error]")) {
      color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
      has_color = true;
    }
    if (has_color)
      ImGui::PushStyleColor(ImGuiCol_Text, color);
    ImGui::TextUnformatted(item);
    if (has_color)
      ImGui::PopStyleColor();
  }

  ImGui::SetScrollY(ImGui::GetScrollMaxY()); 
  ImGui::EndChild();

  // Separator and input text box for command input with Execute button
  ImGui::Separator();
  if (ImGui::InputText("Input", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue, nullptr)) {
    if (InputBuf[0] != '\0') {
      ExecCommand(InputBuf);
    }
    strcpy(InputBuf, ""); // Clear input buffer after command execution
  }

  ImGui::SameLine();
  if (ImGui::Button("Execute")) {
    if (InputBuf[0] != '\0') {
      ExecCommand(InputBuf);
      strcpy(InputBuf, ""); // Clear input buffer after command execution
    }
  }

  ImGui::End();
}

int d3DevConsole::TextEditCallback(ImGuiInputTextCallbackData *data) {
  switch (data->EventFlag) {
  case ImGuiInputTextFlags_CallbackHistory:
    // Example of HISTORY: browsing through the command history
    if (data->EventKey == ImGuiKey_UpArrow) {
      // Up arrow pressed
      if (HistoryPos == -1)
        HistoryPos = History.Size - 1;
      else if (HistoryPos > 0)
        --HistoryPos;
      data->DeleteChars(0, data->BufTextLen);
      data->InsertChars(0, History[HistoryPos]);
    } else if (data->EventKey == ImGuiKey_DownArrow) {
      // Down arrow pressed
      if (HistoryPos != -1)
        if (++HistoryPos >= History.Size)
          HistoryPos = -1;
      data->DeleteChars(0, data->BufTextLen);
      if (HistoryPos != -1)
        data->InsertChars(0, History[HistoryPos]);
      else
        data->InsertChars(0, "");
    }
    break;
  }
  return 0; // Return 0 by default
}
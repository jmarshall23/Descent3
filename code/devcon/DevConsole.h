/*
* Descent 3
* Copyright (C) 2024 Parallax Software
*
* Descent 3: Apex
* Copyright (C) 2024 by Justin Marshall
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// DevConsole.h
//

#include <string>
#include <vector>
#include "../libimgui/imgui.h"

class d3DevConsole {
public:
  d3DevConsole();
  ~d3DevConsole();

  void ClearLog();
  void Printf(const char *fmt, ...) IM_FMTARGS(2);
  void Draw(const char *title);
  void ExecCommand(const char *command_line);
  void SetVisible(bool vis) { isVisible = vis; }
  bool IsVisible(void) { return isVisible; }
private:
  char InputBuf[256];
  ImVector<char *> Items;
  ImVector<const char *> Commands;
  ImVector<char *> History;
  int HistoryPos; // -1: new line, 0..History.Size-1 browsing history.
  ImGuiTextFilter Filter;
  bool AutoScroll;
  bool ScrollToBottom;
  bool isVisible;

  void InsertText(const char *text);
  int  TextEditCallback(ImGuiInputTextCallbackData *data);

  static int TextEditCallbackStub(ImGuiInputTextCallbackData *data) {
    d3DevConsole *console = reinterpret_cast<d3DevConsole *>(data->UserData);
    return console->TextEditCallback(data);
  }
};
extern d3DevConsole console;
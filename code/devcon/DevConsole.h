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
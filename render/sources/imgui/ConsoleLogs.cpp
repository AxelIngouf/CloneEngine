#include "ConsoleLogs.h"

#include "core/CLog.h"

using namespace ImGui;

ConsoleLogs ConsoleLogs::consoleLogs;

ConsoleLogs::ConsoleLogs()
{
    autoScroll = true;
    Clear();
    lineOffsets.push_back(0);
    SetLogCallback(ReceiveNewLog);
}

void ConsoleLogs::Clear()
{
    buffer.clear();
    lineOffsets.clear();
}

void ConsoleLogs::AddLog(Core::CLogMessage* message)
{
    if (message->message[message->message.size() - 1] != '\n')
        message->message.push_back('\n');

    const char* fmt = message->message.c_str();

    if (buffer.Buf.Data != nullptr && buffer.Buf.Size > 0)
    {
        if(buffer.Buf.Data[0] < 0)
        {
            return;
        }
    }

    int oldSize = buffer.size();
    buffer.append(fmt);
    for (const int newSize = buffer.size(); oldSize < newSize; oldSize++)
    {
        if (buffer[oldSize] == '\n')
        {
            lineOffsets.push_back(oldSize + 1);
            AddLogLevelColor(message->level);
            lineFile.push_back(message->filePath);
            lineFileLine.push_back(message->logLine);
        }
    }
}

void ConsoleLogs::AddLogLevelColor(const Core::ELogLevel& level)
{
    switch(level)
    {
    case Core::ELogLevel::CLOG_WARNING:
        lineColors.push_back(warningColor);
        break;
    case Core::ELogLevel::CLOG_ERROR:
        lineColors.push_back(errorColor);
        break;
    case Core::ELogLevel::CLOG_DEBUG:
        lineColors.push_back(debugColor);
        break;
    default:
        lineColors.push_back(defaultColor);
        break;
    }
}

void ConsoleLogs::Draw(const char* title, bool* p_open)
{
    if (!BeginTabItem(title, p_open))
    {
        EndTabItem();
        return;
    }

    if (BeginPopup("Options"))
    {
        Checkbox("Auto-scroll", &autoScroll);
        EndPopup();
    }

    if (Button("Options"))
        OpenPopup("Options");
    SameLine();
    const bool clear = Button("Clear");
    SameLine();
    const bool copy = Button("Copy");
    SameLine();
    filter.Draw("Filter", -100.0f);

    Separator();
    BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        Clear();
    if (copy)
        LogToClipboard();

    PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    const char* buf = buffer.begin();
    const char* bufEnd = buffer.end();
    if (filter.IsActive())
    {
        for (int iLine = 0; iLine < lineOffsets.Size; iLine++)
        {
            const char* lineStart = buf + lineOffsets[iLine];
            const char* lineEnd = iLine + 1 < lineOffsets.Size ? buf + lineOffsets[iLine + 1] - 1 : bufEnd;
            if (filter.PassFilter(lineStart, lineEnd))
            {
                if(iLine >= lineColors.size())
                {
                    TextUnformatted(lineStart, lineEnd);
                }
                else
                {
                    PushStyleColor(ImGuiCol_Text, lineColors[iLine]);
                    TextUnformatted(lineStart, lineEnd);
                    PopStyleColor();
                    if(IsItemHovered(0))
                    {
                        SetTooltip("\"%s\" [line:%d]", lineFile[iLine], lineFileLine[iLine]);
                    }
                }
            }
        }
    }
    else
    {
        ImGuiListClipper clipper;
        clipper.Begin(lineOffsets.Size);
        while (clipper.Step())
        {
            for (int iLine = clipper.DisplayStart; iLine < clipper.DisplayEnd; iLine++)
            {
                const char* lineStart = buf + lineOffsets[iLine];
                const char* lineEnd = iLine + 1 < lineOffsets.Size ? buf + lineOffsets[iLine + 1] - 1 : bufEnd;

                if (iLine >= lineColors.size())
                {
                    TextUnformatted(lineStart, lineEnd);
                }
                else
                {
                    PushStyleColor(ImGuiCol_Text, lineColors[iLine]);
                    TextUnformatted(lineStart, lineEnd);
                    PopStyleColor();

                    if (IsItemHovered(0))
                    {
                        PushStyleColor(ImGuiCol_Text, static_cast<ImU32>(ImColor(200, 150, 50)));
                        
                        SetTooltip("\"%s\" [line:%d]", lineFile[iLine], lineFileLine[iLine]);
                        PopStyleColor();
                    }
                }
            }
        }
        clipper.End();
    }
    PopStyleVar();

    if (autoScroll && GetScrollY() >= GetScrollMaxY())
        SetScrollHereY(1.0f);

    EndChild();
    EndTabItem();
}

void ReceiveNewLog(Core::CLogMessage* message)  
{
    if(message != nullptr)
        ConsoleLogs::consoleLogs.AddLog(message);
}

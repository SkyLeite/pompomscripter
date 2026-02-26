#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "PonscripterLabel.h"
#include <iostream>
#include <source_location>
#include <string_view>

class LogMessage {
    public:
        LogMessage(const loguru::Message &message);

        loguru::Verbosity   verbosity;   // Already part of preamble
        std::string filename;    // Already part of preamble
        unsigned    line;        // Already part of preamble
        std::string preamble;    // Date, time, uptime, thread, file:line, verbosity.
        std::string prefix;      // Assertion failure info goes here (or "").
        std::string message;     // User message goes here.

        std::string verbosity_str();
        std::string source();
};

class Debug {
    private:
        PonscripterLabel *ons;

        Debug(Debug const &) = delete;
        Debug &operator=(Debug const &) = delete;

        std::vector<LogMessage> messages;
        bool show_console = false;
        bool show_imgui_demo = false;
        bool show_inspector = true;

        AnimationInfo* selected_animation = nullptr;
        bool inspector_auto_scroll = true;
        bool inspector_enable_hover_preview = false;

    public:
        Debug() {};

        static Debug* instance_;
        static Debug* Instance();
        void Init(PonscripterLabel *ons);
        void Draw();
        void DrawConsole();
        void DrawInspector();
        void DrawImageButton(size_t i, AnimationInfo *si);

        void AddLog(LogMessage message);
};

#endif
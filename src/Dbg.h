#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "PonscripterLabel.h"
#include <iostream>
#include <source_location>
#include <string_view>

class Debug {
    private:
        PonscripterLabel *ons;

        Debug(Debug const &) = delete;
        Debug &operator=(Debug const &) = delete;

        std::vector<std::string> messages;
        bool show_console = false;

        bool show_inspector = true;

    public:
        Debug() {};

        static Debug* instance_;
        static Debug* Instance();
        void Init(PonscripterLabel *ons);
        void Draw();
        void DrawConsole();
        void DrawInspector();
        void AddLog(std::string *message);
};

#endif
#include "Dbg.h"

#include "PonscripterLabel.h"
#include <loguru.hpp>
#include <imgui.h>
#include <imgui_internal.h>
#include <format>

Debug* Debug::instance_ = NULL;

LogMessage::LogMessage(const loguru::Message& message) {
    auto filepath = std::string(message.filename);
    auto last_slash_idx = filepath.find_last_of('/');
    auto filename = filepath.substr(last_slash_idx, filepath.size() - 1);

    this->filename = filename;
    this->line = message.line;
    this->message = std::string(message.message);
    this->preamble = std::string(message.preamble);
    this->prefix = std::string(message.prefix);
    this->verbosity = message.verbosity;
}

void log_handler(void* user_data, const loguru::Message& message)
{
    auto log_message = LogMessage(message);
    Debug::Instance()->AddLog(log_message);
}

void Debug::Init(PonscripterLabel* ons) {
    this->ons = ons;
    loguru::add_callback("debug_handler", log_handler, NULL, loguru::Verbosity_MAX);
    instance_ = this;
}

Debug* Debug::Instance() {
    if (instance_ == nullptr) {
        LOG_F(INFO, "ERROR!!");
    }

    fflush(stderr);

    return instance_;
}

void Debug::Draw() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem("Console", NULL, &this->show_console);
            ImGui::MenuItem("Inspector", NULL, &this->show_inspector);
            ImGui::MenuItem("ImGui Demo", NULL, &this->show_imgui_demo);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (this->show_imgui_demo) {
        ImGui::ShowDemoWindow(&this->show_imgui_demo);
    }

    if (this->show_console) {
        this->DrawConsole();
    }

    if (this->show_inspector) {
        this->DrawInspector();
    }
}

void Debug::DrawConsole() {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Console", &this->show_console)) {
        ImGui::End();
        return;
    }

    ImGui::Checkbox("Auto scroll", &this->inspector_auto_scroll);

    auto table_flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_ScrollX | ImGuiTableFlags_Resizable;
    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        if (ImGui::BeginTable("log_table", 3, table_flags)) {
            ImGui::TableSetupColumn("Level");
            ImGui::TableSetupColumn("Source");
            ImGui::TableSetupColumn("Message");
            ImGui::TableSetupScrollFreeze(0, 1);

            ImGui::TableHeadersRow();

            ImGuiListClipper clipper;
            clipper.Begin(this->messages.size());
            if (this->messages.size() > 0) {
                clipper.IncludeItemByIndex(this->messages.size() - 1); // Always include last item so we can auto-scroll
            }
            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++ ) {
                    auto message = this->messages[row_n];
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, 10.0);

                    ImGui::TableSetColumnIndex(0);
                    ImGui::SetNextItemWidth(5.0);
                    ImGui::TextUnformatted(message.verbosity_str().c_str());

                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetNextItemWidth(20.0);
                    ImGui::TextUnformatted(message.source().c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextUnformatted(message.message.c_str());

                    // Scroll to last item
                    if (this->inspector_auto_scroll && this->messages.size() > 0 && row_n == this->messages.size() - 1) {
                        ImGui::SetScrollHereY(1.0f);
                    }
                }
            }
        }
        ImGui::EndTable();
    }

    ImGui::EndChild();
    ImGui::End();
}

void Debug::AddLog(LogMessage message) {
    this->messages.push_back(message);
}

std::string LogMessage::verbosity_str() {
    switch (this->verbosity) {
        case -9:
            return std::string("OFF");
        case -3:
            return std::string("FATAL");
        case -2:
            return std::string("ERROR");
        case -1:
            return std::string("WARNING");
        case 0:
            return std::string("INFO");
        default:
            return std::string("INVALID");
    }
}

std::string LogMessage::source() {
    return std::format("{}:{}", this->filename, this->line);
}

void renderAnimationImage(AnimationInfo* si) {
    if (si == nullptr || si->image_texture == nullptr) {
        return;
    }

    ImGui::Text("file_name: %s | image_name: %s", (const char *)si->file_name, (const char *)si->image_name);

    float width = (float)si->image_surface->w;
    float height = (float)si->image_surface->h;

    float biggest_size = std::max(width, height);

    float max_width = 100;
    float scale_factor = max_width / biggest_size;

    ImGui::Image(si->image_texture, ImVec2(width * scale_factor, height * scale_factor));
}

void Debug::DrawInspector() {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Inspector", &this->show_inspector)) {
        ImGui::End();
        return;
    }

    SDL_RenderClear(this->ons->renderer);
    for (size_t i = 0; i < MAX_SPRITE_NUM; i++)
    {
        auto si = &this->ons->sprite_info[i];
        renderAnimationImage(si);
    }

    for (size_t i = 0; i < MAX_SPRITE2_NUM; i++)
    {
        auto si = &this->ons->sprite2_info[i];
        renderAnimationImage(si);
    }

    for (size_t i = 0; i < 3; i++)
    {
        auto si = &this->ons->tachi_info[i];
        renderAnimationImage(si);
    }

    ImGui::End();
}
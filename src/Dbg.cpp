#include "Dbg.h"

#include "PonscripterLabel.h"
#include "imgui.h"

Debug* Debug::instance_ = NULL;

void Debug::Init(PonscripterLabel* ons) {
    this->ons = ons;
    instance_ = this;
}

Debug* Debug::Instance() {
    if (instance_ == nullptr) {
        fprintf(stderr, "ERROR!!\n");
    }

    fflush(stderr);

    return instance_;
}

void Debug::Draw() {
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

    if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar)) {
        for (size_t i = 0; i < this->messages.size(); i++)
        {
            auto message = this->messages[i];
            ImGui::TextUnformatted(message.c_str());
        }
    }

    ImGui::EndChild();
    ImGui::End();
}

void Debug::AddLog(std::string* message) {
    // std::string new_msg = *message;
    // this->messages.push_back(new_msg);
}

void Debug::DrawInspector() {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Inspector", &this->show_inspector)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Hello from inspector");

    SDL_RenderClear(this->ons->renderer);
    for (size_t i = 0; i < 500; i++)
    {
        auto si = &this->ons->sprite_info[i];

        if (si == nullptr || si->image_surface == nullptr) {
            continue;
        }

        ImGui::Text("file_name: %s | image_name: %s", (const char *)si->file_name, (const char *)si->image_name);
        ImGui::Image(si->image_texture, ImVec2((float)si->image_surface->w, (float)si->image_surface->h));
    }

    ImGui::End();
}
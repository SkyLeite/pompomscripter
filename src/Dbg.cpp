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
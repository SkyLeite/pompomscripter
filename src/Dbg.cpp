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

void renderAnimationImage(AnimationInfo* si, int max_width = 100) {
    if (si == nullptr || si->image_texture == nullptr) {
        return;
    }

    float width = (float)si->image_surface->w;
    float height = (float)si->image_surface->h;

    float biggest_size = std::max(width, height);

    float scale_factor = max_width / biggest_size;

    ImGui::Image(si->image_texture, ImVec2(width * scale_factor, height * scale_factor));
}

void Debug::DrawImageButton(size_t i, AnimationInfo* si) {
    auto button_sz = ImVec2(80.0, 80.0);

    if (si == nullptr || si->image_texture == nullptr) {
        return;
    }

    float width = (float)si->image_surface->w;
    float height = (float)si->image_surface->h;

    float biggest_size = std::max(width, height);

    float max_width = 60;
    float scale_factor = max_width / biggest_size;

    auto button_pos = ImGui::GetCursorScreenPos();
    ImGui::SetNextItemAllowOverlap();

    ImGui::PushID((const char*)si->file_name);
    if (ImGui::Button("", button_sz)) {
        this->selected_animation = si;
    }
    ImGui::PopID();

    if (ImGui::IsItemHovered() && this->inspector_enable_hover_preview) {
        ImGui::BeginTooltip();
        ImGui::Image(si->image_texture, ImVec2(width, height));
        ImGui::EndTooltip();
    }

    ImGui::SetCursorScreenPos(ImVec2(button_pos.x + 9, button_pos.y + 20));
    ImGui::Image(si->image_texture, ImVec2(width * scale_factor, height * scale_factor));

    ImGui::SetCursorScreenPos(ImVec2(button_pos.x + 2, button_pos.y + 2));
    ImGui::Text("%zu", i);

    ImGui::SetCursorScreenPos(ImVec2(button_pos.x, button_pos.y + button_sz.y));
    ImGui::Dummy(ImVec2(1.0, 2.0));
}

void Debug::DrawInspector() {
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);

    if (!ImGui::Begin("Inspector", &this->show_inspector, ImGuiWindowFlags_MenuBar)) {
        ImGui::End();
        return;
    }

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            ImGui::MenuItem("Preview on hover", NULL, &this->inspector_enable_hover_preview);
            ImGui::MenuItem("Highlight selected image", NULL, &this->inspector_enable_image_finder);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    auto style = ImGui::GetStyle();

    auto content = ImGui::GetContentRegionAvail();
    ImGui::BeginChild("Sprite1", ImVec2(110, content.y), ImGuiChildFlags_Borders, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

    ImGui::BeginMenuBar();
    ImGui::MenuItem("Images", NULL, false, false);
    ImGui::EndMenuBar();
    for (size_t i = 0; i < MAX_SPRITE_NUM; i++)
    {
        auto si = &this->ons->sprite_info[i];
        Debug::DrawImageButton(i, si);
    }

    for (size_t i = 0; i < MAX_SPRITE2_NUM; i++)
    {
        auto si = &this->ons->sprite2_info[i];
        Debug::DrawImageButton(i, si);
    }

    for (size_t i = 0; i < 3; i++)
    {
        auto si = &this->ons->tachi_info[i];
        Debug::DrawImageButton(i, si);
    }

    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("SelectedImage", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders, ImGuiWindowFlags_MenuBar);

    ImGui::BeginMenuBar();
    ImGui::MenuItem("Details", NULL, false, false);
    ImGui::EndMenuBar();

    if (this->selected_animation != nullptr) {
        auto si = this->selected_animation;
        ImGui::Text("Image name: %s", (const char*)si->image_name);
        ImGui::Text("File name: %s", (const char*)si->file_name);
        ImGui::Text("Mask file name: %s", (const char*)si->mask_file_name);
        ImGui::Text("Size: %d x %d", si->pos.w, si->pos.h);
        ImGui::Text("Position: %d x %d", si->pos.x, si->pos.y);
        ImGui::Text("Blending mode: %d", si->blending_mode);
        ImGui::Text("Color: R%dG%dB%d", si->color.r, si->color.g, si->color.b);
        ImGui::Text("Direction: %d", si->direction);
        ImGui::Text("Enable mode: %d", si->enablemode);
        renderAnimationImage(si, 500);
    }
    ImGui::EndChild();

    // Draw image finder
    if (this->selected_animation && this->inspector_enable_image_finder) {
        auto si = this->selected_animation;

        auto draw_list = ImGui::GetBackgroundDrawList();
        auto upper_left = ImVec2(si->pos.x, si->pos.y);
        auto lower_right = ImVec2(upper_left.x + si->pos.w, upper_left.y + si->pos.h);
        draw_list->AddRect(upper_left, lower_right, IM_COL32(0, 255, 0, 255));
    }

    ImGui::End();
}
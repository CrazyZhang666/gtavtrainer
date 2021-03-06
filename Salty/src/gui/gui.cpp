#include "common.hpp"
#include "gui.hpp"
#include "script.hpp"
#include "base_tab.h"
#include "misc.h"
#include "features.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include <script_global.hpp>
#include "math.h"
#include <time.h>
#include <sstream>
#include <../imgui/imgui.h>
#include <../imgui/imgui_internal.h>
#include <../imgui/examples/imgui_impl_dx9.h>
#include <../imgui/examples/imgui_impl_win32.h>
#include <renderer.hpp>
#include <logger.hpp>

using namespace std;

namespace big
{
	static int selected_tab = 0;
	static vector<string> tabs = { "Local", "Vehicle", "Online", "Settings" };
	static vector<int> blocked_controls = { 22, 23, 75, 145, 14, 15, 16, 17, 27, 99, 115, 199, 244, 245, 246, 247, 248, 1, 2, 3, 4, 5, 6, 24, 25, 68, 69, 70, 91, 92, 106, 114, 122, 135, 142, 144, 176, 177, 257, 329, 346, 157, 158, 159, 160, 161, 162, 163, 164, 165, 26, 79 };

	float width = 750;
	float height = 400;

	#pragma region Confiruation

	bool config::save(const string file_name)
	{
		const size_t class_size = sizeof(config);
		void* buffer = malloc(class_size);

		memcpy(buffer, &g_config, class_size);

		ofstream output(file_name, ios::binary);
		if (!output.is_open())
			return false;

		output.write(static_cast<const char*>(buffer), class_size);
		output.flush();
		output.close();

		free(buffer);
		buffer = NULL;
		return true;
	}

	bool config::load(const string file_name)
	{
		ifstream input(file_name, ios::binary);
		if (!input.is_open())
			return false;

		const size_t class_size = sizeof(config);
		void* buffer = malloc(class_size);
		input.read(static_cast<char*>(buffer), class_size);
		input.close();

		memcpy(&g_config, buffer, class_size);
		free(buffer);
		buffer = NULL;

		return true;
	}

#pragma endregion

	#pragma region Script initialization

	void gui::script_on_tick()
	{
		TRY_CLAUSE
		{
			if (g_gui.m_opened)
			{
				for (int i = 0; i < blocked_controls.size(); i++)
				{
					CONTROLS::DISABLE_CONTROL_ACTION(0, blocked_controls.at(i), 1);
					CONTROLS::DISABLE_CONTROL_ACTION(2, blocked_controls.at(i), 1);
				}
			}
		}
			EXCEPT_CLAUSE
	}

	void gui::script_func()
	{
		g_gui.script_init();
		while (true)
		{
			g_gui.script_on_tick();
			script::get_current()->yield();
		}
	}

	void gui::script_init() {}

#pragma endregion

	void gui::style_async()
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_Header] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(g_config.menucolor[0], g_config.menucolor[1], g_config.menucolor[2], 1.00f);
	}

	void gui::style()
	{
		auto& style = ImGui::GetStyle();
		auto& colors = style.Colors;

		style.WindowRounding = 0.1f;
		style.TabRounding = 0.f;
		style.ChildRounding = 0.0f;

		colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.74f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
	}

	void gui::draw() 
	{
		ImGui::SetNextWindowSize(ImVec2{ 750, 400 }, ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2{ 100, 200 }, ImGuiCond_Once);

		if (ImGui::Begin("", &g_gui.m_opened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar))
		{
			ImGui::Columns(2);
			{
				ImGui::SetColumnWidth(0, 100.f);

				ImGui::BeginChild("##col 1", ImVec2(0.0f, 384), true);
				{
					for (size_t i = 0; i < tabs.size(); i++)
					{
						if (ImGui::Selectable(tabs.at(i).c_str(), selected_tab == i))
							selected_tab = i;
					}
				}
				ImGui::EndChild();

				ImGui::NextColumn();

				ImGui::BeginChild("##col 2", ImVec2(0.0f, 384), true);
				{
					switch (selected_tab)
					{
					case 0:
						base_tab::render_local_tab();
						break;
					case 1:
						base_tab::render_vehicle_tab();
						break;
					case 2:
						base_tab::render_online_tab();
						break;
					case 3:
						base_tab::render_settings_tab();
						break;
					}
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}
	}

	void gui::dx_on_tick()
	{
		style_async();
		draw();
	}

	void gui::dx_init() { style(); }
}

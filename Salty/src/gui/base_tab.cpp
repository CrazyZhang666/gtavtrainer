#include "common.hpp"
#include "base_tab.h"
#include "script.hpp"
#include "gui.hpp"
#include "misc.h"
#include "features.hpp"
#include "natives.hpp"
#include "pointers.hpp"
#include <script_global.hpp>
#include "math.h"
#include <time.h>
#include <sstream>
#include <renderer.hpp>

// https://github.com/Sainan/GTA-V-Decompiled-Scripts
// https://github.com/Pocakking/BigBaseV2
// https://bitbucket.org/gir489/bigbasev2-fix/src/master/
// https://wiki.rage.mp/index.php?title=Category:Assets
// https://github.com/MAFINS/MenyooSP
// https://www.reddit.com/r/Gta5Modding/comments/h17xia/list_of_gta_v_paid_mod_menus_for_pc/

using namespace std;
using namespace fmt;
using namespace rage;

namespace big::base_tab
{
	static const int DISTANCE_SPAWN = 10;

	const bool LOG_LOG = false;

#pragma region Move to IMGUI
	//move to imgui
	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			// Resize string callback
			string* str = (string*)data->UserData;
			IM_ASSERT(data->Buf == str->c_str());
			str->resize(data->BufTextLen);
			data->Buf = (char*)str->c_str();
		}
		return 0;
	}

	bool InputText(const char* label, string* str, ImGuiInputTextFlags flags)
	{
		flags |= ImGuiInputTextFlags_CallbackResize;
		return ImGui::InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, (void*)str);
	}

	bool VectorGetter(void* vec, int idx, const char** out_text)
	{
		auto& vector = *static_cast<std::vector<string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool Combo(const char* label, int* item, vector<string> strs)
	{
		ImGui::Combo(label, item, VectorGetter, static_cast<void*>(&strs), strs.size());
		return false;
	}
	///
#pragma endregion

	void render_local_tab()
	{
		if (ImGui::BeginTabBar("##local tab", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Player"))
			{
				ImGui::Checkbox("Godmode", &g_config.localped.godmode);
				ImGui::Checkbox("Ragdoll prevention", &g_config.localped.ragdoll);
				ImGui::Checkbox("Fast run", &g_config.localped.fastrun);

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.0f, 0.2f, 1.f));
				ImGui::Checkbox("No wanted level", &g_config.localped.neverwanted);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("This feature is only partially working as of now");
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .0f, .0f, 1.f));
				ImGui::Checkbox("Off radar", &g_config.localped.offradar);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("This feature is not working as of now");
				ImGui::PopStyleColor();

				ImGui::Checkbox("No mental state", &g_config.localped.nomental);

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.0f, 0.2f, 1.f));
				ImGui::Checkbox("No phone", &g_config.localped.nophone);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("This feature is only partially working as of now");
				ImGui::PopStyleColor();

				ImGui::Checkbox("Ped invisibility", &g_config.localped.invisible);
				ImGui::Checkbox("Noclip", &g_config.localped.noclip);

				if (ImGui::Button("Teleport to waypoint"))
					g_config.localped.teleportwp = true;

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Weapon"))
			{
				ImGui::Checkbox("Infinite ammo", &g_config.weapons.infammo);
				ImGui::Checkbox("Insta kill", &g_config.weapons.instakill);
				ImGui::Checkbox("Triggerbot", &g_config.weapons.triggerbot);

				if (ImGui::Button("Give all weapons"))
					g_config.weapons.addweapons = true;

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}

	void render_vehicle_tab()
	{
		if (ImGui::BeginTabBar("##vehicle tab", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Main"))
			{
				ImGui::Checkbox("Indestructible vehicle", &g_config.vehicle.godmode);
				ImGui::Checkbox("Clean vehicle", &g_config.vehicle.autorepair);
				ImGui::Checkbox("Seatbelt", &g_config.vehicle.seatbelt);
				ImGui::Checkbox("Horn boost", &g_config.vehicle.hornboost);
				ImGui::Checkbox("Vehicle invisibility", &g_config.vehicle.invisible);
				ImGui::Checkbox("Perfect handling", &g_config.vehicle.phandling);

				if (ImGui::Button("Upgrade current vehicle"))
					g_config.vehicle.upgrade = true;

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Spawner"))
			{
				ImGui::Separator();

				ImGui::ColorEdit3("Vehicle color", g_config.Vfeatures_vcol, ImGuiColorEditFlags_AlphaBar);

				static string currentModelSearch = "";
				static const char* currentModel = NULL;
				if (ImGui::BeginCombo("Vehicles", currentModel))
				{
					InputText("Search", &currentModelSearch, 0);

					for (int n = 0; n < IM_ARRAYSIZE(features::vehicleModels); n++) //update and add new vehicle hashes for LOS SANTOS TUNERS (WIP)
					{
						string str = features::vehicleModels[n], search = currentModelSearch;
						transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return tolower(c); });
						transform(search.begin(), search.end(), search.begin(), [](unsigned char c) { return tolower(c); });
						bool searchFound = str.find(search) != string::npos;

						if (currentModelSearch.size() > 0 && !searchFound)
							continue;

						if (ImGui::Selectable(features::vehicleModels[n]))
						{
							currentModel = features::vehicleModels[n];
							features::carToSpawn = currentModel;
						}
					}
					ImGui::EndCombo();
				}

				InputText("Custom vehicle", &features::carToSpawn, 0);

				ImGui::Checkbox("Spawn indestructible", &g_config.vehicle.spawnggodmode);
				ImGui::Checkbox("Spawn fully upgraded", &g_config.vehicle.spawnupgraded);
				ImGui::Checkbox("Spawn in vehicle", &g_config.vehicle.spawninvehicle);
				ImGui::Checkbox("Spawn with random colors", &g_config.vehicle.randomizecolor);

				if (ImGui::Button("Import model"))
					g_config.vehicle.requestentity = true;

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Attributes"))
			{
				if (features::localCPed && features::lastVehicle)
				{
					ImGui::SliderFloat("Mass", &features::lastVehicleHandling->fMass, 0.f, 10000.f);
					ImGui::SliderFloat("Acceleration", &features::lastVehicle->acceleration, 0.f, 10.f);
					ImGui::SliderFloat("Brake force", &features::lastVehicleHandling->fBrakeForce, 0.f, 10.f);
					ImGui::SliderFloat("Hand brake force", &features::lastVehicleHandling->fHandBrakeForce, 0.f, 10.f);
					ImGui::SliderFloat("Steering lock", &features::lastVehicleHandling->fSteeringLock, 0.f, 1.f);
					ImGui::SliderFloat("Traction", &features::lastVehicleHandling->m_fAcceleration, 0.f, 1.f);
					ImGui::SliderFloat("Traction curve minimum", &features::lastVehicleHandling->fTractionCurveMin, 0.f, 25.f);
					ImGui::SliderFloat("Traction curve maximum", &features::lastVehicleHandling->fTractionCurveMax, 0.f, 25.f);
					ImGui::SliderFloat("Up shift", &features::lastVehicleHandling->fClutchChangeRateScaleUpShift, 0, 10.f);
					ImGui::SliderFloat("Down shift", &features::lastVehicleHandling->fClutchChangeRateScaleDownShift, 0, 10.f);
					ImGui::SliderFloat("Suspension force", &features::lastVehicleHandling->fSuspensionForce, 0, 10.f);
					ImGui::SliderFloat("Suspension lower limit", &features::lastVehicleHandling->fSuspensionLowerLimit, -1.f, 1.f);
					ImGui::SliderFloat("Suspension upper limit", &features::lastVehicleHandling->fSuspensionUpperLimit, -1.f, 1.f);
					ImGui::SliderFloat("Suspension dampening", &features::lastVehicleHandling->funknown, 0, 10.f);
					ImGui::SliderFloat("Suspension rebound dampening", &features::lastVehicleHandling->fSuspensionReboundDamp, 0, 10.f);
					ImGui::SliderFloat("Grip", &features::lastVehicleHandling->m_fGrip, 0, 10.f);
					ImGui::SliderFloat("Ride height", &features::lastVehicle->rideHeight1, -1.f, 1.f);
					ImGui::SliderFloat("Gravity", &features::lastVehicle->gravity, -30.f, 30.f);
					ImGui::SliderFloat("Speed limit", &features::lastVehicle->speedLimit, -1.f, 500.f);
					ImGui::SliderFloat("Anti roll bar force", &features::lastVehicleHandling->fAntiRollBarForce, 0.f, 10.f);
					ImGui::SliderFloat("Drive inertia", &features::lastVehicleHandling->fDriveInertia, 0.f, 5.f);
					ImGui::SliderFloat("Drive bias", &features::lastVehicleHandling->nDriveBias, 0.f, 5.f);
					ImGui::SliderFloat("Traction loss multiplier", &features::lastVehicleHandling->fLowSpeedTractionLostMult, 0.f, 1.f);
					ImGui::SliderFloat("Initial drive force", &features::lastVehicleHandling->fInitialDriveForce, 0.f, 1.f);
					if (features::lastVehicleHandling->bikeHandlingData && (*features::lastVehicleHandling->bikeHandlingData))
						ImGui::SliderFloat("Bike jump height", &(*features::lastVehicleHandling->bikeHandlingData)->jumpHeight, 0.f, 50.f);
				}
				else
					ImGui::Text("You must enter a vehicle in order to modify its attributes");
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void render_online_tab()
	{
		if (ImGui::BeginTabBar("##online tabs", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Protection"))
			{
				ImGui::Checkbox("Game event protection", &features::features_gameeventprotection);
				ImGui::Checkbox("Crash protection", &features::protection);
				ImGui::Checkbox("Kick protection", &features::features_kickprotection);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.0f, 0.2f, 1.f));
				{
					ImGui::Checkbox("Event protection", &features::features_maleventprotection);
					if (ImGui::IsItemHovered())
						ImGui::SetTooltip("Experimental. This will trigger false positives and may cause issues with the game.");
				}
				ImGui::PopStyleColor();

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("ESP")) //todo: fix esp alignment
			{
				ImGui::ColorEdit3("ESP Color", g_config.ESPfeatures_color, ImGuiColorEditFlags_AlphaBar);
				ImGui::Checkbox("Name", &g_config.esp.name);
				ImGui::Checkbox("Box", &g_config.esp.box);
				ImGui::Checkbox("Snapline", &g_config.esp.snapline);
				ImGui::Checkbox("Skeleton", &g_config.esp.skeleton);
				ImGui::Checkbox("Marker", &g_config.esp.marker);

				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .0f, .0f, 1.f));
				ImGui::Checkbox("Display distance", &g_config.esp.distance);
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("This feature is not working as of now");
				ImGui::PopStyleColor();
		
				ImGui::Checkbox("Health based colors", &g_config.esp.health);
				ImGui::Checkbox("Visible only", &g_config.esp.visible);
				ImGui::SliderFloat("Render distance", &g_config.esp.render_distance, 100.f, 10000.f);

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Players"))
			{
				if (*g_pointers->m_is_session_started)
				{
					ImGui::Columns(2);
					{
						ImGui::BeginChild("##col 3", ImVec2(0.0f, 340), false);
						{
							static string currentPlayerSearch = "";
							ImGui::PushItemWidth(215);
							InputText("##Search##players", &currentPlayerSearch, 0);
							ImGui::PopItemWidth();
							for (int i = 0; i < 32; i++)
							{
								string name = features::players[i].name; name += " ##"; name += to_string(i).c_str();
								string str = features::players[i].name, search = currentPlayerSearch;
								transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return tolower(c); });
								transform(search.begin(), search.end(), search.begin(), [](unsigned char c) { return tolower(c); });
								bool searchFound = str.find(search) != string::npos;

								if (!features::players[i].ped)
									continue;

								if (!features::players[features::selectedPlayer].name)
								{
									ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "NAME IS NULLPTR");
									continue;
								}

								if (currentPlayerSearch.size() > 0 && !searchFound)
									continue;

								if (ImGui::Selectable(name.c_str(), features::selectedPlayer == i))
									features::selectedPlayer = i;

								ImGui::SameLine();
								ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), format("{}", features::players[i].index).c_str());

								if (features::players[i].index == features::localIndex)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "LOCAL");
								}
								if (features::players[i].isfriend)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(0.1f, 1.f, 0.1f, 1.f), "FRIEND");
								}
								if (features::players[i].isSessionHost)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.1f, 1.f, 0.1f, 1.f), "HOST");
								}
								if (features::players[i].invehicle > 0)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "[Driving]");
								}
								if (features::players[i].interior > 0)
								{
									ImGui::SameLine();
									ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "[Inside]");
								}

								if (g_config.debug.enabled)
								{
									if (features::scriptHost == i)
									{
										ImGui::SameLine();
										ImGui::TextColored(ImVec4(0.f, 0.7f, 1.f, 1.f), "SCRIPT HOST");
									}
								}
							}
						}
						ImGui::EndChild();

						ImGui::NextColumn();

						ImGui::BeginChild("##col 4", ImVec2(0.0f, 340), false);
						{
							if (features::selectedPlayer > -1)
							{
								ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "WARNING: INFO MAY BE INACCURATE");

								if (features::players[features::selectedPlayer].name)
									ImGui::TextColored(ImVec4(0.1f, 1.0f, 1.0f, 1.f), features::players[features::selectedPlayer].name);
								else
									ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "NAME IS NULLPTR");

								ImGui::Text("Health:");

								int maxHealth = features::players[features::selectedPlayer].maxHealth - 100;
								int health = clamp(features::players[features::selectedPlayer].health - 100, 0, maxHealth);
								Vector3 healthColor = features::FromHSB(clamp((float)(health) / (float)(maxHealth * 3.6f), 0.f, 0.277777777778f), 1.f, 1.f);

								if (features::players[features::selectedPlayer].invincible) { healthColor.x = 255;	healthColor.y = 255;	healthColor.z = 255; }

								ImGui::SameLine();

								ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(healthColor.x / 255.f, healthColor.y / 255.f, healthColor.z / 255.f, 1.f));
								ImGui::Text(format("{}/{}", features::players[features::selectedPlayer].health, features::players[features::selectedPlayer].maxHealth).c_str());
								ImGui::PopStyleColor();

								ImGui::PushItemWidth(150);

								if (features::players[features::selectedPlayer].player && features::players[features::selectedPlayer].info)
								{
									BYTE* IP = reinterpret_cast<BYTE*>(&features::players[features::selectedPlayer].info->externalIP);
									if (IP)
									{
										string IPString = fmt::format("{}.{}.{}.{}:{}", *(IP + 3), *(IP + 2), *(IP + 1), *IP, features::players[features::selectedPlayer].info->externalPort);
										InputText("IP", &IPString, 0);
									}
									else
										ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "IP is nullptr");
								}

								string rockstarId = features::players[features::selectedPlayer].rockstarId;
								InputText("RSID", &rockstarId, 0);
								ImGui::PopItemWidth();

								if (ImGui::Button("Teleport to player"))
									g_config.players.teleport = true;

								/*if (ImGui::Button("Kick from vehicle"))
									g_config.players.kickfromvehicle = true;*/

								//kicks and crashes dont work anymore ):

								/*ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.0f, 0.2f, 1.f));
								{
									if (ImGui::Button("Kick from lobby"))
										g_config.Pfeatures_kick = true;
								}
								ImGui::PopStyleColor();

								ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, .0f, .0f, 1.f));
								{
									if (ImGui::Button("Crash player"))
										g_config.Pfeatures_crash = true;
								}
								ImGui::PopStyleColor();*/

								if (features::players[features::selectedPlayer].player && g_config.debug.enabled)
								{
									ImGui::Separator();
									string debug = fmt::format("CNetGamePlayer: {}", reinterpret_cast<void*>(features::players[features::selectedPlayer].player));
									InputText("##debug 1", &debug, 0);

									debug = fmt::format("CPlayerInfo: {}", reinterpret_cast<void*>(features::players[features::selectedPlayer].info));
									InputText("##debug 2", &debug, 0);

									debug = fmt::format("netData: {}", reinterpret_cast<void*>(features::players[features::selectedPlayer].netData));
									InputText("##debug 3", &debug, 0);

									if (features::players[features::selectedPlayer].info)
									{
										debug = fmt::format("CPed: {}", reinterpret_cast<void*>(features::players[features::selectedPlayer].info->ped));
										InputText("##debug 4", &debug, 0);
									}
								}
							}
						}
						ImGui::EndChild();
					}
				}
				else
				{
					ImGui::Text("You must be in a session in order to use the playerlist");
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
	}

	static int selectedConfig = 0;
	void render_settings_tab()
	{
		auto& style = ImGui::GetStyle();

		if (ImGui::BeginTabBar("##config tab", ImGuiTabBarFlags_FittingPolicyScroll | ImGuiTabBarFlags_NoTooltip))
		{
			if (ImGui::BeginTabItem("Config"))
			{
				ImGui::ColorEdit3("Menu color", g_config.menucolor, ImGuiColorEditFlags_AlphaBar);

				vector<string> configs = { "Alpha", "Bravo", "Delta", "Echo", "Foxtrot", "Golf", "Hotel", "India" };
				Combo("Configs", &selectedConfig, configs);

				if (ImGui::Button("Save"))
					g_config.save(configs.at(selectedConfig));

				ImGui::SameLine();

				if (ImGui::Button("Load"))
					g_config.load(configs.at(selectedConfig));

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc"))
			{
				ImGui::Checkbox("Enable debug mode", &g_config.debug.enabled);
				if (ImGui::Button("Unload"))
				{
					exit(0);
					g_running = false;
				}
				ImGui::EndTabItem();
			}

			if (g_config.debug.enabled && *g_pointers->m_is_session_started)
			{
				if (ImGui::BeginTabItem("Debug"))
				{
					InputText("localIndex", &format("{}", features::localIndex), 0);
					InputText("localPed", &format("{}", (int)features::localPed), 0);
					InputText("local", &format("{}", reinterpret_cast<void*>(features::local)), 0);
					InputText("localInfo", &format("{}", reinterpret_cast<void*>(features::localInfo)), 0);
					InputText("localCPed", &format("{}", reinterpret_cast<void*>(features::localCPed)), 0);
					InputText("lastVehicle", &format("{}", reinterpret_cast<void*>(features::lastVehicle)), 0);
					InputText("lastVehicleHandling", &format("{}", reinterpret_cast<void*>(features::lastVehicleHandling)), 0);

					ImGui::Text(fmt::format("numberOfPlayers: {}", features::numberOfPlayers).c_str());
					ImGui::Text(fmt::format("sync: {}", features::sync).c_str());
					ImGui::Text(fmt::format("network: {}", features::network).c_str());
					ImGui::Text(fmt::format("script: {}", features::script).c_str());
					ImGui::Text(fmt::format("script2: {}", features::script2).c_str());

					ImGui::SliderInt("In game log length", &g_config.debug.log_limit, 0, 100);

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
}

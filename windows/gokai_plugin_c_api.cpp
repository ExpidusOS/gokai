#include "include/gokai/gokai_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "gokai_plugin.h"

void GokaiPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  gokai::GokaiPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}

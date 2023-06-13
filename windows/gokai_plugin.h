#ifndef FLUTTER_PLUGIN_GOKAI_PLUGIN_H_
#define FLUTTER_PLUGIN_GOKAI_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>

namespace gokai {

class GokaiPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  GokaiPlugin();

  virtual ~GokaiPlugin();

  // Disallow copy and assign.
  GokaiPlugin(const GokaiPlugin&) = delete;
  GokaiPlugin& operator=(const GokaiPlugin&) = delete;

  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace gokai

#endif  // FLUTTER_PLUGIN_GOKAI_PLUGIN_H_

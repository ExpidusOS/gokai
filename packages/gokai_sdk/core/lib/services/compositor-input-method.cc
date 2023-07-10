#include <gokai/services/compositor-input-method.h>
#include <gokai/services/engine-manager.h>

#define TAG "Gokai::Services::CompositorInputMethod"

using namespace Gokai::Services;

CompositorInputMethod::CompositorInputMethod(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string("flutter/textinput") },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::vector<uint8_t> message) {
    auto call = this->method_codec.decodeMethodCall(message);
    this->logger->debug("{}", std::string(message.begin(), message.end()));
    if (call.method.compare("TextInput.show") == 0) {
      try {
        this->showInput();
        return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
      } catch (const std::exception& ex) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Got exception: {}", ex.what()), std::make_any<void*>(nullptr));
      }
    }

    if (call.method.compare("TextInput.hide") == 0) {
      try {
        this->hideInput();
        return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
      } catch (const std::exception& ex) {
        return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Got exception: {}", ex.what()), std::make_any<void*>(nullptr));
      }
    }

    if (call.method.compare("TextInput.setEditableSizeAndTransform") == 0) {
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.setMarkedTextRect") == 0) {
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.setStyle") == 0) {
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.setClient") == 0) {
      auto args = std::any_cast<std::list<std::any>>(call.arguments);
      if (!args.empty()) {
        auto first = std::any_cast<std::map<std::string, std::any>>(args.back());
        auto input_type = std::any_cast<std::map<std::string, std::any>>(first["inputType"]);

        this->client_id = std::any_cast<int>(args.front());
        this->input_type = std::any_cast<std::string>(input_type["name"]);
        this->input_action = std::any_cast<std::string>(first["inputAction"]);
      }
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.setCaretRect") == 0) {
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.clearClient") == 0) {
      this->model = Gokai::Flutter::TextInputModel();
      this->input_type = std::string();
      this->input_action = std::string();
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.requestAutofill") == 0) {
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.setEditingState") == 0) {
      auto args = std::any_cast<std::map<std::string, std::any>>(call.arguments);

      int base = std::any_cast<int>(args["selectionBase"]);
      int extent = std::any_cast<int>(args["selectionExtent"]);

      this->model.setText(std::any_cast<std::string>(args["text"]));
      this->model.setSelection(Gokai::Flutter::TextRange(static_cast<size_t>(base), static_cast<size_t>(extent)));
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }
    return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
  });
}

std::shared_ptr<Gokai::ServiceChannel> CompositorInputMethod::getServiceChannel() {
  return this->service_channel;
}

bool CompositorInputMethod::sendStateUpdate(xg::Guid engine_id) {
  auto engine_manager = reinterpret_cast<EngineManager*>(this->context->getSystemService(EngineManager::SERVICE_NAME));
  auto engine = engine_manager->get(engine_id);
  if (engine == nullptr) return false;

  auto value_map = std::map<std::string, std::any>();
  auto selection = this->model.getSelection();
  value_map["composingBase"] = -1;
  value_map["composingExtent"] = -1;
  value_map["selectionAffinity"] = "TextAffinity.downstream";
  value_map["selectionBase"] = selection.getBase();
  value_map["selectionExtent"] = selection.getExtent();
  value_map["selectionIsDirectional"] = false;
  value_map["text"] = strdup(this->model.getText().c_str());

  std::list<std::any> args;
  args.push_back(this->client_id);
  args.push_back(value_map);

  auto value = this->method_codec.encodeMethodCall(Gokai::Flutter::MethodCall("TextInputClient.updateEditingState", args));
  this->logger->debug("{}", std::string(value.begin(), value.end()));
  engine->send("flutter/textinput", value);
  return true;
}

bool CompositorInputMethod::performActionMethod(xg::Guid engine_id) {
  if (!this->input_action.empty()) {
    auto engine_manager = reinterpret_cast<EngineManager*>(this->context->getSystemService(EngineManager::SERVICE_NAME));
    auto engine = engine_manager->get(engine_id);
    if (engine == nullptr) return false;

    auto args = std::list<std::any>();
    args.push_back(this->client_id);
    args.push_back(this->input_action);

    auto value = this->method_codec.encodeMethodCall(Gokai::Flutter::MethodCall("TextInputClient.performAction", args));
    engine->send("flutter/textinput", value);
    return true;
  }
  return false;
}

void CompositorInputMethod::showInput() {}

void CompositorInputMethod::hideInput() {}

bool CompositorInputMethod::isActive() {
  return this->is_active;
}

std::string CompositorInputMethod::getInputType() {
  return this->input_type;
}

const std::string CompositorInputMethod::SERVICE_NAME = "CompositorInputMethod";

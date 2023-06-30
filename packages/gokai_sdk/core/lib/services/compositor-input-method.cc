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

    if (call.method.compare("TextInput.setCaretRect") == 0) {
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.clearClient") == 0) {
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.requestAutofill") == 0) {
      return this->method_codec.encodeSuccessEnvelope(std::make_any<void*>(nullptr));
    }

    if (call.method.compare("TextInput.setEditingState") == 0) {
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

  auto args = std::map<std::string, std::any>();
  auto selection = this->model.getSelection();
  args["composingBase"] = -1;
  args["composingExtent"] = -1;
  args["selectionAffinity"] = "TextAffinity.downstream";
  args["selectionBase"] = selection.getBase();
  args["selectionExtent"] = selection.getExtent();
  args["selectionIsDirectional"] = false;
  args["text"] = strdup(this->model.getText().c_str());

  auto value = this->method_codec.encodeMethodCall(Gokai::Flutter::MethodCall("TextInputClient.updateEditingState", args));
  this->logger->debug("{}", std::string(value.begin(), value.end()));
  engine->send("flutter/textinput", value);
  return true;
}

void CompositorInputMethod::showInput() {}

void CompositorInputMethod::hideInput() {}

bool CompositorInputMethod::isActive() {
  return this->is_active;
}

const std::string CompositorInputMethod::SERVICE_NAME = "CompositorInputMethod";
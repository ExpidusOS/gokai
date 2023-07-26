#include <gokai/services/engine-manager.h>
#include <gokai/services/window-manager.h>
#include <gokai/casting.h>

#define TAG "Gokai::Services::WindowManager"

using namespace Gokai::Services;

WindowManager::WindowManager(Gokai::ObjectArguments arguments) : Service(arguments), Loggable(TAG, arguments), method_codec(arguments) {
  this->logger->debug("Service created");

  this->service_channel = std::shared_ptr<Gokai::ServiceChannel>(new Gokai::ServiceChannel(Gokai::ObjectArguments({
    { "context", this->context },
    { "logger", this->getLogger() },
    { "name", std::string(TAG) },
  })));

  this->service_channel->onReceive.push_back([this](xg::Guid engine_id, std::string channel, std::vector<uint8_t> message) {
    return std::async(std::launch::async, [this, engine_id, channel, message] {
      auto call = this->method_codec.decodeMethodCall(message);

      if (call.method.compare("getIds") == 0) {
        auto ids = this->getIds();
        std::list<std::any> list;
        for (const auto& id : ids) list.push_back(id.str());
        return this->method_codec.encodeSuccessEnvelope(list);
      }

      if (call.method.compare("isToplevel") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        return this->method_codec.encodeSuccessEnvelope(win->isToplevel());
      }


      if (call.method.compare("isActive") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        return this->method_codec.encodeSuccessEnvelope(win->isActive());
      }

      if (call.method.compare("setActive") == 0) {
        auto map = std::any_cast<std::map<std::string, std::any>>(call.arguments);
        auto id = xg::Guid(std::any_cast<std::string>(map["id"]));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        auto value = std::any_cast<bool>(map["value"]);
        win->setActive(value);
        return this->method_codec.encodeSuccessEnvelope(nullptr);
      }

      if (call.method.compare("isMapped") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        return this->method_codec.encodeSuccessEnvelope(win->isMapped());
      }

      if (call.method.compare("getRole") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        auto role = win->getRole();
        if (role.empty()) {
          return this->method_codec.encodeSuccessEnvelope(nullptr);
        }
        return this->method_codec.encodeSuccessEnvelope(role);
      }

      if (call.method.compare("getChildrenAbove") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        std::list<std::any> list;
        auto ids = win->getChildrenAbove();
        for (const auto& id : ids) list.push_back(id.str());
        return this->method_codec.encodeSuccessEnvelope(list);
      }

      if (call.method.compare("getChildrenBelow") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        std::list<std::any> list;
        auto ids = win->getChildrenBelow();
        for (const auto& id : ids) list.push_back(id.str());
        return this->method_codec.encodeSuccessEnvelope(list);
      }

      if (call.method.compare("getRect") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        auto rect = win->getRect();
        std::map<std::string, std::any> map;
        map["x"] = rect.pos.x;
        map["y"] = rect.pos.y;
        map["width"] = rect.size.x;
        map["height"] = rect.size.y;
        return this->method_codec.encodeSuccessEnvelope(map);
      }

      if (call.method.compare("setRect") == 0) {
        auto map = std::any_cast<std::map<std::string, std::any>>(call.arguments);
        auto id = xg::Guid(std::any_cast<std::string>(map["id"]));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        Gokai::View::URect rect(
          glm::uvec2(
            Gokai::Casting::castInt(map["x"]),
            Gokai::Casting::castInt(map["y"])
          ),
          glm::uvec2(
            Gokai::Casting::castInt(map["width"]),
            Gokai::Casting::castInt(map["height"])
          )
        );
        win->setRect(rect);
        return this->method_codec.encodeSuccessEnvelope(nullptr);
      }

      if (call.method.compare("hasDecorations") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }
        return this->method_codec.encodeSuccessEnvelope(win->hasDecorations());
      }

      if (call.method.compare("hasTexture") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }
        return this->method_codec.encodeSuccessEnvelope(win->hasTexture());
      }

      if (call.method.compare("getTexture") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        auto tex = win->getTextureId();
        if (tex < 1) {
          return this->method_codec.encodeSuccessEnvelope(nullptr);
        }
        return this->method_codec.encodeSuccessEnvelope(tex);
      }

      if (call.method.compare("enter") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        for (const auto& func : win->onEnter) func();
        return this->method_codec.encodeSuccessEnvelope(nullptr);
      }

      if (call.method.compare("leave") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        for (const auto& func : win->onLeave) func();
        return this->method_codec.encodeSuccessEnvelope(nullptr);
      }

      if (call.method.compare("getTitle") == 0) {
        auto id = xg::Guid(std::any_cast<std::string>(call.arguments));
        auto win = this->get(id);
        if (win == nullptr) {
          return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Window \"{}\" does not exist", id.str()), std::make_any<void*>(nullptr));
        }

        auto title = win->getTitle();
        if (title.empty()) {
          return this->method_codec.encodeSuccessEnvelope(nullptr);
        }
        return this->method_codec.encodeSuccessEnvelope(title);
      }
      return this->method_codec.encodeErrorEnvelope(TAG, fmt::format("Unimplemented method: {}", call.method), std::make_any<void*>(nullptr));
    });
  });

  this->changed.push_back([this]() {
    auto engine_manager = reinterpret_cast<EngineManager*>(this->context->getSystemService(EngineManager::SERVICE_NAME));
    auto call = Gokai::Flutter::MethodCall();
    call.method = "changed";
    engine_manager->sendAll(TAG, this->method_codec.encodeMethodCall(call));
  });
}

std::shared_ptr<Gokai::ServiceChannel> WindowManager::getServiceChannel() {
  return this->service_channel;
}

std::list<xg::Guid> WindowManager::getIds() {
  return std::list<xg::Guid>();
}

Gokai::View::Window* WindowManager::get(xg::Guid id) {
  return nullptr;
}

const std::string WindowManager::SERVICE_NAME = "WindowManager";

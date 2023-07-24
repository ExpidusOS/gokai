#pragma once

#ifdef __cplusplus
#include <gokai/context.h>
#include <gokai/object.h>
#include <memory>

namespace Gokai {
  namespace Input {
    class Base : public Gokai::Object {
      public:
        Base(Gokai::ObjectArguments arguments);

        virtual std::string getName();
        xg::Guid getId();
      protected:
        std::shared_ptr<Gokai::Context> context;
      private:
        xg::Guid id;
    };
  }
}
#endif

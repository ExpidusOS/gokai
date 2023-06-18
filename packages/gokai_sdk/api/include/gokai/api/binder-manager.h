#pragma once

#ifdef __cplusplus
#include <gokai/api/binder.h>
#include <gokai/object.h>

namespace Gokai {
  namespace API {
    class BinderManager : public Gokai::Object {
      public:
        BinderManager(ObjectArguments arguments);
        ~BinderManager();

        bool isCached(std::string name);
        bool isCached(Binder* binder);
        Binder* getCached(std::string name);
        virtual Binder* load(std::string name);
        virtual Binder* getDefault();
        virtual std::map<std::string, Binder*> getAll();

        static BinderManager* create(ObjectArguments arguments);
      protected:
        Binder* binder_default;
        std::map<std::string, Binder*> binder_cache;
        bool is_portable;
    };
  }
}
#endif
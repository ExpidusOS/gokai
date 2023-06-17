#pragma once

#ifdef __cplusplus
namespace Gokai {
  enum TypeSafePolicy {
    GOKAI_TYPE_SAFE_POLICY_ASSERT = 0,
    GOKAI_TYPE_SAFE_POLICY_EXCEPTION,
    GOKAI_TYPE_SAFE_POLICY_NONE
  };
}
#endif
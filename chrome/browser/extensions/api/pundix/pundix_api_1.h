#ifndef __PUNDIX_H__
#define __PUNDIX_H__
#include "extensions/browser/extension_function.h"

namespace extensions {
namespace api {

class PundixSignatureStringFunction : public ExtensionFunction {
 public:
  PundixSignatureStringFunction() = default;
  PundixSignatureStringFunction(
      const PundixSignatureStringFunction&) = delete;
  PundixSignatureStringFunction& operator=(
      const PundixSignatureStringFunction&) = delete;
  DECLARE_EXTENSION_FUNCTION("pundix.signatureString", PUNDIX_SIGNATURESTRING)

 protected:
  ~PundixSignatureStringFunction() override = default;

  // ExtensionFunction overrides.
  ResponseAction Run() override;
};

}
}  // namespace extensions
#endif
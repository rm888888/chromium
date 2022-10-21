#ifndef __PUNDIX_H__
#define __PUNDIX_H__
#include "extensions/browser/extension_function.h"

namespace extensions {
namespace api {
//Signature
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
private:
  void DoTask();
  void OnFinished();

};
//addWidget
class PundixAddWidgetFunction : public ExtensionFunction {
public:
    PundixAddWidgetFunction() = default;
    PundixAddWidgetFunction(
                const PundixAddWidgetFunction&) = delete;
    PundixAddWidgetFunction& operator=(
                const PundixAddWidgetFunction&) = delete;
    DECLARE_EXTENSION_FUNCTION("pundix.addWidget", PUNDIX_ADD_WIDGET)

protected:
    ~PundixAddWidgetFunction() override = default;

    // ExtensionFunction overrides.
    ResponseAction Run() override;
private:
};
//removeWidget
class PundixRemoveWidgetFunction : public ExtensionFunction {
public:
    PundixRemoveWidgetFunction() = default;
    PundixRemoveWidgetFunction(
                const PundixRemoveWidgetFunction&) = delete;
    PundixRemoveWidgetFunction& operator=(
                const PundixRemoveWidgetFunction&) = delete;
    DECLARE_EXTENSION_FUNCTION("pundix.removeWidget", PUNDIX_REMOVE_WIDGET)

protected:
    ~PundixRemoveWidgetFunction() override = default;

    // ExtensionFunction overrides.
    ResponseAction Run() override;
private:
};
//moveWidget
class PundixMoveWidgetFunction : public ExtensionFunction {
public:
    PundixMoveWidgetFunction() = default;
    PundixMoveWidgetFunction(
                const PundixMoveWidgetFunction&) = delete;
    PundixMoveWidgetFunction& operator=(
                const PundixMoveWidgetFunction&) = delete;
    DECLARE_EXTENSION_FUNCTION("pundix.moveWidget", PUNDIX_MOVE_WIDGET)

protected:
    ~PundixMoveWidgetFunction() override = default;

    // ExtensionFunction overrides.
    ResponseAction Run() override;
private:
};
//setWidgetRadius
class PundixSetWidgetRadiusFunction : public ExtensionFunction {
public:
    PundixSetWidgetRadiusFunction() = default;
    PundixSetWidgetRadiusFunction(
                const PundixSetWidgetRadiusFunction&) = delete;
    PundixSetWidgetRadiusFunction& operator=(
                const PundixSetWidgetRadiusFunction&) = delete;
    DECLARE_EXTENSION_FUNCTION("pundix.setWidgetRadius", PUNDIX_SET_WIDGET_RADIUS)

protected:
    ~PundixSetWidgetRadiusFunction() override = default;
    // ExtensionFunction overrides.
    ResponseAction Run() override;
private:
};
//
}
}  // namespace extensions
#endif
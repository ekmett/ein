export module ein.cpu;

namespace ein::cpu {

/// \brief cpu vendor id
export enum class vendor {
  intel=0,
  amd=1,
  unknown=2
};

/// current cpu vendor
export extern const enum vendor vendor;

/// intel check for user-level mwait/monitor
export extern const bool has_mwait;

/// amd check for mwaitx/monitorx
export extern const bool has_mwaitx;

}

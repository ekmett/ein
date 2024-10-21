export module ein.cpu;

namespace ein::cpu {

// \brief runtime cpu vendor
export enum class vendor {
  intel=0,
  amd=1,
  unknown=2
};

// Declaration of the static const variable (to be defined in the .cpp file)
export extern const enum vendor vendor;
export extern const bool has_mwait; // intel check for user-level mwait/monitor
export extern const bool has_mwaitx; // amd check for mwaitx/monitorx

}

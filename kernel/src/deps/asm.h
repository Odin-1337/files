#ifndef asmH
#define asmH

extern "C" {
	std::uintptr_t get_ntoskrnl_base();
}

__forceinline static std::uintptr_t ntoskrnl_base() {
	return get_ntoskrnl_base();
}

#endif asmH // asmH
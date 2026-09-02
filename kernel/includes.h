#ifndef includesH
#define includesH

#include <intrin.h>
namespace std {
	using size_t = unsigned __int64;
}
#include <cstdint>
namespace std {
    struct m128a_t {
        std::uint64_t m_low;
        std::int64_t m_high;
    };
}

#include <src/deps/variables.h>
#include <src/deps/status.h>
#include <src/deps/krnl.h>
#include <src/deps/crt.h>
#include <src/deps/asm.h>
#include <src/deps/exports.h>

#include <src/memory/memory.h>

#include <src/eprocess/eprocess.h>

#include <src/client/client.h>
#include <src/client/handler.h>

#endif includesH
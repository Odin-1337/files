#ifndef memoryH
#define memoryH

namespace memory {
    struct pt_cache_t {
        std::uint64_t dirbase{ 0 };

        std::uint64_t pml4_idx{ ~0ull };
        std::uint64_t pdpt_idx{ ~0ull };
        std::uint64_t pd_idx{ ~0ull };

        std::uint64_t pdpt_base{ 0 };
        std::uint64_t pd_base{ 0 };
        std::uint64_t pt_base{ 0 };

        void reset(std::uint64_t new_dirbase) noexcept;

        __forceinline void sync(std::uint64_t new_dirbase) noexcept {
            const std::uint64_t clean = new_dirbase & ~0xFFFull;
            if (clean != dirbase)
                reset(clean);
        }
    };

    template<typename T>
    __forceinline bool phys_read(std::uint64_t phys, T& out) {
        physical_address_t pa{};
        pa.m_quad_part = static_cast<LONGLONG>(phys);

        const auto va = mm_get_virtual_for_physical(pa);
        if (!va) return false;

        out = *reinterpret_cast<T*>(va); // single dereference, no copy overhead
        return true;
    }

    __forceinline bool phys_read_buf(
        std::uint64_t phys,
        void* dst,
        std::size_t size
    ) noexcept {
        physical_address_t pa{};
        pa.m_quad_part = static_cast<LONGLONG>(phys);

        const auto va = mm_get_virtual_for_physical(pa);
        if (!va) return false;

        rtl_copy_memory(dst, reinterpret_cast<const void*>(va), size);
        return true;
    }

    __forceinline bool phys_write_buf(
        std::uint64_t phys,
        const void* src,
        std::size_t size
    ) noexcept {
        physical_address_t pa{};
        pa.m_quad_part = static_cast<LONGLONG>(phys);

        const auto va = mm_get_virtual_for_physical(pa);
        if (!va) return false;

        rtl_copy_memory(reinterpret_cast<void*>(va), src, size);
        return true;
    }

    nt_status_t copy_memory(
        std::uint64_t address,
        void* buffer,
        std::size_t size
    );

    std::uint64_t translate_linear_address(
        std::uint64_t dirbase,
        std::uint64_t virtual_address
    );

    std::uint64_t translate_cached(
        pt_cache_t& cache,
        std::uint64_t virtual_address
    ) noexcept;

    std::uint64_t translate_cached(
        pt_cache_t& cache,
        std::uint64_t dirbase,
        std::uint64_t virtual_address
    ) noexcept;

    static bool is_valid_pml4(const pml4e* table) noexcept;

    std::uint64_t find_process_dtb(eprocess_t* process);

    __forceinline std::uint64_t find_min(std::size_t g, std::size_t f) {
        return (g < (f) ? (g) : (f));
    }
} // namespace memory

#endif memoryH
#include <includes.h>

namespace memory {

    void pt_cache_t::reset(std::uint64_t new_dirbase) noexcept {
        dirbase = new_dirbase & ~0xFFFull;
        pml4_idx = ~0ull;
        pdpt_idx = ~0ull;
        pd_idx = ~0ull;
    }

	nt_status_t copy_memory(
		std::uint64_t address,
		void* buffer,
		std::size_t size
	) {
		if (!mm_is_address_valid(address)) {
			return nt_status_t::invalid_parameter;
		}

		mm_copy_address_t mm_copy{ NULL };
		mm_copy.physical_address.m_quad_part = address;

		size_t bytes = 0;
		
		return mm_copy_memory(
			buffer,
			mm_copy,
			size,
			mm_copy_flags_e::physical_address,
			&bytes
		);
	}

    std::uint64_t translate_linear_address(
        std::uint64_t dirbase,
        std::uint64_t virtual_address
    ) {
        dirbase &= ~page_4kb_mask; // strip PCID
        const virt_addr_t virt{ virtual_address };

        pml4e pml4_entry{};
        if (!phys_read(dirbase + virt.pml4e_index * 8, pml4_entry)
            || !pml4_entry.hard.present)
            return 0;

        pdpte pdpte_entry{};
        if (!phys_read((pml4_entry.hard.pfn << 12) + virt.pdpte_index * 8, pdpte_entry)
            || !pdpte_entry.hard.present)
            return 0;

        // 1GB large page
        if (pdpte_entry.hard.page_size)
            return ((pdpte_entry.hard.pfn << 12) & ~page_1gb_mask)
            | (virtual_address & page_1gb_mask);

        pde pde_entry{};
        if (!phys_read((pdpte_entry.hard.pfn << 12) + virt.pde_index * 8, pde_entry)
            || !pde_entry.hard.present)
            return 0;

        // 2MB large page
        if (pde_entry.hard.page_size)
            return ((pde_entry.hard.pfn << 12) & ~page_2mb_mask)
            | (virtual_address & page_2mb_mask);

        pte pte_entry{};
        if (!phys_read((pde_entry.hard.pfn << 12) + virt.pte_index * 8, pte_entry)
            || !pte_entry.hard.present)
            return 0;

        // 4KB page
        return (pte_entry.hard.pfn << 12) | virt.offset;
    }

    // Cached translate — skips levels whose index hasn't changed since last call.
    // Best case (same 2MB region): 1 phys_read instead of 4.
    // NOTE: not thread-safe — use one pt_cache_t per thread/call site.
    std::uint64_t translate_cached(
        pt_cache_t& cache,
        std::uint64_t virtual_address
    ) noexcept {
        const virt_addr_t virt{ virtual_address };

        if (virt.pml4e_index != cache.pml4_idx) {
            pml4e entry{};
            if (!phys_read(cache.dirbase + virt.pml4e_index * 8, entry)
                || !entry.hard.present)
                return 0;

            cache.pml4_idx = virt.pml4e_index;
            cache.pdpt_base = entry.hard.pfn << 12;
            cache.pdpt_idx = ~0ull;
            cache.pd_idx = ~0ull;
        }

        if (virt.pdpte_index != cache.pdpt_idx) {
            pdpte entry{};
            if (!phys_read(cache.pdpt_base + virt.pdpte_index * 8, entry)
                || !entry.hard.present)
                return 0;

            // 1GB large page
            if (entry.hard.page_size)
                return ((entry.hard.pfn << 12) & ~page_1gb_mask)
                | (virtual_address & page_1gb_mask);

            cache.pdpt_idx = virt.pdpte_index;
            cache.pd_base = entry.hard.pfn << 12;
            cache.pd_idx = ~0ull;
        }

        if (virt.pde_index != cache.pd_idx) {
            pde entry{};
            if (!phys_read(cache.pd_base + virt.pde_index * 8, entry)
                || !entry.hard.present)
                return 0;

            // 2MB large page
            if (entry.hard.page_size)
                return ((entry.hard.pfn << 12) & ~page_2mb_mask)
                | (virtual_address & page_2mb_mask);

            cache.pd_idx = virt.pde_index;
            cache.pt_base = entry.hard.pfn << 12;
        }

        pte entry{};
        if (!phys_read(cache.pt_base + virt.pte_index * 8, entry)
            || !entry.hard.present)
            return 0;

        return (entry.hard.pfn << 12) | virt.offset;
    }

    std::uint64_t translate_cached(
        pt_cache_t& cache,
        std::uint64_t dirbase,
        std::uint64_t virtual_address
    ) noexcept {
        // Auto-reset if this is a different process
        cache.sync(dirbase);

        const virt_addr_t virt{ virtual_address };

        if (virt.pml4e_index != cache.pml4_idx) {
            pml4e entry{};
            if (!phys_read(cache.dirbase + virt.pml4e_index * 8, entry)
                || !entry.hard.present)
                return 0;

            cache.pml4_idx = virt.pml4e_index;
            cache.pdpt_base = entry.hard.pfn << 12;
            cache.pdpt_idx = ~0ull;
            cache.pd_idx = ~0ull;
        }

        if (virt.pdpte_index != cache.pdpt_idx) {
            pdpte entry{};
            if (!phys_read(cache.pdpt_base + virt.pdpte_index * 8, entry)
                || !entry.hard.present)
                return 0;

            if (entry.hard.page_size)
                return ((entry.hard.pfn << 12) & ~0x3FFFFFFFull)
                | (virtual_address & 0x3FFFFFFF);

            cache.pdpt_idx = virt.pdpte_index;
            cache.pd_base = entry.hard.pfn << 12;
            cache.pd_idx = ~0ull;
        }

        if (virt.pde_index != cache.pd_idx) {
            pde entry{};
            if (!phys_read(cache.pd_base + virt.pde_index * 8, entry)
                || !entry.hard.present)
                return 0;

            if (entry.hard.page_size)
                return ((entry.hard.pfn << 12) & ~0x1FFFFFull)
                | (virtual_address & 0x1FFFFF);

            cache.pd_idx = virt.pde_index;
            cache.pt_base = entry.hard.pfn << 12;
        }

        pte entry{};
        if (!phys_read(cache.pt_base + virt.pte_index * 8, entry)
            || !entry.hard.present)
            return 0;

        return (entry.hard.pfn << 12) | virt.offset;
    }

    static bool is_valid_pml4(const pml4e* table) noexcept {
        std::uint32_t present = 0;

        for (std::uint32_t i = 256; i < 512; i++) {
            if (!table[i].hard.present)
                continue;

            if (!table[i].hard.pfn || table[i].hard.pfn > 0x00FFFFFFull)
                return false;

            present++;
        }

        return present >= 2 && present < 200;
    }

    std::uint64_t find_process_dtb(eprocess_t* process) {
        if (!process)
            return 0;

        char image_name[16]{};
        rtl_copy_memory(image_name, process->m_image_file_name, 15);

        const std::uint64_t target_va = reinterpret_cast<std::uint64_t>(process);

        physical_memory_range_t* ranges = mm_get_physical_memory_ranges();
        if (!ranges)
            return 0;

        std::uint64_t found_dtb = 0;

        for (std::uint32_t i = 0;
            ranges[i].BaseAddress.m_quad_part || ranges[i].NumberOfBytes.m_quad_part;
            i++)
        {
            const std::uint64_t base = static_cast<std::uint64_t>(ranges[i].BaseAddress.m_quad_part);
            const std::uint64_t size = static_cast<std::uint64_t>(ranges[i].NumberOfBytes.m_quad_part);

            for (std::uint64_t offset = 0; offset < size; offset += 0x1000) {
                const std::uint64_t phys = base + offset;

                physical_address_t pa{};
                pa.m_quad_part = static_cast<LONGLONG>(phys);

                const auto kva = mm_get_virtual_for_physical(pa);
                if (!kva) continue;

                if (!is_valid_pml4(reinterpret_cast<const pml4e*>(kva)))
                    continue;

                const std::uint64_t eproc_phys = translate_linear_address(phys, target_va);
                if (!eproc_phys) continue;

                physical_address_t name_pa{};
                name_pa.m_quad_part = static_cast<LONGLONG>(
                    eproc_phys + offsetof(eprocess_t, m_image_file_name)
                    );

                const auto name_kva = mm_get_virtual_for_physical(name_pa);
                if (!name_kva) continue;

                if (rtl_equal_memory(name_kva, image_name, 15)) {
                    found_dtb = phys;
                    break;
                }
            }

            if (found_dtb) break;
        }

        ex_free_pool(ranges);
        return found_dtb;
    }
} // namespace memory
#include <includes.h>

namespace client {
    mdl_t* m_comm_mdl{ nullptr };
    data_request_t* m_data_request{ nullptr };
    std::uint32_t   m_syscall_index{ 0 };
    ksemaphore_t* m_response_handle{ nullptr };
    ksemaphore_t* m_request_handle{ nullptr };

    memory::pt_cache_t m_pt_cache{};

    nt_status_t registry_callback(
        void* callback_context,
        void* argument1,
        void* argument2
    ) {
        if (!argument1)
            return nt_status_t::success;

        const auto notify_class = reinterpret_cast<std::uint32_t>(argument1);
        if (notify_class != 1)
            return nt_status_t::success;

        auto* info = reinterpret_cast<reg_set_value_key_information_t*>(argument2);
        if (!info || !info->m_value_name)
            return nt_status_t::success;

        if (info->m_data && info->m_data_size >= sizeof(data_initialize_t)) {
            auto* value_name = info->m_value_name;

            if (!mm_is_address_valid(value_name) ||
                !mm_is_address_valid(value_name->m_buffer))
                return nt_status_t::success;

            if (wcsstr(value_name->m_buffer, L"InitializeSharedMemory")) {
                auto* init_data = reinterpret_cast<data_initialize_t*>(info->m_data);
                if (!mm_is_address_valid(init_data))
                    return nt_status_t::unsuccessful;

                // Lookup client process
                eprocess_t* client_process{ nullptr };
                ps_lookup_process_by_process_id(
                    init_data->m_pid,
                    &client_process
                );
                if (!client_process)
                    return nt_status_t::unsuccessful;

                // Attach so address space is correct for the translate walk
                auto* orig_eprocess = eprocess::attach_process(client_process);
                if (!orig_eprocess)
                    return nt_status_t::unsuccessful;

                const auto base_address = init_data->m_base_address;
                const auto data_size = sizeof(data_request_t);

                // Allocate MDL describing the user-mode shared buffer
                m_comm_mdl = reinterpret_cast<mdl_t*>(
                    io_allocate_mdl(nullptr, data_size, false, false, nullptr)
                    );
                if (!m_comm_mdl) {
                    eprocess::detach_process(orig_eprocess);
                    return nt_status_t::unsuccessful;
                }

                const auto page_aligned_va = base_address & ~(page_4kb_size - 1);
                const auto byte_offset = base_address & (page_4kb_size - 1);
                const auto page_count =
                    (byte_offset + data_size + page_4kb_size - 1) / page_4kb_size;

                m_comm_mdl->m_start_va = reinterpret_cast<void*>(page_aligned_va);
                m_comm_mdl->m_byte_offset = static_cast<std::uint32_t>(byte_offset);
                m_comm_mdl->m_byte_count = static_cast<std::uint32_t>(data_size);
                m_comm_mdl->m_mdl_flags |= 0x0001 | 0x0004;
                m_comm_mdl->m_process = client_process;

                // Fill PFN array by walking user-mode page tables
                auto* pfn_array = reinterpret_cast<std::uint64_t*>(
                    reinterpret_cast<std::uint8_t*>(m_comm_mdl) + sizeof(mdl_t)
                    );

                // Get client DTB for the page table walk
                const std::uint64_t client_dtb = memory::find_process_dtb(client_process);
                if (!client_dtb) {
                    eprocess::detach_process(orig_eprocess);
                    return nt_status_t::unsuccessful;
                }

                for (std::uint64_t idx = 0; idx < page_count; idx++) {
                    const std::uint64_t current_va = base_address + (idx * page_4kb_size);

                    const std::uint64_t current_pa = memory::translate_cached(
                        m_pt_cache, client_dtb, current_va
                    );
                    if (!current_pa) {
                        eprocess::detach_process(orig_eprocess);
                        return nt_status_t::unsuccessful;
                    }

                    pfn_array[idx] = current_pa >> 12;
                }

                m_data_request = reinterpret_cast<data_request_t*>(
                    mm_map_locked_pages_specify_cache(
                        m_comm_mdl,
                        mode_e::kernel_mode,
                        memory_caching_type_e::mm_cached,
                        nullptr,
                        false,
                        63
                    )
                    );

                eprocess::detach_process(orig_eprocess);

                if (!m_data_request)
                    return nt_status_t::unsuccessful;

                dbg_print("[client] Shared memory mapped at %p\n", m_data_request);
                return nt_status_t::success;
            }
        }

        if (info->m_data && info->m_data_size == sizeof(std::uint32_t)) {
            auto* value_name = info->m_value_name;

            if (!mm_is_address_valid(value_name) ||
                !mm_is_address_valid(value_name->m_buffer))
                return nt_status_t::success;

            if (wcsstr(value_name->m_buffer, L"RequestSharedMemory")) {
                if (!m_data_request)
                    return nt_status_t::unsuccessful; // not initialized yet

                const auto request_type = *reinterpret_cast<std::uint32_t*>(info->m_data);
                dbg_print("[client] Request type: %u\n", request_type);

                switch (static_cast<request_type_e>(request_type)) {

                case request_type_e::ping_driver:
                    return nt_status_t::success;

                case request_type_e::get_eprocess: {
                    if (!m_data_request->m_process_id)
                        return nt_status_t::invalid_parameter;

                    eprocess_t* target = eprocess::find_eprocess(
                        m_data_request->m_process_id
                    );

                    if (!target)
                        return nt_status_t::not_found;

                    memcpy(
                        &m_data_request->m_eprocess,
                        &target,
                        sizeof(m_data_request->m_eprocess)
                    );
                    
                    // NOTE: caller must send a release request or you'll leak
                    //       the EPROCESS reference added by PsLookupProcess...
                    return nt_status_t::success;
                }

                case request_type_e::get_image_base: {
                    auto* proc = m_data_request->m_eprocess;
                    if (!proc)
                        return nt_status_t::invalid_parameter;

                    
                    void* image = eprocess::get_section_base_address(proc);

                    if (!image)
                        return nt_status_t::not_found;

                    memcpy(
                        &m_data_request->m_image_base,
                        &image,
                        sizeof(m_data_request->m_image_base)
                    );

                    return m_data_request->m_image_base
                        ? nt_status_t::success
                        : nt_status_t::not_found;
                }

                case request_type_e::get_directory_table_base: {
                    auto* proc = m_data_request->m_eprocess;
                    if (!proc)
                        return nt_status_t::invalid_parameter;

                    const std::uint64_t dtb = memory::find_process_dtb(proc);
                    if (!dtb)
                        return nt_status_t::not_found;

                    // Reset the cache whenever the client requests a new DTB —
                    // this ensures subsequent reads use the correct page tables
                    m_pt_cache.reset(dtb);
                    memcpy(
                        &m_data_request->m_dtb,
                        &dtb,
                        sizeof(m_data_request->m_dtb)
                    );
                    return nt_status_t::success;
                }
                case request_type_e::read_memory: {
                    if (!m_data_request->m_dtb ||
                        !m_data_request->m_address ||
                        !m_data_request->m_buffer ||
                        !m_data_request->m_size)
                        return nt_status_t::invalid_parameter;

                    const std::size_t size = m_data_request->m_size;
                    std::size_t cur_offset = 0;
                    std::size_t total_read = 0;

                    while (cur_offset < size) {
                        const std::uint64_t cur_pa = memory::translate_cached(
                            m_pt_cache,
                            m_data_request->m_dtb,
                            m_data_request->m_address + cur_offset
                        );

                        if (!cur_pa) {
                            dbg_print("[client] read: translate failed VA 0x%llX\n",
                                m_data_request->m_address + cur_offset);
                            break;
                        }

                        const std::size_t chunk = memory::find_min(
                            size - cur_offset,
                            0x1000 - (cur_pa & 0xFFFull)
                        );

                        // PFN lookup + RtlCopyMemory — no MmCopyMemory overhead
                        if (!memory::phys_read_buf(
                            cur_pa,
                            static_cast<std::uint8_t*>(m_data_request->m_buffer) + cur_offset,
                            chunk)) {
                            dbg_print("[client] read: phys_read_buf failed PA 0x%llX\n", cur_pa);
                            break;
                        }

                        cur_offset += chunk;
                        total_read += chunk;
                    }

                    m_data_request->m_size = total_read;
                    return total_read == size
                        ? nt_status_t::success
                        : nt_status_t::partial_copy;
                }

                case request_type_e::write_memory: {
                    if (!m_data_request->m_dtb ||
                        !m_data_request->m_address ||
                        !m_data_request->m_buffer ||
                        !m_data_request->m_size)
                        return nt_status_t::invalid_parameter;

                    const std::size_t size = m_data_request->m_size;
                    std::size_t cur_offset = 0;
                    std::size_t total_written = 0;

                    while (cur_offset < size) {
                        const std::uint64_t cur_pa = memory::translate_cached(
                            m_pt_cache,
                            m_data_request->m_dtb,
                            m_data_request->m_address + cur_offset
                        );

                        if (!cur_pa) {
                            dbg_print("[client] write: translate failed VA 0x%llX\n",
                                m_data_request->m_address + cur_offset);
                            break;
                        }

                        const std::size_t chunk = memory::find_min(
                            size - cur_offset,
                            0x1000 - (cur_pa & 0xFFFull)
                        );

                        // PFN lookup + RtlCopyMemory — replaces MmMapIoSpace/MmUnmapIoSpace
                        if (!memory::phys_write_buf(
                            cur_pa,
                            static_cast<const std::uint8_t*>(m_data_request->m_buffer) + cur_offset,
                            chunk)) {
                            dbg_print("[client] write: phys_write_buf failed PA 0x%llX\n", cur_pa);
                            break;
                        }

                        cur_offset += chunk;
                        total_written += chunk;
                    }

                    m_data_request->m_size = total_written;
                    return total_written == size
                        ? nt_status_t::success
                        : nt_status_t::partial_copy;
                }

                //case request_type_e::read_memory: {
                //    if (!m_data_request->m_dtb ||
                //        !m_data_request->m_address ||
                //        !m_data_request->m_buffer ||
                //        !m_data_request->m_size)
                //        return nt_status_t::invalid_parameter;

                //    const std::size_t size = m_data_request->m_size;
                //    std::size_t cur_offset = 0;
                //    std::size_t total_read = 0;

                //    while (cur_offset < size) {
                //        const std::uint64_t cur_pa = memory::translate_cached(
                //            m_pt_cache, m_data_request->m_dtb, m_data_request->m_address + cur_offset
                //        );

                //        if (!cur_pa) {
                //            dbg_print("[client] read: failed to translate VA 0x%llX\n",
                //                m_data_request->m_address + cur_offset);
                //            break;
                //        }

                //        const std::size_t chunk = memory::find_min((size - cur_offset), (0x1000 - (cur_pa & 0xFFFull)));

                //        std::size_t bytes_read = 0;
                //        const nt_status_t status = memory::copy_memory(
                //            cur_pa,
                //            static_cast<std::uint8_t*>(m_data_request->m_buffer) + cur_offset,
                //            chunk
                //        );

                //        if (!nt_success(status)) {
                //            dbg_print("[client] read: copy_memory failed at PA 0x%llX status 0x%08X\n",
                //                cur_pa, status);
                //            break;
                //        }

                //        cur_offset += chunk;
                //        total_read += chunk;
                //    }

                //    m_data_request->m_size = total_read;
                //    return total_read == size
                //        ? nt_status_t::success
                //        : nt_status_t::partial_copy; // partial reads aren't failures —
                //    // caller checks m_size to see how much arrived
                //}

                //case request_type_e::write_memory: {
                //    // m_dtb = DTB of target process
                //    // m_address  = destination VA in target process
                //    // m_buffer = source — kernel-mapped shared buffer
                //    // m_size     = bytes to write

                //    if (!m_data_request->m_dtb ||
                //        !m_data_request->m_address ||
                //        !m_data_request->m_buffer ||
                //        !m_data_request->m_size)
                //        return nt_status_t::invalid_parameter;

                //    const std::size_t size = m_data_request->m_size;
                //    std::size_t cur_offset = 0;
                //    std::size_t total_written = 0;

                //    while (cur_offset < size) {
                //        const std::uint64_t cur_pa = memory::translate_cached(
                //            m_pt_cache, m_data_request->m_dtb, m_data_request->m_address + cur_offset
                //        );

                //        if (!cur_pa) {
                //            dbg_print("[client] write: failed to translate VA 0x%llX\n",
                //                m_data_request->m_address + cur_offset);
                //            break;
                //        }

                //        const std::size_t chunk = memory::find_min((size - cur_offset), (0x1000 - (cur_pa & 0xFFFull)));

                //        physical_address_t pa{};
                //        pa.m_quad_part = static_cast<LONGLONG>(cur_pa);

                //        auto* mapped = static_cast<std::uint8_t*>(
                //            mm_map_io_space(pa, chunk, memory_caching_type_e::mm_non_cached)
                //            );

                //        if (!mapped) {
                //            dbg_print("[client] write: MmMapIoSpace failed at PA 0x%llX\n", cur_pa);
                //            break;
                //        }

                //        rtl_copy_memory(
                //            mapped,
                //            (reinterpret_cast<const std::uint8_t*>(m_data_request->m_buffer)) + cur_offset,
                //            chunk
                //        );
                //        mm_unmap_io_space(mapped, chunk);

                //        cur_offset += chunk;
                //        total_written += chunk;
                //    }

                //    m_data_request->m_size = total_written;
                //    return total_written == size
                //        ? nt_status_t::success
                //        : nt_status_t::partial_copy;
                //}

                default:
                    return nt_status_t::invalid_parameter;
                }
            }
        }

        return nt_status_t::success;
    }
} // namespace client
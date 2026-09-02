#ifndef exportsH
#define exportsH

inline std::uint8_t* get_system_routine(const char* export_name) {
    auto dos_header{ reinterpret_cast<image_dos_header_t*> (ntoskrnl_base()) };
    auto nt_headers{ reinterpret_cast<image_nt_headers64_t*> (ntoskrnl_base() + dos_header->m_e_lfanew) };
    if (!dos_header->is_valid()
        || !nt_headers->is_valid())
        return {};

    auto exp_dir{ nt_headers->m_optional_header.m_data_directory[0].as_rva< image_export_directory_t* >(ntoskrnl_base()) };
    if (!exp_dir->m_address_of_functions
        || !exp_dir->m_address_of_names
        || !exp_dir->m_address_of_name_ordinals)
        return {};

    auto name{ reinterpret_cast<std::int32_t*> (ntoskrnl_base() + exp_dir->m_address_of_names) };
    auto func{ reinterpret_cast<std::int32_t*> (ntoskrnl_base() + exp_dir->m_address_of_functions) };
    auto ords{ reinterpret_cast<std::int16_t*> (ntoskrnl_base() + exp_dir->m_address_of_name_ordinals) };

    for (std::int32_t i{}; i < exp_dir->m_number_of_names; i++) {
        auto cur_name{ ntoskrnl_base() + name[i] };
        auto cur_func{ ntoskrnl_base() + func[ords[i]] };
        if (!cur_name
            || !cur_func)
            continue;

        if (strcmp(export_name, reinterpret_cast<char*>(cur_name)) == 0)
            return reinterpret_cast<std::uint8_t*>(cur_func);
    }
    return {};
}

inline nt_status_t mm_copy_virtual_memory(eprocess_t* source_process, void* source_address, eprocess_t* target_process, void* target_address, std::size_t buffer_size, mode_e previous_mode, std::size_t* return_size) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmCopyVirtualMemory"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(eprocess_t*, void*, eprocess_t*, void*, std::size_t, mode_e, std::size_t*);
    return reinterpret_cast<function_t>(fn)(source_process, source_address, target_process, target_address, buffer_size, previous_mode, return_size);
}

inline void* mm_map_io_space(large_integer_t physical_address, std::size_t number_of_bytes, memory_caching_type_e cache_type) {
    static std::uint8_t* fn = nullptr;
    if (!fn) {
        fn = get_system_routine("MmMapIoSpace");
        if (!fn) return nullptr;
    }
    using function_t = void* (__stdcall*)(large_integer_t, std::size_t, memory_caching_type_e);
    return reinterpret_cast<function_t>(fn)(physical_address, number_of_bytes, cache_type);
}

inline void* mm_map_io_space_ex(large_integer_t physical_address, std::size_t number_of_bytes, ULONG protect) {
    static std::uint8_t* fn = nullptr;
    if (!fn) {
        fn = get_system_routine("MmMapIoSpaceEx");
        if (!fn) return nullptr;
    }
    using function_t = void* (__stdcall*)(large_integer_t, std::size_t, ULONG);
    return reinterpret_cast<function_t>(fn)(physical_address, number_of_bytes, protect);
}

inline void mm_unmap_io_space(void* base_address, std::size_t number_of_bytes) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmUnmapIoSpace"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*, std::size_t);
    reinterpret_cast<function_t>(fn)(base_address, number_of_bytes);
}

inline mdl_t* mm_allocate_pages_for_mdl(large_integer_t low_address, large_integer_t high_address, large_integer_t skip_bytes, std::size_t total_bytes) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmAllocatePagesForMdl"); if (!fn) return nullptr; }
    using function_t = mdl_t * (__stdcall*)(large_integer_t, large_integer_t, large_integer_t, std::size_t);
    return reinterpret_cast<function_t>(fn)(low_address, high_address, skip_bytes, total_bytes);
}

inline void mm_free_pages_from_mdl(mdl_t* memory_descriptor_list) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmFreePagesFromMdl"); if (!fn) return; }
    using function_t = void(__stdcall*)(mdl_t*);
    reinterpret_cast<function_t>(fn)(memory_descriptor_list);
}

inline void* mm_map_locked_pages_specify_cache(mdl_t* memory_descriptor_list, mode_e access_mode, memory_caching_type_e cache_type, void* requested_address, std::uint32_t bug_check_on_failure, std::uint32_t priority) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmMapLockedPagesSpecifyCache"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(mdl_t*, mode_e, memory_caching_type_e, void*, std::uint32_t, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(memory_descriptor_list, access_mode, cache_type, requested_address, bug_check_on_failure, priority);
}

inline void mm_unmap_locked_pages(void* base_address, mdl_t* memory_descriptor_list) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmUnmapLockedPages"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*, mdl_t*);
    reinterpret_cast<function_t>(fn)(base_address, memory_descriptor_list);
}

inline void mm_protect_mdl_system_address(mdl_t* memory_descriptor_list, std::uint32_t new_protect) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmProtectMdlSystemAddress"); if (!fn) return; }
    using function_t = void(__stdcall*)(mdl_t*, std::uint32_t);
    reinterpret_cast<function_t>(fn)(memory_descriptor_list, new_protect);
}

inline void* mm_get_system_routine_address(unicode_string_t* system_routine_name) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmGetSystemRoutineAddress"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(unicode_string_t*);
    return reinterpret_cast<function_t>(fn)(system_routine_name);
}

inline bool mm_is_address_valid(void* virtual_address) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmIsAddressValid"); if (!fn) return false; }
    using function_t = bool(__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(virtual_address);
}

inline bool mm_is_address_valid(std::uintptr_t virtual_address) {
    return mm_is_address_valid(reinterpret_cast<void*>(virtual_address));
}

inline void* mm_get_physical_address(void* base_address) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmGetPhysicalAddress"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(base_address);
}

inline physical_memory_range_t* mm_get_physical_memory_ranges(void) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmGetPhysicalMemoryRanges"); if (!fn) return nullptr; }
    using function_t = physical_memory_range_t *(__stdcall*)(void);
    return reinterpret_cast<function_t>(fn)();
}

inline void* mm_get_virtual_for_physical(physical_address_t physical_address) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmGetVirtualForPhysical"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(physical_address_t);
    return reinterpret_cast<function_t>(fn)(physical_address);
}

inline mdl_t* mm_allocate_mdl(void* virtual_address, std::uint32_t length, std::uint8_t secondary_buffer, std::uint8_t charge_quota, irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmAllocateMdl"); if (!fn) return nullptr; }
    using function_t = mdl_t * (__stdcall*)(void*, std::uint32_t, std::uint8_t, std::uint8_t, irp_t*);
    return reinterpret_cast<function_t>(fn)(virtual_address, length, secondary_buffer, charge_quota, irp);
}

inline void mm_free_mdl(mdl_t* mdl) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmFreeMdl"); if (!fn) return; }
    using function_t = void(__stdcall*)(mdl_t*);
    reinterpret_cast<function_t>(fn)(mdl);
}

inline void mm_build_mdl_for_non_paged_pool(mdl_t* memory_descriptor_list) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmBuildMdlForNonPagedPool"); if (!fn) return; }
    using function_t = void(__stdcall*)(mdl_t*);
    reinterpret_cast<function_t>(fn)(memory_descriptor_list);
}

inline void* mm_map_locked_pages(mdl_t* memory_descriptor_list, mode_e access_mode) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmMapLockedPages"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(mdl_t*, mode_e);
    return reinterpret_cast<function_t>(fn)(memory_descriptor_list, access_mode);
}

inline void mm_probe_and_lock_pages(mdl_t* memory_descriptor_list, mode_e access_mode, lock_operation_e operation) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmProbeAndLockPages"); if (!fn) return; }
    using function_t = void(__stdcall*)(mdl_t*, mode_e, lock_operation_e);
    reinterpret_cast<function_t>(fn)(memory_descriptor_list, access_mode, operation);
}

inline void mm_unlock_pages(mdl_t* memory_descriptor_list) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmUnlockPages"); if (!fn) return; }
    using function_t = void(__stdcall*)(mdl_t*);
    reinterpret_cast<function_t>(fn)(memory_descriptor_list);
}

inline nt_status_t mm_map_view_in_system_space(void* section, void** mapped_base, std::size_t* view_size) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmMapViewInSystemSpace"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, void**, std::size_t*);
    return reinterpret_cast<function_t>(fn)(section, mapped_base, view_size);
}

inline nt_status_t mm_unmap_view_in_system_space(void* mapped_base) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmUnmapViewInSystemSpace"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(mapped_base);
}

inline nt_status_t mm_map_view_of_section(void* section_object, eprocess_t* process, void** base_address, std::uint64_t zero_bits, std::size_t commit_size, large_integer_t* section_offset, std::size_t* view_size, section_inherit_e inherit_disposition, std::uint32_t allocation_type, std::uint32_t protect) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmMapViewOfSection"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, eprocess_t*, void**, std::uint64_t, std::size_t, large_integer_t*, std::size_t*, section_inherit_e, std::uint32_t, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(section_object, process, base_address, zero_bits, commit_size, section_offset, view_size, inherit_disposition, allocation_type, protect);
}

inline nt_status_t mm_unmap_view_of_section(eprocess_t* process, void* base_address) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmUnmapViewOfSection"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(eprocess_t*, void*);
    return reinterpret_cast<function_t>(fn)(process, base_address);
}

inline void* mm_secure_virtual_memory(void* address, std::size_t size, std::uint32_t probe_mode) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmSecureVirtualMemory"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(void*, std::size_t, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(address, size, probe_mode);
}

inline void mm_unsecure_virtual_memory(void* secure_handle) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmUnsecureVirtualMemory"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*);
    reinterpret_cast<function_t>(fn)(secure_handle);
}

inline nt_status_t mm_copy_memory(void* destination, mm_copy_address_t source, std::size_t size, mm_copy_flags_e flags, std::size_t* bytes) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("MmCopyMemory"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, mm_copy_address_t, std::size_t, mm_copy_flags_e, std::size_t*);
    return reinterpret_cast<function_t>(fn)(destination, source, size, flags, bytes);
}

inline nt_status_t ps_lookup_process_by_process_id(std::uint32_t process_id, eprocess_t** process) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsLookupProcessByProcessId"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(std::uint32_t, eprocess_t**);
    return reinterpret_cast<function_t>(fn)(process_id, process);
}

inline nt_status_t ps_lookup_thread_by_thread_id(void* thread_id, ethread_t** thread) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsLookupThreadByThreadId"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, ethread_t**);
    return reinterpret_cast<function_t>(fn)(thread_id, thread);
}

inline eprocess_t* ps_get_current_process() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetCurrentProcess"); if (!fn) return nullptr; }
    using function_t = eprocess_t * (__stdcall*)();
    return reinterpret_cast<function_t>(fn)();
}

inline ethread_t* ps_get_current_thread() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetCurrentThread"); if (!fn) return nullptr; }
    using function_t = ethread_t * (__stdcall*)();
    return reinterpret_cast<function_t>(fn)();
}

inline void* ps_get_process_id(eprocess_t* process) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetProcessId"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(eprocess_t*);
    return reinterpret_cast<function_t>(fn)(process);
}

inline void* ps_get_thread_id(ethread_t* thread) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetThreadId"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(ethread_t*);
    return reinterpret_cast<function_t>(fn)(thread);
}

inline eprocess_t* ps_get_thread_process(ethread_t* thread) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetThreadProcess"); if (!fn) return nullptr; }
    using function_t = eprocess_t * (__stdcall*)(ethread_t*);
    return reinterpret_cast<function_t>(fn)(thread);
}

inline void* ps_get_current_process_id() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetCurrentProcessId"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)();
    return reinterpret_cast<function_t>(fn)();
}

inline void* ps_get_current_thread_id() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetCurrentThreadId"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)();
    return reinterpret_cast<function_t>(fn)();
}

inline nt_status_t ps_create_system_thread(void** thread_handle, std::uint32_t desired_access, void* object_attributes, void* process_handle, client_id_t* client_id, void* start_routine, void* start_context) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsCreateSystemThread"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void**, std::uint32_t, void*, void*, client_id_t*, void*, void*);
    return reinterpret_cast<function_t>(fn)(thread_handle, desired_access, object_attributes, process_handle, client_id, start_routine, start_context);
}

inline nt_status_t ps_terminate_system_thread(nt_status_t exit_status) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsTerminateSystemThread"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(nt_status_t);
    return reinterpret_cast<function_t>(fn)(exit_status);
}

inline std::uint8_t ps_is_system_thread(ethread_t* thread) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsIsSystemThread"); if (!fn) return 0; }
    using function_t = std::uint8_t(__stdcall*)(ethread_t*);
    return reinterpret_cast<function_t>(fn)(thread);
}

inline char* ps_get_process_image_file_name(eprocess_t* process) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetProcessImageFileName"); if (!fn) return nullptr; }
    using function_t = char* (__stdcall*)(eprocess_t*);
    return reinterpret_cast<function_t>(fn)(process);
}

inline nt_status_t ps_set_create_process_notify_routine(void* notify_routine, std::uint8_t remove) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsSetCreateProcessNotifyRoutine"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(notify_routine, remove);
}

inline nt_status_t ps_set_create_process_notify_routine_ex(void* notify_routine, std::uint8_t remove) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsSetCreateProcessNotifyRoutineEx"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(notify_routine, remove);
}

inline nt_status_t ps_set_create_thread_notify_routine(void* notify_routine) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsSetCreateThreadNotifyRoutine"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(notify_routine);
}

inline nt_status_t ps_remove_create_thread_notify_routine(void* notify_routine) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsRemoveCreateThreadNotifyRoutine"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(notify_routine);
}

inline nt_status_t ps_set_load_image_notify_routine(void* notify_routine) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsSetLoadImageNotifyRoutine"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(notify_routine);
}

inline nt_status_t ps_remove_load_image_notify_routine(void* notify_routine) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsRemoveLoadImageNotifyRoutine"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(notify_routine);
}

inline nt_status_t ps_reference_process_file_pointer(eprocess_t* process, file_object_t** file_object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsReferenceProcessFilePointer"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(eprocess_t*, file_object_t**);
    return reinterpret_cast<function_t>(fn)(process, file_object);
}

inline void* ps_get_process_peb(eprocess_t* process) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetProcessPeb"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(eprocess_t*);
    return reinterpret_cast<function_t>(fn)(process);
}

inline void* ps_get_process_wow64_process(eprocess_t* process) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetProcessWow64Process"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(eprocess_t*);
    return reinterpret_cast<function_t>(fn)(process);
}

inline nt_status_t ps_get_context_thread(ethread_t* thread, void* context, mode_e mode) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsGetContextThread"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(ethread_t*, void*, mode_e);
    return reinterpret_cast<function_t>(fn)(thread, context, mode);
}

inline nt_status_t ps_set_context_thread(ethread_t* thread, void* context, mode_e mode) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsSetContextThread"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(ethread_t*, void*, mode_e);
    return reinterpret_cast<function_t>(fn)(thread, context, mode);
}

inline void ob_reference_object(void* object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObReferenceObject"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*);
    reinterpret_cast<function_t>(fn)(object);
}

inline std::int32_t ob_reference_object_with_tag(void* object, std::uint32_t tag) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObReferenceObjectWithTag"); if (!fn) return 0; }
    using function_t = std::int32_t(__stdcall*)(void*, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(object, tag);
}

inline void ob_dereference_object(void* object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObDereferenceObject"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*);
    reinterpret_cast<function_t>(fn)(object);
}

inline std::int32_t ob_dereference_object_with_tag(void* object, std::uint32_t tag) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObDereferenceObjectWithTag"); if (!fn) return 0; }
    using function_t = std::int32_t(__stdcall*)(void*, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(object, tag);
}

inline nt_status_t ob_reference_object_by_handle(void* handle, std::uint32_t desired_access, void* object_type, mode_e access_mode, void** object, void* handle_information) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObReferenceObjectByHandle"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, std::uint32_t, void*, mode_e, void**, void*);
    return reinterpret_cast<function_t>(fn)(handle, desired_access, object_type, access_mode, object, handle_information);
}

inline nt_status_t ob_reference_object_by_pointer(void* object, std::uint32_t desired_access, void* object_type, mode_e access_mode) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObReferenceObjectByPointer"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, std::uint32_t, void*, mode_e);
    return reinterpret_cast<function_t>(fn)(object, desired_access, object_type, access_mode);
}

inline nt_status_t ob_open_object_by_pointer(void* object, std::uint32_t handle_attributes, void* access_state, std::uint32_t desired_access, void* object_type, mode_e access_mode, void** handle) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObOpenObjectByPointer"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, std::uint32_t, void*, std::uint32_t, void*, mode_e, void**);
    return reinterpret_cast<function_t>(fn)(object, handle_attributes, access_state, desired_access, object_type, access_mode, handle);
}

inline nt_status_t ob_query_name_string(void* object, void* object_name_info, std::uint32_t length, std::uint32_t* return_length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObQueryNameString"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, void*, std::uint32_t, std::uint32_t*);
    return reinterpret_cast<function_t>(fn)(object, object_name_info, length, return_length);
}

inline object_type_t* ob_get_object_type(void* object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ObGetObjectType"); if (!fn) return nullptr; }
    using function_t = object_type_t * (__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(object);
}

inline void* ex_allocate_pool(pool_type_e pool_type, std::size_t number_of_bytes) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExAllocatePool"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(pool_type_e, std::size_t);
    return reinterpret_cast<function_t>(fn)(pool_type, number_of_bytes);
}

inline void* ex_allocate_pool_with_tag(pool_type_e pool_type, std::size_t number_of_bytes, std::uint32_t tag) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExAllocatePoolWithTag"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(pool_type_e, std::size_t, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(pool_type, number_of_bytes, tag);
}

inline void ex_free_pool(void* pool) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExFreePool"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*);
    reinterpret_cast<function_t>(fn)(pool);
}

inline void ex_free_pool_with_tag(void* pool, std::uint32_t tag) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExFreePoolWithTag"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*, std::uint32_t);
    reinterpret_cast<function_t>(fn)(pool, tag);
}

inline void ex_initialize_push_lock(ex_push_lock_t* push_lock) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExInitializePushLock"); if (!fn) return; }
    using function_t = void(__stdcall*)(ex_push_lock_t*);
    reinterpret_cast<function_t>(fn)(push_lock);
}

inline void ex_acquire_push_lock_exclusive(ex_push_lock_t* push_lock) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExAcquirePushLockExclusive"); if (!fn) return; }
    using function_t = void(__stdcall*)(ex_push_lock_t*);
    reinterpret_cast<function_t>(fn)(push_lock);
}

inline void ex_release_push_lock_exclusive(ex_push_lock_t* push_lock) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExReleasePushLockExclusive"); if (!fn) return; }
    using function_t = void(__stdcall*)(ex_push_lock_t*);
    reinterpret_cast<function_t>(fn)(push_lock);
}

inline void ex_acquire_push_lock_shared(ex_push_lock_t* push_lock) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExAcquirePushLockShared"); if (!fn) return; }
    using function_t = void(__stdcall*)(ex_push_lock_t*);
    reinterpret_cast<function_t>(fn)(push_lock);
}

inline void ex_release_push_lock_shared(ex_push_lock_t* push_lock) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExReleasePushLockShared"); if (!fn) return; }
    using function_t = void(__stdcall*)(ex_push_lock_t*);
    reinterpret_cast<function_t>(fn)(push_lock);
}

inline void ex_acquire_resource_exclusive_lite(eresource_t* resource, std::uint8_t wait) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExAcquireResourceExclusiveLite"); if (!fn) return; }
    using function_t = void(__stdcall*)(eresource_t*, std::uint8_t);
    reinterpret_cast<function_t>(fn)(resource, wait);
}

inline void ex_release_resource_lite(eresource_t* resource) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExReleaseResourceLite"); if (!fn) return; }
    using function_t = void(__stdcall*)(eresource_t*);
    reinterpret_cast<function_t>(fn)(resource);
}

inline nt_status_t ex_initialize_resource_lite(eresource_t* resource) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExInitializeResourceLite"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(eresource_t*);
    return reinterpret_cast<function_t>(fn)(resource);
}

inline nt_status_t ex_delete_resource_lite(eresource_t* resource) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExDeleteResourceLite"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(eresource_t*);
    return reinterpret_cast<function_t>(fn)(resource);
}

inline void ex_acquire_fast_mutex(fast_mutex_t* fast_mutex) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExAcquireFastMutex"); if (!fn) return; }
    using function_t = void(__stdcall*)(fast_mutex_t*);
    reinterpret_cast<function_t>(fn)(fast_mutex);
}

inline void ex_release_fast_mutex(fast_mutex_t* fast_mutex) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExReleaseFastMutex"); if (!fn) return; }
    using function_t = void(__stdcall*)(fast_mutex_t*);
    reinterpret_cast<function_t>(fn)(fast_mutex);
}

inline void ex_initialize_fast_mutex(fast_mutex_t* fast_mutex) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExInitializeFastMutex"); if (!fn) return; }
    using function_t = void(__stdcall*)(fast_mutex_t*);
    reinterpret_cast<function_t>(fn)(fast_mutex);
}

inline nt_status_t ex_uuid_create(guid_t* uuid_out) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ExUuidCreate"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(guid_t*);
    return reinterpret_cast<function_t>(fn)(uuid_out);
}

inline void ke_initialize_event(kevent_t* event, event_type_e type, std::uint8_t state) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeInitializeEvent"); if (!fn) return; }
    using function_t = void(__stdcall*)(kevent_t*, event_type_e, std::uint8_t);
    reinterpret_cast<function_t>(fn)(event, type, state);
}

inline std::int32_t ke_set_event(kevent_t* event, std::int32_t increment, std::uint8_t wait) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeSetEvent"); if (!fn) return 0; }
    using function_t = std::int32_t(__stdcall*)(kevent_t*, std::int32_t, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(event, increment, wait);
}

inline void ke_clear_event(kevent_t* event) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeClearEvent"); if (!fn) return; }
    using function_t = void(__stdcall*)(kevent_t*);
    reinterpret_cast<function_t>(fn)(event);
}

inline std::int32_t ke_reset_event(kevent_t* event) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeResetEvent"); if (!fn) return 0; }
    using function_t = std::int32_t(__stdcall*)(kevent_t*);
    return reinterpret_cast<function_t>(fn)(event);
}

inline nt_status_t ke_wait_for_single_object(void* object, kwait_reason_e wait_reason, mode_e wait_mode, std::uint8_t alertable, large_integer_t* timeout) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeWaitForSingleObject"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, kwait_reason_e, mode_e, std::uint8_t, large_integer_t*);
    return reinterpret_cast<function_t>(fn)(object, wait_reason, wait_mode, alertable, timeout);
}

inline nt_status_t ke_wait_for_multiple_objects(std::uint32_t count, void** object, wait_type_e wait_type, kwait_reason_e wait_reason, mode_e wait_mode, std::uint8_t alertable, large_integer_t* timeout, kwait_block_t* wait_block_array) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeWaitForMultipleObjects"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(std::uint32_t, void**, wait_type_e, kwait_reason_e, mode_e, std::uint8_t, large_integer_t*, kwait_block_t*);
    return reinterpret_cast<function_t>(fn)(count, object, wait_type, wait_reason, wait_mode, alertable, timeout, wait_block_array);
}

inline void ke_initialize_mutex(kmutant_t* mutex, std::uint32_t level) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeInitializeMutex"); if (!fn) return; }
    using function_t = void(__stdcall*)(kmutant_t*, std::uint32_t);
    reinterpret_cast<function_t>(fn)(mutex, level);
}

inline std::int32_t ke_release_mutex(kmutant_t* mutex, std::uint8_t wait) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeReleaseMutex"); if (!fn) return 0; }
    using function_t = std::int32_t(__stdcall*)(kmutant_t*, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(mutex, wait);
}

inline void ke_initialize_semaphore(ksemaphore_t* semaphore, std::int32_t count, std::int32_t limit) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeInitializeSemaphore"); if (!fn) return; }
    using function_t = void(__stdcall*)(ksemaphore_t*, std::int32_t, std::int32_t);
    reinterpret_cast<function_t>(fn)(semaphore, count, limit);
}

inline std::int32_t ke_release_semaphore(ksemaphore_t* semaphore, std::int32_t increment, std::int32_t adjustment, std::uint8_t wait) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeReleaseSemaphore"); if (!fn) return 0; }
    using function_t = std::int32_t(__stdcall*)(ksemaphore_t*, std::int32_t, std::int32_t, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(semaphore, increment, adjustment, wait);
}

inline void ke_initialize_timer(ktimer_t* timer) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeInitializeTimer"); if (!fn) return; }
    using function_t = void(__stdcall*)(ktimer_t*);
    reinterpret_cast<function_t>(fn)(timer);
}

inline std::uint8_t ke_set_timer(ktimer_t* timer, large_integer_t due_time, kdpc_t* dpc) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeSetTimer"); if (!fn) return 0; }
    using function_t = std::uint8_t(__stdcall*)(ktimer_t*, large_integer_t, kdpc_t*);
    return reinterpret_cast<function_t>(fn)(timer, due_time, dpc);
}

inline std::uint8_t ke_cancel_timer(ktimer_t* timer) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeCancelTimer"); if (!fn) return 0; }
    using function_t = std::uint8_t(__stdcall*)(ktimer_t*);
    return reinterpret_cast<function_t>(fn)(timer);
}

inline void ke_initialize_dpc(kdpc_t* dpc, void* deferred_routine, void* deferred_context) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeInitializeDpc"); if (!fn) return; }
    using function_t = void(__stdcall*)(kdpc_t*, void*, void*);
    reinterpret_cast<function_t>(fn)(dpc, deferred_routine, deferred_context);
}

inline std::uint8_t ke_insert_queue_dpc(kdpc_t* dpc, void* system_argument1, void* system_argument2) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeInsertQueueDpc"); if (!fn) return 0; }
    using function_t = std::uint8_t(__stdcall*)(kdpc_t*, void*, void*);
    return reinterpret_cast<function_t>(fn)(dpc, system_argument1, system_argument2);
}

inline std::uint8_t ke_remove_queue_dpc(kdpc_t* dpc) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeRemoveQueueDpc"); if (!fn) return 0; }
    using function_t = std::uint8_t(__stdcall*)(kdpc_t*);
    return reinterpret_cast<function_t>(fn)(dpc);
}

inline void ke_stall_execution_processor(std::uint32_t microseconds) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeStallExecutionProcessor"); if (!fn) return; }
    using function_t = void(__stdcall*)(std::uint32_t);
    reinterpret_cast<function_t>(fn)(microseconds);
}

inline nt_status_t ke_delay_execution_thread(mode_e wait_mode, std::uint8_t alertable, large_integer_t* interval) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeDelayExecutionThread"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(mode_e, std::uint8_t, large_integer_t*);
    return reinterpret_cast<function_t>(fn)(wait_mode, alertable, interval);
}

inline irql_e ke_get_current_irql() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeGetCurrentIrql"); if (!fn) return irql_e::passive_level; }
    using function_t = irql_e(__stdcall*)();
    return reinterpret_cast<function_t>(fn)();
}

inline irql_e ke_raise_irql_to_dpc_level() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeRaiseIrqlToDpcLevel"); if (!fn) return irql_e::passive_level; }
    using function_t = irql_e(__stdcall*)();
    return reinterpret_cast<function_t>(fn)();
}

inline void ke_lower_irql(irql_e new_irql) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeLowerIrql"); if (!fn) return; }
    using function_t = void(__stdcall*)(irql_e);
    reinterpret_cast<function_t>(fn)(new_irql);
}

inline void ke_initialize_apc(kapc_t* apc, ethread_t* thread, std::uint8_t environment, void* kernel_routine, void* rundown_routine, void* normal_routine, mode_e processor_mode, void* normal_context) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeInitializeApc"); if (!fn) return; }
    using function_t = void(__stdcall*)(kapc_t*, ethread_t*, std::uint8_t, void*, void*, void*, mode_e, void*);
    reinterpret_cast<function_t>(fn)(apc, thread, environment, kernel_routine, rundown_routine, normal_routine, processor_mode, normal_context);
}

inline std::uint8_t ke_insert_queue_apc(kapc_t* apc, void* system_argument1, void* system_argument2, std::int8_t increment) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeInsertQueueApc"); if (!fn) return 0; }
    using function_t = std::uint8_t(__stdcall*)(kapc_t*, void*, void*, std::int8_t);
    return reinterpret_cast<function_t>(fn)(apc, system_argument1, system_argument2, increment);
}

inline std::uint8_t ke_test_alert_thread(mode_e alert_mode) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeTestAlertThread"); if (!fn) return 0; }
    using function_t = std::uint8_t(__stdcall*)(mode_e);
    return reinterpret_cast<function_t>(fn)(alert_mode);
}

inline void ke_attach_process(eprocess_t* process) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeAttachProcess"); if (!fn) return; }
    using function_t = void(__stdcall*)(eprocess_t*);
    reinterpret_cast<function_t>(fn)(process);
}

inline void ke_detach_process() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeDetachProcess"); if (!fn) return; }
    using function_t = void(__stdcall*)();
    reinterpret_cast<function_t>(fn)();
}

inline void ke_stack_attach_process(eprocess_t* process, kapc_state_t* apc_state) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeStackAttachProcess"); if (!fn) return; }
    using function_t = void(__stdcall*)(eprocess_t*, kapc_state_t*);
    reinterpret_cast<function_t>(fn)(process, apc_state);
}

inline void ke_unstack_detach_process(kapc_state_t* apc_state) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeUnstackDetachProcess"); if (!fn) return; }
    using function_t = void(__stdcall*)(kapc_state_t*);
    reinterpret_cast<function_t>(fn)(apc_state);
}

inline std::uint64_t ke_query_performance_counter(large_integer_t* performance_frequency) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeQueryPerformanceCounter"); if (!fn) return 0; }
    using function_t = std::uint64_t(__stdcall*)(large_integer_t*);
    return reinterpret_cast<function_t>(fn)(performance_frequency);
}

inline void ke_query_system_time(large_integer_t* current_time) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeQuerySystemTime"); if (!fn) return; }
    using function_t = void(__stdcall*)(large_integer_t*);
    reinterpret_cast<function_t>(fn)(current_time);
}

inline std::uint32_t ke_query_active_processor_count(std::uint64_t* active_processors) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeQueryActiveProcessorCount"); if (!fn) return 0; }
    using function_t = std::uint32_t(__stdcall*)(std::uint64_t*);
    return reinterpret_cast<function_t>(fn)(active_processors);
}

inline std::uint32_t ke_query_maximum_processor_count() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeQueryMaximumProcessorCount"); if (!fn) return 0; }
    using function_t = std::uint32_t(__stdcall*)();
    return reinterpret_cast<function_t>(fn)();
}

inline std::uint32_t ke_get_current_processor_number() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("KeGetCurrentProcessorNumber"); if (!fn) return 0; }
    using function_t = std::uint32_t(__stdcall*)();
    return reinterpret_cast<function_t>(fn)();
}

inline nt_status_t io_create_driver(unicode_string_t* driver_name, void* function_address) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoCreateDriver"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(unicode_string_t*, void*);
    return reinterpret_cast<function_t>(fn)(driver_name, function_address);
}

inline void io_delete_driver(driver_object_t* driver_object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoDeleteDriver"); if (!fn) return; }
    using function_t = void(__stdcall*)(driver_object_t*);
    reinterpret_cast<function_t>(fn)(driver_object);
}

inline nt_status_t io_create_device(driver_object_t* driver_object, std::uint32_t device_extension_size, unicode_string_t* device_name, std::uint32_t device_type, std::uint32_t device_characteristics, std::uint8_t exclusive, device_object_t** device_object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoCreateDevice"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(driver_object_t*, std::uint32_t, unicode_string_t*, std::uint32_t, std::uint32_t, std::uint8_t, device_object_t**);
    return reinterpret_cast<function_t>(fn)(driver_object, device_extension_size, device_name, device_type, device_characteristics, exclusive, device_object);
}

inline void io_delete_device(device_object_t* device_object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoDeleteDevice"); if (!fn) return; }
    using function_t = void(__stdcall*)(device_object_t*);
    reinterpret_cast<function_t>(fn)(device_object);
}

inline device_object_t* io_attach_device_to_device_stack(device_object_t* source_device, device_object_t* target_device) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoAttachDeviceToDeviceStack"); if (!fn) return nullptr; }
    using function_t = device_object_t * (__stdcall*)(device_object_t*, device_object_t*);
    return reinterpret_cast<function_t>(fn)(source_device, target_device);
}

inline void io_detach_device(device_object_t* target_device) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoDetachDevice"); if (!fn) return; }
    using function_t = void(__stdcall*)(device_object_t*);
    reinterpret_cast<function_t>(fn)(target_device);
}

inline nt_status_t io_create_symbolic_link(unicode_string_t* symbolic_link_name, unicode_string_t* device_name) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoCreateSymbolicLink"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(unicode_string_t*, unicode_string_t*);
    return reinterpret_cast<function_t>(fn)(symbolic_link_name, device_name);
}

inline nt_status_t io_delete_symbolic_link(unicode_string_t* symbolic_link_name) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoDeleteSymbolicLink"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(unicode_string_t*);
    return reinterpret_cast<function_t>(fn)(symbolic_link_name);
}

inline irp_t* io_allocate_irp(std::int8_t stack_size, std::uint8_t charge_quota) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoAllocateIrp"); if (!fn) return nullptr; }
    using function_t = irp_t * (__stdcall*)(std::int8_t, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(stack_size, charge_quota);
}

inline void io_free_irp(irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoFreeIrp"); if (!fn) return; }
    using function_t = void(__stdcall*)(irp_t*);
    reinterpret_cast<function_t>(fn)(irp);
}

inline void io_complete_request(irp_t* irp, std::int8_t priority_boost) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoCompleteRequest"); if (!fn) return; }
    using function_t = void(__stdcall*)(irp_t*, std::int8_t);
    reinterpret_cast<function_t>(fn)(irp, priority_boost);
}

inline nt_status_t io_call_driver(device_object_t* device_object, irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoCallDriver"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(device_object_t*, irp_t*);
    return reinterpret_cast<function_t>(fn)(device_object, irp);
}

inline io_stack_location_t* io_get_current_irp_stack_location(irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoGetCurrentIrpStackLocation"); if (!fn) return nullptr; }
    using function_t = io_stack_location_t * (__stdcall*)(irp_t*);
    return reinterpret_cast<function_t>(fn)(irp);
}

inline io_stack_location_t* io_get_next_irp_stack_location(irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoGetNextIrpStackLocation"); if (!fn) return nullptr; }
    using function_t = io_stack_location_t * (__stdcall*)(irp_t*);
    return reinterpret_cast<function_t>(fn)(irp);
}

inline void io_skip_current_irp_stack_location(irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoSkipCurrentIrpStackLocation"); if (!fn) return; }
    using function_t = void(__stdcall*)(irp_t*);
    reinterpret_cast<function_t>(fn)(irp);
}

inline void io_copy_current_irp_stack_location_to_next(irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoCopyCurrentIrpStackLocationToNext"); if (!fn) return; }
    using function_t = void(__stdcall*)(irp_t*);
    reinterpret_cast<function_t>(fn)(irp);
}

inline void io_set_completion_routine(irp_t* irp, void* completion_routine, void* context, std::uint8_t invoke_on_success, std::uint8_t invoke_on_error, std::uint8_t invoke_on_cancel) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoSetCompletionRoutine"); if (!fn) return; }
    using function_t = void(__stdcall*)(irp_t*, void*, void*, std::uint8_t, std::uint8_t, std::uint8_t);
    reinterpret_cast<function_t>(fn)(irp, completion_routine, context, invoke_on_success, invoke_on_error, invoke_on_cancel);
}

inline void io_mark_irp_pending(irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoMarkIrpPending"); if (!fn) return; }
    using function_t = void(__stdcall*)(irp_t*);
    reinterpret_cast<function_t>(fn)(irp);
}

inline nt_status_t io_get_device_object_pointer(unicode_string_t* object_name, std::uint32_t desired_access, file_object_t** file_object, device_object_t** device_object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoGetDeviceObjectPointer"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(unicode_string_t*, std::uint32_t, file_object_t**, device_object_t**);
    return reinterpret_cast<function_t>(fn)(object_name, desired_access, file_object, device_object);
}

inline device_object_t* io_get_attached_device_reference(device_object_t* device_object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoGetAttachedDeviceReference"); if (!fn) return nullptr; }
    using function_t = device_object_t * (__stdcall*)(device_object_t*);
    return reinterpret_cast<function_t>(fn)(device_object);
}

inline device_object_t* io_get_lower_device_object(device_object_t* device_object) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoGetLowerDeviceObject"); if (!fn) return nullptr; }
    using function_t = device_object_t * (__stdcall*)(device_object_t*);
    return reinterpret_cast<function_t>(fn)(device_object);
}

inline nt_status_t io_register_driver_reinitialization(driver_object_t* driver_object, void* driver_reinitialize_routine, void* context) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoRegisterDriverReinitialization"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(driver_object_t*, void*, void*);
    return reinterpret_cast<function_t>(fn)(driver_object, driver_reinitialize_routine, context);
}

inline nt_status_t rtl_guid_to_string(const guid_t& guid, unicode_string_t* guid_string) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlStringFromGUID"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(const guid_t&, unicode_string_t*);
    return reinterpret_cast<function_t>(fn)(guid, guid_string);
}

inline nt_status_t rtl_guid_from_string(unicode_string_t** guid_in, guid_t* guid_out) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlGUIDFromString"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(unicode_string_t**, guid_t*);
    return reinterpret_cast<function_t>(fn)(guid_in, guid_out);
}

inline void rtl_init_unicode_string(unicode_string_t* destination_string, const wchar_t* source_string) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlInitUnicodeString"); if (!fn) return; }
    using function_t = void(__stdcall*)(unicode_string_t*, const wchar_t*);
    reinterpret_cast<function_t>(fn)(destination_string, source_string);
}

inline void rtl_copy_unicode_string(unicode_string_t* destination_string, unicode_string_t* source_string) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlCopyUnicodeString"); if (!fn) return; }
    using function_t = void(__stdcall*)(unicode_string_t*, unicode_string_t*);
    reinterpret_cast<function_t>(fn)(destination_string, source_string);
}

inline std::int32_t rtl_compare_unicode_string(unicode_string_t* string1, unicode_string_t* string2, std::uint8_t case_insensitive) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlCompareUnicodeString"); if (!fn) return 0; }
    using function_t = std::int32_t(__stdcall*)(unicode_string_t*, unicode_string_t*, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(string1, string2, case_insensitive);
}

inline std::uint8_t rtl_equal_unicode_string(unicode_string_t* string1, unicode_string_t* string2, std::uint8_t case_insensitive) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlEqualUnicodeString"); if (!fn) return 0; }
    using function_t = std::uint8_t(__stdcall*)(unicode_string_t*, unicode_string_t*, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(string1, string2, case_insensitive);
}

inline void rtl_free_unicode_string(unicode_string_t* unicode_string) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlFreeUnicodeString"); if (!fn) return; }
    using function_t = void(__stdcall*)(unicode_string_t*);
    reinterpret_cast<function_t>(fn)(unicode_string);
}

inline nt_status_t rtl_append_unicode_string_to_string(unicode_string_t* destination, unicode_string_t* source) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlAppendUnicodeStringToString"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(unicode_string_t*, unicode_string_t*);
    return reinterpret_cast<function_t>(fn)(destination, source);
}

inline nt_status_t rtl_append_unicode_to_string(unicode_string_t* destination, const wchar_t* source) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlAppendUnicodeToString"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(unicode_string_t*, const wchar_t*);
    return reinterpret_cast<function_t>(fn)(destination, source);
}

inline nt_status_t rtl_ansi_string_to_unicode_string(unicode_string_t* destination_string, string_t* source_string, std::uint8_t allocate_destination_string) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlAnsiStringToUnicodeString"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(unicode_string_t*, string_t*, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(destination_string, source_string, allocate_destination_string);
}

inline nt_status_t rtl_unicode_string_to_ansi_string(string_t* destination_string, unicode_string_t* source_string, std::uint8_t allocate_destination_string) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlUnicodeStringToAnsiString"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(string_t*, unicode_string_t*, std::uint8_t);
    return reinterpret_cast<function_t>(fn)(destination_string, source_string, allocate_destination_string);
}

inline void rtl_copy_memory(void* destination, const void* source, std::size_t length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlCopyMemory"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*, const void*, std::size_t);
    reinterpret_cast<function_t>(fn)(destination, source, length);
}

inline void rtl_zero_memory(void* destination, std::size_t length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlZeroMemory"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*, std::size_t);
    reinterpret_cast<function_t>(fn)(destination, length);
}

inline bool rtl_equal_memory(void* destination, void* source, std::size_t length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlEqualMemory"); if (!fn) return false; }
    using function_t = bool(__stdcall*)(void*, void*, std::size_t);
    reinterpret_cast<function_t>(fn)(destination, source, length);
}

inline void rtl_fill_memory(void* destination, std::size_t length, std::uint8_t fill) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlFillMemory"); if (!fn) return; }
    using function_t = void(__stdcall*)(void*, std::size_t, std::uint8_t);
    reinterpret_cast<function_t>(fn)(destination, length, fill);
}

inline std::int32_t rtl_compare_memory(const void* source1, const void* source2, std::size_t length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlCompareMemory"); if (!fn) return 0; }
    using function_t = std::int32_t(__stdcall*)(const void*, const void*, std::size_t);
    return reinterpret_cast<function_t>(fn)(source1, source2, length);
}

inline std::uint32_t rtl_random(std::uint32_t* seed) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlRandom"); if (!fn) return 0; }
    using function_t = std::uint32_t(__stdcall*)(std::uint32_t*);
    return reinterpret_cast<function_t>(fn)(seed);
}

inline nt_status_t rtl_get_version(void* version_information) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlGetVersion"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(version_information);
}

inline void* rtl_image_nt_header(void* base) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlImageNtHeader"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(base);
}

inline void* rtl_image_directory_entry_to_data(void* base, std::uint8_t mapped_as_image, std::uint16_t directory_entry, std::uint32_t* size) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlImageDirectoryEntryToData"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(void*, std::uint8_t, std::uint16_t, std::uint32_t*);
    return reinterpret_cast<function_t>(fn)(base, mapped_as_image, directory_entry, size);
}

inline void* rtl_find_exported_routine_by_name(void* base, const char* routine_name) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlFindExportedRoutineByName"); if (!fn) return nullptr; }
    using function_t = void* (__stdcall*)(void*, const char*);
    return reinterpret_cast<function_t>(fn)(base, routine_name);
}

inline nt_status_t rtl_query_registry_values(std::uint32_t relative_to, const wchar_t* path, void* query_table, void* context, void* environment) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlQueryRegistryValues"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(std::uint32_t, const wchar_t*, void*, void*, void*);
    return reinterpret_cast<function_t>(fn)(relative_to, path, query_table, context, environment);
}

inline nt_status_t rtl_write_registry_value(std::uint32_t relative_to, const wchar_t* path, const wchar_t* value_name, std::uint32_t value_type, void* value_data, std::uint32_t value_length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlWriteRegistryValue"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(std::uint32_t, const wchar_t*, const wchar_t*, std::uint32_t, void*, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(relative_to, path, value_name, value_type, value_data, value_length);
}

inline nt_status_t rtl_delete_registry_value(std::uint32_t relative_to, const wchar_t* path, const wchar_t* value_name) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("RtlDeleteRegistryValue"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(std::uint32_t, const wchar_t*, const wchar_t*);
    return reinterpret_cast<function_t>(fn)(relative_to, path, value_name);
}

template<class... args_t>
inline std::uint8_t dbg_print(const char* format, args_t... va_args) {
    static std::uint8_t* fn = nullptr;
    if (!fn) {
        fn = get_system_routine("DbgPrintEx");
        if (!fn) return 0;
    }
    using function_t = std::uint8_t(__stdcall*)(std::uint32_t, std::uint32_t, const char*, args_t... va_args);
    return reinterpret_cast<function_t>(fn)(0, 0, format, va_args...);
}

inline nt_status_t zw_query_system_information(system_information_class_e system_information_class, void* system_information, ULONG system_information_length, ULONG* return_length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ZwQuerySystemInformation"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(system_information_class_e, void*, ULONG, ULONG*);
    return reinterpret_cast<function_t>(fn)(system_information_class, system_information, system_information_length, return_length);
}

inline nt_status_t zw_set_system_information(system_information_class_e system_information_class, void* system_information, std::uint32_t system_information_length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ZwSetSystemInformation"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(system_information_class_e, void*, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(system_information_class, system_information, system_information_length);
}

inline nt_status_t zw_open_process(void** process_handle, std::uint32_t desired_access, void* object_attributes, client_id_t* client_id) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ZwOpenProcess"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void**, std::uint32_t, void*, client_id_t*);
    return reinterpret_cast<function_t>(fn)(process_handle, desired_access, object_attributes, client_id);
}

inline nt_status_t zw_open_thread(void** thread_handle, std::uint32_t desired_access, void* object_attributes, client_id_t* client_id) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ZwOpenThread"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void**, std::uint32_t, void*, client_id_t*);
    return reinterpret_cast<function_t>(fn)(thread_handle, desired_access, object_attributes, client_id);
}

inline nt_status_t zw_close(void* handle) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ZwClose"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*);
    return reinterpret_cast<function_t>(fn)(handle);
}

inline nt_status_t zw_query_virtual_memory(void* process_handle, void* base_address, memory_information_class_e memory_information_class, void* memory_information, std::size_t memory_information_length, std::size_t* return_length) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ZwQueryVirtualMemory"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, void*, memory_information_class_e, void*, std::size_t, std::size_t*);
    return reinterpret_cast<function_t>(fn)(process_handle, base_address, memory_information_class, memory_information, memory_information_length, return_length);
}

inline nt_status_t zw_allocate_virtual_memory(void* process_handle, void** base_address, std::uint64_t zero_bits, std::size_t* region_size, std::uint32_t allocation_type, std::uint32_t protect) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ZwAllocateVirtualMemory"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, void**, std::uint64_t, std::size_t*, std::uint32_t, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(process_handle, base_address, zero_bits, region_size, allocation_type, protect);
}

inline nt_status_t zw_free_virtual_memory(void* process_handle, void** base_address, std::size_t* region_size, std::uint32_t free_type) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("ZwFreeVirtualMemory"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void*, void**, std::size_t*, std::uint32_t);
    return reinterpret_cast<function_t>(fn)(process_handle, base_address, region_size, free_type);
}

inline nt_status_t cm_register_callback_ex(ex_callback_function_t callback_function, unicode_string_t altitude, large_integer_t* cookie) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("CmRegisterCallbackEx"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(ex_callback_function_t, unicode_string_t*, void*, void*, large_integer_t*, std::uint64_t);
    return reinterpret_cast<function_t>(fn)(callback_function, &altitude, reinterpret_cast<void*>(1), nullptr, cookie, 0);
}

inline nt_status_t io_allocate_mdl(void* virtual_address, std::size_t length, bool secondary_buffer, bool charge_quota, irp_t* irp) {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("IoAllocateMdl"); if (!fn) return nt_status_t::invalid_address; }
    using function_t = nt_status_t(__stdcall*)(void* virtual_address, std::size_t length, bool secondary_buffer, bool charge_quota, irp_t* irp);
    return reinterpret_cast<function_t>(fn)(virtual_address, length, secondary_buffer, charge_quota, irp);
}

inline std::uintptr_t ps_initial_system_process() {
    static std::uint8_t* fn = nullptr;
    if (!fn) { fn = get_system_routine("PsInitialSystemProcess"); if (!fn) return 0; }
    return *reinterpret_cast<std::uintptr_t*>(fn);
}

inline list_entry_t* ps_active_process_head() {
    static list_entry_t* ps_active_process_head = nullptr;
    if (!ps_active_process_head) {
        static std::uint8_t* fn = nullptr;
        if (!fn) {
            fn = reinterpret_cast<std::uint8_t*>(get_system_routine("KeCapturePersistentThreadState"));
            if (!fn) return {};
        }

        while (fn[0x0] != 0x20
            || fn[0x1] != 0x48
            || fn[0x2] != 0x8d)
            fn++;

        ps_active_process_head = *reinterpret_cast<list_entry_t**>
            (&fn[0x8] + *reinterpret_cast<std::int32_t*>(&fn[0x4]));
    }

    return ps_active_process_head;
}

inline std::uint32_t get_image_file_name_offset() {
    static std::uint32_t image_file_name_offset = 0;
    if (!image_file_name_offset) {
        auto ps_get_process_image_file_name = reinterpret_cast<std::uint8_t*>(get_system_routine("PsGetProcessImageFileName"));

        if (!ps_get_process_image_file_name) {
            return 0;
        }

        while (!(ps_get_process_image_file_name[0] == 0x48 &&
            ps_get_process_image_file_name[1] == 0x8D &&
            ps_get_process_image_file_name[2] == 0x81)) {
            ps_get_process_image_file_name++;
        }

        image_file_name_offset = *reinterpret_cast<std::uint32_t*>(ps_get_process_image_file_name + 3);
    }

    return image_file_name_offset;
}

inline std::uint32_t get_section_base_address_offset() {
    static std::uint32_t section_base_address_offset = 0;
    if (!section_base_address_offset) {
        auto ps_get_process_section_base_address = reinterpret_cast<std::uint8_t*>(get_system_routine("PsGetProcessSectionBaseAddress"));

        if (!ps_get_process_section_base_address)
            return { };

        while (!(ps_get_process_section_base_address[0] == 0x48 ||
            ps_get_process_section_base_address[1] == 0x8B ||
            ps_get_process_section_base_address[2] == 0x81))
            ps_get_process_section_base_address++;

        section_base_address_offset = *reinterpret_cast<std::uint32_t*>(ps_get_process_section_base_address + 3);
    }

    return section_base_address_offset;
}

#define nt_current_process() ( (void*)(LONGLONG) -1 )  
#define zw_current_process() nt_current_process()         
#define nt_current_thread() ( (void*)(LONGLONG) -2 )   
#define zw_current_thread() nt_current_thread()           
#define nt_current_session() ( (void*)(LONGLONG) -3 )  
#define zw_current_session() nt_current_session()

#endif exportsH // exportsH
#ifndef krnlH
#define krnlH

struct unicode_string_t;
struct list_entry_t;
struct single_list_entry_t;
struct kthread_t;
struct ethread_t;
struct kprocess_t;
struct eprocess_t;
struct mmvad_short_t;
struct teb_t;
struct device_object_t;


/* enums */
enum class pool_type_e : std::int32_t {
    non_paged_pool = 0,
    paged_pool = 1,
    non_paged_pool_must_succeed = 2,
    dont_use_this_type = 3,
    non_paged_pool_cache_aligned = 4,
    paged_pool_cache_aligned = 5,
    non_paged_pool_cache_aligned_must_s = 6,
    max_pool_type = 7,
    non_paged_pool_session = 32,
    paged_pool_session = 33,
    non_paged_pool_must_succeed_session = 34,
    dont_use_this_type_session = 35,
    non_paged_pool_cache_aligned_session = 36,
    paged_pool_cache_aligned_session = 37,
    non_paged_pool_cache_aligned_must_s_session = 38,
    non_paged_pool_nx = 512,
    non_paged_pool_nx_cache_aligned = non_paged_pool_nx + 4,
    non_paged_pool_session_nx = non_paged_pool_nx + 32,

    pool_zero_allocation = 1024
};

constexpr pool_type_e operator|(pool_type_e a, pool_type_e b)
{
    return static_cast<pool_type_e>(static_cast<std::int32_t>(a) | static_cast<std::int32_t>(b));
}

enum class memory_caching_type_e : std::int32_t {
    mm_non_cached = 0,
    mm_cached = 1,
    mm_write_combined = 2,
    mm_hardware_coherent_cached = 3,
    mm_non_cached_unordered = 4,
    mm_uswc_cached = 5,
    mm_maximum_cache_type = 6
};

enum class mode_e : std::uint8_t {
    kernel_mode = 0,
    user_mode = 1,
    max_mode = 2
};

enum class mm_copy_flags_e : ULONG {
    physical_address = 0x1,
    virtual_address = 0x2
};

enum class kwait_reason_e : std::int32_t {
    executive = 0,
    free_page = 1,
    page_in = 2,
    pool_allocation = 3,
    delay_execution = 4,
    suspended = 5,
    user_request = 6,
    wr_executive = 7,
    wr_free_page = 8,
    wr_page_in = 9,
    wr_pool_allocation = 10,
    wr_delay_execution = 11,
    wr_suspended = 12,
    wr_user_request = 13,
    wr_event_pair = 14,
    wr_queue = 15,
    wr_lpc_receive = 16,
    wr_lpc_reply = 17,
    wr_virtual_memory = 18,
    wr_page_out = 19,
    wr_rendez_vous = 20,
    wr_keyed_event = 21,
    wr_terminated = 22,
    wr_process_in_swap = 23,
    wr_cpu_rate_control = 24,
    wr_callout_stack = 25,
    wr_kernel = 26,
    wr_resource = 27,
    wr_push_lock = 28,
    wr_mutex = 29,
    wr_quantum_end = 30,
    wr_dispatch_int = 31,
    wr_preempted = 32,
    wr_yield_execution = 33,
    wr_fast_mutex = 34,
    wr_guard_mutex = 35,
    wr_rundown = 36,
    maximum_wait_reason = 37
};

enum class kthread_state_e : std::uint8_t {
    initialized = 0,
    ready = 1,
    running = 2,
    standby = 3,
    terminated = 4,
    waiting = 5,
    transition = 6,
    deferred_ready = 7,
    gate_waiting = 8
};

enum class kobjects_e : std::uint8_t {
    event_notification_object = 0,
    event_synchronization_object = 1,
    mutant_object = 2,
    process_object = 3,
    queue_object = 4,
    semaphore_object = 5,
    thread_object = 6,
    gate_object = 7,
    timer_notification_object = 8,
    timer_synchronization_object = 9,
    spare2_object = 10,
    spare3_object = 11,
    spare4_object = 12,
    spare5_object = 13,
    spare6_object = 14,
    spare7_object = 15,
    spare8_object = 16,
    spare9_object = 17,
    apc_object = 18,
    dpc_object = 19,
    device_queue_object = 20,
    event_pair_object = 21,
    interrupt_object = 22,
    profile_object = 23,
    thread_count_object = 24,
    maximum_kernel_object = 25
};

enum class kprofile_source_e : std::int32_t {
    profile_time = 0,
    profile_alignement_fixup = 1,
    profile_total_issues = 2,
    profile_pipeline_dry = 3,
    profile_load_instructions = 4,
    profile_pipeline_frozen = 5,
    profile_branch_instructions = 6,
    profile_total_non_issues = 7,
    profile_dcache_misses = 8,
    profile_icache_misses = 9,
    profile_cache_flush_count = 10,
    profile_branches_mispredicted = 11,
    profile_store_instructions = 12,
    profile_maximum = 13
};

enum class event_type_e : std::int32_t {
    notification_event = 0,
    synchronization_event = 1
};

enum class timer_type_e : std::int32_t {
    notification_timer = 0,
    synchronization_timer = 1
};

enum class wait_type_e : std::int32_t {
    wait_all = 0,
    wait_any = 1,
    wait_notification = 2
};

enum class irql_e : std::uint8_t {
    passive_level = 0,
    low_level = 0,
    apc_level = 1,
    dispatch_level = 2,
    cmci_level = 5,
    clock_level = 13,
    ipi_level = 14,
    drs_level = 14,
    power_level = 14,
    profile_level = 15,
    high_level = 15
};

enum class priority_class_e : std::uint8_t {
    idle_priority_class = 0,
    below_normal_priority_class = 1,
    normal_priority_class = 2,
    above_normal_priority_class = 3,
    high_priority_class = 4,
    realtime_priority_class = 5
};

enum class memory_information_class_e : std::int32_t {
    memory_basic_information = 0,
    memory_working_set_information = 1,
    memory_mapped_filename_information = 2,
    memory_region_information = 3,
    memory_working_set_ex_information = 4,
    memory_shared_commit_information = 5,
    memory_image_information = 6,
    memory_region_information_ex = 7,
    memory_privileges_information = 8,
    memory_encoded_commit_information = 9
};

enum class section_inherit_e : std::int32_t {
    view_share = 1,
    view_unmap = 2
};

enum class lock_operation_e : std::int32_t {
    io_read_access = 0,
    io_write_access = 1,
    io_modify_access = 2
};

enum class io_priority_hint_e : std::int32_t {
    io_priority_very_low = 0,
    io_priority_low = 1,
    io_priority_normal = 2,
    io_priority_high = 3,
    io_priority_critical = 4,
    max_io_priority_types = 5
};

enum class system_information_class_e : std::int32_t {
    system_basic_information = 0,
    system_processor_information = 1,
    system_performance_information = 2,
    system_time_of_day_information = 3,
    system_path_information = 4,
    system_process_information = 5,
    system_call_count_information = 6,
    system_device_information = 7,
    system_processor_performance_information = 8,
    system_flags_information = 9,
    system_call_time_information = 10,
    system_module_information = 11,
    system_locks_information = 12,
    system_stack_trace_information = 13,
    system_paged_pool_information = 14,
    system_non_paged_pool_information = 15,
    system_handle_information = 16,
    system_object_information = 17,
    system_page_file_information = 18,
    system_vdm_instemul_information = 19,
    system_vdm_bop_information = 20,
    system_file_cache_information = 21,
    system_pool_tag_information = 22,
    system_interrupt_information = 23,
    system_dpc_behavior_information = 24,
    system_full_memory_information = 25,
    system_load_gdi_driver_information = 26,
    system_unload_gdi_driver_information = 27,
    system_time_adjustment_information = 28,
    system_summary_memory_information = 29,
    system_next_event_id_information = 30,
    system_event_ids_information = 31,
    system_crashdump_information = 32,
    system_exception_information = 33,
    system_crashdump_state_information = 34,
    system_kernel_debugger_information = 35,
    system_context_switch_information = 36,
    system_registry_quota_information = 37,
    system_extend_service_table_information = 38,
    system_priority_separation = 39,
    system_plug_play_bus_information = 40,
    system_dock_information = 41,
    system_power_information_native = 42,
    system_processor_speed_information = 43,
    system_current_time_zone_information = 44,
    system_lookaside_information = 45,
    system_bigpool_information = 66,
    system_session_pool_tagging_information = 68,
    system_interrupt_steering_information = 182,
    system_kernel_va_shadow_information = 196,
    system_speculation_control_information = 201
};

enum class file_information_class_e : std::int32_t {
    file_directory_information = 1,
    file_full_directory_information = 2,
    file_both_directory_information = 3,
    file_basic_information = 4,
    file_standard_information = 5,
    file_internal_information = 6,
    file_ea_information = 7,
    file_access_information = 8,
    file_name_information = 9,
    file_rename_information = 10,
    file_link_information = 11,
    file_names_information = 12,
    file_disposition_information = 13,
    file_position_information = 14,
    file_full_ea_information = 15,
    file_mode_information = 16,
    file_alignment_information = 17,
    file_all_information = 18,
    file_allocation_information = 19,
    file_end_of_file_information = 20,
    file_alternate_name_information = 21,
    file_stream_information = 22,
    file_pipe_information = 23,
    file_pipe_local_information = 24,
    file_pipe_remote_information = 25,
    file_mailslot_query_information = 26,
    file_mailslot_set_information = 27,
    file_compression_information = 28,
    file_object_id_information = 29,
    file_completion_information = 30,
    file_move_cluster_information = 31,
    file_quota_information = 32,
    file_reparse_point_information = 33,
    file_network_open_information = 34,
    file_attribute_tag_information = 35,
    file_tracking_information = 36,
    file_id_both_directory_information = 37,
    file_id_full_directory_information = 38,
    file_valid_data_length_information = 39,
    file_short_name_information = 40,
    file_maximum_information = 41
};

enum pe_magic_e {
    dos_header = 0x5a4d,
    nt_headers = 0x4550,
    opt_header = 0x020b
};

struct list_entry_t {
    list_entry_t* m_flink;
    list_entry_t* m_blink;
};

struct single_list_entry_t {
    single_list_entry_t* m_next;
};

struct unicode_string_t {
    std::uint16_t m_length;
    std::uint16_t m_maximum_length;
    wchar_t* m_buffer;
};

struct string_t {
    std::uint16_t m_length;
    std::uint16_t m_maximum_length;
    char* m_buffer;
};

struct large_integer_t {
    union {
        struct {
            std::uint32_t m_low_part;
            std::int32_t m_high_part;
        };
        struct {
            std::uint32_t m_low_part_alt;
            std::int32_t m_high_part_alt;
        } u;
        std::int64_t m_quad_part;
    };
};

using physical_address_t = large_integer_t;

struct mm_copy_address_t {
    union {
        void* virtual_address;
        physical_address_t physical_address;
    };
};

struct physical_memory_range_t {
    physical_address_t BaseAddress;
    large_integer_t NumberOfBytes;
};


struct guid_t {
    std::uint32_t  m_data1;
    std::uint16_t m_data2;
    std::uint16_t m_data3;
    std::uint8_t  m_data4[8];
};

struct luid_t {
    std::uint32_t m_low_part;
    std::int32_t m_high_part;
};

struct dispatcher_header_t {
    union {
        struct {
            std::uint8_t m_type;
            std::uint8_t m_signalling;
            std::uint8_t m_size;
            std::uint8_t m_reserved1;
        };
        std::int32_t m_lock;
    };
    std::int32_t m_signal_state;
    list_entry_t m_wait_list_head;
};

struct kevent_t {
    dispatcher_header_t m_header;
};

struct kmutant_t {
    dispatcher_header_t m_header;
    list_entry_t m_mutant_list_entry;
    void* m_owner_thread;
    std::uint8_t m_abandoned;
    std::uint8_t m_apc_disable;
};

struct ksemaphore_t {
    dispatcher_header_t m_header;
    std::int32_t m_limit;
};

struct kqueue_t {
    dispatcher_header_t m_header;
    list_entry_t m_entry_list_head;
    std::uint32_t m_current_count;
    std::uint32_t m_maximum_count;
    list_entry_t m_thread_list_head;
};

struct kgate_t {
    dispatcher_header_t m_header;
};

struct fast_mutex_t {
    std::int32_t m_count;
    void* m_owner;
    std::uint32_t m_contention;
    kevent_t m_event;
    std::uint32_t m_old_irql;
};

struct ex_push_lock_t {
    union {
        struct {
            std::uint64_t m_locked : 1;
            std::uint64_t m_waiting : 1;
            std::uint64_t m_waking : 1;
            std::uint64_t m_multiple_shared : 1;
            std::uint64_t m_shared : 60;
        };
        std::uint64_t m_value;
        void* m_ptr;
    };
};

struct eresource_t {
    list_entry_t m_system_resource_list;
    void* m_owner_table;
    std::int16_t m_active_count;
    std::uint16_t m_flag;
    std::uint32_t m_padding1;
    void* m_shared_waiters;
    void* m_exclusive_waiters;
    list_entry_t m_owner_entry;
    std::uint32_t m_active_entries;
    std::uint32_t m_contention_count;
    std::uint32_t m_number_of_shared_waiters;
    std::uint32_t m_number_of_exclusive_waiters;
    void* m_reserved2;
    union {
        void* m_address;
        std::uint64_t m_creator_back_trace_index;
    };
    std::uint64_t m_spin_lock;
};

struct kdpc_t {
    union {
        std::uint32_t m_target_info_as_ulong;
        struct {
            std::uint8_t m_type;
            std::uint8_t m_importance;
            std::uint16_t m_number;
        };
    };
    single_list_entry_t m_dpc_list_entry;
    std::uint64_t m_processor_history;
    void* m_deferred_routine;
    void* m_deferred_context;
    void* m_system_argument1;
    void* m_system_argument2;
    void* m_dpc_data;
};

struct ktimer_t {
    dispatcher_header_t m_header;
    large_integer_t m_due_time;
    list_entry_t m_timer_list_entry;
    kdpc_t* m_dpc;
    std::int32_t m_processor;
    std::int32_t m_period;
};

struct kdpc_data_t {
    list_entry_t m_dpc_list_head;
    std::uint64_t m_dpc_lock;
    std::int32_t m_dpc_queue_depth;
    std::uint32_t m_dpc_count;
};

struct kapc_t {
    std::uint8_t m_type;
    std::uint8_t m_spare_byte0;
    std::uint8_t m_size;
    std::uint8_t m_spare_byte1;
    std::uint32_t m_spare_long0;
    kthread_t* m_thread;
    list_entry_t m_apc_list_entry;
    void* m_kernel_routine;
    void* m_rundown_routine;
    void* m_normal_routine;
    void* m_normal_context;
    void* m_system_argument1;
    void* m_system_argument2;
    std::int8_t m_apc_state_index;
    std::int8_t m_apc_mode;
    std::uint8_t m_inserted;
};

//struct kapc_state_t {
//    list_entry_t m_apc_list_head[2];
//    kprocess_t* m_process;
//    std::uint8_t m_kernel_apc_in_progress;
//    std::uint8_t m_kernel_apc_pending;
//    std::uint8_t m_user_apc_pending;
//};

struct kapc_state_t {
    list_entry_t m_apc_list_head[2];
    eprocess_t* m_process;
    std::uint8_t m_kernel_apc_in_progress;
    std::uint8_t m_kernel_apc_pending;
    std::uint8_t m_user_apc_pending;
    std::uint8_t m_pad;
}; // Size: 0x40

struct kwait_block_t {
    list_entry_t m_wait_list_entry;
    std::uint8_t m_wait_type;
    std::uint8_t m_block_state;
    std::uint16_t m_wait_key;
    std::int32_t m_spare_long;
    void* m_object;
    void* m_spare_ptr;
};

struct kaffinity_ex_t {
    std::uint16_t m_count;
    std::uint16_t m_size;
    std::uint32_t m_reserved;
    std::uint64_t m_bitmap[20];
};

struct kwait_status_register_t {
    union {
        std::uint8_t m_flags;
        struct {
            std::uint8_t m_state : 3;
            std::uint8_t m_affinity : 1;
            std::uint8_t m_priority : 1;
            std::uint8_t m_apc : 1;
            std::uint8_t m_user_apc : 1;
            std::uint8_t m_alert : 1;
        };
    };
}; // Size: 0x1

struct ktrap_frame_t {
    std::uint64_t m_p1_home;                    // +0x000
    std::uint64_t m_p2_home;                    // +0x008
    std::uint64_t m_p3_home;                    // +0x010
    std::uint64_t m_p4_home;                    // +0x018
    std::uint64_t m_p5;                         // +0x020

    std::uint8_t m_previous_mode;               // +0x028
    std::uint8_t m_previous_irql;               // +0x029
    std::uint8_t m_fault_indicator;             // +0x02A
    std::uint8_t m_exception_active;            // +0x02B
    std::uint32_t m_mx_csr;                      // +0x02C

    std::uint64_t m_rax;                        // +0x030
    std::uint64_t m_rcx;                        // +0x038
    std::uint64_t m_rdx;                        // +0x040
    std::uint64_t m_r8;                         // +0x048
    std::uint64_t m_r9;                         // +0x050
    std::uint64_t m_r10;                        // +0x058
    std::uint64_t m_r11;                        // +0x060

    union {
        std::uint64_t m_gs_base;                // +0x068
        std::uint64_t m_gs_swap;
    };

    std::m128a_t m_xmm0;                               // +0x070
    std::m128a_t m_xmm1;                               // +0x080
    std::m128a_t m_xmm2;                               // +0x090
    std::m128a_t m_xmm3;                               // +0x0A0
    std::m128a_t m_xmm4;                               // +0x0B0
    std::m128a_t m_xmm5;                               // +0x0C0

    union {
        std::uint64_t m_fault_address;          // +0x0D0
        std::uint64_t m_context_record;
    };

    std::uint64_t m_dr0;                        // +0x0D8
    std::uint64_t m_dr1;                        // +0x0E0
    std::uint64_t m_dr2;                        // +0x0E8
    std::uint64_t m_dr3;                        // +0x0F0
    std::uint64_t m_dr6;                        // +0x0F8
    std::uint64_t m_dr7;                        // +0x100

    // Debug registers block
    std::uint64_t m_debug_control;              // +0x108
    std::uint64_t m_last_branch_to_rip;         // +0x110
    std::uint64_t m_last_branch_from_rip;       // +0x118
    std::uint64_t m_last_exception_to_rip;      // +0x120
    std::uint64_t m_last_exception_from_rip;    // +0x128

    std::uint16_t m_seg_ds;                     // +0x130
    std::uint16_t m_seg_es;                     // +0x132
    std::uint16_t m_seg_fs;                     // +0x134
    std::uint16_t m_seg_gs;                     // +0x136

    std::uint64_t m_nested_trap_frame;                 // +0x138
    std::uint64_t m_rbx;                        // +0x140
    std::uint64_t m_rdi;                        // +0x148
    std::uint64_t m_rsi;                        // +0x150
    std::uint64_t m_rbp;                        // +0x158

    union {
        std::uint64_t m_error_code;             // +0x160
        std::uint64_t m_exception_frame;
    };

    std::uint64_t m_rip;                        // +0x168
    std::uint16_t m_seg_cs;                     // +0x170
    std::uint8_t m_fill0;                       // +0x172
    std::uint8_t m_logging;                     // +0x173
    std::uint16_t m_fill1[2];                   // +0x174
    std::uint32_t m_eflags;                     // +0x178
    std::uint32_t m_fill2;                      // +0x17C
    std::uint64_t m_rsp;                        // +0x180
    std::uint16_t m_seg_ss;                     // +0x188
    std::uint16_t m_fill3;                      // +0x18A
    std::uint32_t m_fill4;                      // +0x18C
}; // Size = 0x190

struct group_affinity_t {
    std::uint64_t m_mask;
    std::uint16_t m_group;
    std::uint16_t m_reserved[3];
}; // Size: 0x10

struct kthread_t {
    // Base section (0x000 - 0x090)
    dispatcher_header_t m_header;                      // +0x000
    void* m_slist_fault_address;                      // +0x018
    std::uint64_t m_quantum_target;                   // +0x020
    void* m_initial_stack;                            // +0x028
    void* m_stack_limit;                              // +0x030
    void* m_stack_base;                              // +0x038
    std::uint64_t m_thread_lock;                     // +0x040
    std::uint64_t m_cycle_time;                      // +0x048
    std::uint32_t m_current_run_time;                // +0x050
    std::uint32_t m_expected_run_time;               // +0x054
    void* m_kernel_stack;                            // +0x058
    void* m_state_save_area;                         // +0x060
    void* m_scheduling_group;                        // +0x068
    kwait_status_register_t m_wait_register;         // +0x070
    std::uint8_t m_running;                          // +0x071
    std::uint8_t m_alerted[2];                      // +0x072
    std::uint32_t m_auto_alignment;                  // +0x074
    std::uint8_t m_tag;                              // +0x07C
    std::uint8_t m_system_hetero_cpu_policy;         // +0x07D
    std::uint8_t m_spare_byte;                       // +0x07E
    std::uint32_t m_system_call_number;              // +0x080
    std::uint32_t m_ready_time;                      // +0x084
    void* m_first_argument;                          // +0x088
    ktrap_frame_t* m_trap_frame;                     // +0x090

    // Callback and system section (0x098 - 0x0F8)
    kapc_state_t m_apc_state;                          // +0x098
    std::uint8_t m_apc_queueable;                    // +0x0A0
    std::uint8_t m_apc_queueable_padding[7];         // +0x0A1
    void* m_service_table;                          // +0x0A8
    std::uint32_t m_kernel_reserve_apc;             // +0x0B0
    std::uint32_t m_kernel_reserve_apc_padding;     // +0x0B4
    void* m_win32_thread;                           // +0x0B8
    void* m_trap_frame_base;                        // +0x0C0
    std::uint64_t m_wait_status;                     // +0x0C8
    void* m_wait_block_list;                         // +0x0D0
    void* m_queue;                                   // +0x0E8
    teb_t* m_teb;                                     // +0x0F0
    std::uint64_t m_relative_timer_bias;             // +0x0F8

    // Timer and flags section (0x100 - 0x177)
    ktimer_t m_timer;                                // +0x100
    std::uint32_t m_misc_flags;                      // +0x170
    std::uint8_t m_kernel_apc_disable;               // +0x174
    std::uint8_t m_kernel_apc_pending;               // +0x175
    std::uint8_t m_io_pending;                       // +0x176
    std::uint8_t m_io_pending_high;                  // +0x177

    // Wait info section (0x178 - 0x1FC)
    std::int64_t m_entropy_count;                    // +0x178
    std::uint32_t m_permission_key;                  // +0x180
    std::uint32_t m_permission_key_non_paged;        // +0x184
    void* m_wait_prcb;                              // +0x188
    void* m_wait_next;                              // +0x190
    void* m_wait_value;                             // +0x198
    void* m_wait_reason;                            // +0x1A0
    std::uint32_t m_wait_irql;                      // +0x1A8
    std::uint8_t m_wait_mode;                       // +0x1AC
    std::uint8_t m_wait_next_flag;                  // +0x1AD
    std::uint8_t m_wait_reason_flag;                // +0x1AE
    std::uint8_t m_wait_response;                   // +0x1AF
    void* m_wait_pointer;                          // +0x1B0
    std::uint32_t m_thread_flags;                    // +0x1B8
    std::uint32_t m_spare0;                         // +0x1BC
    void* m_wait_block_list2;                       // +0x1C0
    std::uint32_t m_wait_block_count;               // +0x1C8
    std::uint32_t m_wait_block_offset;              // +0x1CC
    void* m_wait_blocks;                            // +0x1D0
    // Queue and process section (0x1D8 - 0x250)
    list_entry_t m_wait_list_entry;                 // +0x1D8
    std::uint32_t m_wait_status2;                   // +0x1E8
    std::uint32_t m_wait_state_sequence;            // +0x1EC
    std::uint32_t m_wait_irql_old;                  // +0x1F0
    std::uint32_t m_wait_mode_old;                  // +0x1F4
    std::uint32_t m_wait_queue_timeout;             // +0x1F8
    std::uint32_t m_wait_block_multiple;            // +0x1FC
    void* m_thread_queue_list_entry;                // +0x200
    list_entry_t m_queue_list_entry;                // +0x208
    std::uint16_t m_queue_index;                    // +0x218
    std::uint32_t m_queue_priority;                 // +0x21C
    kprocess_t* m_process;                          // +0x220
    group_affinity_t m_affinity;                    // +0x228
    std::uint64_t m_affinity_version;               // +0x238
    void* m_npx_state;                             // +0x250

    // Performance and kernel stack section (0x258 - 0x2E0)
    void* m_performance_counters;                   // +0x258
    void* m_context_switch_count;                   // +0x260
    void* m_scheduler_assist_thread;                // +0x268
    void* m_kernel_stack_control;                   // +0x270
    void* m_kernel_stack_limit;                     // +0x278
    void* m_kernel_stack_base;                      // +0x280
    void* m_thread_lock_owner;                      // +0x288
    void* m_kernel_wait_always;                     // +0x290
    void* m_user_wait_always;                       // +0x298
    void* m_win32k_thread;                         // +0x2A0
    void* m_worker_routine;                        // +0x2A8
    void* m_worker_context;                        // +0x2B0
    void* m_win32_start_address;                   // +0x2B8
    void* m_lpaccel;                              // +0x2C0
    void* m_lpfnwndproc;                          // +0x2C8
    void* m_win32k_callback;                      // +0x2D0
    void* m_win32k_callback_context;               // +0x2D8

    // Final section (0x2E0 - 0x430)
    kevent_t m_suspend_event;                       // +0x2E0
    list_entry_t m_thread_list_entry;               // +0x2F8
    list_entry_t m_mutant_list_head;                // +0x308
    std::uint8_t m_ab_entry_summary;                // +0x318
    std::uint8_t m_ab_wait_entry_count;             // +0x319
    std::uint8_t m_ab_allocation_region_count;      // +0x31A
    std::uint8_t m_system_priority;                 // +0x31B
    std::uint32_t m_secure_thread_cookie;           // +0x31C
    void* m_lock_entries;                          // +0x320
    single_list_entry_t m_propagate_boosts_entry;   // +0x328
    single_list_entry_t m_io_self_boosts_entry;     // +0x330
    std::uint8_t m_priority_floor_counts[16];       // +0x338
    std::uint8_t m_priority_floor_counts_reserved[16]; // +0x348
    std::uint32_t m_priority_floor_summary;         // +0x358
    std::uint32_t m_ab_completed_io_boost_count;    // +0x35C
    std::uint32_t m_ab_completed_io_qos_boost_count; // +0x360
    std::uint16_t m_ke_reference_count;             // +0x364
    std::uint8_t m_ab_orphaned_entry_summary;       // +0x366
    std::uint8_t m_ab_owned_entry_count;            // +0x367
    std::uint32_t m_foreground_loss_time;           // +0x368
    std::uint64_t m_read_operation_count;           // +0x380
    std::uint64_t m_write_operation_count;          // +0x388
    std::uint64_t m_other_operation_count;          // +0x390
    std::uint64_t m_read_transfer_count;            // +0x398
    std::uint64_t m_write_transfer_count;           // +0x3A0
    std::uint64_t m_other_transfer_count;           // +0x3A8
    void* m_queued_scb;                            // +0x3B0
    std::uint32_t m_thread_timer_delay;             // +0x3B8
    void* m_tracing_private;                        // +0x3C0
    void* m_scheduler_assist;                       // +0x3C8
    void* m_ab_wait_object;                        // +0x3D0
    std::uint32_t m_reserved_previous_ready_time_value; // +0x3D8
    std::uint64_t m_kernel_wait_time;               // +0x3E0
    std::uint64_t m_user_wait_time;                 // +0x3E8
    void* m_explicit_scheduling;                    // +0x3F0
    void* m_debug_active;                          // +0x3F8
    std::uint32_t m_scheduler_assist_priority_floor; // +0x400
    std::uint32_t m_spare28;                       // +0x404
    std::uint8_t m_resource_index;                  // +0x408
    std::uint8_t m_spare31[3];                     // +0x409
    std::uint64_t m_end_padding[4];                // +0x410
}; // Size: 0x430

struct client_id_t {
    void* m_unique_process;
    void* m_unique_thread;
};

struct ethread_t {
    struct kthread_t m_kthread;              // +0x000 - KTHREAD portion
    list_entry_t m_thread_list_entry;                // +0x430
    void* m_create_time;                             // +0x440
    union {
        void* m_exit_time;                           // +0x448
        list_entry_t m_active_execution_list;
    };
    union {
        void* m_exit_status;                         // +0x450
        void* m_post_block_old;
    };
    union {
        void* m_terminate_apc;                       // +0x458
        void* m_terminate_pending;
    };
    union {
        void* m_thread_flags;                        // +0x460
        union {
            std::uint32_t m_thread_flags_value;
            struct {
                std::uint32_t m_terminate_requested : 1;
                std::uint32_t m_dead_thread : 1;
                std::uint32_t m_hide_from_debugger : 1;
                std::uint32_t m_active_impersonation_info : 1;
                std::uint32_t m_system_thread : 1;
                std::uint32_t m_hard_errors_are_disabled : 1;
                std::uint32_t m_break_on_termination : 1;
                std::uint32_t m_skip_creation_msg : 1;
                std::uint32_t m_skip_termination_msg : 1;
            };
        };
        std::uint32_t m_padding;
    };
    union {
        void* m_create_info;                         // +0x468
        void* m_win32_start_address;
    };
    void* m_teb;                                     // +0x470
    client_id_t m_client_id;                       // +0x478
    void* m_security_port;                           // +0x480
    void* m_previous_mode;                           // +0x488
    void* m_resource_index;                          // +0x490
    void* m_large_stack;                             // +0x498
    void* m_fx_save_area;                           // +0x4A0
    void* m_priority_class;                          // +0x4A8
    void* m_desktop;                                 // +0x4B0
    void* m_suspend_semaphore;                       // +0x4B8
    union {
        void* m_win32_thread;                        // +0x4C0
        struct {
            std::uint32_t m_io_priority : 3;
            std::uint32_t m_memory_priority : 3;
            std::uint32_t m_absolute_cpu_priority : 1;
        };
    };
    void* m_working_on_behalf_ticket;                // +0x4C8
    void* m_impersonation_info;                      // +0x4D0
    void* m_io_pending_mr;                           // +0x4D8
    void* m_io_suppress_thread;                      // +0x4E0
    void* m_memory_attribute;                         // +0x4E8
    union {
        void* m_win32_thread_event;                  // +0x4F0
        void* m_running_down;
    };
    void* m_thread_lock;                             // +0x4F8
    std::uint32_t m_read_operation_count;            // +0x500
    std::uint32_t m_write_operation_count;           // +0x504
    std::uint32_t m_other_operation_count;           // +0x508
    std::uint32_t m_io_priority_boost;               // +0x50C
    void* m_io_client_pointer;                       // +0x510
    void* m_file_object;                             // +0x518
    void* m_word_list_head;                          // +0x520
    void* m_process_context;                         // +0x528
    void* m_granted_access;                          // +0x530
    void* m_cross_thread_flags;                      // +0x538
    union {
        std::uint32_t m_cross_thread_flags_uint;     // +0x540
        struct {
            std::uint32_t m_terminated : 1;
            std::uint32_t m_debug_active : 1;
            std::uint32_t m_system_process : 1;
            std::uint32_t m_impersonating : 1;
            std::uint32_t m_break_on_termination : 1;
            std::uint32_t m_reserved : 27;
        };
    };
    std::uint32_t m_cross_thread_flags_padding;      // +0x544
    void* m_start_address;                           // +0x548
    void* m_win32_thread_info;                       // +0x550
    void* m_lpaccel;                                 // +0x558
    void* m_lpfnwndproc;                            // +0x560
    void* m_win32k;                                  // +0x568
}; // Size = 0x570

struct kprocess_t {
    dispatcher_header_t m_header;
    list_entry_t m_profile_list_head;
    std::uint64_t m_directory_table_base;
    list_entry_t m_thread_list_head;
    std::uint32_t m_process_lock;
    std::uint32_t m_process_time_quotas;
    std::uint64_t m_deferred_processor_set;
    kaffinity_ex_t m_affinity;
    std::uint8_t m_base_priority;
    std::int8_t m_quantum_reset;
    std::uint8_t m_visited;
    std::uint8_t m_flags;
    std::uint32_t m_thread_seed[4];
    std::uint16_t m_ideal_node;
    std::uint16_t m_ideal_global_node;
    std::uint16_t m_spare;
    std::uint16_t m_flags_reserved;
};

struct ex_fast_ref_t {
    union {
        void* m_object;
        std::uint64_t m_ref_cnt : 4;
        std::uint64_t m_value;
    };
};

struct se_audit_process_creation_info_t {
    void* m_image_file_name;
};

struct mmsupport_t {
    std::uint64_t m_last_trim_time;
    std::uint64_t m_last_trim_fault_count;
    std::uint64_t m_page_fault_count;
    std::uint64_t m_peak_working_set_size;
    std::uint64_t m_grow_ws_lock;
    std::uint64_t m_minimum_working_set_size;
    std::uint64_t m_maximum_working_set_size;
    void* m_vm_support;
    std::uint32_t m_flags;
    std::uint32_t m_padding;
};

struct mm_avl_node_t {
    mm_avl_node_t* m_left_child;
    mm_avl_node_t* m_right_child;
    union {
        struct {
            std::uint64_t m_balance : 2;
            std::uint64_t m_parent : 62;
        };
        std::uint64_t m_value;
    };
};

struct mmvad_short_t {
    mm_avl_node_t m_vad_node;
    std::uint64_t m_starting_vpn;
    std::uint64_t m_ending_vpn;
    std::uint8_t m_starting_vpn_high;
    std::uint8_t m_ending_vpn_high;
    std::uint8_t m_commit_charge_high;
    std::uint8_t m_spare_nt64_vad_flags;
    std::int32_t m_reference_count;
    ex_push_lock_t m_address_creation_lock;
    void* m_vad_root;
    void* m_vad_hint;
    std::uint64_t m_vad_free_hint;
    std::uint64_t m_clone_root;
    std::uint64_t m_number_of_private_pages;
    std::uint64_t m_number_of_locked_pages;
    void* m_win32_process;
    void* m_job;
    void* m_section_object;
    void* m_section_base_address;
    std::uint32_t m_cookie;
    std::uint32_t m_padding1;
    void* m_work_on_behalf;
    void* m_session;
    list_entry_t m_process_list_entry;
    void* m_image_file_pointer;
    std::uint32_t m_image_base_address;
    std::uint32_t m_session_id;
    large_integer_t m_create_time;
    large_integer_t m_exit_time;
    void* m_ldt_information;
    void* m_vdm_objects;
    std::uint64_t m_console_host_process;
    std::uint32_t m_flags;
    std::uint32_t m_flags2;
    std::uint32_t m_flags3;
    std::uint32_t m_padding2;
    mmsupport_t m_vm;
    std::uint32_t m_modified_page_count;
    std::int32_t m_exit_status;
    mmvad_short_t* m_vad_root_avl; // should be mmvad_t*   
    void* m_allocate_vm_lock;
    void* m_trim_lock;
};

struct peb_ldr_data_t {
    std::uint32_t m_length;
    std::uint8_t m_initialized;
    void* m_ss_handle;
    list_entry_t m_in_load_order_module_list;
    list_entry_t m_in_memory_order_module_list;
    list_entry_t m_in_initialization_order_module_list;
    void* m_entry_in_progress;
    std::uint8_t m_shutdown_in_progress;
    void* m_shutdown_thread_id;
};

struct ldr_data_table_entry_t {
    list_entry_t m_in_load_order_links;
    list_entry_t m_in_memory_order_links;
    list_entry_t m_in_initialization_order_links;
    void* m_dll_base;
    void* m_entry_point;
    std::uint32_t m_size_of_image;
    unicode_string_t m_full_dll_name;
    unicode_string_t m_base_dll_name;
    std::uint32_t m_flags;
    std::uint16_t m_load_count;
    std::uint16_t m_tls_index;
    list_entry_t m_hash_links;
    std::uint32_t m_time_date_stamp;
    void* m_entry_point_activation_context;
    void* m_lock;
    void* m_ddag_node;
    list_entry_t m_node_module_link;
    void* m_load_context;
    void* m_parent_dll_base;
    void* m_switch_back_context;
};

struct rtl_user_process_parameters_t {
    std::uint32_t m_maximum_length;
    std::uint32_t m_length;
    std::uint32_t m_flags;
    std::uint32_t m_debug_flags;
    void* m_console_handle;
    std::uint32_t m_console_flags;
    void* m_standard_input;
    void* m_standard_output;
    void* m_standard_error;
    unicode_string_t m_current_directory_path;
    void* m_current_directory_handle;
    unicode_string_t m_dll_path;
    unicode_string_t m_image_path_name;
    unicode_string_t m_command_line;
    void* m_environment;
    std::uint32_t m_starting_x;
    std::uint32_t m_starting_y;
    std::uint32_t m_count_x;
    std::uint32_t m_count_y;
    std::uint32_t m_count_chars_x;
    std::uint32_t m_count_chars_y;
    std::uint32_t m_fill_attribute;
    std::uint32_t m_window_flags;
    std::uint32_t m_show_window_flags;
    unicode_string_t m_window_title;
    unicode_string_t m_desktop_info;
    unicode_string_t m_shell_info;
    unicode_string_t m_runtime_data;
};

struct rtl_process_module_information {
    void* m_section;
    void* m_mapped_base;
    void* m_image_base;
    ULONG m_image_size;
    ULONG m_flags;
    std::uint16_t m_load_order_index;
    std::uint16_t m_init_order_index;
    std::uint16_t m_load_count;
    std::uint16_t m_offset_to_file_name;
    std::uint8_t m_full_path_name[256];
};

struct rtl_process_modules {
    ULONG m_number_of_modules;
    rtl_process_module_information m_modules[1];
};

struct peb_t {
    std::uint8_t m_inherited_address_space;
    std::uint8_t m_read_image_file_exec_options;
    std::uint8_t m_being_debugged;
    std::uint8_t m_bit_field;
    void* m_mutant;
    void* m_image_base_address;
    peb_ldr_data_t* m_ldr;
    rtl_user_process_parameters_t* m_process_parameters;
    void* m_sub_system_data;
    void* m_process_heap;
    void* m_fast_peb_lock;
    void* m_atl_thunk_s_list_ptr;
    void* m_ifeo_key;
    std::uint32_t m_cross_process_flags;
    void* m_kernel_callback_table;
    std::uint32_t m_system_reserved[1];
    std::uint32_t m_atl_thunk_s_list_ptr32;
    void* m_api_set_map;
    std::uint32_t m_tls_expansion_counter;
    void* m_tls_bitmap;
    std::uint32_t m_tls_bitmap_bits[2];
    void* m_read_only_shared_memory_base;
    void* m_shared_data;
    void* m_read_only_static_server_data;
    void* m_ansi_code_page_data;
    void* m_oem_code_page_data;
    void* m_unicode_case_table_data;
    std::uint32_t m_number_of_processors;
    std::uint32_t m_nt_global_flag;
    large_integer_t m_critical_section_timeout;
    std::uint64_t m_heap_segment_reserve;
    std::uint64_t m_heap_segment_commit;
    std::uint64_t m_heap_de_commit_total_free_threshold;
    std::uint64_t m_heap_de_commit_free_block_threshold;
    std::uint32_t m_number_of_heaps;
    std::uint32_t m_maximum_number_of_heaps;
    void* m_process_heaps;
    void* m_gdi_shared_handle_table;
    void* m_process_starter_helper;
    std::uint32_t m_gdi_dc_attribute_list;
    void* m_loader_lock;
    std::uint32_t m_os_major_version;
    std::uint32_t m_os_minor_version;
    std::uint16_t m_os_build_number;
    std::uint16_t m_os_csd_version;
    std::uint32_t m_os_platform_id;
    std::uint32_t m_image_subsystem;
    std::uint32_t m_image_subsystem_major_version;
    std::uint32_t m_image_subsystem_minor_version;
    std::uint64_t m_active_process_affinity_mask;
    std::uint32_t m_gdi_handle_buffer[60];
    void* m_post_process_init_routine;
    void* m_tls_expansion_bitmap;
    std::uint32_t m_tls_expansion_bitmap_bits[32];
    std::uint32_t m_session_id;
    large_integer_t m_app_compat_flags;
    large_integer_t m_app_compat_flags_user;
    void* m_shim_data;
    void* m_app_compat_info;
    unicode_string_t m_csd_version;
    void* m_activate_context_data;
    void* m_process_assembly_storage_map;
    void* m_system_default_activation_context_data;
    void* m_system_assembly_storage_map;
    std::uint64_t m_minimum_stack_commit;
};

struct nt_tib_t {
    void* m_exception_list;
    void* m_stack_base;
    void* m_stack_limit;
    void* m_sub_system_tib;
    union {
        void* m_fiber_data;
        std::uint32_t m_version;
    };
    void* m_arbitrary_user_pointer;
    nt_tib_t* m_self;
};

struct teb_t {
    nt_tib_t m_nt_tib;
    void* m_environment_pointer;
    client_id_t m_client_id;
    void* m_active_rpc_handle;
    void* m_thread_local_storage_pointer;
    peb_t* m_process_environment_block;
    std::uint32_t m_last_error_value;
    std::uint32_t m_count_of_owned_critical_sections;
    void* m_csr_client_thread;
    void* m_win32_thread_info;
    std::uint32_t m_user32_reserved[26];
    std::uint32_t m_user_reserved[5];
    void* m_wow64_reserved;
    std::uint32_t m_current_locale;
    std::uint32_t m_fp_software_status_register;
    void* m_system_reserved1[54];
    std::int32_t m_exception_code;
    std::uint8_t m_activation_context_stack_pointer[0x28];
    std::uint8_t m_spare_bytes[0x28];
    std::uint32_t m_tx_fb_context;
    void* m_gdi_teb_batch;
    client_id_t m_real_client_id;
    void* m_gdi_cached_process_handle;
    std::uint32_t m_gdi_client_pid;
    std::uint32_t m_gdi_client_tid;
    void* m_gdi_thread_local_info;
    std::uint64_t m_win32_client_info[62];
    void* m_gl_dispatch_table[233];
    std::uint64_t m_gl_reserved1[29];
    void* m_gl_reserved2;
    void* m_gl_section_info;
    void* m_gl_section;
    void* m_gl_table;
    void* m_gl_current_rc;
    void* m_gl_context;
    std::uint32_t m_last_status_value;
    unicode_string_t m_static_unicode_string;
    wchar_t m_static_unicode_buffer[261];
    std::uint16_t m_padding5;
    void* m_deallocation_stack;
    void* m_tls_slots[64];
    list_entry_t m_tls_links;
    void* m_vdm;
    void* m_reserved_for_nt_rpc;
    void* m_dbg_ss_reserved[2];
};

struct mdl_t {
    mdl_t* m_next;
    std::int16_t m_size;
    std::int16_t m_mdl_flags;
    void* m_process;
    void* m_mapped_system_va;
    void* m_start_va;
    std::uint32_t m_byte_count;
    std::uint32_t m_byte_offset;
};

struct driver_extension_t {
    struct driver_object_t* m_driver_object;
    void* m_add_device;
    std::uint32_t m_count;
    std::uint32_t m_padding;
    unicode_string_t m_service_key_name;
};

struct __declspec(align(8)) driver_object_t {
    std::int16_t m_type;
    std::int16_t m_size;
    device_object_t* m_device_object;
    std::uint32_t m_flags;
    void* m_driver_start;
    std::uint32_t m_driver_size;
    void* m_driver_section;
    driver_extension_t* m_driver_extension;
    unicode_string_t m_driver_name;
    unicode_string_t* m_hardware_database;
    void* m_fast_io_dispatch;
    void* m_driver_init;
    void* m_driver_start_io;
    void* m_driver_unload;
    void* m_major_function[28];
};

struct vpb_t {
    std::int16_t m_type;
    std::int16_t m_size;
    std::uint16_t m_flags;
    std::uint16_t m_volume_label_length;
    void* m_device_object;
    void* m_real_device;
    std::uint32_t m_serial_number;
    std::uint32_t m_reference_count;
    wchar_t m_volume_label[32];
};

struct __declspec(align(8)) device_object_t {
    std::int16_t m_type;
    std::uint16_t m_size;
    std::int32_t m_reference_count;
    driver_object_t* m_driver_object;
    device_object_t* m_next_device;
    device_object_t* m_attached_device;
    void* m_current_irp;
    void* m_timer;
    std::uint32_t m_flags;
    std::uint32_t m_characteristics;
    vpb_t* m_vpb;
    void* m_device_extension;
    std::uint32_t m_device_type;
    std::int8_t m_stack_size;
    list_entry_t m_queue;
    std::uint32_t m_alignment_requirement;
    void* m_device_queue;
    kdpc_t* m_dpc;
    std::uint32_t m_active_threads;
    void* m_security_descriptor;
    kevent_t m_device_lock;
    std::uint16_t m_sector_size;
    std::uint16_t m_spare1;
    void* m_device_object_extension;
    void* m_reserved;
};

struct section_object_pointers_t {
    void* m_data_section_object;
    void* m_shared_cache_map;
    void* m_image_section_object;
};

struct __declspec(align(8)) file_object_t {
    std::int16_t m_type;
    std::int16_t m_size;
    device_object_t* m_device_object;
    vpb_t* m_vpb;
    void* m_fs_context;
    void* m_fs_context2;
    section_object_pointers_t* m_section_object_pointer;
    void* m_private_cache_map;
    std::int32_t m_final_status;
    file_object_t* m_related_file_object;
    std::uint8_t m_lock_operation;
    std::uint8_t m_delete_pending;
    std::uint8_t m_read_access;
    std::uint8_t m_write_access;
    std::uint8_t m_delete_access;
    std::uint8_t m_shared_read;
    std::uint8_t m_shared_write;
    std::uint8_t m_shared_delete;
    std::uint32_t m_flags;
    unicode_string_t m_file_name;
    large_integer_t m_current_byte_offset;
    std::uint32_t m_waiters;
    std::uint32_t m_busy;
    void* m_last_lock;
    kevent_t m_lock;
    kevent_t m_event;
    void* m_completion_context;
    std::uint64_t m_irp_list_lock;
    list_entry_t m_irp_list;
    void* m_file_object_extension;
};

struct io_status_block_t {
    union {
        std::int32_t m_status;
        void* m_pointer;
    };
    std::uint64_t m_information;
};

#pragma pack(push, 8)  // Use 8-byte alignment for x64 Windows

struct io_stack_location_t {
    std::uint8_t m_major_function;
    std::uint8_t m_minor_function;
    std::uint8_t m_flags;
    std::uint8_t m_control;

    union {
        // For NtCreateFile
        struct {
            void* m_security_context;
            std::uint32_t m_options;
            std::uint16_t m_file_attributes;
            std::uint16_t m_share_access;
            std::uint32_t m_ea_length;
        } m_create;

        // For NtCreateNamedPipeFile
        struct {
            void* m_security_context;
            std::uint32_t m_options;
            std::uint16_t m_reserved;
            std::uint16_t m_share_access;
            void* m_parameters;
        } m_create_pipe;

        // For NtCreateMailslotFile
        struct {
            void* m_security_context;
            std::uint32_t m_options;
            std::uint16_t m_reserved;
            std::uint16_t m_share_access;
            void* m_parameters;
        } m_create_mailslot;

        // For NtReadFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_key;
#if defined(_WIN64)
            std::uint32_t m_flags;
#endif
            std::int64_t m_byte_offset;
        } m_read;

        // For NtWriteFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_key;
#if defined(_WIN64)
            std::uint32_t m_flags;
#endif
            std::int64_t m_byte_offset;
        } m_write;

        // For NtQueryDirectoryFile
        struct {
            std::uint32_t m_length;
            void* m_file_name;
            std::uint32_t m_file_information_class;
            std::uint32_t m_file_index;
        } m_query_directory;

        // For NtNotifyChangeDirectoryFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_completion_filter;
        } m_notify_directory;

        // For NtNotifyChangeDirectoryFileEx
        struct {
            std::uint32_t m_length;
            std::uint32_t m_completion_filter;
            std::uint32_t m_directory_notify_information_class;
        } m_notify_directory_ex;

        // For NtQueryInformationFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_file_information_class;
        } m_query_file;

        // For NtSetInformationFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_file_information_class;
            void* m_file_object;
            union {
                struct {
                    std::uint8_t m_replace_if_exists;
                    std::uint8_t m_advance_only;
                };
                std::uint32_t m_cluster_count;
                void* m_delete_handle;
            };
        } m_set_file;

        // For NtQueryEaFile
        struct {
            std::uint32_t m_length;
            void* m_ea_list;
            std::uint32_t m_ea_list_length;
            std::uint32_t m_ea_index;
        } m_query_ea;

        // For NtSetEaFile
        struct {
            std::uint32_t m_length;
        } m_set_ea;

        // For NtQueryVolumeInformationFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_fs_information_class;
        } m_query_volume;

        // For NtSetVolumeInformationFile
        struct {
            std::uint32_t m_length;
            std::uint32_t m_fs_information_class;
        } m_set_volume;

        // For NtFsControlFile
        struct {
            std::uint32_t m_output_buffer_length;
            std::uint32_t m_input_buffer_length;
            std::uint32_t m_fs_control_code;
            void* m_type3_input_buffer;
        } m_file_system_control;

        // For NtLockFile/NtUnlockFile
        struct {
            void* m_length;
            std::uint32_t m_key;
            std::int64_t m_byte_offset;
        } m_lock_control;

        // For NtDeviceIoControlFile
        struct {
            std::uint32_t m_output_buffer_length;
            std::uint32_t m_input_buffer_length;
            std::uint32_t m_io_control_code;
            void* m_type3_input_buffer;
        } m_device_io_control;

        // For NtQuerySecurityObject
        struct {
            std::uint32_t m_security_information;
            std::uint32_t m_length;
        } m_query_security;

        // For NtSetSecurityObject
        struct {
            std::uint32_t m_security_information;
            void* m_security_descriptor;
        } m_set_security;

        // For MountVolume
        struct {
            void* m_vpb;
            void* m_device_object;
            std::uint32_t m_output_buffer_length;
        } m_mount_volume;

        // For VerifyVolume
        struct {
            void* m_vpb;
            void* m_device_object;
        } m_verify_volume;

        // For Scsi with internal device control
        struct {
            void* m_srb;
        } m_scsi;

        // For NtQueryQuotaInformationFile
        struct {
            std::uint32_t m_length;
            void* m_start_sid;
            void* m_sid_list;
            std::uint32_t m_sid_list_length;
        } m_query_quota;

        // For NtSetQuotaInformationFile
        struct {
            std::uint32_t m_length;
        } m_set_quota;

        // For IRP_MN_QUERY_DEVICE_RELATIONS
        struct {
            std::uint32_t m_type;
        } m_query_device_relations;

        // For IRP_MN_QUERY_INTERFACE
        struct {
            const void* m_interface_type;
            std::uint16_t m_size;
            std::uint16_t m_version;
            void* m_interface;
            void* m_interface_specific_data;
        } m_query_interface;

        // For IRP_MN_QUERY_CAPABILITIES
        struct {
            void* m_capabilities;
        } m_device_capabilities;

        // For IRP_MN_FILTER_RESOURCE_REQUIREMENTS
        struct {
            void* m_io_resource_requirement_list;
        } m_filter_resource_requirements;

        // For IRP_MN_READ_CONFIG and IRP_MN_WRITE_CONFIG
        struct {
            std::uint32_t m_which_space;
            void* m_buffer;
            std::uint32_t m_offset;
            std::uint32_t m_length;
        } m_read_write_config;

        // For IRP_MN_SET_LOCK
        struct {
            std::uint8_t m_lock;
        } m_set_lock;

        // For IRP_MN_QUERY_ID
        struct {
            std::uint32_t m_id_type;
        } m_query_id;

        // For IRP_MN_QUERY_DEVICE_TEXT
        struct {
            std::uint32_t m_device_text_type;
            std::uint32_t m_locale_id;
        } m_query_device_text;

        // For IRP_MN_DEVICE_USAGE_NOTIFICATION
        struct {
            std::uint8_t m_in_path;
            std::uint8_t m_reserved[3];
            std::uint32_t m_type;
        } m_usage_notification;

        // For IRP_MN_WAIT_WAKE
        struct {
            std::uint32_t m_power_state;
        } m_wait_wake;

        // For IRP_MN_POWER_SEQUENCE
        struct {
            void* m_power_sequence;
        } m_power_sequence;

        // For IRP_MN_SET_POWER and IRP_MN_QUERY_POWER
#if (NTDDI_VERSION >= NTDDI_VISTA)
        struct {
            union {
                std::uint32_t m_system_context;
                std::uint64_t m_system_power_state_context;
            };
            std::uint32_t m_type;
            std::uint32_t m_state;
            std::uint32_t m_shutdown_type;
        } m_power;
#else
        struct {
            std::uint32_t m_system_context;
            std::uint32_t m_type;
            std::uint32_t m_state;
            std::uint32_t m_shutdown_type;
        } m_power;
#endif

        // For StartDevice
        struct {
            void* m_allocated_resources;
            void* m_allocated_resources_translated;
        } m_start_device;

        // For WMI Irps
        struct {
            std::uint64_t m_provider_id;
            void* m_data_path;
            std::uint32_t m_buffer_size;
            void* m_buffer;
        } m_wmi;

        // Others - driver-specific
        struct {
            void* m_arg1;
            void* m_arg2;
            void* m_arg3;
            void* m_arg4;
        } m_others;

    } m_parameters;

    void* m_device_object;
    void* m_file_object;
    void* m_completion_routine;
    void* m_context;
};

#pragma pack(pop)

struct irp_t {
    std::uint16_t m_type;                  // Type
    std::uint16_t m_size;                  // Size
    void* m_mdl;                           // MdlAddress
    std::uint32_t m_flags;                 // Flags

    // AssociatedIrp union
    union {
        struct iop_irp_t* m_master_irp;    // MasterIrp
        volatile std::int32_t m_irp_count; // IrpCount
        void* m_system_buffer;             // SystemBuffer
    } m_associated_irp;

    // ThreadListEntry
    struct {
        void* m_flink;
        void* m_blink;
    } m_thread_list_entry;

    // IoStatus
    struct {
        nt_status_t m_status;             // Status
        std::uint64_t m_information;       // Information
    } m_io_status;

    std::uint8_t m_requestor_mode;         // RequestorMode
    std::uint8_t m_pending_returned;       // PendingReturned
    std::int8_t m_stack_count;             // StackCount
    std::int8_t m_current_location;        // CurrentLocation
    std::uint8_t m_cancel;                 // Cancel
    std::uint8_t m_cancel_irql;            // CancelIrql
    std::int8_t m_apc_environment;         // ApcEnvironment
    std::uint8_t m_allocation_flags;       // AllocationFlags

    // User parameters
    union {
        void* m_user_iosb;                 // UserIosb
        void* m_io_ring_context;           // IoRingContext
    } m_user_iosb_union;

    void* m_user_event;                    // UserEvent

    // Overlay union
    union {
        struct {
            union {
                void* m_user_apc_routine;   // UserApcRoutine
                void* m_issuing_process;    // IssuingProcess
            } m_apc_routine_union;

            union {
                void* m_user_apc_context;   // UserApcContext
                void* m_io_ring;            // IoRing
            } m_apc_context_union;
        } m_asynchronous_parameters;

        std::uint64_t m_allocation_size;    // AllocationSize
    } m_overlay;

    volatile void* m_cancel_routine;        // CancelRoutine
    void* m_user_buffer;                    // UserBuffer

    // Tail union
    union {
        struct {
            union {
                // DeviceQueueEntry
                struct {
                    void* m_device_list_entry;
                    void* m_sorting_key;
                    std::uint8_t m_inserted;
                } m_device_queue_entry;

                // DriverContext
                struct {
                    void* m_driver_context[4];
                };
            } m_device_context_union;

            void* m_thread;                 // Thread
            void* m_auxiliary_buffer;       // AuxiliaryBuffer

            struct {
                // ListEntry
                struct {
                    void* m_flink;
                    void* m_blink;
                } m_list_entry;

                union {
                    // CurrentStackLocation
                    struct iop_irp_stack_profiler_t* m_current_stack_location;
                    std::uint32_t m_packet_type;  // PacketType
                } m_stack_location_union;
            } m_list_stack_struct;

            void* m_original_file_object;   // OriginalFileObject
        } m_overlay;

        // Apc
        struct {
            // KAPC structure fields
            std::uint16_t m_type;
            std::uint16_t m_size;
            std::uint32_t m_spare0;
            void* m_thread;
            void* m_apc_list_entry;
            void* m_kernel_routine;
            void* m_rundown_routine;
            void* m_normal_routine;
            void* m_normal_context;
            void* m_system_argument1;
            void* m_system_argument2;
            std::uint8_t m_apc_state_index;
            std::uint8_t m_apc_mode;
            std::uint8_t m_inserted;
            std::uint8_t m_padding;
        } m_apc;

        void* m_completion_key;             // CompletionKey
    } m_tail;

    // The stack array is at the end (variable length)
    io_stack_location_t m_stack[1];
};

struct handle_table_entry_t {
    union {
        std::uint64_t m_object_pointer_bits;
        struct {
            std::uint64_t m_unlocked : 1;
            std::uint64_t m_ref_cnt : 16;
            std::uint64_t m_attributes : 3;
            std::uint64_t m_object_pointer : 44;
        };
        struct {
            std::uint64_t m_unlocked2 : 1;
            std::uint64_t m_info_table : 63;
        };
    };
    union {
        std::uint32_t m_granted_access_bits;
        struct {
            std::uint16_t m_granted_access;
            std::uint16_t m_creator_back_trace_index;
        };
    };
    std::uint32_t m_next_free_table_entry;
};

struct handle_table_t {
    std::uint32_t m_next_handle_needing_pool;
    std::int32_t m_extra_info_pages;
    std::uint64_t m_table_code;
    eprocess_t* m_quota_process;
    list_entry_t m_handle_table_list;
    std::uint32_t m_unique_process_id;
    std::uint32_t m_flags;
    ex_push_lock_t m_handle_content_lock;
    ex_push_lock_t m_handle_table_lock;
    union {
        std::uint8_t m_free_list_lock;
        std::uint8_t m_single_level_table;
    };
    std::uint8_t m_table_level;
    void* m_debug_info;
};

struct __declspec(align(8)) object_header_t {
    std::int64_t m_pointer_count;
    union {
        std::int64_t m_handle_count;
        void* m_next_to_free;
    };
    ex_push_lock_t m_lock;
    std::uint8_t m_type_index;
    std::uint8_t m_trace_flags;
    std::uint8_t m_info_mask;
    std::uint8_t m_flags;
    union {
        void* m_quota_block_charged;
        struct {
            void* m_object_create_info;
            void* m_security_descriptor;
        };
    };
};

using ex_callback_function_t = nt_status_t(__stdcall*)(
    void* callback_context,
    void* argument1,
    void* argument2
    );

struct __declspec(align(8)) object_type_t {
    list_entry_t m_type_list;
    unicode_string_t m_name;
    void* m_default_object;
    std::uint8_t m_index;
    std::uint32_t m_total_number_of_objects;
    std::uint32_t m_total_number_of_handles;
    std::uint32_t m_high_water_number_of_objects;
    std::uint32_t m_high_water_number_of_handles;
    std::uint8_t m_type_info[0x78];
    ex_push_lock_t m_type_lock;
    std::uint32_t m_key;
    list_entry_t m_callback_list;
};

struct pool_header_t {
    union {
        struct {
            std::uint16_t m_previous_size : 8;
            std::uint16_t m_pool_index : 8;
            std::uint16_t m_block_size : 8;
            std::uint16_t m_pool_type : 8;
        };
        std::uint32_t m_ulong1;
    };
    std::uint32_t m_pool_tag;
    union {
        eprocess_t* m_process_billed;
        struct {
            std::uint16_t m_alloc_tag_high;
            std::uint16_t m_pool_tag_high;
        };
    };
};

struct system_service_descriptor_table_t {
    void* m_service_table_base;
    void* m_service_counter_table_base;
    std::uint64_t m_number_of_services;
    void* m_param_table_base;
};

typedef struct reg_set_value_key_information_t {
    void* m_object;
    unicode_string_t* m_value_name;
    std::uint32_t m_title_index;
    std::uint32_t m_type;
    void* m_data;
    std::uint32_t m_data_size;
    void* m_call_context;  // new to Windows Vista
    void* m_object_context;// new to Windows Vista
    void* m_reserved;     // new to Windows Vista
};

struct  kservice_table_descriptor_t {
    system_service_descriptor_table_t m_ntoskrnl;
    system_service_descriptor_table_t m_win32k;
    system_service_descriptor_table_t m_unused1;
    system_service_descriptor_table_t m_unused2;
};

struct  sid_identifier_authority_t {
    std::uint8_t m_value[6];
};

struct  sid_t {
    std::uint8_t m_revision;
    std::uint8_t m_sub_authority_count;
    sid_identifier_authority_t m_identifier_authority;
    std::uint32_t m_sub_authority[1];
};

struct  acl_t {
    std::uint8_t m_acl_revision;
    std::uint8_t m_sbz1;
    std::uint16_t m_acl_size;
    std::uint16_t m_ace_count;
    std::uint16_t m_sbz2;
};

struct  security_descriptor_t {
    std::uint8_t m_revision;
    std::uint8_t m_sbz1;
    std::uint16_t m_control;
    void* m_owner;
    void* m_group;
    acl_t* m_sacl;
    acl_t* m_dacl;
};

struct image_dos_header_t {
    std::uint16_t m_e_magic;
    std::uint16_t m_e_cblp;
    std::uint16_t m_e_cp;
    std::uint16_t m_e_crlc;
    std::uint16_t m_e_cparhdr;
    std::uint16_t m_e_minalloc;
    std::uint16_t m_e_maxalloc;
    std::uint16_t m_e_ss;
    std::uint16_t m_e_sp;
    std::uint16_t m_e_csum;
    std::uint16_t m_e_ip;
    std::uint16_t m_e_cs;
    std::uint16_t m_e_lfarlc;
    std::uint16_t m_e_ovno;
    std::uint16_t m_e_res[4];
    std::uint16_t m_e_oemid;
    std::uint16_t m_e_oeminfo;
    std::uint16_t m_e_res2[10];
    std::int32_t m_e_lfanew;

    [[ nodiscard ]]
    constexpr bool is_valid() {
        return m_e_magic == pe_magic_e::dos_header;
    }
};

struct image_file_header_t {
    std::uint16_t m_machine;
    std::uint16_t m_number_of_sections;
    std::uint32_t m_time_date_stamp;
    std::uint32_t m_pointer_to_symbol_table;
    std::uint32_t m_number_of_symbols;
    std::uint16_t m_size_of_optional_header;
    std::uint16_t m_characteristics;
};

struct image_data_directory_t {
    std::uint32_t m_virtual_address;
    std::uint32_t m_size;

    template< class type_t >
    [[ nodiscard ]]
    type_t as_rva(
        std::uintptr_t rva
    ) const {
        return reinterpret_cast<type_t>(rva + m_virtual_address);
    }
};

struct image_optional_header64_t {
    std::uint16_t m_magic;
    std::uint8_t m_major_linker_version;
    std::uint8_t m_minor_linker_version;
    std::uint32_t m_size_of_code;
    std::uint32_t m_size_of_initialized_data;
    std::uint32_t m_size_of_uninitialized_data;
    std::uint32_t m_address_of_entry_point;
    std::uint32_t m_base_of_code;
    std::uint64_t m_image_base;
    std::uint32_t m_section_alignment;
    std::uint32_t m_file_alignment;
    std::uint16_t m_major_operating_system_version;
    std::uint16_t m_minor_operating_system_version;
    std::uint16_t m_major_image_version;
    std::uint16_t m_minor_image_version;
    std::uint16_t m_major_subsystem_version;
    std::uint16_t m_minor_subsystem_version;
    std::uint32_t m_win32_version_value;
    std::uint32_t m_size_of_image;
    std::uint32_t m_size_of_headers;
    std::uint32_t m_check_sum;
    std::uint16_t m_subsystem;
    std::uint16_t m_dll_characteristics;
    std::uint64_t m_size_of_stack_reserve;
    std::uint64_t m_size_of_stack_commit;
    std::uint64_t m_size_of_heap_reserve;
    std::uint64_t m_size_of_heap_commit;
    std::uint32_t m_loader_flags;
    std::uint32_t m_number_of_rva_and_sizes;
    image_data_directory_t m_data_directory[16];
};

struct image_nt_headers64_t {
    std::uint32_t m_signature;
    image_file_header_t m_file_header;
    image_optional_header64_t m_optional_header;

    [[ nodiscard ]]
    constexpr bool is_valid() {
        return m_signature == pe_magic_e::nt_headers
            && m_optional_header.m_magic == pe_magic_e::opt_header;
    }
};

struct image_section_header_t {
    std::uint8_t m_name[8];
    union {
        std::uint32_t m_physical_address;
        std::uint32_t m_virtual_size;
    } m_misc;
    std::uint32_t m_virtual_address;
    std::uint32_t m_size_of_raw_data;
    std::uint32_t m_pointer_to_raw_data;
    std::uint32_t m_pointer_to_relocations;
    std::uint32_t m_pointer_to_linenumbers;
    std::uint16_t m_number_of_relocations;
    std::uint16_t m_number_of_linenumbers;
    std::uint32_t m_characteristics;
};

struct image_export_directory_t {
    std::uint32_t m_characteristics;
    std::uint32_t m_time_date_stamp;
    std::uint16_t m_major_version;
    std::uint16_t m_minor_version;
    std::uint32_t m_name;
    std::uint32_t m_base;
    std::uint32_t m_number_of_functions;
    std::uint32_t m_number_of_names;
    std::uint32_t m_address_of_functions;
    std::uint32_t m_address_of_names;
    std::uint32_t m_address_of_name_ordinals;
};

struct image_import_descriptor_t {
    union {
        std::uint32_t m_characteristics;
        std::uint32_t m_original_first_thunk;
    };
    std::uint32_t m_time_date_stamp;
    std::uint32_t m_forwarder_chain;
    std::uint32_t m_name;
    std::uint32_t m_first_thunk;
};

struct eprocess_t {
    kprocess_t m_pcb;
    ex_push_lock_t m_process_lock;
    void* m_run_down_protect;
    std::uint64_t m_create_time;
    void* m_unique_process_id;
    list_entry_t m_active_process_links;
    ex_fast_ref_t m_token;
    std::uint64_t m_quota_block;
    void* m_object_table;
    void* m_debug_port;
    void* m_section_object;
    void* m_section_base_address;
    std::uint32_t m_cookie;
    std::uint32_t m_padding1;
    void* m_device_map;
    void* m_session;
    std::uint8_t m_image_file_name[15];
    std::uint8_t m_padding2;
    std::uint32_t m_session_id;
    std::uint32_t m_padding3;
    list_entry_t m_job;
    void* m_spare_ptr1;
    void* m_spare_ptr2;
    std::uint32_t m_flags;
    std::uint32_t m_flags2;
    void* m_peb;
    std::uint64_t m_inherit_allowed_from;
    void* m_inherited_from_unique_process_id;
    void* m_launch_prefetched;
    mmsupport_t m_vm;
    std::uint64_t m_work_set_size;
    fast_mutex_t m_address_creation_lock;
    ex_push_lock_t m_address_space_deleted;
    void* m_vad_root;
    se_audit_process_creation_info_t m_se_audit_process_creation_info;
};

typedef union cr4_t {
    std::uint64_t value;
    struct {
        std::uint64_t vme : 1;
        std::uint64_t pvi : 1;
        std::uint64_t tsd : 1;
        std::uint64_t de : 1;
        std::uint64_t pse : 1;
        std::uint64_t pae : 1;
        std::uint64_t mce : 1;
        std::uint64_t pge : 1;
        std::uint64_t pce : 1;
        std::uint64_t osfxsr : 1;
        std::uint64_t osxmmexcpt : 1;
        std::uint64_t umip : 1;
        std::uint64_t la57 : 1;
        std::uint64_t vmxe : 1;
        std::uint64_t smxe : 1;
        std::uint64_t reserved1 : 1;
        std::uint64_t fsgsbase : 1;
        std::uint64_t pcide : 1;
        std::uint64_t osxsave : 1;
        std::uint64_t kl : 1;
        std::uint64_t smep : 1;
        std::uint64_t smap : 1;
        std::uint64_t pke : 1;
        std::uint64_t cet : 1;
        std::uint64_t pks : 1;
        std::uint64_t reserved2 : 39;
    };
};

typedef union _virt_addr_t {
    std::uintptr_t value;
    struct {
        std::uint64_t offset : 12;        // 0:11
        std::uint64_t pte_index : 9;      // 12:20
        std::uint64_t pde_index : 9;      // 21:29
        std::uint64_t pdpte_index : 9;    // 30:38
        std::uint64_t pml4e_index : 9;    // 39:47
        std::uint64_t reserved : 16;      // 48:63
    };
} virt_addr_t, * pvirt_addr_t;

typedef union _pfn_t {
    std::uintptr_t value;
    std::uintptr_t offset;
} pfn_t, * ppfn_t;

typedef union _pml4e {
    struct {
        std::uint64_t present : 1;                   // Must be 1 if valid
        std::uint64_t read_write : 1;               // Write access control
        std::uint64_t user_supervisor : 1;           // User/supervisor access control
        std::uint64_t page_write_through : 1;        // Write-through caching
        std::uint64_t cached_disable : 1;            // Cache disable
        std::uint64_t accessed : 1;                  // Set when accessed
        std::uint64_t ignored0 : 1;                  // Ignored
        std::uint64_t large_page : 1;               // Reserved (must be 0)
        std::uint64_t ignored1 : 4;                 // Ignored
        std::uint64_t pfn : 36;                     // Physical frame number
        std::uint64_t reserved : 4;                 // Reserved for software
        std::uint64_t ignored2 : 11;                // Ignored
        std::uint64_t no_execute : 1;               // No-execute bit
    } hard;
    std::uint64_t value;
} pml4e, * ppml4e;

typedef union _pdpte {
    struct {
        std::uint64_t present : 1;                   // Must be 1 if valid
        std::uint64_t read_write : 1;               // Write access control
        std::uint64_t user_supervisor : 1;           // User/supervisor access control
        std::uint64_t page_write_through : 1;        // Write-through caching
        std::uint64_t cached_disable : 1;            // Cache disable
        std::uint64_t accessed : 1;                  // Set when accessed
        std::uint64_t dirty : 1;                    // Set when written to (1GB pages)
        std::uint64_t page_size : 1;                // 1=1GB page, 0=points to page directory
        std::uint64_t ignored1 : 4;                 // Ignored
        std::uint64_t pfn : 36;                     // Physical frame number
        std::uint64_t reserved : 4;                 // Reserved for software
        std::uint64_t ignored2 : 11;                // Ignored
        std::uint64_t no_execute : 1;               // No-execute bit
    } hard;
    std::uint64_t value;
} pdpte, * ppdpte;

typedef union _pde {
    struct {
        std::uint64_t present : 1;                   // Must be 1 if valid
        std::uint64_t read_write : 1;               // Write access control
        std::uint64_t user_supervisor : 1;           // User/supervisor access control
        std::uint64_t page_write_through : 1;        // Write-through caching
        std::uint64_t cached_disable : 1;            // Cache disable
        std::uint64_t accessed : 1;                  // Set when accessed
        std::uint64_t dirty : 1;                    // Set when written to (2MB pages)
        std::uint64_t page_size : 1;                // 1=2MB page, 0=points to page table
        std::uint64_t global : 1;                   // Global page (if CR4.PGE=1)
        std::uint64_t ignored1 : 3;                 // Ignored
        std::uint64_t pfn : 36;                     // Physical frame number
        std::uint64_t reserved : 4;                 // Reserved for software
        std::uint64_t ignored2 : 11;                // Ignored
        std::uint64_t no_execute : 1;               // No-execute bit
    } hard;
    std::uint64_t value;
} pde, * ppde;

typedef union _pte {
    struct {
        std::uint64_t present : 1;                   // Must be 1 if valid
        std::uint64_t read_write : 1;               // Write access control
        std::uint64_t user_supervisor : 1;           // User/supervisor access control
        std::uint64_t page_write_through : 1;        // Write-through caching
        std::uint64_t cached_disable : 1;            // Cache disable
        std::uint64_t accessed : 1;                  // Set when accessed
        std::uint64_t dirty : 1;                    // Set when written to
        std::uint64_t pat : 1;                      // Page Attribute Table bit
        std::uint64_t global : 1;                   // Global page
        std::uint64_t ignored1 : 3;                 // Ignored
        std::uint64_t pfn : 36;                     // Physical frame number
        std::uint64_t reserved : 4;                 // Reserved for software
        std::uint64_t ignored2 : 7;                 // Ignored
        std::uint64_t protection_key : 4;           // Protection key
        std::uint64_t no_execute : 1;               // No-execute bit
    } hard;
    std::uint64_t value;
} pte, * ppte;

typedef union cr3 {
    std::uint64_t flags;
    struct {
        std::uint64_t reserved1 : 3;
        std::uint64_t page_level_write_through : 1;
        std::uint64_t page_level_cache_disable : 1;
        std::uint64_t reserved2 : 7;
        std::uint64_t dirbase : 36;
        std::uint64_t reserved3 : 16;
    };
};

static_assert(sizeof(list_entry_t) == 0x10, "list_entry_t size mismatch");
static_assert(sizeof(unicode_string_t) == 0x10, "unicode_string_t size mismatch");
static_assert(sizeof(dispatcher_header_t) == 0x18, "dispatcher_header_t size mismatch");
static_assert(sizeof(kevent_t) == 0x18, "kevent_t size mismatch");
static_assert(sizeof(handle_table_entry_t) == 0x10, "handle_table_entry_t size mismatch");
static_assert(sizeof(guid_t) == 0x10, "guid_t size mismatch");

#endif krnlH // krnlH
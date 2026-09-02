#ifndef clientH
#define clientH

namespace client {

    struct data_initialize_t {
        std::uint32_t m_pid;
        std::uint64_t m_base_address;
    };

    struct data_request_t {
        std::uint32_t m_process_id;
        eprocess_t* m_eprocess;
        std::uint64_t m_image_base;
        std::uint64_t m_dtb;
        std::uint64_t m_address;
        void* m_buffer;
        std::size_t   m_size;
    };

    enum class request_type_e : std::uint32_t {
        ping_driver = 0,
        get_eprocess,
        get_image_base,
        get_directory_table_base,
        read_memory,
        write_memory
    };

    extern mdl_t* m_comm_mdl;
    extern data_request_t* m_data_request;
    extern std::uint32_t   m_syscall_index;
    extern ksemaphore_t* m_response_handle;
    extern ksemaphore_t* m_request_handle;

    // One cache per comm channel — lives for the lifetime of the session
    extern  memory::pt_cache_t m_pt_cache;

    nt_status_t registry_callback(
        void* callback_context,
        void* argument1,
        void* argument2
    );
} // namespace client

#endif // clientH
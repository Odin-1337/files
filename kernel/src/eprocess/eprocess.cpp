#include <includes.h>

namespace eprocess {
	std::uint64_t get_directory_table_base(eprocess_t* process) {
		return process->m_pcb.m_directory_table_base;
	}

	const char* get_image_file_name(eprocess_t* eprocess) {
		auto current_process = ps_get_current_process();
		if (!mm_is_address_valid(current_process))
			return 0;

		auto image_file_name_offset = get_image_file_name_offset();
		auto image_file_name_ptr = reinterpret_cast<const char*>(
			reinterpret_cast<std::uintptr_t>(current_process) + image_file_name_offset
			);

		if (!mm_is_address_valid(const_cast<char*>(image_file_name_ptr)))
			return 0;

		char image_file_name[16] = { 0 };
		__try {
			for (int i = 0; i < 15; i++) {
				image_file_name[i] = image_file_name_ptr[i];
				if (image_file_name[i] == '\0')
					break;
			}
			image_file_name[15] = '\0';
		}
		__except (1) {
			return 0;
		}

		return image_file_name;
	}

	void* get_section_base_address(eprocess_t* eprocess) {
		auto section_base_address_offset = get_section_base_address_offset();
		return *reinterpret_cast<void**>(
			reinterpret_cast<std::uintptr_t>(eprocess) + section_base_address_offset
			);
	}

	eprocess_t* find_eprocess(std::uint32_t target_pid) {
		auto process_list_head = ps_active_process_head();
		if (!process_list_head)
			return nullptr;

		auto linkage_va = reinterpret_cast<std::uintptr_t>(process_list_head) -
			ps_initial_system_process();
		if (!linkage_va)
			return nullptr;

		for (auto flink = process_list_head->m_flink; flink; flink = flink->m_flink) {
			if (!mm_is_address_valid(flink))
				break;

			auto curr_eprocess = reinterpret_cast<eprocess_t*>(
				reinterpret_cast<std::uintptr_t>(flink) - linkage_va
				);
			if (!curr_eprocess)
				continue;

			auto process_id = reinterpret_cast<std::uint32_t>(ps_get_process_id(curr_eprocess));
			if (process_id == target_pid)
				return curr_eprocess;
		}

		return nullptr;
	}

	eprocess_t* attach_process(eprocess_t* eprocess) {
		auto current_thread = reinterpret_cast<ethread_t*>(__readgsqword(0x188));
		if (!current_thread)
			return nullptr;

		auto apc_state = &current_thread->m_kthread.m_apc_state;
		auto org_process = apc_state->m_process;

		apc_state->m_process = eprocess;
		__writecr3(eprocess->m_pcb.m_directory_table_base);
		return org_process;
	}

	void detach_process(eprocess_t* original_process) {
		if (!original_process)
			return;

		auto* current_thread = reinterpret_cast<ethread_t*>(__readgsqword(0x188));
		if (!current_thread)
			return;

		auto* apc_state = &current_thread->m_kthread.m_apc_state;
		apc_state->m_process = original_process;
		__writecr3(original_process->m_pcb.m_directory_table_base);
	}
} // namespace eprocess
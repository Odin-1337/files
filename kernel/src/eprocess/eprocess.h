#ifndef eprocessH
#define eprocessH

namespace eprocess {
	std::uint64_t get_directory_table_base(eprocess_t* process);

	const char* get_image_file_name(eprocess_t* eprocess);

	void* get_section_base_address(eprocess_t* eprocess);

	eprocess_t* find_eprocess(std::uint32_t target_pid);

	eprocess_t* attach_process(eprocess_t* eprocess);

	void detach_process(eprocess_t* original_process);
} // namespace eprocess

#endif eprocessH // eprocessH
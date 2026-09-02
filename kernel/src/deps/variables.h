#ifndef variablesH
#define variablesH

constexpr auto page_4kb_size = 0x1000ull;
constexpr auto page_2mb_size = 0x200000ull;
constexpr auto page_1gb_size = 0x40000000ull;

constexpr auto page_shift = 12ull;
constexpr auto page_2mb_shift = 21ull;
constexpr auto page_1gb_shift = 30ull;

constexpr auto page_4kb_mask = 0xFFFull;
constexpr auto page_2mb_mask = 0x1FFFFFull;
constexpr auto page_1gb_mask = 0x3FFFFFFFull;

using ULONG = unsigned long;
using LONG = long;

using LONGLONG = __int64;
using ULONGLONG = unsigned __int64;

constexpr auto irp_mj_create = 0x00;
constexpr auto irp_mj_close = 0x02;
constexpr auto irp_mj_device_control = 0x0E;
constexpr auto irp_mj_maximum_function = 0x1B;
constexpr auto irp_mj_cleanup = 18;
constexpr auto do_device_initializing = 0x00000080;
constexpr auto do_buffered_io = 0x00000004;
constexpr auto file_device_secure_open = 0x00000100;

#endif variablesH // variablesH
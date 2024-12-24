#pragma once

#include "../common/CommonConfig.h"

#ifdef USE_WINAPI_LIST

#include "../common/CommonUtils.h"
#include "WinapiSemaphore.h"

#ifndef USE_WINAPI
#error "This file should not be included by client-code directly."
#endif // USE_WINAPI

namespace concurrent::details {

class list {
public:
	list();
	list(const list &other) = delete;
	list &operator=(const list &other) = delete;

	template <typename Callable>
	bool push_is_first(Callable &&callable) {
		return push_entry(AllocateEntry(std::forward<Callable>(callable)));
	}
	bool process();
	bool empty() const;

	~list();

private:
#if defined WINAPI_X64
	static constexpr auto kLockFreeAlignment = 16;
#elif defined WINAPI_X86 // WINAPI_X64
	static constexpr auto kLockFreeAlignment = 8;
#else // CRL_WINAPI_X86
#error "Configuration is not supported."
#endif // !WINAPI_X86 && !WINAPI_X64

	// Hide WinAPI SLIST_HEADER
	struct alignas(kLockFreeAlignment) lock_free_list {
		void *Next__; // Hide WinAPI SLIST_ENTRY
		unsigned short Depth__; // Hide WinAPI WORD
		unsigned short CpuId__; // Hide WinAPI WORD
	};

	struct alignas(kLockFreeAlignment) BasicEntry;
	using ProcessEntryMethod = void(*)(BasicEntry *entry);

	struct alignas(kLockFreeAlignment) BasicEntry {
		void *plain; // Hide WinAPI SLIST_ENTRY
		ProcessEntryMethod process;
	};

	static_assert(std::is_trivial_v<BasicEntry>);
	static_assert(std::is_standard_layout_v<BasicEntry>);
	static_assert(offsetof(BasicEntry, plain) == 0);

	template <typename Function>
	struct Entry : BasicEntry {
		Entry(Function &&function) : function(std::move(function)) {
		}
		Entry(const Function &function) : function(function) {
		}
		Function function;

		static void Process(BasicEntry *entry) {
			auto full = static_cast<Entry*>(entry);
			auto guard = details::finally([=] { delete full; });
			full->function();
		}

	};

	template <typename Callable>
	static Entry<std::decay_t<Callable>> *AllocateEntry(
			Callable &&callable) {
		using Function = std::decay_t<Callable>;
		using Type = Entry<Function>;

		auto result = new Type(std::forward<Callable>(callable));
		result->process = &Type::Process;
		return result;
	}

	bool push_entry(BasicEntry *entry);

	const std::unique_ptr<lock_free_list> _impl;
	bool *_alive = nullptr;

};

} // namespace concurrent::details

#endif // USE_WINAPI_LIST

#pragma once

#include "Lumen/Core/Core.hpp"
#include "Lumen/Internal/IO/Print.hpp"
#include "Lumen/Internal/Utils/Settings.hpp"

#include <cstdint>
#include <ranges>
#include <type_traits>

namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// DeferredConstruct<T>
	////////////////////////////////////////////////////////////////////////////////////
	template<typename T, bool destroyable = false>
	class DeferredConstruct
	{
	public:
		// Constructor & Destructor
		DeferredConstruct() = default;
		~DeferredConstruct();

		// Copy/Move constructors
		DeferredConstruct(const DeferredConstruct&) = delete;
		DeferredConstruct(DeferredConstruct&&) = delete;
		DeferredConstruct& operator = (const DeferredConstruct&) = delete;
		DeferredConstruct& operator = (DeferredConstruct&&) = delete;

		// Operators
		forceinline operator T& () { CheckConstructed(); return *reinterpret_cast<T*>(m_Storage); }
		forceinline operator const T& () const { CheckConstructed(); return *reinterpret_cast<const T*>(m_Storage); }
		forceinline operator T* () { CheckConstructed(); return reinterpret_cast<T*>(m_Storage); }
		forceinline operator const T* () const { CheckConstructed(); return reinterpret_cast<const T*>(m_Storage); }

		forceinline T* operator -> () { CheckConstructed(); return reinterpret_cast<T*>(m_Storage); }
		forceinline const T* operator -> () const { CheckConstructed(); return reinterpret_cast<const T*>(m_Storage); }

		// Getter
		forceinline T& Get() { CheckConstructed(); return *reinterpret_cast<T*>(m_Storage); }
		forceinline const T& Get() const { CheckConstructed(); return *reinterpret_cast<const T*>(m_Storage); }

		// Methods
		template<typename ...Args>
		void Construct(Args&& ...args);

		void Destroy() requires(destroyable);

	private:
		// Debug method
		forceinline void CheckConstructed() const
		{
			#if defined(LU_CONFIG_DEBUG)
			LU_ASSERT(m_Constructed, "[DeferredConstruct<T>] Object not constructed yet!");
			#endif
		}
	
	private:
		// std::aligned_storage is deprecated as of C++23
		alignas(T) std::byte m_Storage[sizeof(T)] = {};

		#if defined(LU_CONFIG_DEBUG)
		bool m_Constructed = false;
		#endif
	};

}

#include "Lumen/Internal/Memory/DeferredConstruct.inl"
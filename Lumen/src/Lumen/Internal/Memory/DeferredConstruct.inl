namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	template<typename T, bool destroyable>
	hintinline DeferredConstruct<T, destroyable>::~DeferredConstruct()
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			if constexpr (destroyable)
			{
				if (!std::ranges::all_of(m_Storage, [](std::byte b) { return b == std::byte{ 0 }; }))
				{
					CheckConstructed();
					reinterpret_cast<T*>(m_Storage)->~T();
				}
			}
			else
			{
				CheckConstructed();
				reinterpret_cast<T*>(m_Storage)->~T();
			}
		}
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	template<typename T, bool destroyable>
	template<typename ...Args>
	hintinline void DeferredConstruct<T, destroyable>::Construct(Args&& ...args)
	{
		#if defined(LU_CONFIG_DEBUG)
		if (m_Constructed) [[unlikely]]
		{
			LU_LOG_WARN("[DeferredConstruct<T>] Object already constructed, calling destructor on previous object! Please change your implementation to avoid re-constructing, since these checks are only available in Debug mode.");
			if constexpr (!std::is_trivially_destructible_v<T>)
				reinterpret_cast<T*>(m_Storage)->~T();
		}

		m_Constructed = true;
		#endif

		new (m_Storage) T(std::forward<Args>(args)...);
	}

	template<typename T, bool destroyable>
	hintinline void DeferredConstruct<T, destroyable>::Destroy() requires (destroyable)
	{
		CheckConstructed();

		if constexpr (!std::is_trivially_destructible_v<T>)
			reinterpret_cast<T*>(m_Storage)->~T();

		std::memset(m_Storage, 0, sizeof(m_Storage));

		#if defined(LU_CONFIG_DEBUG)
		m_Constructed = false;
		#endif
	}

}
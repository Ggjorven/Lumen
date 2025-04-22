namespace Lumen
{

	////////////////////////////////////////////////////////////////////////////////////
	// Constructor & Destructor
	////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	forceinline DeferredConstruct<T>::~DeferredConstruct()
	{
		if constexpr (!std::is_trivially_destructible_v<T>)
			reinterpret_cast<T*>(m_Storage)->~T();
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	template<typename T>
	template<typename ...Args>
	hintinline void DeferredConstruct<T>::Construct(Args&& ...args)
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

}
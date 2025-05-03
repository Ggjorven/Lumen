namespace Lumen::Internal
{

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	hintinline bool QueueFamilyInfo::SupportsRequired() const
	{
		return ((static_cast<bool>(Flags & QueueFamilyFlags::Graphics)) && (static_cast<bool>(Flags & QueueFamilyFlags::Compute)) && (static_cast<bool>(Flags & QueueFamilyFlags::Present)));
	}

	hintinline bool QueueFamilyInfo::EnoughQueues() const
	{
		return Count >= 3;
	}

	////////////////////////////////////////////////////////////////////////////////////
	// Methods
	////////////////////////////////////////////////////////////////////////////////////
	hintinline bool QueueFamilyIndices::SameQueue() const
	{
		return ((GraphicsQueue == PresentQueue) && (PresentQueue == ComputeQueue));
	}

}
#pragma once

#include "Lumen/Internal/Renderer/RendererSpec.hpp"

#include "Lumen/Core/Core.hpp"

#include <cstdint>
#include <vector>

namespace Lumen::Internal
{

	class CommandBuffer;

	////////////////////////////////////////////////////////////////////////////////////
	// Waitable
	////////////////////////////////////////////////////////////////////////////////////
	enum class WaitOperation : uint8_t { None = 0, AcquireImage, CommandBuffer };

	struct Waitable
	{
	public:
		WaitOperation Operation = WaitOperation::None;

		CommandBuffer* Command = nullptr;
		Queue UsedQueue = Queue::Graphics;

	public:
		Waitable() = default;
		forceinline Waitable(WaitOperation operation, CommandBuffer* command = nullptr, Queue usedQueue = Queue::Graphics)
			: Operation(operation), Command(command), UsedQueue(usedQueue) {}
		~Waitable() = default;
	};

	////////////////////////////////////////////////////////////////////////////////////
	// GraphElement
	////////////////////////////////////////////////////////////////////////////////////
	struct GraphElement
	{
	public:
		CommandBuffer* Command = nullptr;
		Queue UsedQueue = Queue::Graphics;

		std::vector<Waitable> WaitOn = { };

	public:
		GraphElement() = default;
		forceinline GraphElement(CommandBuffer* command = nullptr, Queue usedQueue = Queue::Graphics, std::initializer_list<Waitable> waitOn = { })
			: Command(command), UsedQueue(usedQueue), WaitOn(waitOn) {}
		forceinline GraphElement(CommandBuffer* command = nullptr, Queue usedQueue = Queue::Graphics, const std::vector<Waitable>& waitOn = { })
			: Command(command), UsedQueue(usedQueue), WaitOn(waitOn) {}
		~GraphElement() = default;
	};

	////////////////////////////////////////////////////////////////////////////////////
	// FrameGraph
	////////////////////////////////////////////////////////////////////////////////////
	struct FrameGraph // Note: First thing is acquiring the image, Last thing is presenting
	{
	public:
		std::vector<GraphElement> Elements = { };
	};

}
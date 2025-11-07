#pragma once

namespace Serialization {

	namespace Stream {
		class OutputMemoryStream;
		class InputMemoryStream;

		class DeprecatedOutputMemoryBitStream;
		class DeprecatedInputMemoryBitStream;

		class OutputMemoryBitStream;
		class InputMemoryBitStream;

		class InputMemoryBitStreamV2;
	}


	class ISerializableObject
	{
	public:
		virtual void Serialize(Stream::OutputMemoryStream& stream) = 0;
		virtual void Deserialize(Stream::InputMemoryStream& stream) = 0;

		virtual void Serialize(Stream::DeprecatedOutputMemoryBitStream& stream) = 0;
		virtual void Deserialize(Stream::DeprecatedInputMemoryBitStream& stream) = 0;

		virtual void Serialize(Stream::OutputMemoryBitStream& stream) = 0;
		virtual void Deserialize(Stream::InputMemoryBitStream& stream) = 0;

		virtual void Deserialize(Stream::InputMemoryBitStreamV2& stream) = 0;

		virtual ~ISerializableObject() = default;
	};
}
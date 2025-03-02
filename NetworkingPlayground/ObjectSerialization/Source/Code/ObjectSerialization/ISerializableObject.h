#pragma once

namespace Serialization {

	namespace Stream {
		class OutputMemoryStream;
		class InputMemoryStream;

		class OutputMemoryBitStream;
		//class InputMemoryBitStream;
	}


	class ISerializableObject
	{
	public:
		virtual void Serialize(Stream::OutputMemoryStream& stream) = 0;
		virtual void Deserialize(Stream::InputMemoryStream& stream) = 0;

		virtual void Serialize(Stream::OutputMemoryBitStream& stream) = 0;
		//virtual void Deserialize(Stream::InputMemoryBitStream& stream) = 0;

		virtual ~ISerializableObject() = default;
	};
}
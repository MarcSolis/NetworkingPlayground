#pragma once

namespace Serialization {

	namespace Stream {
		class OutputMemoryStream;
		class InputMemoryStream;
	}


	class ISerializableObject
	{
	public:
		virtual void Serialize(Stream::OutputMemoryStream& stream) = 0;
		virtual void Deserialize(Stream::InputMemoryStream& stream) = 0;

		virtual ~ISerializableObject() = default;
	};
}
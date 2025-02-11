#pragma once

namespace Stream {
	class OutputMemoryStream;
	class InputMemoryStream;
}

namespace Serialization {

	class ISerializableObject
	{
	public:
		virtual void Serialize(Stream::OutputMemoryStream& stream) = 0;
		virtual void Deserialize(Stream::InputMemoryStream& stream) = 0;
	};
}
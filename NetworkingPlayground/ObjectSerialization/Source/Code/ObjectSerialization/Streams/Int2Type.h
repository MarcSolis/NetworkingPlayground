#pragma once

namespace Serialization { namespace Stream {

		template <int v>
		struct Int2Type
		{
			enum { value = v };
		};
}};
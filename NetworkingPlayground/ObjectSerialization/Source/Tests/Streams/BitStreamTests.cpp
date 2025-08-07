
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "Tests/MockSerializableObject.h"


using namespace ::testing;

namespace Serialization {
    namespace Stream {


        namespace Tests {

            class ABitStreamTest : public Test
            {
            protected:
                Serialization::Stream::OutputMemoryBitStream mOutputStream;
                Serialization::Tests::MockSerializationObject mSerializableObj;

				auto GetBufferByteLength() const noexcept
				{
					return mOutputStream.GetBitLength() >> 3;
				}
            };


            TEST_F(ABitStreamTest, WhenVariableSerialized_ThenValueIsPreserved)
            {
               const auto orginalObjectState = mSerializableObj;
			   mSerializableObj.Serialize(mOutputStream);

                EXPECT_TRUE(orginalObjectState == mSerializableObj);
            }

			TEST_F(ABitStreamTest, WhenSerializeVariableFullSize_ThenStoresCorrectByteAmount)
			{
				const auto serializableVal = mSerializableObj.ui32Val;
				EXPECT_CALL(mSerializableObj, Serialize(testing::An<Stream::OutputMemoryBitStream&>())).WillRepeatedly([this, serializableVal](Stream::OutputMemoryBitStream& stream)
					{
						stream.Write(serializableVal);
					});

				mSerializableObj.Serialize(mOutputStream);

				EXPECT_TRUE(GetBufferByteLength() == sizeof(serializableVal));
			}

			TEST_F(ABitStreamTest, WhenSerializeVariableCustomSize_ThenStoresCorrectBitAmount)
			{
				const uint32_t serializableVal = 2042u;
				constexpr size_t BITS_TO_SERIALIZE = 11;
				EXPECT_CALL(mSerializableObj, Serialize(testing::An<Stream::OutputMemoryBitStream&>())).WillRepeatedly([this, serializableVal](Stream::OutputMemoryBitStream& stream)
					{
						stream.Write<uint32_t, BITS_TO_SERIALIZE>(serializableVal);
					});

				mSerializableObj.Serialize(mOutputStream);

				EXPECT_TRUE(mOutputStream.GetBitLength() == BITS_TO_SERIALIZE);
			}

			
			TEST_F(ABitStreamTest, WhenSerializeMultipleVariables_ThenStoresCorrectByteAmount)
			{
				const auto serializableVal = mSerializableObj.ui32Val;
				const auto serializableVal2 = mSerializableObj.ui16Val;
				constexpr size_t BITS_TO_SERIALIZE_1 = 11;
				constexpr size_t BITS_TO_SERIALIZE_2 = 7;
				EXPECT_CALL(mSerializableObj, Serialize(testing::An<Stream::OutputMemoryBitStream&>())).WillRepeatedly([this, serializableVal, serializableVal2](Stream::OutputMemoryBitStream& stream)
					{
						stream.Write<decltype(serializableVal), BITS_TO_SERIALIZE_1>(serializableVal);
						stream.Write<decltype(serializableVal2), BITS_TO_SERIALIZE_2>(serializableVal2);
					});

				mSerializableObj.Serialize(mOutputStream);

				EXPECT_TRUE(mOutputStream.GetBitLength() == BITS_TO_SERIALIZE_1 + BITS_TO_SERIALIZE_2);
			}
		}
}}
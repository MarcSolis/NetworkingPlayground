
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ObjectSerialization/Streams/OutputMemoryStream.h"
#include "ObjectSerialization/Streams/InputMemoryStream.h"

#include "Tests/MockSerializableObject.h"


using namespace ::testing;

namespace Serialization {
    namespace Stream {

		class OutputMemoryStream_Testable : public OutputMemoryStream
		{
		public:
			size_t GetCurrentCapacity() const noexcept { return mCapacity; }
		};


        namespace Tests {

            class AByteStreamTest : public Test
            {
            protected:
                Serialization::Stream::OutputMemoryStream_Testable mOutputStream;
                Serialization::Tests::MockSerializationObject mSerializableObj;


				bool CompareBufferContent(const void* compareData, size_t length)
				{
					if (length == mOutputStream.GetLength())
					{
						return std::memcmp(compareData, mOutputStream.GetBufferPtr(), length) == 0;
					}

					return false;
				}
            };


            TEST_F(AByteStreamTest, WhenVariableSerialized_ThenValueIsPreserved)
            {
               const auto orginalObjectState = mSerializableObj;
			   mSerializableObj.Serialize(mOutputStream);

                EXPECT_TRUE(orginalObjectState == mSerializableObj);
            }

			TEST_F(AByteStreamTest, WhenSerializeVariable_ThenStoresCorrectByteAmount)
			{
				const auto serializableVal = mSerializableObj.ui32Val;
				EXPECT_CALL(mSerializableObj, Serialize(testing::An<Stream::OutputMemoryStream&>())).WillRepeatedly([this, serializableVal](Stream::OutputMemoryStream& stream)
					{
						stream.Write(serializableVal);
					});

				mSerializableObj.Serialize(mOutputStream);

				EXPECT_TRUE(mOutputStream.GetLength() == sizeof(serializableVal));
			}

			TEST_F(AByteStreamTest, WhenSerializeMultipleVariables_ThenStoresCorrectByteAmount)
			{
				const auto serializableVal = mSerializableObj.ui32Val;
				const auto serializableVal2 = mSerializableObj.ui16Val;
				EXPECT_CALL(mSerializableObj, Serialize(testing::An<Stream::OutputMemoryStream&>())).WillRepeatedly([this, serializableVal, serializableVal2](Stream::OutputMemoryStream& stream)
					{
						stream.Write(serializableVal);
						stream.Write(serializableVal2);
					});

				mSerializableObj.Serialize(mOutputStream);

				EXPECT_TRUE(mOutputStream.GetLength() == sizeof(serializableVal) + sizeof(serializableVal2));
			}

			TEST_F(AByteStreamTest, WhenSerializeVariable_ThenStoresCorrectValue)
			{
				const auto serializableVal = mSerializableObj.ui32Val;
				EXPECT_CALL(mSerializableObj, Serialize(testing::An<Stream::OutputMemoryStream&>())).WillRepeatedly([this, serializableVal](Stream::OutputMemoryStream& stream)
					{
						stream.Write(serializableVal);
					});

				mSerializableObj.Serialize(mOutputStream);

				EXPECT_TRUE(CompareBufferContent(&serializableVal, sizeof(serializableVal)));
			}

			TEST_F(AByteStreamTest, WhenSerializeMultipleVariables_ThenStoresCorrectValues)
			{
				const auto serializableVal = mSerializableObj.ui32Val;
				const auto serializableVal2 = mSerializableObj.ui16Val;
				EXPECT_CALL(mSerializableObj, Serialize(testing::An<Stream::OutputMemoryStream&>())).WillRepeatedly([this, serializableVal, serializableVal2](Stream::OutputMemoryStream& stream)
					{
						stream.Write(serializableVal);
						stream.Write(serializableVal2);
					});

				mSerializableObj.Serialize(mOutputStream);

				constexpr size_t serializedDataAmount = sizeof(serializableVal) + sizeof(serializableVal2);
				char* referenceBuffer = new char[serializedDataAmount];
				std::memmove(referenceBuffer, &serializableVal, sizeof(serializableVal));
				std::memmove(referenceBuffer + sizeof(serializableVal), &serializableVal2, sizeof(serializableVal2));

				EXPECT_TRUE(CompareBufferContent(referenceBuffer, serializedDataAmount));
			}

			TEST_F(AByteStreamTest, WhenCapacityReached_ThenReallocationHappens)
			{
				const char arbitraryVal{'A'};
				EXPECT_CALL(mSerializableObj, Serialize(testing::An<Stream::OutputMemoryStream&>())).WillRepeatedly([this, arbitraryVal](Stream::OutputMemoryStream& stream)
					{
						stream.Write(arbitraryVal);
					});

				const auto initialCapacity = mOutputStream.GetCurrentCapacity();

				for (auto i = 0; i <= initialCapacity; ++i)
				{
					mSerializableObj.Serialize(mOutputStream);
				}

				EXPECT_TRUE(initialCapacity < mOutputStream.GetCurrentCapacity());
				EXPECT_TRUE(mOutputStream.GetLength() == initialCapacity + 1);
			}

			TEST_F(AByteStreamTest, WhenDeserializeObject_ThenDataIsCorrect)
			{
				mSerializableObj.SetByteVal(1).Set2ByteVal(2).Set4ByteVal(4).Set8ByteVal(8);
				Serialization::Tests::MockSerializationObject defaultObject;
				
				ASSERT_FALSE(mSerializableObj == defaultObject);

				mSerializableObj.Serialize(mOutputStream);
				Serialization::Stream::InputMemoryStream inputStream{mOutputStream.GetBufferPtr(), mOutputStream.GetLength()};
				defaultObject.Deserialize(inputStream);

				ASSERT_TRUE(mSerializableObj == defaultObject);
			}

			TEST_F(AByteStreamTest, GivenSerializedObject_WhenObjectModified_ThenBufferDataIsPreserved)
			{
				mSerializableObj.SetByteVal(1).Set2ByteVal(2).Set4ByteVal(4).Set8ByteVal(8);
				Serialization::Tests::MockSerializationObject defaultObject;

				ASSERT_FALSE(mSerializableObj == defaultObject);

				mSerializableObj.Serialize(mOutputStream);
				const auto expectedObjValue = mSerializableObj;
				mSerializableObj.SetByteVal(0).Set2ByteVal(0).Set4ByteVal(0).Set8ByteVal(0);

				Serialization::Stream::InputMemoryStream inputStream{mOutputStream.GetBufferPtr(), mOutputStream.GetLength()};
				defaultObject.Deserialize(inputStream);

				ASSERT_TRUE(expectedObjValue == defaultObject);
			}
		}
}}
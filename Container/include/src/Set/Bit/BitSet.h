#pragma once
#include <cinttypes>
#include <memory>

namespace utl
{

	template<size_t BIT_NUM, typename BlockType = uint64_t>
	class BitSet
	{
		template<size_t SUB_BIT_NUM, typename SubBlockType>
		friend class BitSet;
		constexpr inline static size_t BLOCK_SIZE = sizeof(BlockType) * 8;
		constexpr inline static size_t SLACK_SIZE = BIT_NUM % BLOCK_SIZE ? 1 : 0;
		constexpr inline static size_t BLOCK_NUM = (BIT_NUM / (BLOCK_SIZE)) + SLACK_SIZE;
	public:

		//-----------------------------------------
		// 作成
		//-----------------------------------------

		BitSet()
			:bits_()
		{

		}

		//-----------------------------------------
		// 代入
		//-----------------------------------------


		BitSet(const BitSet<BIT_NUM, BlockType>& src)
			:bits_()
		{
			*this = src;
		}
		BitSet& operator=(const BitSet<BIT_NUM, BlockType>& src)
		{
			memcpy(&bits_, &src.bits_, sizeof(bits_));
			return *this;
		}

		//-----------------------------------------
		// 操作
		//-----------------------------------------


		constexpr void Set(size_t index)noexcept
		{
			bits_[index / BLOCK_SIZE] |= BlockType(1) << (index % BLOCK_SIZE);
		}


		constexpr void Reset(size_t index)noexcept
		{
			bits_[index / BLOCK_SIZE] &= ~(BlockType(1) << (index % BLOCK_SIZE));
		}

		constexpr void Clear()noexcept
		{
			for (size_t i = 0; i < BLOCK_NUM; ++i)
			{
				bits_[i] = 0;
			}
		}

		//-----------------------------------------
		// 比較
		//-----------------------------------------

		[[nodiscard]] constexpr bool Empty()const noexcept
		{
			for (size_t i = 0; i < BLOCK_NUM; i++)
			{
				if (bits_[i] != 0)
				{
					return false;
				}
			}
			return true;
		}

		[[nodiscard]] constexpr bool Test(size_t index) const noexcept
		{
			return (bits_[index / BLOCK_SIZE] >> (index % BLOCK_SIZE)) & 1;
		}

		[[nodiscard]] constexpr bool operator==(const BitSet<BIT_NUM, BlockType>& src) const noexcept
		{
			for (size_t i = 0; i < BLOCK_NUM; ++i)
			{
				if (bits_[i] != src.bits_[i])
				{
					return false;
				}
			}
			return true;
		}
		[[nodiscard]] constexpr bool operator!=(const BitSet<BIT_NUM, BlockType>& src) const noexcept
		{
			return !(*this == src);
		}

		[[nodiscard]] constexpr bool IsContain(const BitSet<BIT_NUM, BlockType>& src) const noexcept
		{
			for (size_t i = 0; i < BLOCK_NUM; ++i)
			{
				// 二つのBitsの共通が引数と一致しなければ失敗
				if ((bits_[i] & src.bits_[i]) != src.bits_[i])
				{
					return false;
				}
			}
			return true;
		}

	private:

		BlockType bits_[BLOCK_NUM];

	};
}

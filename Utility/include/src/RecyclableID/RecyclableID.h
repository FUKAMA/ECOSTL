#pragma once
#include <cinttypes>
#include "Container.hpp"

namespace utl
{
	struct RecyclableID
	{
		uint32_t index = 0;
		uint32_t version = 0;
	};

	class RecyclableIDAllocator
	{
	private:
		struct IDInfo
		{
			uint32_t version = 0;
			bool activate = false;
		};

	public:


		RecyclableIDAllocator()
			:nextGeneIndex_(0)
		{
		}

		RecyclableID Allocate()
		{
			RecyclableID result;
			Allocate(result, &RecyclableID::index, &RecyclableID::version);
			return result;
		}

		/// <summary>
		/// 再利用可能なIDを生成し、任意の型の任意の変数にセットする
		/// </summary>
		/// <typeparam name="IDType"></typeparam>
		/// <typeparam name="IndexType"></typeparam>
		/// <typeparam name="VersionType"></typeparam>
		/// <param name="result"></param>
		/// <param name="indexPtr"></param>
		/// <param name="versionPtr"></param>
		template<typename IDType, typename IndexType, typename VersionType>
		void Allocate(IDType& result, IndexType IDType::* indexPtr, VersionType IDType::* versionPtr)
		{
			uint32_t index = 0;
			uint32_t version = 0;

			// リサイクル待ちの添え字がなければ新しく生成して終了
			if (recycleWaitIndeces_.Empty())
			{
				index = ++nextGeneIndex_;
				indexToInfo_.Resize(index + 1);
			}
			else
			{
				index = *recycleWaitIndeces_.Back();
				recycleWaitIndeces_.Resize(recycleWaitIndeces_.Size() - 1);
			}

			// 添え字の情報を更新する
			IDInfo& info = indexToInfo_[index];
			info.activate = true;
			version = ++info.version;

			result.*indexPtr = static_cast<IndexType>(index);
			result.*versionPtr = static_cast<VersionType>(version);
		}

		void Deallocate(const RecyclableID& id)
		{
			Deallocate(id.index, id.version);
		}

		void Deallocate(const uint32_t index, const uint32_t version)
		{
			// すでに無効なIDならなにもしない
			if (!CheckActive(index, version))
			{
				return;
			}

			// 添え字を無効にし、添え字をリサイクル待ちリストに追加
			IDInfo& info = indexToInfo_[index];
			info.activate = false;
			recycleWaitIndeces_.EmplaceBack(index);

		}

		bool CheckActive(const uint32_t index, const uint32_t version)
		{
			// まだ生成されてない添え字なら無効
			if (indexToInfo_.Size() <= index)
			{
				return false;
			}

			// 添え字の最新バージョンで無ければ無効
			IDInfo& info = indexToInfo_[index];
			if (info.version != version)
			{
				return false;
			}

			// 添え字の有効状態を返す
			return info.activate;
		}

	private:

		// 次に生成する添え字
		uint32_t nextGeneIndex_;

		// 添え字と有効状態を紐づけて管理するコンテナ
		Vector<IDInfo> indexToInfo_;
		// リサイクル待ちの添え字を格納するコンテナ
		Vector<uint32_t> recycleWaitIndeces_;

	};

}

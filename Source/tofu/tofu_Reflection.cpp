//------------------------------------------------------------------------------
/**
 * @file    tofu_Reflection.cpp
 * @brief   リフレクション
 * @author  y.fujisawa
 * @par     copyright
 * Copyright (C) 2025 Yasuhito Fujisawa\n
 * This software is released under the MIT License, see LICENSE
*/
//------------------------------------------------------------------------------

#include <tofu_Reflection.h>

#include <tofu_Singleton.h>

#include <unordered_map>
#include <iostream>

namespace tofu {
namespace reflection {

namespace detail
{
	namespace
	{
		/// @brief リフレクション管理クラス
		class Manager : public Singleton<Manager>
		{
		public:
			Manager() = default;

			// クラス登録
			void EntryClass(const detail::ClassCreator& creator)
			{
				TypeId typeId = creator.GetTypeId();
				std::string_view name = typeId.info().GetName();
				// 登録済みは何もしない
				if(m_Classes.contains(name)){
					return;
				}
				
				std::cout << "EntryClass: " << name << std::endl;
				m_Classes.try_emplace(name, &creator);
			}
			
			// クラス生成
			AnyBasePtr<void, std::shared_ptr> Create(std::string_view typeName) const
			{
				auto it = m_Classes.find(typeName);
				if(it != m_Classes.end())
				{
					return it->second->Create();
				}
				return {};
			}

		private:
			std::unordered_map<std::string_view, const detail::ClassCreator*> m_Classes;
		};
	}

	//------------------------------------------------------------------------------
	void EntryClass(const ClassCreator& creator)
	{
		Manager::Instance().EntryClass(creator);
	}
}

//------------------------------------------------------------------------------
AnyBasePtr<void, std::shared_ptr> Create(std::string_view typeName)
{
	return detail::Manager::Instance().Create(typeName);
}

} // reflection
} // tofu


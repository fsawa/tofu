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
			void EntryClass(const ClassReflection& creator)
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

			// 型名からクラスリフレクションクラスを取得する
			SafePtr<const ClassReflection> FindClassReflection(std::string_view name) const noexcept
			{
				auto it = m_Classes.find(name);
				if(it != m_Classes.end())
				{
					return it->second;
				}
				return nullptr;
			}
			
			// クラス生成
			InstancePtr Create(std::string_view type_name) const
			{
				auto it = m_Classes.find(type_name);
				if(it != m_Classes.end())
				{
					return it->second->Create();
				}
				return {};
			}

		private:
			std::unordered_map<std::string_view, const ClassReflection*> m_Classes;
		};
	}

	//------------------------------------------------------------------------------
	void EntryClass(const ClassReflection& creator)
	{
		Manager::Instance().EntryClass(creator);
	}
}

//------------------------------------------------------------------------------
SafePtr<const ClassReflection> FindClassReflection(std::string_view name) noexcept
{
	return detail::Manager::Instance().FindClassReflection(name);
}

//------------------------------------------------------------------------------
InstancePtr Create(std::string_view type_name)
{
	if(auto ptr = FindClassReflection(type_name))
		return ptr->Create();
	return nullptr;
	//return detail::Manager::Instance().Create(type_name);
}

} // reflection
} // tofu


﻿/**
 * FileName: UIManager.cpp
 *
 * Copyright (C) 2024 Maplespe
 *
 * This file is part of MToolBox and DWMBlurGlass.
 * DWMBlurGlass is free software: you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by the Free Software Foundation, either version 3
 * of the License, or any later version.
 *
 * DWMBlurGlass is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with Foobar.
 * If not, see <https://www.gnu.org/licenses/lgpl-3.0.html>.
*/
#include "UIManager.h"
#include "Helper/Helper.h"
#include "pugixml/pugixml.hpp"

namespace MDWMBlurGlass
{
	using namespace Mui;

	std::unordered_map<std::wstring, std::wstring> g_langfileList;
	std::unordered_map<std::wstring, std::wstring> g_baselangList;
	LangInfo g_curlangInfo;

	bool LoadLanguageFileList()
	{
		std::wstring path = Utils::GetCurrentDir() + L"\\data\\lang\\";

		std::vector<std::wstring> fileList;
		EnumFiles(path, L"*.xml", fileList);

		if (fileList.empty())
			return false;

		for(auto& file : fileList)
		{
			pugi::xml_document xmldoc;

			if (!xmldoc.load_file(file.c_str()))
				continue;

			auto root = xmldoc.child(L"lang");
			if (root.empty())
				continue;

			auto local = root.attribute(L"local");
			if (local.empty())
				continue;

			g_langfileList.insert({ static_cast<std::wstring>(local.as_string()), file });
		}

		return !g_langfileList.empty();
	}

	bool LoadBaseLanguageString(std::wstring_view local)
	{
		return LoadLanguageString((XML::MuiXML*)0x01, local, false);
	}

	std::wstring GetBaseLanguageString(std::wstring_view name)
	{
		if (auto iter = g_baselangList.find(name.data()); iter != g_baselangList.end())
			return iter->second;
		return {};
	}

	void ClearBaseLanguage()
	{
		g_baselangList.clear();
	}

	bool LoadLanguageString(XML::MuiXML* ui, std::wstring_view local, bool init, bool replaceCur)
	{
		const auto iter = g_langfileList.find(local.data());
		if (iter == g_langfileList.end() || !ui)
			return false;

		pugi::xml_document xmldoc;

		if (!xmldoc.load_file(iter->second.c_str()))
			return false;

		const auto root = xmldoc.child(L"lang");
		if (root.empty())
			return false;

		for(auto& element : root)
		{
			std::wstring name = element.name();
			std::wstring value = Helper::M_ReplaceString(element.text().get(), L"\\n", L"\n");

			if (name.empty() || value.empty())
				continue;

			if ((_m_ptrv)ui != 1)
			{
				if (init)
					ui->AddStringList(name, value);
				else
					ui->SetStringValue(name, value);
			}
			else
				g_baselangList.insert({ name, value });
		}

		if ((_m_ptrv)ui != 1 && replaceCur)
		{
			g_curlangInfo.local = local;
			g_curlangInfo.author = root.attribute(L"author").value();
		}
		return true;
	}

	bool LoadDefualtUIStyle(XML::MuiXML* ui)
	{
		if (!ui->Mgr()->AddResourcePath(Utils::GetCurrentDir() + L"\\data\\defaultres.dmres", L"12345678"))
			return false;

		ui->Mgr()->LoadStyleList();
		ui->LoadDefaultStyle();

		std::wstring_view xml = 
		MXMLCODE(
		<DefPropGroup control="UICheckBox" style="style_checkbox" minSize="16,16" />
		<DefPropGroup control="UIComBox" dropIcon="dropIcon" />
		);

		if (!ui->AddDefPropGroup(xml, true))
			_M_OutErrorDbg_(L"AddDefaultProp failed! The XML code is invalid.", true);

		return true;
	}

	LangInfo GetCurrentLangInfo()
	{
		return g_curlangInfo;
	}
}
// Copyright (c) 2016-2018 Easy2D - Nomango
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once
#include "Button.h"

namespace easy2d
{
	// 菜单
	class E2D_API Menu
		: public Node
	{
	public:
		Menu();

		explicit Menu(
			Array<ButtonPtr> const& buttons	/* 按钮数组 */
		);

		// 获取菜单是否禁用
		bool IsEnable() const;

		// 获取菜单中的按钮数量
		int GetButtonCount() const;

		// 设置菜单启用或禁用
		void SetEnabled(
			bool enabled
		);

		// 添加按钮
		void AddButton(
			ButtonPtr const& button
		);

		// 移除按钮
		bool RemoveButton(
			ButtonPtr const& button
		);

		// 获取所有按钮
		Array<ButtonPtr> const& GetAllButtons() const;

	private:
		bool enabled_;
		Array<ButtonPtr> buttons_;
	};

}
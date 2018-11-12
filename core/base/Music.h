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
#include "base.h"
#include "audio.h"
#include "RefCounter.h"
#include "Resource.h"
#include <xaudio2.h>

namespace easy2d
{
	// ����
	class Music
		: public RefCounter
	{
		E2D_DISABLE_COPY(Music);

	public:
		Music();

		Music(
			const String& file_path	/* �����ļ�·�� */
		);

		Music(
			Resource& res					/* ������Դ */
		);

		virtual ~Music();

		// �������ļ�
		bool Load(
			const String& file_path	/* �����ļ�·�� */
		);

		// ��������Դ
		bool Load(
			Resource& res					/* ������Դ */
		);

		// ����
		bool Play(
			int loop_count = 0				/* ����ѭ������ (-1 Ϊѭ������) */
		);

		// ��ͣ
		void Pause();

		// ����
		void Resume();

		// ֹͣ
		void Stop();

		// �رղ�������Դ
		void Close();

		// �Ƿ����ڲ���
		bool IsPlaying() const;

		// ��ȡ����
		float GetVolume() const;

		// ��������
		bool SetVolume(
			float volume	/* 1 Ϊԭʼ����, ���� 1 Ϊ�Ŵ�����, 0 Ϊ��С���� */
		);

	protected:
		bool	opened_;
		bool	playing_;
		UINT32	size_;
		BYTE*	wave_data_;
		Voice	voice_;
	};
}
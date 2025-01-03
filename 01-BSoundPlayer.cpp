//
// Copyright (c) 2025 Fas Xmut (fasxmut at protonmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <SoundPlayer.h>
#include <iostream>
#include <future>
#include <bit>

namespace app
{

class cookie
{
private:
	float __incrementer = 0.0f;
public:
	virtual ~cookie()
	{
		std::cout << "cookie is removed" << std::endl;
	}
	cookie() = default;
public:
	float get()
	{
		__incrementer += 0.02f;
		if (__incrementer >= 1.0f)
			__incrementer = -1.0;
		return __incrementer;
	}
	void reset()
	{
		__incrementer = 0.0f;
	}
};

}

int main()
{
	auto cookie = app::cookie{};
	auto player = BSoundPlayer{
		"player",
		[] (
			void * cookie__,
			void * buffer__,
			std::size_t size__,
			const media_raw_audio_format & format__
		)
		{
			if (format__.format != media_raw_audio_format::B_AUDIO_FLOAT)
			{
				std::cout << "Only float format is supported by this program" << std::endl;
				return;
			}
			int dimy = size__ / sizeof float{};
			int dimx = format__.channel_count;

			auto buffer = std::bit_cast<float *>(buffer__);

			auto cookie = std::bit_cast<app::cookie *>(cookie__);
			cookie->reset();

			for (int j=0; j<dimy; ++j)
			{
				for (int i=0; i<dimx; ++i)
				{
					buffer[j*dimx + i] = cookie->get();
				}
			}
		},
		{},
		&cookie
	};
	player.Start();
	player.SetHasData(true);
	std::this_thread::sleep_for(std::chrono::minutes(1));
	player.Stop();
}


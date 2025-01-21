//
// Copyright (c) 2025 Fas Xmut (fasxmut at protonmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <SoundPlayer.h>
#include <iostream>
#include <stdfloat>
#include <future>

namespace audio
{

class cookie
{
public:
	std::float32_t __value;
	std::float32_t __time;
	const std::float32_t __frequency;
	const std::float32_t __k_factor;
	unsigned long long int __frames;
private:
	const std::chrono::system_clock::time_point __start;
	std::chrono::system_clock::time_point __stop;
public:
	virtual ~cookie()
	{
		std::cout << "__frames: " << __frames << std::endl;
		std::cout << "cookie is removed" << std::endl;
		__stop = std::chrono::system_clock::now();
		std::cout << "App duration: "
			<< (__stop - __start).count()  * 1.0 / 1'000'000'000 << " seconds"
			<< std::endl;
	}
	cookie(std::float32_t frequency__):
		__value{0},
		__time{0},
		__frequency{frequency__},
		__k_factor{2.0f32 * __frequency},
		__frames{0},
		__start{std::chrono::system_clock::now()},
		__stop{__start}
	{
	}
	cookie() = delete;
public:
	void sleep(std::float32_t seconds) const
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1)*(seconds*1000));
	}
};

auto make_buffer = []
(
	void * cookie__,
	void * buffer__,
	std::size_t size__,
	const media_raw_audio_format & format__
)
{
	if (format__.format != media_raw_audio_format::B_AUDIO_FLOAT)
	{
		std::cout << "float audio format only" << std::endl;
		return;
	}
	auto & cookie = * std::bit_cast<audio::cookie *>(cookie__);
	auto buffer = std::bit_cast<float *>(buffer__);
	int size = size__ / sizeof float{};
	int channels = format__.channel_count;
	int rate = format__.frame_rate;
	for (int j=0; j<size; ++j)
	{
		std::float32_t time = 1.0f32 / rate * ++cookie.__frames;
		cookie.__value += (time - cookie.__time) * cookie.__k_factor;
		if (cookie.__value > 1.0f)
			cookie.__value = -1.0f;
		cookie.__time = time;
		for (int i=0; i<channels; ++i)
		{
			buffer[j*channels+i] = cookie.__value;
		}
	}
};

}

int main()
{
	auto cookie = audio::cookie{440};
	auto player = BSoundPlayer{
		"buffer",
		audio::make_buffer,
		{},
		&cookie
	};
	player.Start();
	player.SetHasData(true);
	cookie.sleep(120);	// 120 seconds: 2 minutes
	player.Stop();
}


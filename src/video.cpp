#include "video.hpp"
#include "yuv2rgb.h"
#include <godot_cpp/variant/utility_functions.hpp>
using namespace godot;

Ref<VideoStreamPlayback> VideoStreamY4m::_instantiate_playback()
{
	try
	{
		return memnew(VideoStreamPlaybackY4m(get_file()));
	}
	catch (...)
	{
		return Ref<VideoStreamPlayback>();
	}
}

void VideoStreamPlaybackY4m::_bind_methods() {}

void VideoStreamPlaybackY4m::_play()
{
	playing = true;
}
void VideoStreamPlaybackY4m::_stop()
{
	playing = false;
	current_pos = 0;
}
bool VideoStreamPlaybackY4m::_is_playing() const
{
	return playing;
}

void VideoStreamPlaybackY4m::_set_paused(bool p_paused)
{
	paused = p_paused;
}
bool VideoStreamPlaybackY4m::_is_paused() const
{
	return paused;
}

double VideoStreamPlaybackY4m::_get_length() const
{
	return length;
}

double VideoStreamPlaybackY4m::_get_playback_position() const
{
	return current_pos;
}

void VideoStreamPlaybackY4m::_seek(double p_time)
{
	current_pos = p_time;
	_update(0); // trigger update to refresh texture
}

Ref<Texture2D> VideoStreamPlaybackY4m::_get_texture() const
{
	return texture;
}

void VideoStreamPlaybackY4m::_update(double p_delta)
{
	paused || !playing ? 0 : current_pos += p_delta;
	if (current_pos > length)
	{
		_stop();
		return;
	}
	size_t new_frame = current_pos * frame_rate;
	if (new_frame == current_frame)
		return;
	else if (new_frame == current_frame + 1)
	{
		decode_frame();
		current_frame++;
	}
	else if (new_frame < current_frame)
	{
		file->seek(frame_offsets[new_frame]);
		decode_frame();
		current_frame = new_frame;
	}
	else // (new_frame >current_frame)
	{
		current_frame = std::min<int64_t>(frame_offsets.size() - 1, new_frame);
		file->seek(frame_offsets[current_frame]);
		while (current_frame < new_frame-1)
			fwd();
		decode_frame();
		current_frame++;
	}

	texture->update(rgba_image);
	return;
}

int VideoStreamPlaybackY4m::_get_channels() const { return 1; }
int VideoStreamPlaybackY4m::_get_mix_rate() const { return 44100; }

void VideoStreamPlaybackY4m::_set_audio_track(int p_idx) {}

VideoStreamPlaybackY4m::VideoStreamPlaybackY4m() {}
VideoStreamPlaybackY4m::VideoStreamPlaybackY4m(const String &path) : file(FileAccess::open(path, FileAccess::READ))
{
	// read header
	uint8_t YUV4MPEG2[10];
	file->get_buffer(YUV4MPEG2, 10);
	if (memcmp(YUV4MPEG2, R"(YUV4MPEG2 )", 10) != 0)
		throw std::exception("not y4m file");
	auto params = file->get_csv_line(" ");
	for (auto it = params.begin(); it != params.end(); ++it)
		switch ((*it)[0])
		{
		case 'W':
			size.width = it->substr(1).to_int();
			break;
		case 'H':
			size.height = it->substr(1).to_int();
		case 'C':
			// todo read colorspace and don't assume c420jpeg
		case 'F':
			frame_rate = static_cast<float>(it->get_slice(":", 0).to_int()) / it->get_slice(":", 1).to_int();
		case 'I':
			// please don't use interlaced in {current_year}
		case 'A':
		case 'X':
		default:
			break;
		}
	frame_offsets.push_back(file->get_position());
	length = (file->get_length() / get_YUV_frame_buffer_size()-1) / frame_rate;
	rgba_image = Image::create_empty(size.x, size.y, false, Image::FORMAT_RGBA8);
	// frame_data_rgba=const_cast<uint8_t*> (rgba_image->get_data().ptr());
	// decode first frame
	decode_frame();
	texture.instantiate();
	texture->set_image(rgba_image);
}

VideoStreamPlaybackY4m::~VideoStreamPlaybackY4m() {}

constexpr size_t VideoStreamPlaybackY4m::get_YUV_frame_buffer_size() const
{
	return (size.x * size.y * 3) / 2;
}

constexpr size_t VideoStreamPlaybackY4m::get_U_offset() const
{
	return size.x * size.y;
}

constexpr size_t VideoStreamPlaybackY4m::get_V_offset() const
{
	return (size.x * size.y * 5) / 4;
}

void VideoStreamPlaybackY4m::fwd()
{
	skip_frame_header();
	file->seek(file->get_position() + get_YUV_frame_buffer_size());
	frame_offsets.push_back(file->get_position());
	current_frame++;
}

void VideoStreamPlaybackY4m::skip_frame_header()
{
	auto FRAME = file->get_line();
	// todo check that the string starts by "FRAME + potential garbage before new line that is ignored"
}

void VideoStreamPlaybackY4m::decode_frame()
{

	skip_frame_header();
	auto frame_data_yuv = file->get_buffer(get_YUV_frame_buffer_size());
	yuv420_2_rgb8888(rgba_image->ptrw(), frame_data_yuv.ptr(), frame_data_yuv.ptr() + get_U_offset(), frame_data_yuv.ptr() + get_V_offset(), size.x, size.y, size.x, size.x >> 1, size.x << 2);
	if (frame_offsets.size() <= current_frame + 1)
		frame_offsets.push_back(file->get_position());
}

Variant ResourceFormatLoaderY4m::_load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const
{
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	if (f.is_null())
	{
		return Ref<Resource>();
	}

	Ref<VideoStreamY4m> Y4m_stream = Ref<VideoStreamY4m>();
	Y4m_stream.instantiate();
	Y4m_stream->set_file(p_path);

	return Y4m_stream;
}

PackedStringArray ResourceFormatLoaderY4m::_get_recognized_extensions() const
{
	PackedStringArray arr;
	arr.push_back("y4m");
	return arr;
}

bool ResourceFormatLoaderY4m::_handles_type(const StringName &p_type) const
{
	return ClassDB::is_parent_class(p_type, "VideoStream");
}

String ResourceFormatLoaderY4m::_get_resource_type(const String &p_path) const
{
	String el = p_path.get_extension().to_lower();
	if (el == "y4m")
	{
		return "VideoStreamY4m";
	}
	return "";
}
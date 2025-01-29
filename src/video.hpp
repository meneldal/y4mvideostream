#pragma once

#include <cstdint>
#include <cmath>

#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>

#include <fstream>

namespace godot
{

	class VideoStreamY4m : public VideoStream
	{
		GDCLASS(VideoStreamY4m, VideoStream);

	public:
		virtual Ref<VideoStreamPlayback> _instantiate_playback() override;

	protected:
		static void _bind_methods() {}
	};
	class VideoStreamPlaybackY4m : public VideoStreamPlayback
	{
		GDCLASS(VideoStreamPlaybackY4m, VideoStreamPlayback);

	protected:
		static void _bind_methods();

	public:
		virtual void _play() override;
		virtual void _stop() override;
		virtual bool _is_playing() const override;

		virtual void _set_paused(bool p_paused) override;
		virtual bool _is_paused() const override;

		virtual double _get_length() const override;

		virtual double _get_playback_position() const override;
		virtual void _seek(double p_time) override;

		virtual Ref<Texture2D> _get_texture() const override;
		virtual void _update(double p_delta) override;

		virtual int _get_channels() const override;
		virtual int _get_mix_rate() const override;

		virtual void _set_audio_track(int p_idx) override;

		VideoStreamPlaybackY4m();
		VideoStreamPlaybackY4m(const String &path);
		~VideoStreamPlaybackY4m();

	private:
		Ref<FileAccess> file;
		Vector2i size = Vector2i{0, 0};
		float current_pos = 0;
		bool paused = false;
		bool playing = false;
		Vector<size_t> frame_offsets;
		float frame_rate = 25;
		float length = 0;
		Ref<ImageTexture> texture;
		// uint8_t* frame_data_rgba=nullptr; //extracted from Image with a `const_cast` because Godot API sucks
		Ref<Image> rgba_image;
		size_t current_frame = 0;
		// internal functions
		constexpr size_t get_YUV_frame_buffer_size() const;
		constexpr size_t get_U_offset() const;
		constexpr size_t get_V_offset() const;
		void fwd();
		void skip_frame_header();
		void decode_frame();
	};

	class ResourceFormatLoaderY4m : public ResourceFormatLoader
	{
		GDCLASS(ResourceFormatLoaderY4m, ResourceFormatLoader);

	public:
		virtual Variant _load(const String &p_path, const String &p_original_path, bool p_use_sub_threads, int32_t p_cache_mode) const override;
		virtual PackedStringArray _get_recognized_extensions() const override;
		virtual bool _handles_type(const StringName &p_type) const override;
		virtual String _get_resource_type(const String &p_path) const override;

	protected:
		static void _bind_methods() {}
	};

}

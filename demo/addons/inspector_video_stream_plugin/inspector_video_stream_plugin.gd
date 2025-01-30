extends EditorInspectorPlugin

var scroll_editor = preload("res://addons/inspector_video_stream_plugin/seek_bar.gd")

func _can_handle(object):
	return object is VideoStreamPlayer

func _parse_category(object: Object, category: String) -> void:
	if(category=="VideoStreamPlayer"):
		var myscroll=scroll_editor.new()
		myscroll.set_max(object.get_stream_length())
		myscroll.set_value(object.stream_position)
		add_property_editor("stream_position", myscroll,false,"Stream Position")

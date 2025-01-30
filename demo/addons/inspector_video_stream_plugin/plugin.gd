@tool
extends EditorPlugin

var plugin

func _enter_tree() -> void:
	plugin = preload("res://addons/inspector_video_stream_plugin/inspector_video_stream_plugin.gd").new()
	add_inspector_plugin(plugin)
	pass


func _exit_tree() -> void:
	remove_inspector_plugin(plugin)
	pass

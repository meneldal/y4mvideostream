extends EditorProperty

var property_control = EditorSpinSlider.new()

var current_value = 0

# Called when the node enters the scene tree for the first time.
func _init()-> void:
	add_child(property_control)
	add_focusable(property_control)
	property_control.min_value=0
	property_control.step=0.5
	property_control.value_changed.connect(_on_value_changed)

func set_max(m:float)->void:
	property_control.max_value=m

func _on_value_changed(value:float)->void:
	current_value = value
	emit_changed(get_edited_property(), current_value)

func _update_property() -> void:
	var new_value = get_edited_object()[get_edited_property()]
	if (new_value == current_value):
		return

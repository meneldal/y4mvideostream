#include "register_types.hpp"
#include <godot_cpp/classes/resource_loader.hpp>

static Ref<ResourceFormatLoaderY4m> y4m_loader;

void initialize_y4m_videostream_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) 
		return;
	
	ClassDB::register_class<VideoStreamPlaybackY4m>();
	ClassDB::register_class<VideoStreamY4m>();
	ClassDB::register_class<ResourceFormatLoaderY4m>();
	y4m_loader.instantiate();
	ResourceLoader::get_singleton()->add_resource_format_loader(y4m_loader);
}

void uinitialize_y4m_videostream_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) 
		return;
	ResourceLoader::get_singleton()->remove_resource_format_loader(y4m_loader);
	y4m_loader.unref();
}

extern "C" {
	// Initialization
	GDExtensionBool GDE_EXPORT
	y4m_videostream_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		
		init_obj.register_initializer(initialize_y4m_videostream_module);
		init_obj.register_terminator(uinitialize_y4m_videostream_module);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}

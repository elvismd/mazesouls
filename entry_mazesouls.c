typedef enum EntityArchetype
{
	arch_nil = 0,
	arch_player = 1,
	arch_bush = 2
} EntityArchetype;

typedef struct Entity
{
	bool is_valid;
	bool render_sprite;
	EntityArchetype arch;
	Vector2 pos;
	Vector2 size;
	Gfx_Image* sprite;
} Entity;
#define MAX_ENTITY_COUNT 1024

typedef struct World
{
	Entity entities[MAX_ENTITY_COUNT];
} World;
World* world = 0;

Entity* entity_create()
{
	Entity* entity_found = 0;
	for(int i = 0; i < MAX_ENTITY_COUNT; i++)
	{
		Entity* existing_entity = &world->entities[i];
		if(!existing_entity->is_valid)
		{
			entity_found = existing_entity;
			existing_entity->is_valid = true;
			break;
		}
	}
	assert(entity_found, "No more free entities!");
	return entity_found;
}

void entity_destroy(Entity* entity)
{
	memset(entity, 0, sizeof(Entity));
}

void setup_bush(Entity* en)
{
	en->arch = arch_bush;
	en->pos.x -= 10;
	en->sprite = load_image_from_disk(STR("content/Deco/09.png"), get_heap_allocator());
	assert(en->sprite, "Failed loading berry_bush.png");

	en->size = v2(en->sprite->width, en->sprite->height);
}

void setup_player(Entity* en)
{
	en->arch = arch_player;
	en->pos.x = 0;

	en->sprite = load_image_from_disk(STR("content/Factions/Knights/Troops/Warrior/Yellow/Warrior_Yellow.png"), get_heap_allocator());
	assert(en->sprite, "Failed loading Warrior_Yellow.png");

	en->size = v2(en->sprite->width, en->sprite->height);
}

int entry(int argc, char **argv) {
	
	window.title = STR("Minimal Game Example");
	window.scaled_width = 1280; // We need to set the scaled size if we want to handle system scaling (DPI)
	window.scaled_height = 720; 
	window.x = 200;
	window.y = 90;
	window.clear_color = hex_to_rgba(0x6495EDff);

	world = alloc(get_heap_allocator(), sizeof(World));

	Entity* player_en = entity_create();
	setup_player(player_en);
	
	AnimationFlipbook* idle_anim = create_anim_flipbook(
		player_en->sprite, 
		v2i(8, 6), 
		0, 6,
		20.0f,
		get_heap_allocator());

	AnimationFlipbook* run_right_anim = create_anim_flipbook(
		player_en->sprite, 
		v2i(8, 6), 
		1, 6, 
		10.0f,
		get_heap_allocator());

	for(int i = 0; i < 30; i++)
	{
		Entity* en = entity_create();
		setup_bush(en);
		en->pos = v2(get_random_float32_in_range(-130, 130), get_random_float32_in_range(-130, 130));
		
	}
	
	float64 last_time = os_get_current_time_in_seconds();
	float64 seconds_counter = 0.0;
	int frame_counter = 0;

	while (!window.should_close) {
		reset_temporary_storage();
		
		f32 zoom = 1;
		// defining the right screen size of the ortho projection to fit the real size in pixels of the sprites
		float32 aspect = ((float32)window.width/(float32)window.height);
		draw_frame.projection = m4_make_orthographic_projection(
			window.width * -0.5, 
			window.width * 0.5,
			window.height * -0.5,
			window.height * 0.5,
			-1, 10);
		// zooming the ortho projection
		draw_frame.view = m4_make_scale(v3(1.0/zoom, 1.0/zoom, 1.0));

		float64 now = os_get_current_time_in_seconds();
		float64 delta = now - last_time;
		last_time = now;

		update_anim_flipbook(run_right_anim, delta);
		player_en->size = v2(run_right_anim->sourceRectangle.z, run_right_anim->sourceRectangle.w);

		if (is_key_just_released(KEY_ESCAPE)) {
			window.should_close = true;
		}

		Vector2 input_move_axis = v2(0, 0);
		if (is_key_down('A')) {
			input_move_axis.x -= 1.0;
		}
		if (is_key_down('D')) {
			input_move_axis.x += 1.0;
		}
		if (is_key_down('S')) {
			input_move_axis.y -= 1.0;
		}
		if (is_key_down('W')) {
			input_move_axis.y += 1.0;
		}
		input_move_axis = v2_normalize(input_move_axis);
		
		//float aspect = (f32)window.width/(f32)window.height;
		// float mx = (input_frame.mouse_x/(f32)window.width  * 2.0 - 1.0)*aspect;
		// float my = input_frame.mouse_y/(f32)window.height * 2.0 - 1.0;
		player_en->pos = v2_add(player_en->pos, v2_mulf(input_move_axis, 20 * delta));
		
		for(int i = 0; i < MAX_ENTITY_COUNT; i++)
		{
			Entity* en = &world->entities[i];
			if(en->is_valid)
			{
				switch(en->arch)
				{
					case arch_player:				
					{
						Matrix4 hammer_xform = m4_scalar(1.0);
						hammer_xform         = m4_rotate_z(hammer_xform, 0.0f);
						hammer_xform         = m4_translate(hammer_xform, v3(en->pos.x, en->pos.y, 0));
						hammer_xform         = m4_translate(hammer_xform, v3(en->size.x * -0.5, en->size.y * -0.5, 0));			
						
						draw_image_xform_source(
							en->sprite, 
							hammer_xform,
							run_right_anim->sourceRectangle, 
							en->size, 
							COLOR_WHITE);
					}
					break;
					default:
					{
						Matrix4 hammer_xform = m4_scalar(1.0);
						hammer_xform         = m4_rotate_z(hammer_xform, 0.0f);
						hammer_xform         = m4_translate(hammer_xform, v3(en->pos.x, en->pos.y, 0));
						hammer_xform         = m4_translate(hammer_xform, v3(en->size.x * -0.5, en->size.y * -0.5, 0));			
						draw_image_xform(
							en->sprite, 
							hammer_xform,
							en->size, 
							COLOR_WHITE);
					}
					break;
				}
			}
		}
		
		os_update(); 
		gfx_update();

		seconds_counter += delta;
		frame_counter += 1;
		if (seconds_counter > 1.0) 
		{
			log("FPS: %i", frame_counter);
			seconds_counter = 0.0;
			frame_counter = 0;
		}
	}

	return 0;
}
#pragma pack(push,1)
struct ColorControl {
  int color_value;
};
struct ColorConversion {
  int components[4];
  char temp_value;
};
struct ImageBuffer {
  void * data;
  int width;
  int height;
  int stride;
  int bit_depth;
  int flags;
  void * color_map;
};
struct Stats {
  int health;
};
struct GameStructure {
  int is_active;
  unsigned char gap4[40];
  int time_value;
};
struct CharacterData {
  unsigned char gap0[151];
  char type;
};
struct Character {
  unsigned char gap0[172];
  char type;
};
struct WorldController {
  unsigned char gap0[480];
  int current_player;
};
struct Vector3 {
  int x;
  int y;
  int z;
};
struct Model {
  char data[60];
  Vector3 position;
  char padding1[20];
  int sound_position;
  char padding2[28];
  int controller;
  char padding3[16];
};
struct EntityAttributes {
  char unknown_attributes[132];
  int energy;
  unsigned char gap88[14];
  char incapacitated;
  unsigned char gap97[101];
  int stamina;
};
struct Point {
  int x;
  unsigned char gap4[4];
  int y;
};
struct Vector {
  int x;
  unsigned char gap4[4];
  int y;
};
struct Position {
  char _dummy;
};
struct ModelController {
  char data[84];
  int rotation_offset;
};
struct ModelData {
  char data[250];
  __int16 flags;
  int rotation_adjustment;
};
struct Vector2D {
  int x;
  unsigned char gap4[4];
  int y;
};
struct RevivalData {
  unsigned char gap0[476];
  int min_x;
  unsigned char gap1E0[4];
  int min_y;
  int max_x;
  unsigned char gap1EC[4];
  int max_y;
};
struct BoundsData {
  unsigned char gap0[480];
  int z_position;
};
struct Matrix4x4 {
  int scale_x;
  unsigned char gap4[12];
  int scale_y;
  unsigned char gap14[12];
  int scale_z;
  unsigned char gap24[12];
  int translation_x;
  int translation_y;
  int translation_z;
};
struct MeshHeader {
  unsigned char gap0[36];
  int submesh_count;
};
struct MeshData {
  unsigned char gap0[36];
  char vertex_count[4];
  unsigned char gap28[16];
  int vertices;
  int header;
};
struct BitmapHeader {
  unsigned char gap0[16];
  int buffer_size;
};
struct Bitmap {
  char _dummy;
};
struct Resource {
  char _dummy;
};
struct Vertex {
  unsigned char gap0[24];
  int x;
  int y;
};
struct BitmapInfo {
  int pitch;
  int buffer_ptr;
};
struct RenderState {
  unsigned char gap0[40];
  int color_buffer;
};
struct ScaleParams {
  unsigned char gap0[8];
  int scale_factor;
};
struct Node {
  unsigned char gap0[32];
  int model_data;
  unsigned char gap24[88];
  int next;
  unsigned char gap80[4];
  int child;
  unsigned char gap88[20];
  int element_count;
  int elements;
};
struct NodeElement {
  char data[28];
};
struct ControlPoint {
  int control_value;
  int x;
  int y;
  int z;
};
struct MemoryManager {
  char _dummy;
};
struct SceneController {
  unsigned char gap0[80];
  int root_node;
};
struct GameInterface {
  char _dummy;
};
struct ObjectVtable {
  unsigned char gap0[32];
  int initialized;
};
struct ObjectBase {
  int vtable;
};
struct GraphicsElement {
  unsigned char gap0[32];
  int visibility_flag;
};
struct Entity {
  char unknown_data[172];
  int character_data;
  char more_unknown_data[2812];
  unsigned char gapBAC[124];
  int animation_time_primary;
  int animation_time_tertiary;
  unsigned char gapC30[4];
  int animation_time_secondary;
  unsigned char gapC38[36];
  int attack_threshold;
  char more_data[4];
  int cooldown_timer;
  unsigned char gapC68[12];
  int hit_flag;
  int special_state;
  unsigned char gapC7C[112];
  int animation_param1;
  int animation_param2;
  int animation_param3;
  int animation_param4;
  unsigned char gapCFC[8];
  int level;
  int previous_level;
  unsigned char gapD0C[16];
  int invulnerability_flag;
  Vector3 target_position;
  char padding4[36];
  int model_data;
};
struct entity_t {
  char unknown_data[172];
  int character_data;
  char more_unknown_data[2812];
  int animation_time_primary;
  int animation_time_tertiary;
  int animation_time_secondary;
  int attack_threshold;
  char more_data[4];
  int cooldown_timer;
  int hit_flag;
  int special_state;
  int animation_param1;
  int animation_param2;
  int animation_param3;
  int animation_param4;
  int level;
  int previous_level;
  int invulnerability_flag;
  int target_position[3];
  char padding4[36];
  int model_data;
};
struct vector3_t {
  int x;
  int y;
  int z;
};
struct model_data_t {
  char data[60];
  int position[3];
  char padding1[20];
  int sound_position;
  char padding2[28];
  int controller;
  char padding3[16];
};
struct bounds_data_t {
  char padding[4];
  int z_position;
  int min_x;
  int min_y;
  int max_x;
  int max_y;
};
struct render_state_t {
  char padding[12];
  int color_buffer;
};
struct GraphicsContext {
  int rendering_data;
};
struct GameContext {
  int width;
  int height;
  int stride;
  int buffer;
};
struct RGBColor {
  char red;
  char green;
  char blue;
};
struct ColorTransform {
  unsigned char gap0[4];
  int luminance;
  int chroma_rg;
  int chroma_gb;
};
struct SceneNode {
  unsigned char gap0[32];
  char mesh_data[4];
  unsigned char gap24[88];
  char next_sibling[4];
  unsigned char gap80[4];
  char color_attachment[4];
};
struct VertexColorData {
  unsigned char gap0[40];
  char transformed_color;
  unsigned char gap29[51];
  char packed_color[4];
};
struct ModelElement {
  unsigned char gap0[40];
  char state;
};
struct InventoryData {
  int quantity;
  int max_capacity;
};
struct ShellEffectParams {
  char _dummy;
};
struct PlayerData {
  char multiplayer_flag[4];
  char creature_config[4];
};
struct CreatureType {
  char DEFAULT[4];
  char SERPENT[4];
  char POISSON[4];
  char PIAF[4];
  char CHSOURIS[4];
  char TITI[4];
  char LAME[4];
};
struct PlayerInventory {
  int secondary_inventory;
  int primary_inventory;
  int player_data_1;
  int player_data_2;
  int player_creatures;
  int secondary_flag;
};
struct EntityInteractionData {
  int x_position;
  int y_position;
  int grid_x;
  int grid_y;
  char interaction_threshold;
};
struct EntityPositionParams {
  int sine_offset;
  int sine_multiplier;
  int time_difference;
  int position_offset;
};
struct EntityState {
  int direction;
  __int64 angle;
  __int64 position_x;
  __int64 position_y;
  __int64 velocity;
};
struct entity_state {
  int state_flag;
  float angle[2];
};
struct DataPointer {
  unsigned char gap0[108];
  int sub_data_offset;
  unsigned char gap70[12];
  int sine_table_offset;
};
struct SineTable {
  char sine_values[8192];
};
struct vector3d {
  char x[8];
  char y[8];
  char z[8];
};
struct point3d_t {
  int x;
  int y;
  int z;
};
struct world_controller_t {
  int entity_count;
  char entities[4];
};
struct render_entity_t {
  char position[12];
  int state_flag;
  int additional_data;
};
struct health_buffer_t {
  int buffer;
  int size;
};
struct Weapon {
  int weapon_data;
  int weapon_state;
  char weapon_name[32];
};
struct EntityTransformation {
  int position_x;
  int position_y;
  int position_z;
  int transformed_x;
  int transformed_y;
  int transformed_z;
  int random_value;
  int color_mode;
};
struct Vector3D {
  int vector_x;
  int vector_y;
  int vector_z;
};
#pragma pack(pop)

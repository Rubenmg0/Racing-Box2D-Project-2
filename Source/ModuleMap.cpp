#include "Globals.h"
#include "Application.h"
#include "ModuleMap.h"
#include "ModuleGame.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"

#include <iostream>
#include <sstream>
#include <algorithm>

#include "ModulePhysics.h"

ModuleMap::ModuleMap(Application* app, bool start_enabled) : Module(app, start_enabled), map_loaded(false)
{
}

ModuleMap::~ModuleMap()
{
}

bool ModuleMap::Start()
{
    LOG("Loading Map");
    bool ret = true;

    return ret;
}

bool ModuleMap::Load(const char* file_name)
{
    pugi::xml_document map_file;
    pugi::xml_parse_result result = map_file.load_file(file_name);

    if (result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
        return false;
    }

    //Load Map Properties
    pugi::xml_node map = map_file.child("map");

    map_data.width = map.attribute("width").as_int();
    map_data.height = map.attribute("height").as_int();
    map_data.tilewidth = map.attribute("tilewidth").as_int();
    map_data.tileheight = map.attribute("tileheight").as_int();

    //Load Tilesets
    for (pugi::xml_node tileset = map.child("tileset"); tileset; tileset = tileset.next_sibling("tileset"))
    {
        TileSet* set = new TileSet();
        set->firstgid = tileset.attribute("firstgid").as_int();
        set->name = tileset.attribute("name").as_string();
        set->tilewidth = tileset.attribute("tilewidth").as_int();
        set->tileheight = tileset.attribute("tileheight").as_int();
        set->spacing = tileset.attribute("spacing").as_int();
        set->margin = tileset.attribute("margin").as_int();

        pugi::xml_node image = tileset.child("image");
        set->tex_width = image.attribute("width").as_int();
        set->tex_height = image.attribute("height").as_int();

        //Build image path 
        std::string image_source = image.attribute("source").as_string();
        std::string path = "Assets/map/" + image_source;

        set->texture = LoadTexture(path.c_str());

        map_data.tilesets.push_back(set);
    }

    //Load Layers
    for (pugi::xml_node layer = map.child("layer"); layer; layer = layer.next_sibling("layer"))
    {
        MapLayer* lay = new MapLayer();
        lay->name = layer.attribute("name").as_string();
        lay->width = layer.attribute("width").as_int();
        lay->height = layer.attribute("height").as_int();
        lay->visible = layer.attribute("visible").as_bool(true);

        for (pugi::xml_node tileNode = layer.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
            lay->tiles.push_back(tileNode.attribute("gid").as_uint());
        }

        map_data.layers.push_back(lay);
    }

    for (pugi::xml_node objectGroupNode = map_file.child("map").child("objectgroup"); objectGroupNode != NULL; objectGroupNode = objectGroupNode.next_sibling("objectgroup")) {

        // Load Object Group
        ObjectGroup* objectgroup = new ObjectGroup();
        objectgroup->name = objectGroupNode.attribute("name").as_string();
        for (pugi::xml_node objectNode = objectGroupNode.child("object"); objectNode != NULL; objectNode = objectNode.next_sibling("object")) {
            ObjectGroup::Object* o = new ObjectGroup::Object;

            // Save all Object attributes
            o->id = objectNode.attribute("id").as_int();
            o->x = objectNode.attribute("x").as_float();
            o->y = objectNode.attribute("y").as_float();
            o->width = objectNode.attribute("width").as_float();
            o->height = objectNode.attribute("height").as_float();

            pugi::xml_node properties = objectNode.child("properties");
            if (properties)
            {
                for (pugi::xml_node prop = properties.child("property"); prop; prop = prop.next_sibling("property"))
                {
                    // Buscamos una propiedad que se llame "order" en Tiled
                    if (std::string(prop.attribute("name").as_string()) == "order")
                    {
                        o->order = prop.attribute("value").as_int();
                    }
                }
            }

            if (objectNode.child("polygon").attribute("points") != NULL)
            {
                std::string pointString = objectNode.child("polygon").attribute("points").as_string();
                size_t start = 0;

                while (start < pointString.length())
                {
                    size_t end = pointString.find(' ', start);
                    if (end == std::string::npos) { end = pointString.length(); }

                    std::string pair = pointString.substr(start, end - start);
                    size_t comma = pair.find(',');

                    if (comma != std::string::npos)
                    {
                        b2Vec2 pointPos = { stoi(pair.substr(0, comma)) + o->x,  stoi(pair.substr(comma + 1)) + o->y };
                        o->points.push_back(pointPos);
                    }

                    start = end + 1;
                }
            }
            objectgroup->objects.push_back(o);
        }

        //Add the objects to the map
        map_data.objectGroups.push_back(objectgroup);
    }

    int cpCount = 0; //Counter for assigning IDs

    //Creation of colliders and assign their type
    for (const auto& objectsGroups : map_data.objectGroups) {

        if (objectsGroups->name == "CheckPoints")
        {
            std::sort(objectsGroups->objects.begin(), objectsGroups->objects.end(), [](ObjectGroup::Object* a, ObjectGroup::Object* b) {
                    return a->order < b->order;
                });
        }

        for (const auto& obj : objectsGroups->objects) {
            if (objectsGroups->name == "Collision") {
                PhysBody* pb = App->physics->CreateStaticWall(obj->x + obj->width / 2, obj->y + obj->height / 2, obj->width, obj->height);
                pb->type = BodyType::WALL;
                map_data.colliders.push_back(pb);
            }
            else if (objectsGroups->name == "CheckPoints") {
                PhysBody* pb = App->physics->CreateRectangleSensor(obj->x + obj->width / 2, obj->y + obj->height / 2, obj->width, obj->height);
                pb->type = BodyType::CHECKPOINT;
                pb->checkpointID = cpCount++;
                pb->listener = (Module*)App->scene_intro;
                map_data.colliders.push_back(pb);
            }
        }
    }
    //Save the total number of checkpoints in ModuleGame to know when a lap ends.
    App->scene_intro->totalCheckpoints = cpCount;


    map_loaded = true;
    LOG("Map loaded successfully");
    return true;
}

update_status ModuleMap::Update()
{
    if (map_loaded) Draw();
    return UPDATE_CONTINUE;
}

void ModuleMap::Draw()
{
    //Go through all layers
    for (const auto& layer : map_data.layers)
    {
        if (!layer->visible) continue;

        //Iterate through every tile in the layer
        for (int y = 0; y < map_data.height; ++y)
        {
            for (int x = 0; x < map_data.width; ++x)
            {
                int index = (y * layer->width) + x;

                int gid = layer->tiles[index];

                // gid of 0 means it's empty
                if (gid > 0)
                {
                    TileSet* tileset = nullptr;

                    //Search the tileset that corresponds to the gid
                    for (auto& ts : map_data.tilesets)
                    {
                        if (gid >= ts->firstgid)
                        {
                            tileset = ts;
                        }
                    }

                    if (tileset != nullptr)
                    {
                        Rectangle rect = tileset->GetTileRect(gid);
                        Vector2 pos = MapToWorld(x, y);

                        //Use Draw from ModuleRender because of the camera
                        App->renderer->Draw(tileset->texture, (int)pos.x, (int)pos.y, &rect);
                    }
                }
            }
        }
    }
}

bool ModuleMap::CleanUp()
{
    LOG("Unloading map");

    for (auto& layer : map_data.layers) delete layer;
    map_data.layers.clear();

    for (auto& tileset : map_data.tilesets) {
        UnloadTexture(tileset->texture);
        delete tileset;
    }
    map_data.tilesets.clear();

    for (auto& group : map_data.objectGroups) {
        for (auto& obj : group->objects) {
            delete obj;
        }
        delete group; 
    }
    map_data.objectGroups.clear();

    for (auto& collider : map_data.colliders) {
        collider->DeleteAllBodies(App);
        delete collider;
    }
    map_data.colliders.clear();

    return true;
}

Vector2 ModuleMap::MapToWorld(int x, int y) const
{
    return Vector2{ (float)(x * map_data.tilewidth), (float)(y * map_data.tileheight) };
}

TileSet* ModuleMap::GetTileset(const char* name)
{
    for (auto& tileset : map_data.tilesets)
    {
        if (tileset->name == name)
        {
            return tileset;
        }
    }
    return nullptr;
}

Rectangle TileSet::GetTileRect(int gid) const
{
    int local_id = gid - firstgid;
    int tiles_per_row = tex_width / tilewidth;

    int tile_x = local_id % tiles_per_row;
    int tile_y = local_id / tiles_per_row;

    return Rectangle{
        (float)(margin + (tile_x * tilewidth) + (tile_x * spacing)),
        (float)(margin + (tile_y * tileheight) + (tile_y * spacing)),
        (float)tilewidth,
        (float)tileheight
    };
}

void ModuleMap::DrawMinimap(Rectangle area, Vector2 playerPos)
{
    float mapWidth = (float)(map_data.width * map_data.tilewidth);
    float mapHeight = (float)(map_data.height * map_data.tileheight);
    float scaleX = area.width / mapWidth;
    float scaleY = area.height / mapHeight;

    for (const auto& layer : map_data.layers)
    {
        if (layer->name == "Circuito" || layer->visible)
        {
            for (int y = 0; y < map_data.height; ++y)
            {
                for (int x = 0; x < map_data.width; ++x)
                {
                    int index = (y * layer->width) + x;
                    int gid = layer->tiles[index];

                        TileSet* tileset = nullptr;
                        for (auto& ts : map_data.tilesets) {
                            if (gid >= ts->firstgid) {
                                tileset = ts;
                            }
                        }

                        if (tileset != nullptr)
                        {
                            Rectangle origin = tileset->GetTileRect(gid);
                            Rectangle position;
                            position.x = area.x + (x * map_data.tilewidth * scaleX);
                            position.y = area.y + (y * map_data.tileheight * scaleY);
                            position.width = ceil(map_data.tilewidth * scaleX);
                            position.height = ceil(map_data.tileheight * scaleY);

                            DrawTexturePro(tileset->texture, origin, position, Vector2{ 0,0 }, 0.0f, WHITE);
                        }
                    
                }
            }
        }
    }
    for (int i = 0; i < App->scene_intro->entities.size(); i++)
    {
        int x, y;
        App->scene_intro->entities[i]->body->GetPhysicPosition(x, y);
        float miniX = area.x + (x * scaleX);
        float miniY = area.y + (y * scaleY);
        DrawCircle((int)miniX, (int)miniY, 4.0f, BLUE);
        DrawCircle((int)miniX, (int)miniY, 2.0f, GREEN);
    }
    float playerMiniX = area.x + (playerPos.x * scaleX);
    float playerMiniY = area.y + (playerPos.y * scaleY);
    DrawCircle((int)playerMiniX, (int)playerMiniY, 4.0f, RED);
    DrawCircle((int)playerMiniX, (int)playerMiniY, 2.0f, YELLOW);    
    DrawRectangleLinesEx(area, 3.0f, WHITE);
}
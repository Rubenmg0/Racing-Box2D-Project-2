#include "Globals.h"
#include "Application.h"
#include "ModuleMap.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"

#include <iostream>
#include <sstream>

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

    //Aqui se carga el mapa del Tiled
    Load("Assets/map/racing.tmx");

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

    //Cargar las propiedades del mapa
    pugi::xml_node map = map_file.child("map");

    map_data.width = map.attribute("width").as_int();
    map_data.height = map.attribute("height").as_int();
    map_data.tilewidth = map.attribute("tilewidth").as_int();
    map_data.tileheight = map.attribute("tileheight").as_int();

    //Cargar Tilesets
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

        //Construir path de la imagen
        std::string image_source = image.attribute("source").as_string();
        std::string path = "Assets/map/" + image_source;

        set->texture = LoadTexture(path.c_str());

        map_data.tilesets.push_back(set);
    }

    //Cargar Layers
    for (pugi::xml_node layer = map.child("layer"); layer; layer = layer.next_sibling("layer"))
    {
        MapLayer* lay = new MapLayer();
        lay->name = layer.attribute("name").as_string();
        lay->width = layer.attribute("width").as_int();
        lay->height = layer.attribute("height").as_int();
        lay->visible = layer.attribute("visible").as_bool(true);

        pugi::xml_node data = layer.child("data");
        std::string data_string = data.child_value();

        for (char& c : data_string)
        {
            if (c == ',' || c == '\r' || c == '\n') c = ' ';
        }

        std::stringstream ss(data_string);
        int gid;

        while (ss >> gid)
        {
            lay->data.push_back(gid);
        }

        map_data.layers.push_back(lay);
    }

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
    //Recorremos todas las capas
    for (const auto& layer : map_data.layers)
    {
        if (!layer->visible) continue;

        //Iteramos sobre cada tile de la capa
        for (int y = 0; y < map_data.height; ++y)
        {
            for (int x = 0; x < map_data.width; ++x)
            {
                int index = (y * layer->width) + x;

                if (index >= layer->data.size()) break;

                int gid = layer->data[index];

                //Si gid es 0, es un tile vacío
                if (gid > 0)
                {
                    TileSet* tileset = nullptr;

                    //Buscar a qué tileset pertenece este gid
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

                        //Usamos el Draw de ModuleRender para la cámara
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

    return true;
}

Vector2 ModuleMap::MapToWorld(int x, int y) const
{
    return Vector2{ (float)(x * map_data.tilewidth), (float)(y * map_data.tileheight) };
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
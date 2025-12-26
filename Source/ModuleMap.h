#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>

// PugiXML to read the file
#include "pugixml.hpp"

#include "box2d/box2d.h"

struct TileSet
{
    int firstgid;
    std::string name;
    int tilewidth;
    int tileheight;
    int spacing;
    int margin;
    int tex_width;
    int tex_height;
    Texture2D texture;

    Rectangle GetTileRect(int id) const;
};

struct MapLayer
{
    std::string name;
    int width;
    int height;
    std::vector<uint32_t> tiles;
    bool visible = true;
};

struct ObjectGroup
{
    std::string name;
    struct Object
    {
        float id, x, y, width, height;
        std::vector<b2Vec2> points;
        int order = 0;
    };
    std::vector<Object*> objects;
};


struct MapData
{
    int width;
    int height;
    int tilewidth;
    int tileheight;
    Color background_color;
    std::vector<TileSet*> tilesets;
    std::vector<ObjectGroup*> objectGroups;
    std::vector<MapLayer*> layers;
};

class ModuleMap : public Module
{
public:
    ModuleMap(Application* app, bool start_enabled = true);
    ~ModuleMap();
    void DrawMinimap(Rectangle area, Vector2 playerPos);
    bool Start();
    update_status Update();
    bool CleanUp();

    // Carga el mapa desde archivo
    bool Load(const char* path);

    // Dibuja el mapa
    void Draw();

    // Convierte coordenadas de mapa a mundo
    Vector2 MapToWorld(int x, int y) const;

public:
    MapData map_data;
    bool map_loaded;
};
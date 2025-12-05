#pragma once
#include "Module.h"
#include "Globals.h"
#include <vector>
#include <string>

// PugiXML para leer el archivo
#include "pugixml.hpp"

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
    std::vector<int> data; // Array con los GIDs de los tiles
    bool visible = true;
};

struct MapData
{
    int width;
    int height;
    int tilewidth;
    int tileheight;
    Color background_color;
    std::vector<TileSet*> tilesets;
    std::vector<MapLayer*> layers;
};

class ModuleMap : public Module
{
public:
    ModuleMap(Application* app, bool start_enabled = true);
    ~ModuleMap();

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